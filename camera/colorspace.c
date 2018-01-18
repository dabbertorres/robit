#include "camera.h"

#include <linux/videodev2.h>

enum camera_colorspace convert_colorspace(enum v4l2_colorspace c)
{
    switch(c)
    {
    case V4L2_COLORSPACE_SMPTE170M:
        return CAMERA_COLORSPACE_SMPTE170M;

    case V4L2_COLORSPACE_REC709:
        return CAMERA_COLORSPACE_REC709;

    case V4L2_COLORSPACE_SRGB:
        return CAMERA_COLORSPACE_SRGB;

    case V4L2_COLORSPACE_ADOBERGB:
        return CAMERA_COLORSPACE_ADOBERGB;

    case V4L2_COLORSPACE_BT2020:
        return CAMERA_COLORSPACE_BT2020;

    case V4L2_COLORSPACE_DCI_P3:
        return CAMERA_COLORSPACE_DCI_P3;

    case V4L2_COLORSPACE_SMPTE240M:
        return CAMERA_COLORSPACE_SMPTE240M;

    case V4L2_COLORSPACE_470_SYSTEM_M:
        return CAMERA_COLORSPACE_470_SYSTEM_M;

    case V4L2_COLORSPACE_470_SYSTEM_BG:
        return CAMERA_COLORSPACE_470_SYSTEM_BG;

    case V4L2_COLORSPACE_JPEG:
        return CAMERA_COLORSPACE_JPEG;

    case V4L2_COLORSPACE_RAW:
        return CAMERA_COLORSPACE_RAW;

    case V4L2_COLORSPACE_DEFAULT:
    default:
        return CAMERA_COLORSPACE_DEFAULT;
    }
}
