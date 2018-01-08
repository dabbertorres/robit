#pragma once

#include <stdint.h>

// additional information may be available through errno
enum camera_error
{
    CAMERA_SUCCESS                 = 0,
    CAMERA_ERROR_OPEN              = -1,
    CAMERA_ERROR_NOT_SUPPORTED     = -2,
    CAMERA_ERROR_NO_VIDEO_CAPTURE  = -3,
    CAMERA_ERROR_NO_STREAMING      = -4,
    CAMERA_ERROR_NO_MMAP           = -5,
    CAMERA_ERROR_OUT_OF_BUFFER_MEM = -6,
    CAMERA_ERROR_OUT_OF_MEMORY     = -7,
    CAMERA_ERROR_MMAP_FAILED       = -8,
    CAMERA_ERROR_BUF_EXCHANGE_FAIL = -9,
    CAMERA_ERROR_STREAM_START_FAIL = -10,
    CAMERA_ERROR_STREAM_STOP_FAIL  = -11,
    CAMERA_ERROR_TIMEOUT           = -12,
};

struct camera;

struct camera_format
{
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t channels;
};

// frame format from camera_capture_frame is RGB24
struct camera_frame
{
    void* data;
    struct camera_format format;
};

// camera_open inits cam, and returns an error (or success) code
struct camera* camera_alloc();
void camera_free(struct camera* cam);
enum camera_error camera_open(const char* path, struct camera* cam);
void camera_close(struct camera* cam);

enum camera_error camera_get_format(struct camera_format* format);

// requests the driver to apply the named format, and returns the resulting format set by the driver
enum camera_error camera_set_format(struct camera_format* format);

void camera_init_frame(struct camera* cam, struct camera_frame* frame);
void camera_release_frame(struct camera_frame* frame);

enum camera_error camera_capture_start(struct camera* cam);
enum camera_error camera_capture_frame(struct camera* cam, struct camera_frame* frame);
enum camera_error camera_capture_stop(struct camera* cam);

