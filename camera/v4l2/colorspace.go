package v4l2

type Colorspace uint32

const (
	// can be used to let driver pick a colorspace
	ColorspaceDefault Colorspace = iota
	
	// SMPTE 170M (NTSC, PAL, SDTV)
	ColorspaceSMPTE170M
	
	// SMPTE 240M (early HDTV, ie: 1988-1998)
	ColorspaceSMPTE240M

	// ITU BT.709 (HDTV)
	ColorspaceREC709
	
	// Never used, not even by drivers.
	ColorspaceBT878
	
	// NTSC 1953 (obsolete, use SMPTE 170M)
	Colorspace470_SYSTEM_M
	
	// EBU Tech 3213 (PAL/SECAM in 1975, obsolete, use SMPTE 170M)
	Colorspace470_SYSTEM_BG
	
	// (Motion-)JPEG formats - effectively identical to sRGB
	ColorspaceJPEG

	// sRGB (most webcams, computer graphics, etc)
	ColorspaceSRGB

	// AdobeRGB (opRGB)
	ColorspaceADOBERGB

	// ITU BT.2020 (UHDTV)
	ColorspaceBT2020

	// minimal processing - software using this will have to know capture device internals
	ColorspaceRAW
	
	// SMPTE RP 431-2 (cinema projectors)
	ColorspaceDCI_P3
)
