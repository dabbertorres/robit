package main

import (
	"context"
	"fmt"
	"os/exec"
)

func PrepFFmpeg(ctx context.Context, framerate, width, height uint, camera string, socket string) *exec.Cmd {
	return exec.CommandContext(ctx, "ffmpeg", "-f v4l2",
		fmt.Sprint("-framerate", framerate),
		fmt.Sprintf("-video_size %dx%d", width, height),
		fmt.Sprint("-i", camera),
		socket)
}
