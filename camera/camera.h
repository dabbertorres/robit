#pragma once

#include <stdint.h>

// All camera_error values are negative (except for CAMERA_SUCCESS, which is 0)
// if a camera_error value is positive, then it is an errno value
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
    CAMERA_ERROR_FORMAT            = -13,
};

enum camera_colorspace
{
    // can be used to let driver pick a colorspace
    CAMERA_COLORSPACE_DEFAULT,

    // SMPTE 170M (NTSC, PAL, SDTV)
    CAMERA_COLORSPACE_SMPTE170M,

    // ITU BT.709 (HDTV)
    CAMERA_COLORSPACE_REC709,

    // sRGB (most webcams, computer graphics, etc)
    CAMERA_COLORSPACE_SRGB,

    // AdobeRGB (opRGB)
    CAMERA_COLORSPACE_ADOBERGB,

    // ITU BT.2020 (UHDTV)
    CAMERA_COLORSPACE_BT2020,

    // SMPTE RP 431-2 (cinema projectors)
    CAMERA_COLORSPACE_DCI_P3,

    // SMPTE 240M (early HDTV, ie: 1988-1998)
    CAMERA_COLORSPACE_SMPTE240M,

    // NTSC 1953 (obsolete, use SMPTE 170M)
    CAMERA_COLORSPACE_470_SYSTEM_M,

    // EBU Tech 3213 (PAL/SECAM in 1975, obsolete, use SMPTE 170M)
    CAMERA_COLORSPACE_470_SYSTEM_BG,

    // (Motion-)JPEG formats - effectively identical to sRGB
    CAMERA_COLORSPACE_JPEG,

    // minimal processing - software using this will have to know capture device internals
    CAMERA_COLORSPACE_RAW,
};

// comment format: <FourCC code> - <bit memory representation>
// bit memory rep: [byte 0 bits 7..0], [byte 1 bits 7..0]...
// underscores are "empty" bits (ie: don't use them)
enum camera_pixel_format
{
    /* RGB formats */

    // RGB1 - [r2 r1 r0 g2 g1 g0 b1 b0]
    CAMERA_PIXEL_FORMAT_RGB332,

    // AR12 - [g3 g2 g1 g0 b3 b2 b1 b0] [a3 a2 a1 a0 r3 r2 r1 r0]
    CAMERA_PIXEL_FORMAT_ARGB444,

    // XR12 - [g3 g2 g1 g0 b3 b2 b1 b0] [_ _ _ _ r3 r2 r1 r0]
    CAMERA_PIXEL_FORMAT_XRGB444,

    // AR15 - [g2 g1 g0 b4 b3 b2 b1 b0] [a r4 r3 r2 r1 r0 g4 g3]
    CAMERA_PIXEL_FORMAT_ARGB555,

    // XR15 - [g2 g1 g0 b4 b3 b2 b1 b0] [_ r4 r3 r2 r1 r0 g4 g3]
    CAMERA_PIXEL_FORMAT_XRGB555,

    // RGBP - [g2 g1 g0 b4 b3 b2 b1 b0] [r4 r3 r2 r1 r0 g5 g4 g3]
    CAMERA_PIXEL_FORMAT_RGB565,

    // AR15 | 1 << 31 - [a r4 r3 r2 r1 r0 g4 g3] [g2 g1 g0 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_ARGB555X,

    // XR15 | 1 << 31 - [_ r4 r3 r2 r1 r0 g4 g3] [g2 g1 g0 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_XRGB555X,

    // RGBR - [r4 r3 r2 r1 r0 g5 g4 g3] [g2 g1 g0 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_RGB565X,

    // BGR3 - [b7 b6 b5 b4 b3 b2 b1 b0] [g7 g6 g5 g4 g3 g2 g1 g0] [r7 r6 r5 r4 r3 r2 r1 r0]
    CAMERA_PIXEL_FORMAT_BGR24,

    // RGB3 - [r7 r6 r5 r4 r3 r2 r1 r0] [g7 g6 g5 g4 g3 g2 g1 g0] [b7 b6 b5 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_RGB24,

