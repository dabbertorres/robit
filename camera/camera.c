#include "camera.h"

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev2.h>

struct buffer
{
    void* ptr;
    size_t length;
};

struct camera
{
    int fd;
    struct buffer* buffers;
    size_t buffers_count;
};

int ioctl_help(int fd, int req, void* data);

enum camera_error camera_open(const char* path, struct camera* cam)
{
    int status = CAMERA_SUCCESS;

    int fd = open(path, O_RDWR);
    if(fd == -1)
        return CAMERA_ERROR_OPEN;

    // get device capabilities
    struct v4l2_capability cap;
    int err = ioctl_help(fd, VIDIOC_QUERYCAP, &cap);

    // make sure the device satisfies our requirements 

    if(err == -1 && errno == EINVAL)
    {
        status = CAMERA_ERROR_NOT_SUPPORTED;
        goto fail;
    }

    if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE == 0)
    {
        status = CAMERA_ERROR_NO_VIDEO_CAPTURE;
        goto fail;
    }

    if(cap.capabilities & V4L2_CAP_STREAMING == 0)
    {
        status = CAMERA_ERROR_NO_STREAMING;
        goto fail;
    }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));

    // arbitrary really
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    err = ioctl_help(fd, VIDIOC_REQBUFS, &req);
    if(err == -1 && errno == EINVAL)
    {
        status = CAMERA_ERROR_NO_MMAP;
        goto fail;
    }

    // camera will theoretically work. will it work for real?

    // we want a minimum of 2 buffers - front and back
    if(req.count < 2)
    {
        status = CAMERA_ERROR_OUT_OF_BUFFER_MEM;
        goto fail;
    }

    cam->buffers = malloc(req.count * sizeof(struct buffer));
    if(cam->buffers == NULL)
    {
        status = CAMERA_ERROR_OUT_OF_MEMORY;
        goto fail;
    }

    // got our buffers, now lets mmap them

    cam->buffers_count = req.count;

    for(size_t i = 0; i < cam->buffers_count; ++i)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        err = ioctl_help(fd, VIDIOC_QUERYBUF, &buf);
        if(err == -1)
        {
            status = CAMERA_ERROR_MMAP_FAILED;
            goto fail;
        }

        struct buffer* cbuf = &cam->buffers[i];
        cbuf->length = buf.length;
        cbuf->ptr = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if(cbuf->ptr == MAP_FAILED)
        {
            status = CAMERA_ERROR_MMAP_FAILED;
            goto fail;
        }
    }

    // finally, success!

    cam->fd = fd;

done:
    return status;

fail:
    close(fd);
    return status;
}

void camera_close(struct camera* cam)
{
    for(size_t i = 0; i < cam->buffers_count; ++i)
        munmap(cam->buffers[i].ptr, cam->buffers[i].length);

    close(cam->fd);
}

enum camera_error camera_capture_start(struct camera* cam)
{
    enum v4l2_buf_type bufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for(size_t i = 0; i < cam->buffers_count; ++i)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = bufType;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        int err = ioctl_help(cam->fd, VIDIOC_QBUF, &buf);
        if(err == -1)
            return CAMERA_ERROR_BUF_EXCHANGE_FAIL;
    }

    int err = ioctl_help(cam->fd, VIDIOC_STREAMON, &bufType);
    if(err == -1)
        return CAMERA_ERROR_STREAM_START_FAIL;
    else
        return CAMERA_SUCCESS;
}

enum camera_error camera_capture_frame(struct camera* cam)
{
    for(;;)
    {
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(cam->fd, &fds);

        // timeout settings (arbitrary for the moment)
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int err = select(cam->fd + 1, &fds, NULL, NULL, &tv);

        switch(err)
        {
            case -1:
                // just an interrupt, it's fine
                // otherwise, crap
                if(errno != EINTR)
                    return errno;

            case 0:
                // timeout...
                return CAMERA_ERROR_TIMEOUT;
        }

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // grab next frame from driver
        err = ioctl_help(cam->fd, VIDIOC_DQBUF, &buf);
        // if not just a wake up - stop select() loop 
        if(err == -1 && errno != EAGAIN)
            return errno;

        // TODO send frame to encoder
        // github.com/cisco/openh264?
        
        // hand buffer back to driver
        err = ioctl_help(cam->fd, VIDIOC_QBUF, &buf);
        if(err == -1)
            return errno;
    }

    return CAMERA_SUCCESS;
}

enum camera_error camera_capture_stop(struct camera* cam)
{
    enum v4l2_buf_type bufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int err = ioctl_help(cam->fd, VIDIOC_STREAMOFF, &bufType);
    if(err == -1)
        return CAMERA_ERROR_STREAM_STOP_FAIL;
    else
        return CAMERA_SUCCESS;
}

int ioctl_help(int fd, int req, void* data)
{
    int r;

    // syscall may be interrupted, try again until system finishes
    do
        r = ioctl(fd, req, data);
    while(r == -1 && errno == EINTR);

    return r;
}
