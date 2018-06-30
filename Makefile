CC      := rpi3-cc
AR      := rpi3-ar
CFLAGS  += -Wall -Wextra -O2 -I$(SRC_DIR)
GOFLAGS += GOOS=linux GOARCH=arm GOARM=7

DOCKER := docker

# if we're on windows, correct docker executable name
ifeq ($(OS), Windows_NT)
DOCKER := docker.exe
endif

CONTAINER ?= rpi3_make_build
IMAGE     ?= dabbertorres/rpi3-gcc
SRC_DIR   ?= /gopath/src/robit

OUT_DIR_HOST ?= build
OUT_DIR      := $(SRC_DIR)/$(OUT_DIR_HOST)

EXEC = $(DOCKER) exec -w "$(SRC_DIR)" $(CONTAINER) bash -c 'export CC=$(CC); \
															export AR=$(AR); \
															export CFLAGS="$(CFLAGS)"; \
															export OUT_DIR=$(OUT_DIR); \
															$(1)'

.PHONY: all gpio motor sonar robit camera websrv camera.test websrv.test sonar_test test clean start stop

all: gpio motor sonar robit camera websrv
test: camera.test websrv.test sonar_test

camera:      $(OUT_DIR_HOST)/camera
websrv:      $(OUT_DIR_HOST)/websrv
camera.test: $(OUT_DIR_HOST)/camera.test
websrv.test: $(OUT_DIR_HOST)/websrv.test
sonar_test:  $(OUT_DIR_HOST)/sonar_test

gpio: | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C gpio/)

motor: gpio | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C motor/)

sonar: gpio | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C sonar/)

robit: motor | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C robit/)

$(OUT_DIR_HOST)/camera: | $(OUT_DIR_HOST)
	$(GOFLAGS) go build -o $(OUT_DIR_HOST)/camera robit/camera

$(OUT_DIR_HOST)/websrv: | $(OUT_DIR_HOST)
	$(GOFLAGS) go build -o $(OUT_DIR_HOST)/websrv robit/websrv

$(OUT_DIR_HOST)/camera.test: | $(OUT_DIR_HOST)
	$(GOFLAGS) go test -c -o $(OUT_DIR_HOST)/camera.test robit/camera

$(OUT_DIR_HOST)/websrv.test: | $(OUT_DIR_HOST)
	$(GOFLAGS) go test -c -o $(OUT_DIR_HOST)/websrv.test robit/websrv

$(OUT_DIR_HOST)/sonar_test: sonar | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C sonar_test/)

$(OUT_DIR_HOST):
	@mkdir -p $@

clean:
	make -C gpio clean
	make -C motor clean
	make -C sonar clean
	make -C robit clean
	@rm -rf $(OUT_DIR_HOST)

# start container if it's not already running
start:
	$(DOCKER) container inspect $(CONTAINER) > /dev/null 2>&1 || \
		$(DOCKER) run -td -v "${CURDIR}":"$(SRC_DIR)" --name $(CONTAINER) $(IMAGE)

stop:
	@$(DOCKER) stop $(CONTAINER) 2> /dev/null
	@$(DOCKER) container prune -f

