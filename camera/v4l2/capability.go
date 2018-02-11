package v4l2

type CapabilityFlag uint32

type Capability struct {
	Driver       [16]uint8
	Card         [32]uint8
	BusInfo      [32]uint8
	Version      uint32
	Capabilities CapabilityFlag
	DeviceCaps   CapabilityFlag
	_            [3]uint32
}

const (
	CapVideoCapture CapabilityFlag = 0x00000001
	CapVideoOutput  CapabilityFlag = 0x00000002
	CapReadWrite    CapabilityFlag = 0x01000000
	CapAsyncIO      CapabilityFlag = 0x02000000
	CapStreaming    CapabilityFlag = 0x04000000
	CapDeviceCaps   CapabilityFlag = 0x80000000
)
