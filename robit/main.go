package main

import (
	"net"
)

const (
	rmtCtrlSockName = "/tmp/robit-remote-control.socket"
)

func main() {
	conn, err := net.Dial("unixpacket", rmtCtrlSockName)
	if err != nil {
		panic(err)
	}
	defer conn.Close()

	_, err = conn.Write([]byte{'u', 1})
	if err != nil {
		panic(err)
	}
}
