package ioctl

import (
	"reflect"
	"unsafe"

	"golang.org/x/sys/unix"
)

const (
	nrBits   = 8
	typeBits = 8
	sizeBits = 14
	dirBits  = 2

	nrMask   = (1 << nrBits) - 1
	typeMask = (1 << typeBits) - 1
	sizeMask = (1 << sizeBits) - 1
	dirMask  = (1 << dirBits) - 1

	nrShift   = 0
	typeShift = nrShift + nrBits
	sizeShift = typeShift + typeBits
	dirShift  = sizeShift + sizeBits

	none  = 0
	write = 1
	read  = 2
)

type Code uintptr

func ioc(dir, _type, nr, size uintptr) Code {
	return Code((dir << dirShift) | (_type << typeShift) | (nr << nrShift) | (size << sizeShift))
}

func Io(_type, nr uintptr) Code                      { return ioc(none, _type, nr, 0) }
func Ior(_type, nr uintptr, size reflect.Type) Code  { return ioc(read, _type, nr, size.Size()) }
func Iow(_type, nr uintptr, size reflect.Type) Code  { return ioc(write, _type, nr, size.Size()) }
func Iowr(_type, nr uintptr, size reflect.Type) Code { return ioc(read|write, _type, nr, size.Size()) }

func Dir(nr Code) uintptr  { return (uintptr(nr) >> dirShift) & dirMask }
func Type(nr Code) uintptr { return (uintptr(nr) >> typeShift) & typeMask }
func Nr(nr Code) uintptr   { return (uintptr(nr) >> nrShift) & nrMask }
func Size(nr Code) uintptr { return (uintptr(nr) >> sizeShift) & sizeMask }

func Ioctl(fd uintptr, req Code, buf unsafe.Pointer) (err error) {
	_, _, errno := unix.Syscall(unix.SYS_IOCTL, fd, uintptr(req), uintptr(buf))
	if errno != 0 {
		err = errno
	}
	return
}
