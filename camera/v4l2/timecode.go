package v4l2

type (
	TimecodeType uint32
	TimecodeFlag uint32
)

type Timecode struct {
	Type     TimecodeType
	Flags    TimecodeFlag
	Frames   uint8
	Seconds  uint8
	Minutes  uint8
	Hours    uint8
	Userbits [4]uint8
}

const (
	TimecodeType24fps TimecodeType = iota + 1
	TimecodeType25fps
	TimecodeType30fps
	TimecodeType50fps
	TimecodeType60fps
)

const (
	TimecodeFlagDropFrame           TimecodeFlag = 0x0001
	TimecodeFlagColorFrame          TimecodeFlag = 0x0002
	TimecodeFlagUserbitsField       TimecodeFlag = 0x000c
	TimecodeFlagUserbitsUserDefined TimecodeFlag = 0x0000
	TimecodeFlagUserbits8BitChars   TimecodeFlag = 0x0008
)
