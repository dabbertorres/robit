package v4l2

import (
	"unsafe"

	"golang.org/x/sys/unix"
)

type (
	BufType uint32
	BufFlag uint32
	Field   uint32
	Memory  uint32
)

type Buffer struct {
	Index     uint32
	Type      BufType
	BytesUsed uint32
	Flags     BufFlag
	Field     Field
	Timestamp unix.Timeval
	Timecode  Timecode
	Sequence  uint32
	Memory    Memory
	m         [4]byte
	Length    uint32
	_         uint32
	_         uint32
}

func (b *Buffer) Offset() uint32 {
	return *(*uint32)(unsafe.Pointer(&b.m[0]))
}

func (b *Buffer) UserPtr() uintptr {
	return *(*uintptr)(unsafe.Pointer(&b.m[0]))
}

func (b *Buffer) Planes() []Plane {
	return (*(*[1 << 16]Plane)(unsafe.Pointer(&b.m[0])))[:b.Length]
}

func (b *Buffer) Fd() int32 {
	return *(*int32)(unsafe.Pointer(&b.m[0]))
}

type RequestBuffers struct {
	Count  uint32
	Type   BufType
	Memory Memory
	_      [2]uint32
}

const (
	BufTypeVideoCapture BufType = iota + 1
	BufTypeVideoOutput
	BufTypeVideoOverlay
	BufTypeVbiCapture
	BufTypeVbiOutput
	BufTypeSlicedVbiCapture
	BufTypeSlicedVbiOutput
	BufTypeVideoOutputOverlay
	BufTypeVideoCaptureMPlane
	BufTypeVideoOutputMPlane
	BufTypeSdrCapture
	BufTypeSdrOutput
	BufTypeMetaCapture
)

const (
	BufFlagMapped BufFlag = 1 << iota
	BufFlagQueued
	BufFlagDone
	BufFlagKeyframe
	BufFlagPFrame
	BufFlagBFrame
	BufFlagError
	_
	BufFlagTimecode
	_
	BufFlagPrepared
	BufFlagNoCacheInvalidate
	BufFlagNoCacheClean

	BufFlagTimestampMask      BufFlag = 0x0000e000
	BufFlagTimestampUnknown   BufFlag = 0
	BufFlagTimestampMonotonic BufFlag = 0x00002000
	BufFlagTimestampCopy      BufFlag = 0x00004000
	BufFlagTStampSrcMask      BufFlag = 0x00070000
	BufFlagTStampSrcEof       BufFlag = 0
	BufFlagTStampSrcSoe       BufFlag = 0x00010000
)

const (
	FieldAny Field = iota
	FieldNone
	FieldTop
	FieldBottom
	FieldInterlaced
	FieldSeqTB
	FieldSeqBT
	FieldAlternate
	FieldInterlacedTB
	FieldInterlacedBT
)

const (
	MemoryMMap Memory = iota + 1
	MemoryUserPtr
	MemoryOverlay
	MemoryDMABuf
)