    // BGRH - [b5 b4 b3 b2 b1 b0 g5 g4] [g3 g2 g1 g0 r5 r3 r3 r2] [r1 r0 _ _ _ _ _ _]
    CAMERA_PIXEL_FORMAT_BGR666,

    // AR24 - [b7 b6 b5 b4 b3 b2 b1 b0] [g7 g6 g5 g4 g3 g2 g1 g0] [r7 r6 r5 r4 r3 r2 r1 r0] [a7 a6 a5 a4 a3 a2 a1 a0]
    CAMERA_PIXEL_FORMAT_ABGR32,

    // XR24 - [b7 b6 b5 b4 b3 b2 b1 b0] [g7 g6 g5 g4 g3 g2 g1 g0] [r7 r6 r5 r4 r3 r2 r1 r0] [_ _ _ _ _ _ _ _]
    CAMERA_PIXEL_FORMAT_XBGR32,

    // BA24 - [a7 a6 a5 a4 a3 a2 a1 a0] [r7 r6 r5 r4 r3 r2 r1 r0] [g7 g6 g5 g4 g3 g2 g1 g0] [b7 b6 b5 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_ARGB32,

    // BX24 - [_ _ _ _ _ _ _ _] [r7 r6 r5 r4 r3 r2 r1 r0] [g7 g6 g5 g4 g3 g2 g1 g0] [b7 b6 b5 b4 b3 b2 b1 b0]
    CAMERA_PIXEL_FORMAT_XRGB32,

    // more: https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/pixfmt-rgb.html

    /* YUYV formats */

    // Y444
    CAMERA_PIXEL_FORMAT_YUV444,

    // YUVO
    CAMERA_PIXEL_FORMAT_YUV555,

    // YUVP
    CAMERA_PIXEL_FORMAT_YUV565,

    // YUV4
    CAMERA_PIXEL_FORMAT_YUV32,

    // more: https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/yuv-formats.html

    /* HSV formats */

    // HSV4
    CAMERA_PIXEL_FORMAT_HSV32,

    // HSV3
    CAMERA_PIXEL_FORMAT_HSV24,

    /* depth formats */
    // https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/depth-formats.html

    /* compressed formats */
    // https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/pixfmt-compressed.html

    // JPEG
    CAMERA_PIXEL_FORMAT_JPEG,

    /* reserved formats */
    // https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/pixfmt-reserved.html

    // MJPG
    CAMERA_PIXEL_FORMAT_MJPEG,
};

struct camera_format
{
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t byte_size;
    enum camera_pixel_format pixel_format;
    enum camera_colorspace colorspace;
};

struct camera_format_desc
{
    uint32_t index;

    // non-zero if true, zero if false
    uint8_t compressed;
    uint8_t emulated;

    // user-friendly ascii string describing format, ie: "YUV 4:2:2"
    uint8_t description[32];

    enum camera_pixel_format pixel_format;
};

struct camera_frame
{
    void* data;
    struct camera_format format;
};

struct camera;

// a typical call sequence of functions is:
// camera_alloc()
// camera_open()
// camera_init()
// ... capture frames, etc
// camera_deinit()
// camera_close()
// camera_free()

struct camera* camera_alloc();
void camera_free(struct camera* cam);

enum camera_error camera_open(const char* path, struct camera* cam);
void camera_close(struct camera* cam);

enum camera_error camera_init(struct camera* cam);
void camera_deinit(struct camera* cam);

// returns index + 1 if *desc is valid (ie: keep iterating)
// once no more formats are available to enumerate (or an error occurs), it returns negative
int camera_enumerate_formats(struct camera* cam, int index, struct camera_format_desc* desc);

enum camera_error camera_get_format(struct camera* cam, struct camera_format* format);

// requests the driver to apply the named format, and returns the resulting format set by the driver
enum camera_error camera_set_format(struct camera* cam, struct camera_format* format);

void camera_init_frame(struct camera* cam, struct camera_frame* frame);
void camera_release_frame(struct camera_frame* frame);

enum camera_error camera_capture_start(struct camera* cam);
enum camera_error camera_capture_frame(struct camera* cam, struct camera_frame* frame);
enum camera_error camera_capture_stop(struct camera* cam);
