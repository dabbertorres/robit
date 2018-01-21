CC := rpi3-cc

DOCKER ?= docker

BUILD_DIR ?= /rpi3-build

TOOLCHAIN_IMG := dabbertorres/rpi3-gcc

all: sockSrv robit

sockSrv: main.c
	${DOCKER} run -v ${PWD}:${BUILD_DIR} -w ${BUILD_DIR} --rm ${TOOLCHAIN_IMG} ${CC} -o $@ -static $^

robit: main.go
	GOOS=linux GOARCH=arm go build -o robit

