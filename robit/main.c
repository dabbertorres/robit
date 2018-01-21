#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define HANDLE_ERROR(funcName, destLabel)\
    do {\
        int sts = errno;\
        printf(funcName "() error (%d): %s\n", sts, strerror(sts));\
        goto destLabel;\
    } while(0)

typedef int file_desc;

struct unix_socket
{
    file_desc fd;
    struct sockaddr_un addr;
};

struct rmt_ctrl_msg
{
    uint8_t dir;
    uint8_t on;
} __attribute__((packed));

const char* rmtCtrlSockName = "/tmp/robit-remote-control.socket";

// TODO establish arguments
int main(int argc, char** argv)
{
    int status = 0;

    struct unix_socket rmtCtrl = {0};

    rmtCtrl.fd = socket(PF_UNIX, SOCK_SEQPACKET /*| SOCK_NONBLOCK*/, 0);
    if(rmtCtrl.fd == -1)
        HANDLE_ERROR("socket", done_done);

    rmtCtrl.addr.sun_family = AF_UNIX;
    strncpy(rmtCtrl.addr.sun_path, rmtCtrlSockName, sizeof(rmtCtrl.addr.sun_path) - 1);

    if(bind(rmtCtrl.fd, (struct sockaddr*)&rmtCtrl.addr, sizeof(struct sockaddr_un)) == -1)
        HANDLE_ERROR("bind", done_close);

    // arbitrary number of backlog connection requests
    if(listen(rmtCtrl.fd, 3) == -1)
        HANDLE_ERROR("listen", done_unlink);

    //for(;;)
    {
        // dumb implementation. real world, we'd likely have a non-blocking main socket in the first place.
        // loop would be running in a thread, using select(), checking on rmtCtrl.fd for new connections (accept()),
        // AND checking on all client connections for messages/closing/etc.
        //
        // Although, in robit's use case, we're going to have only 1 (persistent) client connection ever.
        // So, maybe not a thread. But a poll() would be good:
        //      In our main execution loop, check if we have control messages.
        //      If so, read/parse it, and make changes to our state as necessary.
        //      If no message, then continue.

        file_desc peerFd;

        peerFd = accept(rmtCtrl.fd, NULL, NULL);
        if(peerFd == -1)
        {
            status = errno;
            printf("accept() from peer connection '%d' error (%d): %s\n", peerFd, status, strerror(status));
            //continue;
            goto done_unlink;
        }

        // generally all we're doing here is reading.
        // maybe consider writing some sort of error status if needed
        struct rmt_ctrl_msg msg = {0};
        
        int readCount = read(peerFd, &msg, sizeof(struct rmt_ctrl_msg));
        if(readCount == -1)
        {
            status = errno;
            printf("read() from peer connection '%d' error (%d): %s\n", peerFd, status, strerror(status));
        }
        else
        {
            printf("message from peer '%d': %c:%s\n", peerFd, msg.dir, msg.on ? "on" : "off");
        }

        close(peerFd);
    }

done_unlink:
    unlink(rmtCtrlSockName);

done_close:
    close(rmtCtrl.fd);

done_done:
    return status;
}

