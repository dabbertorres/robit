package camera

// #include "camera.h"
import "C"
import (
	"errors"
	"image"
)

type Handle struct {
	c *C.struct_camera
}

var cameraError = map[C.enum_camera_error]error{
	C.CAMERA_SUCCESS:                 nil,
	C.CAMERA_ERROR_OPEN:              errors.New("cannot open camera"),
	C.CAMERA_ERROR_NOT_SUPPORTED:     errors.New("camera device not supported"),
	C.CAMERA_ERROR_NO_VIDEO_CAPTURE:  errors.New("camera does not support video capture"),
	C.CAMERA_ERROR_NO_STREAMING:      errors.New("camera does not support streaming"),
	C.CAMERA_ERROR_NO_MMAP:           errors.New("camera does not support memmap"),
	C.CAMERA_ERROR_OUT_OF_BUFFER_MEM: errors.New("out of buffer memory"),
	C.CAMERA_ERROR_OUT_OF_MEMORY:     errors.New("out of memory"),
	C.CAMERA_ERROR_MMAP_FAILED:       errors.New("memmap failed"),
	C.CAMERA_ERROR_BUF_EXCHANGE_FAIL: errors.New("buffer exchange with the driver failed"),
	C.CAMERA_ERROR_STREAM_START_FAIL: errors.New("camera stream starting failed"),
	C.CAMERA_ERROR_STREAM_STOP_FAIL:  errors.New("camera stream stopping failed"),
	C.CAMERA_ERROR_TIMEOUT:           errors.New("camera timeout"),
}

func OpenCamera() (Handle, error) {
	c := C.camera_alloc()
	err := C.camera_open("/dev/video0", c)
	if err != C.CAMERA_SUCCESS {
		return Handle{}, cameraError[err]
	} else {
		return Handle{c}, nil
	}
}

func (c Handle) Release() {
	C.camera_close(c.c)
	C.camera_free(c.c)
}

func (c Handle) Start() error {
	err := C.camera_capture_start(c.c)
	if err != nil {
		return cameraError[err]
	} else {
		return nil
	}
}

func (c Handle) CaptureFrame() (image.Image, error) {
	panic("TODO")
	return nil, nil
}

func (c Handle) Stop() error {
	err := C.camera_capture_stop(c.c)
	if err != nil {
		return cameraError[err]
	} else {
		return nil
	}
}
