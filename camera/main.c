#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <turbojpeg.h>

#include "camera.h"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Device path must be specified. ie: '/dev/video0'\n");
        return 1;
    }

    struct camera* camera = camera_alloc();

    enum camera_error err = camera_open(argv[1], camera);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_open() error: %d\n", err);
        else
            printf("camera_open() error: %s\n", strerror(err));

        goto done_free;
    }

    struct camera_format_desc desc;
    int i = 0;

    do
    {
        i = camera_enumerate_formats(camera, i, &desc);
        if(i < 0) break;

        printf("format #%d:\n"
            "\tcompressed:   %s\n"
            "\temulated:     %s\n"
            "\tdescription:  %s\n"
            "\tpixel_format: %d\n",
            desc.index,
            desc.compressed ? "true" : "false",
            desc.emulated ? "true" : "false",
            desc.description,
            desc.pixel_format);
    }
    while(i >= 0);

    struct camera_format format;
    err = camera_get_format(camera, &format);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_get_format() error: %d\n", err);
        else
            printf("camera_get_format() error: %s\n", strerror(err));

        goto done_free;
    }

    printf("current format:\n"
        "{\n"
        "\twidth:        %7d\n"
        "\theight:       %7d\n"
        "\tstride:       %7d\n"
        "\tbyte_size:    %7d\n"
        "\tpixel_format: %7d\n"
        "\tcolorspace:   %7d\n"
        "}\n",
        format.width,
        format.height,
        format.stride,
        format.byte_size,
        format.pixel_format,
        format.colorspace);

    format.pixel_format = CAMERA_PIXEL_FORMAT_JPEG;
    format.width = 1280;
    format.height = 720;

    err = camera_set_format(camera, &format);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_set_format() error: %d\n", err);
        else
            printf("camera_set_format() error: %s\n", strerror(err));

        goto done_free;
    }
    printf("Set custom format\n");

    printf("current format:\n"
        "{\n"
        "\twidth:        %7d\n"
        "\theight:       %7d\n"
        "\tstride:       %7d\n"
        "\tbyte_size:    %7d\n"
        "\tpixel_format: %7d\n"
        "\tcolorspace:   %7d\n"
        "}\n",
        format.width,
        format.height,
        format.stride,
        format.byte_size,
        format.pixel_format,
        format.colorspace);

    err = camera_init(camera);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_init() error: %d\n", err);
        else
            printf("camera_init() error: %s\n", strerror(err));
        
        goto done_close;
    }
        
    struct camera_frame frame;
    camera_init_frame(camera, &frame);
    printf("Initialized a frame\n");

    // libjpeg-turbo setup
    tjhandle jpegCompress = tjInitCompress();
    unsigned long jpegBufSize = tjBufSize(format.width, format.height, TJSAMP_444);
    unsigned char* jpegBuf = malloc(jpegBufSize);
    printf("Initialized libjpeg-turbo\n");

    err = camera_capture_start(camera);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_capture_start() error: %d\n", err);
        else
            printf("camera_capture_start() error: %s\n", strerror(err));

        goto done_deinit;
    }
    printf("Camera stream started\n");

    for(int i = 0; i < 10; ++i)
    {
        err = camera_capture_frame(camera, &frame);
        if(err != CAMERA_SUCCESS)
        {
            if(err < 0)
                printf("camera_capture_frame() error: %d\n", err);
            else
                printf("camera_capture_frame() error: %s\n", strerror(err));

            break;
        }

        // write ourselves a jpg!
        unsigned long frameSize;
        int status = tjCompress2(jpegCompress, frame.data,
            format.width, format.stride, format.height,
            TJPF_RGB, &jpegBuf, &frameSize, TJSAMP_444, 75, TJFLAG_NOREALLOC);

        if(status != 0)
        {
            printf("tjCompress2() error: %s\n", tjGetErrorStr());
            continue;
        }

        // filename format: "out0#.jpg"
        char filename[10] = {0};
        snprintf(filename, 10, "out%02d.jpg", i);

        FILE* outFile = fopen(filename, "wb");
        if(outFile == NULL)
        {
            int err = errno;
            printf("fopen(%s) failed (%d): %s\n", err, strerror(err));
            continue;
        }

        size_t written = fwrite(frame.data, 1, frame.format.byte_size, outFile);
        if(written < frameSize)
            printf("fwrite() error: bytes written (%d) did not match expected (%d)!\n", written, frameSize);

        fclose(outFile);
    }

    err = camera_capture_stop(camera);
    if(err != CAMERA_SUCCESS)
    {
        if(err < 0)
            printf("camera_capture_stop() error: %d\n", err);
        else
            printf("camera_capture_stop() error: %s\n", strerror(err));
    }
    else
    {
        printf("Camera stream stopped\n");
    }

    // libjpeg-turbo cleanup
    free(jpegBuf);
    tjDestroy(jpegCompress);

    camera_release_frame(&frame);

done_deinit:
    camera_deinit(camera);
    
done_close:
    camera_close(camera);
    
done_free:
    camera_free(camera);

    return 0;
}
