CC := rpi3-cc

SRC := camera.c main.c format.c colorspace.c

I_DIRS := -I/opt/libjpeg-turbo/include

L_DIRS := -L/opt/libjpeg-turbo/lib32

LIBS := -l:libturbojpeg.a

cameraCapture10: ${SRC}
	${CC} -o $@ -static $^ ${I_DIRS} ${L_DIRS} ${LIBS}
