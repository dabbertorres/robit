package camera

import (
	"context"
	"errors"
	"os"
	"unsafe"

	"robit/camera/ioctl"
	"robit/camera/v4l2"

	"golang.org/x/sys/unix"
)

type Camera struct {
	file          *os.File
	buffers       [][]byte
	streamBufType v4l2.BufType
	streamMemType v4l2.Memory
	Capabilities  v4l2.Capability
}

func OpenCamera(path string) (cam *Camera, err error) {
	cam = new(Camera)

	cam.file, err = os.OpenFile(path, os.O_RDWR, 0666)
	if err != nil {
		return
	}
	defer func() {
		if err != nil {
			cam.file.Close()
		}
	}()

	err = ioctl.Ioctl(cam.file.Fd(), v4l2.QueryCapCode(), unsafe.Pointer(&cam.Capabilities))
	if err != nil {
		return
	}

	if cam.Capabilities.Capabilities&v4l2.CapVideoCapture == 0 {
		err = errors.New("device doesn't support video capture")
		return
	}

	if cam.Capabilities.Capabilities&v4l2.CapStreaming == 0 {
		err = errors.New("device doesn't support streaming")
		return
	}

	return
}

func (c *Camera) Close() error {
	return c.file.Close()
}

func (c *Camera) GetFormat(bufType v4l2.BufType) (fmt v4l2.Format, err error) {
	fmt.Type = bufType

	err = ioctl.Ioctl(c.file.Fd(), v4l2.GetFormatCode(), unsafe.Pointer(&fmt))
	if err != nil {
		return
	}

	return
}

func (c *Camera) SetFormat(fmt *v4l2.Format) (err error) {
	err = ioctl.Ioctl(c.file.Fd(), v4l2.SetFormatCode(), unsafe.Pointer(fmt))
	if err != nil {
		return
	}

	return
}

func (c *Camera) SupportedFormats(bufType v4l2.BufType) formatIter {
	return formatIter{
		Fmt: v4l2.FormatDesc{
			Index: 0,
			Type:  bufType,
		},
		fd:  c.file.Fd(),
		err: nil,
	}
}

type formatIter struct {
	Fmt v4l2.FormatDesc
	fd  uintptr
	err error
}

func (it *formatIter) Next() bool {
	it.err = ioctl.Ioctl(it.fd, v4l2.EnumFormatCode(), unsafe.Pointer(&it.Fmt))
	if it.err != nil {
		if it.err == unix.EINVAL {
			it.err = nil
		}
		return false
	} else {
		it.Fmt.Index++
		return true
	}
}

func (it *formatIter) Err() error {
	return it.err
}

func (c *Camera) Init(req *v4l2.RequestBuffers) error {
	err := ioctl.Ioctl(c.file.Fd(), v4l2.RequestBufsCode(), unsafe.Pointer(req))
	if err != nil {
		println("requestbufs")
		return err
	}

	c.buffers = make([][]byte, req.Count)

	for i := 0; i < len(c.buffers); i++ {
		buf := v4l2.Buffer{
			Type:   req.Type,
			Memory: req.Memory,
			Index:  uint32(i),
		}

		err = ioctl.Ioctl(c.file.Fd(), v4l2.QueryBufCode(), unsafe.Pointer(&buf))
		if err != nil {
			println("querybuf")
			return err
		}

		var err error
		c.buffers[i], err = unix.Mmap(int(c.file.Fd()), int64(buf.Offset()), int(buf.Length), unix.PROT_READ|unix.PROT_WRITE, unix.MAP_SHARED)
		if err != nil {
			println("mmap")
			return err
		}
	}

	return nil
}

func (c *Camera) Deinit() {
	for _, b := range c.buffers {
		unix.Munmap(b)
	}
}

func (c *Camera) Start() (context.CancelFunc, <-chan error, error) {
	for i := 0; i < len(c.buffers); i++ {
		buf := v4l2.Buffer{
			Type:   c.streamBufType,
			Memory: c.streamMemType,
			Index:  uint32(i),
		}

		err := ioctl.Ioctl(c.file.Fd(), v4l2.QueueBufCode(), unsafe.Pointer(&buf))
		if err != nil {
			return nil, nil, err
		}
	}

	err := ioctl.Ioctl(c.file.Fd(), v4l2.StreamOnCode(), unsafe.Pointer(&c.streamBufType))
	if err != nil {
		return nil, nil, err
	}

	ctx, cancel := context.WithCancel(context.Background())
	errC := make(chan error, 2)
	go c.captureLoop(ctx, cancel, errC)

	return cancel, errC, nil
}

func (c *Camera) stop() error {
	return ioctl.Ioctl(c.file.Fd(), v4l2.StreamOffCode(), unsafe.Pointer(&c.streamBufType))
}

func (c *Camera) captureLoop(ctx context.Context, cancel context.CancelFunc, errC chan<- error) {
	fd := c.file.Fd()

	// arbitrary amount
	timeout := unix.Timeval{
		Sec:  2,
		Usec: 0,
	}
	read := unix.FdSet{}
	fdSet(fd, &read)

	defer cancel()
	defer c.stop()

	imageQueued := make(chan struct{}, 2)
	selectFailed := make(chan error, 2)

	go func() {
		n, err := unix.Select(int(fd+1), &read, nil, nil, &timeout)
		if err != nil || n < 1 {
			selectFailed <- err
			return
		}
	}()

	buf := v4l2.Buffer{
		Type:   c.streamBufType,
		Memory: c.streamMemType,
	}

	for {
		select {
		case <-ctx.Done():
			return

		case err := <-selectFailed:
			errC <- err
			return

		case <-imageQueued:
			err := ioctl.Ioctl(fd, v4l2.DequeueBufCode(), unsafe.Pointer(&buf))
			if err != nil && err != unix.EAGAIN {
				errC <- err
				return
			}

			// TODO: what do with buffer

			err = ioctl.Ioctl(fd, v4l2.QueueBufCode(), unsafe.Pointer(&buf))
			if err != nil && err != unix.EAGAIN {
				errC <- err
				return
			}
		}
	}
}

func fdSet(fd uintptr, set *unix.FdSet) {
	const nfdBits = 64
	set.Bits[fd/nfdBits] |= 1 << fd % nfdBits
}
