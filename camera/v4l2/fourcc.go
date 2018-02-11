package v4l2

type FourCC uint32

func fourCC(cc string) FourCC {
	return FourCC(uint32(cc[0]) | uint32(cc[1])<<8 | uint32(cc[2])<<16 | uint32(cc[3])<<24)
}
func fourCC_BE(cc string) FourCC { return fourCC(cc) | 1<<31 }

// comments describe bit layout of one pixel
// r: red
// g: green
// b: blue
// a: alpha
// _: unused

/* RGB formats */

func PixelFormatRGB332() FourCC   { return fourCC("RGB1") }    // rrrgggbb
func PixelFormatARGB444() FourCC  { return fourCC("AR12") }    // ggggbbbb aaaarrrr
func PixelFormatXRGB444() FourCC  { return fourCC("XR12") }    // ggggbbbb ____rrrr
func PixelFormatARGB555() FourCC  { return fourCC("AR15") }    // gggbbbbb arrrrrgg
func PixelFormatXRGB555() FourCC  { return fourCC("XR15") }    // gggbbbbb _rrrrrgg
func PixelFormatRGB565() FourCC   { return fourCC("RGBP") }    // gggbbbbb rrrrrggg
func PixelFormatARGB555X() FourCC { return fourCC_BE("AR15") } // arrrrrgg gggbbbbb
func PixelFormatXRGB555X() FourCC { return fourCC_BE("XR15") } // _rrrrrgg gggbbbbb
func PixelFormatRGB565X() FourCC  { return fourCC("RGBR") }    // rrrrrggg gggbbbbb
func PixelFormatBGR666() FourCC   { return fourCC("BGRH") }    // bbbbbbgg ggggrrrr rr______ ________
func PixelFormatBGR24() FourCC    { return fourCC("BGR3") }    // bbbbbbbb gggggggg rrrrrrrr
func PixelFormatRGB24() FourCC    { return fourCC("RGB3") }    // rrrrrrrr gggggggg bbbbbbbb
func PixelFormatABGR32() FourCC   { return fourCC("AR24") }    // bbbbbbbb gggggggg rrrrrrrr aaaaaaaa
func PixelFormatXBGR32() FourCC   { return fourCC("XR24") }    // bbbbbbbb gggggggg rrrrrrrr ________
func PixelFormatARGB32() FourCC   { return fourCC("BA24") }    // aaaaaaaa rrrrrrrr gggggggg bbbbbbbb
func PixelFormatXRGB32() FourCC   { return fourCC("BX24") }    // ________ rrrrrrrr gggggggg bbbbbbbb

/* deprecated RGB formats */

func PixelFormatRGB444() FourCC  { return fourCC("R444") } // ggggbbbb aaaarrrr
func PixelFormatRGB555() FourCC  { return fourCC("RGBO") } // gggbbbbb arrrrrgg
func PixelFormatRGB555X() FourCC { return fourCC("RGBQ") } // arrrrrgg gggbbbbb
func PixelFormatBGR32() FourCC   { return fourCC("BGR4") } // bbbbbbbb gggggggg rrrrrrrr aaaaaaaa
func PixelFormatRGB32() FourCC   { return fourCC("RGB4") } // aaaaaaaa rrrrrrrr gggggggg aaaaaaaa

/* Grey formats */

func PixelFormatGrey() FourCC     { return fourCC("GREY") }
func PixelFormatY4() FourCC       { return fourCC("Y04 ") }
func PixelFormatY6() FourCC       { return fourCC("Y06 ") }
func PixelFormatY10() FourCC      { return fourCC("Y10 ") }
func PixelFormatY12() FourCC      { return fourCC("Y12 ") }
func PixelFormatY16() FourCC      { return fourCC("Y16 ") }
func PixelFormatY16BE() FourCC    { return fourCC_BE("Y16 ") }
func PixelFormatY10BPack() FourCC { return fourCC("Y10B") }

/* palette formats */

func PixelFormatPAL8() FourCC { return fourCC("PAL8") }

/* chrominance formats */

func PixelFormatUV8() FourCC { return fourCC("UV8 ") }

/* YUYV formats */

/* HSV formats */

/* depth formats */

/* compressed formats */

/* reserved formats */

func (cc FourCC) String() string {
	switch cc {
	case PixelFormatRGB332():
		return "RGB1"
	case PixelFormatARGB444():
		return "AR12"
	case PixelFormatXRGB444():
		return "XR12"
	case PixelFormatARGB555():
		return "AR15"
	case PixelFormatXRGB555():
		return "XR15"
	case PixelFormatRGB565():
		return "RGBP"
	case PixelFormatARGB555X():
		return "AR15"
	case PixelFormatXRGB555X():
		return "XR15"
	case PixelFormatRGB565X():
		return "RGBR"
	case PixelFormatBGR666():
		return "RGBH"
	case PixelFormatBGR24():
		return "BGR3"
	case PixelFormatRGB24():
		return "RGB3"
	case PixelFormatABGR32():
		return "AR24"
	case PixelFormatXBGR32():
		return "XR24"
	case PixelFormatARGB32():
		return "BA24"
	case PixelFormatXRGB32():
		return "BX24"
	case PixelFormatRGB444():
		return "R444"
	case PixelFormatRGB555():
		return "RGBO"
	case PixelFormatRGB555X():
		return "RGBQ"
	case PixelFormatBGR32():
		return "BGR4"
	case PixelFormatRGB32():
		return "RGB4"
	case PixelFormatGrey():
		return "GREY"
	case PixelFormatY4():
		return "Y04"
	case PixelFormatY6():
		return "Y06"
	case PixelFormatY10():
		return "Y10"
	case PixelFormatY12():
		return "Y12"
	case PixelFormatY16():
		return "Y16"
	case PixelFormatY16BE():
		return "Y16"
	case PixelFormatY10BPack():
		return "Y10B"
	case PixelFormatPAL8():
		return "PAL8"
	case PixelFormatUV8():
		return "UV8"
	default:
		return "unknown"
	}
}
