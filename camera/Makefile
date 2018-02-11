camera.a: camera.c format.c colorspace.c
	${DOCKER} exec ${BUILD_CONTAINER} /bin/bash -c "${CC} -c $^ && ${AR} rcs $@ $(^:%.c=%.o)"

camera-go.a: camera.go format.go errors.go camera.a
	${DOCKER} run -v ${PWD}:${BUILD_DIR} -w ${BUILD_DIR} --rm ${BUILD_IMG} go build -o $@ -ldflags '-l:camera.a'

cameraCapture10: main.c
	${DOCKER} run -v ${PWD}:${BUILD_DIR} -w ${BUILD_DIR} --rm ${BUILD_IMG} ${CC} -o $@ -static $^ -I./libjpeg-turbo -L./libjpeg-turbo/build/.libs -l:libturbojpeg.a -l:camera.a
