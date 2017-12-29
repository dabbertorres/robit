package main

import (
	"bytes"
	"html/template"
	"log"
	"net/http"
	"os"
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
	router := mux.NewRouter()
	router.Path("/watch").Methods("GET").HandlerFunc(watchHandler)
	router.Path("/watch/dash").Methods("GET").HandlerFunc(watchDashHandler)
	router.Path("/watch/hls").Methods("GET").HandlerFunc(watchHlsHandler)
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

	// TODO open (socket?) video stream locally, then use to provide bytes to requests to /watch/{type}

	if err := srv.ListenAndServe(); err != nil {
		log.Println("ListenAndServe():", err)
		os.Exit(1)
	}
}

var watchTmpl = template.Must(template.New("watch").Parse(`<!doctype html>
<html>
<head>
	<meta charset="utf-8">
</head>
<body>
	<video autoplay width="720">
		<source src="/watch/{{ . }}" type="video/mp4">
		<p>Your browser doesn't support HTML5 video. Get yourself a better browser, kid:
			<ul>
				<li><a href="https://www.google.com/chrome/browser/desktop/index.html">Google Chrome</a></li>
				<li><a href="https://www.mozilla.org/en-US/firefox/">Firefox</a></li>
			</ul>
		</p>
	</video>
</body>
</html>`))

func watchHandler(w http.ResponseWriter, r *http.Request) {
	var buf bytes.Buffer

	if strings.Contains(r.UserAgent(), "Safari") {
		watchTmpl.Execute(&buf, "hls")
	} else {
		watchTmpl.Execute(&buf, "dash")
	}

	w.Write(buf.Bytes())
}

func watchDashHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Accept-Ranges", "bytes")
	w.WriteHeader(http.StatusPartialContent)
}

func watchHlsHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Accept-Ranges", "bytes")
	w.WriteHeader(http.StatusPartialContent)
}

func controlRequestHandler(w http.ResponseWriter, r *http.Request) {
	controlledLock.Lock()
	if alreadyControlled {
		controlledLock.Unlock()

		w.WriteHeader(http.StatusLocked)

		var buf bytes.Buffer
		buf.WriteString(`<!doctype html>
<html>
<head>
	<meta charset="utf-8">
</head>
<body>
	<p>Someone else is already controlling the robot! Try again later. <a href="/watch">Watch</a> live in the meantime?</p>
</body>
</html>`)

		w.Write(buf.Bytes())
		return
	}

	alreadyControlled = true
	controlledLock.Unlock()

	w.WriteHeader(http.StatusTemporaryRedirect)
	w.Header().Add("Location", "/control/run")
}

func controlRunHandler(w http.ResponseWriter, r *http.Request) {
	var buf bytes.Buffer

	buf.WriteString(`<!doctype html>
<html>
<head>
	<meta charset="utf-8">
	<script>
		let ws = new WebSocket("/control/ws");
		
		window.addEventListener("unload", function(event) {
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
		});
	</script>
</head>
<body>
	<video autoplay width="720">
		<source src="/watch/{{ . }}" type="video/mp4">
		<p>Your browser doesn't support HTML5 video. Get yourself a better browser, kid:
			<ul>
				<li><a href="https://www.google.com/chrome/browser/desktop/index.html">Google Chrome</a></li>
				<li><a href="https://www.mozilla.org/en-US/firefox/">Firefox</a></li>
			</ul>
		</p>
	</video>
</body>
</html>`)

	w.Write(buf.Bytes())
}

func controlSocketHandler(w http.ResponseWriter, r *http.Request) {
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
