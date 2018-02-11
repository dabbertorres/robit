package v4l2

type PixFormat struct {
	Width        uint32
	Height       uint32
	PixelFormat  FourCC
	Field        Field
	BytesPerLine uint32
	SizeImage    uint32
	Colorspace   Colorspace
	Priv         uint32
	flags        uint32
	enc          uint32
	Quantization Quantization
	TransferFunc TransferFunc
}

func (f *PixFormat) PreMulAlpha() bool {
	return f.flags&PixFormatFlagPreMulAlpha != 0
}

func (f *PixFormat) YcbcrEnc() YcbcrEncoding {
	return YcbcrEncoding(f.enc)
}

func (f *PixFormat) HsvEnc() HsvEncoding {
	return HsvEncoding(f.enc)
}

type PlanePixFormat struct {
	SizeImage    uint32
	BytesPerLine uint32
	_            [6]uint16
}

type PixFormatMPlane struct {
	Width        uint32
	Height       uint32
	PixelFormat  FourCC
	Field        Field
	Colorspace   Colorspace
	planeFmt     [maxPlanes]PlanePixFormat
	NumPlanes    uint8
	flags        uint8
	enc          uint8
	Quantization Quantization
	TransferFunc TransferFunc
	_            [7]uint8
}

func (f *PixFormatMPlane) PlaneFormats() []PlanePixFormat {
	return f.planeFmt[:f.NumPlanes]
}

func (f *PixFormatMPlane) PreMulAlpha() bool {
	return f.flags&uint8(PixFormatFlagPreMulAlpha) != 0
}

func (f *PixFormatMPlane) YcbcrEnc() YcbcrEncoding {
	return YcbcrEncoding(f.enc)
}

func (f *PixFormatMPlane) HsvEnc() HsvEncoding {
	return HsvEncoding(f.enc)
}

const (
	maxPlanes = 8

	pixFmtPrivMagic uint32 = 0xfeedcafe

	PixFormatFlagPreMulAlpha uint32 = 0x00000001
)
