package main

/*
#cgo CFLAGS: -I../../robit
#include "input.h"
*/
import "C"

import (
	"io/ioutil"
	"log"
	"net"
	"net/http"

	ws "github.com/gorilla/websocket"
)

type ControlMessage struct {
	Input string `json:"input"`
	Press bool   `json:"press"`
}

func main() {
	var (
		upgrader = ws.Upgrader{}
		srv      = &http.Server{
			Addr: ":http",
		}
	)

	inputSock, err := net.Dial("unix", "/tmp/robit.input")
	if err != nil {
		panic(err)
	}
	defer inputSock.Close()

	index, err := ioutil.ReadFile("index.html")
	if err != nil {
		panic(err)
	}

	script, err := ioutil.ReadFile("main.js")
	if err != nil {
		panic(err)
	}

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) { w.Write(index) })
	http.HandleFunc("/main.js", func(w http.ResponseWriter, r *http.Request) { w.Write(script) })
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, r.Header)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			log.Println("websocket upgrade failed:", err)
			return
		}
		defer conn.Close()

		msg := ControlMessage{}
		rawMsg := make([]byte, 2)

		for {
			err := conn.ReadJSON(&msg)
			if err != nil {
				log.Println("error reading websocket json message:", err)
				continue
			}

			switch msg.Input {
			case "up":
				rawMsg[0] = C.INPUT_UP

			case "left":
				rawMsg[0] = C.INPUT_LEFT

			case "right":
				rawMsg[0] = C.INPUT_RIGHT

			case "down":
				rawMsg[0] = C.INPUT_DOWN

			case "stop":
				rawMsg[0] = C.INPUT_STOP
			}

			if msg.Press {
				rawMsg[1] = C.INPUT_PRESS
			} else {
				rawMsg[1] = C.INPUT_RELEASE
			}

			inputSock.Write(rawMsg)
		}
	})

	log.Fatal(srv.ListenAndServe())
}
