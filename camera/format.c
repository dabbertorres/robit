#include "camera.h"

#include <linux/videodev2.h>

enum camera_pixel_format convert_fourcc(uint32_t fourcc)
{
    switch(fourcc)
    {
        /* RGB formats */

        case V4L2_PIX_FMT_RGB332:
            return CAMERA_PIXEL_FORMAT_RGB332;

        case V4L2_PIX_FMT_ARGB444:
            return CAMERA_PIXEL_FORMAT_ARGB444;

        case V4L2_PIX_FMT_XRGB444:
            return CAMERA_PIXEL_FORMAT_XRGB444;

        case V4L2_PIX_FMT_ARGB555:
            return CAMERA_PIXEL_FORMAT_ARGB555;

        case V4L2_PIX_FMT_XRGB555:
            return CAMERA_PIXEL_FORMAT_XRGB555;

        case V4L2_PIX_FMT_RGB565:
            return CAMERA_PIXEL_FORMAT_RGB565;

        case V4L2_PIX_FMT_ARGB555X:
            return CAMERA_PIXEL_FORMAT_ARGB555X;

        case V4L2_PIX_FMT_XRGB555X:
            return CAMERA_PIXEL_FORMAT_XRGB555X;

        case V4L2_PIX_FMT_RGB565X:
            return CAMERA_PIXEL_FORMAT_RGB565X;

        case V4L2_PIX_FMT_BGR24:
            return CAMERA_PIXEL_FORMAT_BGR24;

        case V4L2_PIX_FMT_RGB24:
            return CAMERA_PIXEL_FORMAT_RGB24;

        case V4L2_PIX_FMT_BGR666:
            return CAMERA_PIXEL_FORMAT_BGR666;

        case V4L2_PIX_FMT_ABGR32:
            return CAMERA_PIXEL_FORMAT_ABGR32;

        case V4L2_PIX_FMT_XBGR32:
            return CAMERA_PIXEL_FORMAT_XBGR32;

        case V4L2_PIX_FMT_ARGB32:
            return CAMERA_PIXEL_FORMAT_ARGB32;

        case V4L2_PIX_FMT_XRGB32:
            return CAMERA_PIXEL_FORMAT_XRGB32;

        /* YUV formats */

        case V4L2_PIX_FMT_YUV444:
            return CAMERA_PIXEL_FORMAT_YUV444;

        case V4L2_PIX_FMT_YUV555:
            return CAMERA_PIXEL_FORMAT_YUV555;

        case V4L2_PIX_FMT_YUV565:
            return CAMERA_PIXEL_FORMAT_YUV565;

        case V4L2_PIX_FMT_YUV32:
            return CAMERA_PIXEL_FORMAT_YUV32;

        /* HSV formats */

        case V4L2_PIX_FMT_HSV32:
            return CAMERA_PIXEL_FORMAT_HSV32;

        case V4L2_PIX_FMT_HSV24:
            return CAMERA_PIXEL_FORMAT_HSV24;

        /* compressed formats */

        case V4L2_PIX_FMT_JPEG:
            return CAMERA_PIXEL_FORMAT_JPEG;

        /* reserved formats */

        case V4L2_PIX_FMT_MJPEG:
            return CAMERA_PIXEL_FORMAT_MJPEG;

        default:
            return -1;
    }
}

uint32_t convert_pixel_format(enum camera_pixel_format f)
{
    switch(f)
    {
        /* RGB formats */

        case CAMERA_PIXEL_FORMAT_RGB332:
            return V4L2_PIX_FMT_RGB332;

        case CAMERA_PIXEL_FORMAT_ARGB444:
            return V4L2_PIX_FMT_ARGB444;

        case CAMERA_PIXEL_FORMAT_XRGB444:
            return V4L2_PIX_FMT_XRGB444;

        case CAMERA_PIXEL_FORMAT_ARGB555:
            return V4L2_PIX_FMT_ARGB555;

        case CAMERA_PIXEL_FORMAT_XRGB555:
            return V4L2_PIX_FMT_XRGB555;

        case CAMERA_PIXEL_FORMAT_RGB565:
            return V4L2_PIX_FMT_RGB565;

        case CAMERA_PIXEL_FORMAT_ARGB555X:
            return V4L2_PIX_FMT_ARGB555X;

        case CAMERA_PIXEL_FORMAT_XRGB555X:
            return V4L2_PIX_FMT_XRGB555X;

        case CAMERA_PIXEL_FORMAT_RGB565X:
            return V4L2_PIX_FMT_RGB565X;

        case CAMERA_PIXEL_FORMAT_BGR24:
            return V4L2_PIX_FMT_BGR24;

        case CAMERA_PIXEL_FORMAT_RGB24:
            return V4L2_PIX_FMT_RGB24;

        case CAMERA_PIXEL_FORMAT_BGR666:
            return V4L2_PIX_FMT_BGR666;

        case CAMERA_PIXEL_FORMAT_ABGR32:
            return V4L2_PIX_FMT_ABGR32;

        case CAMERA_PIXEL_FORMAT_XBGR32:
            return V4L2_PIX_FMT_XBGR32;

        case CAMERA_PIXEL_FORMAT_ARGB32:
            return V4L2_PIX_FMT_ARGB32;

        case CAMERA_PIXEL_FORMAT_XRGB32:
            return V4L2_PIX_FMT_XRGB32;

        /* YUV formats */

        case CAMERA_PIXEL_FORMAT_YUV444:
            return V4L2_PIX_FMT_YUV444;

        case CAMERA_PIXEL_FORMAT_YUV555:
            return V4L2_PIX_FMT_YUV555;

        case CAMERA_PIXEL_FORMAT_YUV565:
            return V4L2_PIX_FMT_YUV565;

        case CAMERA_PIXEL_FORMAT_YUV32:
            return V4L2_PIX_FMT_YUV32;

        /* HSV formats */

        case CAMERA_PIXEL_FORMAT_HSV32:
            return V4L2_PIX_FMT_HSV32;

        case CAMERA_PIXEL_FORMAT_HSV24:
            return V4L2_PIX_FMT_HSV24;

        /* compressed formats */

        case CAMERA_PIXEL_FORMAT_JPEG:
            return V4L2_PIX_FMT_JPEG;

        /* reserved formats */

        case CAMERA_PIXEL_FORMAT_MJPEG:
            return V4L2_PIX_FMT_MJPEG;

        default:
            return -1;
    }
}
