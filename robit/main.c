#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>

#include <signal.h>
#include <sys/socket.h>

#include "motor/motor.h"
#include "input.h"

#define INPUT_SOCKET_NAME "/tmp/robit.input"

static int run = 1;

void signal_handler(int)
{
    run = 0;
}

int main()
{
    int pins[] = { 19, 26, 6, 13, 20, 21, 12, 16 };

    struct motor_group* robit = motor_make_group(pins);
    if(robit == NULL)
    {
        printf("something be broke\n");
        return 1;
    }

    // just in case the socket still exists for some reason
    unlink(INPUT_SOCKET_NAME);

    struct sockaddr_un input_sock_addr;
    int input_sock;

    input_sock = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (input_sock < 0)
    {
        printf("socket() failed: %s\n", strerror(errno));
        return 1;
    }

    memset(&input_sock_addr, 0, sizeof(input_sock_addr));
    input_sock_addr.sun_family = AF_UNIX;
    strncpy(input_sock_addr.sun_path, INPUT_SOCKET_NAME, sizeof(input_sock_addr.sun_path) - 1);

    int sts = bind(input_sock, (const struct sockaddr*) &input_sock_addr, sizeof(input_sock_addr));
    if (sts < 0)
    {
        printf("bind() failed: %s\n", strerror(errno));
        return 1;
    }

    sts = listen(input_sock, 16);
    if (sts < 0)
    {
        printf("listen() failed: %s\n", strerror(errno));
        return 1;
    }

    // only ever going to be handling a single connection really...

    int input_data_sock = accept(input_sock, NULL, NULL);
    if (input_data_sock < 0)
    {
        printf("accept() failed: %s\n", strerror(errno));
        return 1;
    }

    char input_buf[2];

    while(run)
    {
        sts = read(input_data_sock, input_buf, INPUT_MSG_SIZE);
        if (sts < 0)
        {
            printf("read() failed: %s\n", strerror(errno));
            continue;
        }

        if (input_buf[1] == INPUT_PRESS)
        {
            switch(input_buf[0])
            {
                case INPUT_UP:
                    motor_forward(robit);
                    break;

                case INPUT_LEFT:
                    motor_rotate_counterwise(robit);
                    break;

                case INPUT_RIGHT:
                    motor_rotate_clockwise(robit);
                    break;

                case INPUT_DOWN:
                    motor_reverse(robit);
                    break;

                case INPUT_STOP:
                    motor_stop(robit);
                    break;

                default:
                    break;
            }
        }
    }

    motor_stop(robit);
    motor_free_group(robit);

    close(input_data_sock);
    close(input_sock);
    unlink(INPUT_SOCKET_NAME);

    return 0;
}
