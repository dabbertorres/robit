package camera

import (
	"testing"
	"time"

	"robit/camera/v4l2"
)

func Test(t *testing.T) {
	cam, err := OpenCamera("/dev/video0")
	if err != nil {
		t.Fatal(err)
	}
	defer cam.Close()

	formats := cam.SupportedFormats(v4l2.BufTypeVideoCapture)
	t.Log("Formats:")
	for formats.Next() {
		t.Log(formats.Fmt.String())
	}

	if formats.Err() != nil {
		t.Error(formats.Err())
		return
	}

	currFmt, err := cam.GetFormat(v4l2.BufTypeVideoCapture)
	if err != nil {
		t.Error(err)
		return
	}
	t.Log("Current format:", currFmt.String())

	reqBufs := v4l2.RequestBuffers{
		Type:   v4l2.BufTypeVideoCapture,
		Memory: v4l2.MemoryMMap,
		Count:  2,
	}
	err = cam.Init(&reqBufs)
	if err != nil {
		t.Error(err)
		return
	}
	defer cam.Deinit()

	cancel, errC, err := cam.Start()
	if err != nil {
		t.Error(err)
		return
	}
	defer cancel()

	time.Sleep(10 * time.Second)

	if err, ok := <-errC; ok && err != nil {
		t.Error(err)
	}
}
