#pragma once

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

// camera_open inits cam, and returns an error (or success) code
enum camera_error camera_open(const char* path, struct camera* cam);
void camera_close(struct camera* cam);

enum camera_error camera_capture_start(struct camera* cam);
enum camera_error camera_capture_frame(struct camera* cam);
enum camera_error camera_capture_stop(struct camera* cam);

