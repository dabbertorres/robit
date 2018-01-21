CC := rpi3-cc

DOCKER ?= docker

BUILD_DIR ?= /rpi3-build

TOOLCHAIN_IMG := dabbertorres/rpi3-gcc

SRC := camera.c main.c format.c colorspace.c

I_DIRS := -I./libjpeg-turbo

L_DIRS := -L./libjpeg-turbo/build/.libs

LIBS := -l:libturbojpeg.a

cameraCapture10: ${SRC}
	${DOCKER} run -v ${PWD}:${BUILD_DIR} -w ${BUILD_DIR} --rm ${TOOLCHAIN_IMG} ${CC} -o $@ -static $^ ${I_DIRS} ${L_DIRS} ${LIBS}

