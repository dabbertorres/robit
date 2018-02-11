package v4l2

import (
	"bytes"
	"fmt"
	"unsafe"
)

type (
	formatDescFlag uint32
	YcbcrEncoding  uint32
	HsvEncoding    uint32
	Quantization   uint32
	TransferFunc   uint32
)

type Format struct {
	Type    BufType
	RawData [200]uint8
}

func (f *Format) Pix() *PixFormat {
	return (*PixFormat)(unsafe.Pointer(&f.RawData[0]))
}

func (f *Format) PixMPlane() *PixFormatMPlane {
	return (*PixFormatMPlane)(unsafe.Pointer(&f.RawData[0]))
}

type Window struct{}
type VbiFormat struct{}
type SlicedVbiFormat struct{}
type SdrFormat struct{}
type MetaFormat struct{}

func (f *Format) Win() *Window {
	panic("not implemented")
}

func (f *Format) Vbi() *VbiFormat {
	panic("not implemented")
}

func (f *Format) Sliced() *SlicedVbiFormat {
	panic("not implemented")
}

func (f *Format) Sdr() *SdrFormat {
	panic("not implemented")
}

func (f *Format) Meta() *MetaFormat {
	panic("not implemented")
}

func (f *Format) String() string {
	switch f.Type {
	case BufTypeVideoCapture, BufTypeVideoOutput:
		return fmt.Sprintf("Video: %s", f.Pix())
	case BufTypeVideoCaptureMPlane, BufTypeVideoOutputMPlane:
		return fmt.Sprintf("Multi-Plane Video: %s", f.PixMPlane())
	case BufTypeVideoOverlay, BufTypeVideoOutputOverlay:
		return fmt.Sprintf("Video Overlay: %s", f.Win())
	case BufTypeVbiCapture, BufTypeVbiOutput:
		return fmt.Sprintf("VBI: %s", f.Vbi())
	case BufTypeSlicedVbiCapture, BufTypeSlicedVbiOutput:
		return fmt.Sprintf("Sliced VBI: %s", f.Sliced())
	case BufTypeSdrCapture, BufTypeSdrOutput:
		return fmt.Sprintf("SDR: %s", f.Sdr())
	case BufTypeMetaCapture:
		return fmt.Sprintf("Meta: %s", f.Meta())
	default:
		return "unknown format type"
	}
}

type FormatDesc struct {
	Index       uint32
	Type        BufType
	flags       formatDescFlag
	description [32]uint8
	PixelFormat FourCC
	_           [4]uint32
}

func (f *FormatDesc) Compressed() bool {
	return f.flags&formatCompressed != 0
}

func (f *FormatDesc) Emulated() bool {
	return f.flags&formatEmulated != 0
}

func (f *FormatDesc) Description() string {
	nullTerm := bytes.IndexByte(f.description[:], 0)
	if nullTerm == -1 {
		nullTerm = len(f.description)
	}
	return string(f.description[:nullTerm])
}

func (f *FormatDesc) String() string {
	return fmt.Sprintf("%s: %s", f.PixelFormat, f.Description())
}

const (
	formatCompressed formatDescFlag = 0x0001
	formatEmulated   formatDescFlag = 0x0002
)
