#include "camera.h"

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
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
    struct frame user_frame;
    pthread_mutex_t user_frame_mutex;
    pthread_t capture_thread;
};

int ioctl_help(int fd, int req, void* data);

struct camera* camera_alloc()
{
    return malloc(sizeof(struct camera));
}

void camera_free(struct camera* cam)
{
    free(cam);
}

enum camera_error camera_open(const char* path, struct camera* cam)
{
    int status = CAMERA_SUCCESS;

    int fd = open(path, O_RDWR | O_NONBLOCK, 0);
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

    // TODO figure out the size the camera wants to stream at (see struct frame)
    // then malloc cam->user_frame.data, and assign cam->user_frame's other fields

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

    pthread_mutex_init()

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

enum camera_error camera_get_format(struct camera_format* format)
{
    v4l2_format fmt;

    int err = ioctl_help(cam->fd, VIDIOC_G_FMT, &fmt);
    if(err == -1)
        return err;

    format->width = fmt.fmt.pix.width;
    format->height = fmt.fmt.pix.height;
    format->stride = fmt.fmt.pix.bytesperline;
    format->channels = format->stride / format->width;

    return CAMERA_SUCCESS;
}

enum camera_error camera_set_format(struct camera_format* format)
{
    v4l2_format fmt;

    fmt.fmt.pix.width = format->width;
    fmt.fmt.pix.height = format->height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    int err = ioctl_help(cam->fd, VIDIOC_S_FMT, &fmt);
    if(err == -1)
        return err;

    format->width = fmt.fmt.pix.width;
    format->height = fmt.fmt.pix.height;
    format->stride = fmt.fmt.pix.bytesperline;
    format->channels = format->stride / format->width;

    return CAMERA_SUCCESS;
}

void camera_init_frame(struct camera* cam, struct camera_frame* frame)
{
    memcpy(frame, &cam->user_frame, sizeof(struct camera_frame));
    frame->data = malloc(frame->stride * frame->height);
}

void camera_release_frame(struct camera_frame* frame)
{
    free(frame->data);
}

void* camera_capture_frame_loop(struct camera* cam)
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
                // just an interrupt? it's fine
                // otherwise, crap
                if(errno != EINTR)
                    return (void*)errno;

            case 0:
                // timeout... lost connection?
                return (void*)CAMERA_ERROR_TIMEOUT;
        }

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // grab next frame from driver
        err = ioctl_help(cam->fd, VIDIOC_DQBUF, &buf);
        // if not just a wake up - stop select() loop
        if(err == -1 && errno != EAGAIN)
            return (void*)errno;

        pthread_mutex_lock(&cam->user_frame_mutex);
        memcpy(cam->user_frame->data, cam->buffers[buf.index].start, buf.bytesused);
        pthread_mutex_unlock(&cam->user_frame_mutex);

        // hand buffer back to driver
        err = ioctl_help(cam->fd, VIDIOC_QBUF, &buf);
        if(err == -1)
            return (void*)errno;
    }
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
    {
        return CAMERA_ERROR_STREAM_START_FAIL;
    }
    else
    {
        err = pthread_create(&cam->capture_thread, NULL, camera_capture_frame_loop, cam);
        if(err != 0)
        {
            camera_capture_stop(cam);
            return err
        }
        else
        {
            return CAMERA_SUCCESS;
        }
    }
}

enum camera_error camera_capture_frame(struct camera* cam, struct camera_frame* frame)
{
    pthread_mutex_lock(&cam->user_frame_mutex);
    memcpy(frame->data, cam->user_frame.data, frame->stride * frame->height);
    pthread_mutex_unlock(&cam->user_frame_mutex);
    return CAMERA_SUCCESS;
}

enum camera_error camera_capture_stop(struct camera* cam)
{
    pthread_cancel(cam->capture_thread);

    enum v4l2_buf_type bufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    err = ioctl_help(cam->fd, VIDIOC_STREAMOFF, &bufType);
    if(err == -1)
    {
        return CAMERA_ERROR_STREAM_STOP_FAIL;
    }
    else
    {
        void* retval;
        err = pthread_join(cam->capture_thread, &retval);

        if(err != 0)
            return err;
        // if the thread wasn't canceled... ¯\_(`-`)_/¯
        else if(retval != PTHREAD_CANCELED)
            return -1;
        else
            return CAMERA_SUCCESS;
    }
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

