package main

import (
	"bytes"
	"fmt"
	"image"
	"image/jpeg"
	"log"
	"mime/multipart"
	"net/http"
	"net/textproto"
	"os"
	"os/exec"
	"strings"
	"sync"
	"time"

	"github.com/gorilla/mux"
	"github.com/gorilla/websocket"
)

var (
	alreadyControlled = false
	controlledLock    sync.Mutex
)

func main() {
	var (
		viewers   = make([]chan<- image.Image, 0, 64)
		newViewer = make(chan chan<- image.Image, 8)
		byeViewer = make(chan chan<- image.Image, 8)
	)

	_, err := exec.Lookup("ffmpeg")
	if err != nil {
		panic(err)
	}

	// TODO open (socket?) video stream locally, then use to provide bytes to requests to /watch/{type}

	router := mux.NewRouter()
	router.Path("/watch").Methods("GET").HandlerFunc(watchHandler)
	router.Path("/control").Methods("GET").HandlerFunc(controlRequestHandler)
	router.Path("/control/run").Methods("GET").HandlerFunc(controlRunHandler)
	router.Path("/control/ws").Methods("GET").HandlerFunc(controlSocketHandler)

	srv := http.Server{
		Addr:           ":8080",
		ReadTimeout:    15 * time.Second,
		WriteTimeout:   15 * time.Second,
		MaxHeaderBytes: 8192,
		Handler:        router,
	}

	go func() {
		if err := srv.ListenAndServe(); err != nil {
			log.Println("ListenAndServe():", err)
			os.Exit(1)
		}
	}()

	for {
		select {
		case nv := <-newViewer:
			viewers = append(viewers, nv)

		case bv := <-byeViewer:
			for i, v := range viewers {
				if v == bv {
					last := len(viewers) - 1
					viewers[last], viewers[i] = viewers[i], viewers[last]
					close(viewers[last])
					viewers = viewers[:last]
					break
				}
			}
		}
	}
}

var watchPage = Page{
	Head: PageHead{
		Title: "Robit: Watch",
	},
	Body: PageBody{
		Header: []Element{
			{
				Type:    "h1",
				Content: "Watch",
			},
		},
		Main: []Element{
			{
				Type: "img",
				Attributes: []Attribute{
					{"src", "/watch/mjpeg"},
					{"width", "720"},
					{"alt", "Unable to display the stream!"},
				},
				Empty: true,
			},
		},
		Footer: []Element{
			/*{
				Type: "a",
				Attributes: []Attribute{
					{"href", "/control"},
				},
				Content: "Take control?",
			},*/
		},
	},
}

func watchHandler(w http.ResponseWriter, r *http.Request) {
	err := watchPage.Build(w)
	if err != nil {
		log.Println("Error building /watch:", err)
		w.WriteHeader(http.StatusInternalServerError)
	}
}

var alreadyControlledPage = Page{
	Head: PageHead{
		Title: "Robot: Cannot Control",
	},
	Body: PageBody{
		Header: []Element{
			{
				Type:    "h1",
				Content: "Robot is already being controlled!",
			},
		},
		Main: []Element{
			{
				Type: "p",
				Children: []Element{
					{
						Type:    "div",
						Content: "Try again later!",
					},
					{
						Type: "a",
						Attributes: []Attribute{
							{"href", "/watch"},
						},
						Content: "Watch in the meantime?",
					},
				},
			},
		},
	},
}

func controlRequestHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusNotImplemented)
	return

	controlledLock.Lock()
	if alreadyControlled {
		controlledLock.Unlock()

		err := alreadyControlledPage.Build(w)
		if err != nil {
			log.Println("Error building /control already controlled:", err)
			w.WriteHeader(http.StatusInternalServerError)
		}

		return
	}

	alreadyControlled = true
	controlledLock.Unlock()

	w.WriteHeader(http.StatusTemporaryRedirect)
	w.Header().Add("Location", "/control/run")
}

var controlRunPage = Page{
	Head: PageHead{
		Title: "Robit: Control!",
		Scripts: []string{
			`let ws = new WebSocket("/control/ws");

			window.addEventListener("beforeunload", function(event) {
				ws.close();
			});

			document.addEventListener("keydown", function(event)
			{
				if(event.repeat) return;

				switch(event.code)
				{
					case "KeyS":
					case "ArrowDown":
						ws.send("down:start");
						break;

					case "KeyW":
					case "ArrowUp":
						ws.send("up:start");
						break;

					case "KeyA":
					case "ArrowLeft":
						ws.send("left:start");
						break;

					case "KeyD":
					case "ArrowRight":
						ws.send("right:start");
						break;
				}

				event.preventDefault();
			});

			document.addEventListener("keyup", function(event)
			{
				if(event.repeat) return;

				switch(event.code)
				{
					case "KeyS":
					case "ArrowDown":
						ws.send("down:end");
						break;

					case "KeyW":
					case "ArrowUp":
						ws.send("up:end");
						break;

					case "KeyA":
					case "ArrowLeft":
						ws.send("left:end");
						break;

					case "KeyD":
					case "ArrowRight":
						ws.send("right:end");
						break;
				}

				event.preventDefault();
			});`,
		},
	},
	Body: PageBody{
		Header: []Element{
			{
				Type:    "h1",
				Content: "Control!",
			},
		},
		Main: []Element{
			{
				Type: "img",
				Attributes: []Attribute{
					{"src", "/watch/mjpeg"},
					{"width", "720"},
					{"alt", "Unable to display the stream!"},
				},
				Empty: true,
			},
		},
		Footer: []Element{
			{
				Type: "a",
				Attributes: []Attribute{
					{"href", "/watch"},
				},
				Content: "Just watch?",
			},
		},
	},
}

func controlRunHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusNotImplemented)
	return

	err := controlRunPage.Build(w)
	if err != nil {
		log.Println("Error building /control/run:", err)
		w.WriteHeader(http.StatusInternalServerError)
	}
}

func controlSocketHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusNotImplemented)
	return

	if !websocket.IsWebSocketUpgrade(r) {
		w.WriteHeader(http.StatusUpgradeRequired)
		return
	}

	up := websocket.Upgrader{
		HandshakeTimeout: 15 * time.Second,
	}

	ws, err := up.Upgrade(w, r, nil)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer ws.Close()

	for {
		msgType, msg, err := ws.ReadMessage()
		if err != nil {
			log.Println("websocket.ReadMessage():", err)
			return
		}

		if msgType == websocket.CloseMessage {
			controlledLock.Lock()
			alreadyControlled = false
			controlledLock.Unlock()
			break
		}

		inputs := strings.Split(string(msg), ":")

		if len(inputs) < 2 {
			log.Println("Ill-formed control message receieved:", msg)
			continue
		}

		// TODO start := inputs[1] == "start"

		// TODO send socket message to robot program for each case
		switch inputs[0] {
		case "up":

		case "down":

		case "left":

		case "right":
		}
	}
}
