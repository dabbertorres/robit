package v4l2

import (
	"reflect"

	"github.com/dabbertorres/robit/camera/ioctl"
)

func QueryCapCode() ioctl.Code { return ioctl.Ior('V', 0, reflect.TypeOf(Capability{})) }
func EnumFormatCode() ioctl.Code { return ioctl.Iowr('V', 2, reflect.TypeOf(FormatDesc{})) }
func GetFormatCode() ioctl.Code { return ioctl.Iowr('V', 4, reflect.TypeOf(Format{})) }
func SetFormatCode() ioctl.Code { return ioctl.Iowr('V', 5, reflect.TypeOf(Format{})) }
func RequestBufsCode() ioctl.Code { return ioctl.Iowr('V', 8, reflect.TypeOf(RequestBuffers{})) }
func QueryBufCode() ioctl.Code    { return ioctl.Iowr('V', 9, reflect.TypeOf(Buffer{})) }
func QueueBufCode() ioctl.Code    { return ioctl.Iowr('V', 15, reflect.TypeOf(Buffer{})) }
func DequeueBufCode() ioctl.Code  { return ioctl.Iowr('V', 17, reflect.TypeOf(Buffer{})) }
func StreamOnCode() ioctl.Code    { return ioctl.Iow('V', 18, reflect.TypeOf(BufType(0))) }
func StreamOffCode() ioctl.Code   { return ioctl.Iow('V', 19, reflect.TypeOf(BufType(0))) }
