package v4l2

import "unsafe"

type Plane struct {
	BytesUsed  uint32
	Length     uint32
	m          [8]byte
	DataOffset uint32
	_          [11]uint32
}

func (p *Plane) MemOffset() uint32 {
	return *(*uint32)(unsafe.Pointer(&p.m[0]))
}

func (p *Plane) UserPtr() uintptr {
	return *(*uintptr)(unsafe.Pointer(&p.m[0]))
}

func (p *Plane) Fd() int32 {
	return *(*int32)(unsafe.Pointer(&p.m[0]))
}
