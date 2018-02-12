CC      := rpi3-cc
AR      := rpi3-ar
CFLAGS  += -Wall -Wextra -O2
GOFLAGS += GOOS=linux GOARCH=arm GOARM=7

DOCKER := docker

# if we're on windows, correct docker executable name
ifeq ($(OS), Windows_NT)
	DOCKER := docker.exe
endif

CONTAINER ?= rpi3_make_build
IMAGE     ?= dabbertorres/rpi3-gcc
SRC_DIR   ?= /gopath/src/robit

OUT_DIR_HOST   ?= build
OUT_DIR := $(SRC_DIR)/$(OUT_DIR_HOST)

EXEC = $(DOCKER) exec $(CONTAINER) bash -c "export CC=$(CC); \
											export AR=$(AR); \
											export CFLAGS='$(CFLAGS)'; \
											export OUT_DIR=$(OUT_DIR); \
											$(1)"

.PHONY: all tests gpio robit camera websrv clean start stop

all: gpio robit camera websrv
tests: $(OUT_DIR_HOST)/camera.test $(OUT_DIR_HOST)/websrv.test

gpio:   $(OUT_DIR_HOST)/gpio.a
robit:  $(OUT_DIR_HOST)/robit
camera: $(OUT_DIR_HOST)/camera
websrv: $(OUT_DIR_HOST)/websrv

$(OUT_DIR_HOST)/gpio.a: | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C gpio/)

$(OUT_DIR_HOST)/robit: $(OUT_DIR_HOST)/gpio.a | $(OUT_DIR_HOST) start
	$(call EXEC,make -j$(nproc) -C robit/)

$(OUT_DIR_HOST)/camera: | $(OUT_DIR_HOST)
	$(GOFLAGS) go build -o $(OUT_DIR_HOST)/camera robit/camera

$(OUT_DIR_HOST)/camera.test: | $(OUT_DIR_HOST)
	$(GOFLAGS) go test -c -o $(OUT_DIR_HOST)/camera.test robit/camera

$(OUT_DIR_HOST)/websrv: | $(OUT_DIR_HOST)
	$(GOFLAGS) go build -o $(OUT_DIR_HOST)/websrv robit/websrv

$(OUT_DIR_HOST)/websrv.test: | $(OUT_DIR_HOST)
	$(GOFLAGS) go test -c -o $(OUT_DIR_HOST)/websrv.test robit/websrv

$(OUT_DIR_HOST):
	mkdir $@

clean:
	make -C gpio clean
	make -C robit clean
	@rm -rf $(OUT_DIR_HOST)

# start container if it's not already running
start:
	@$(DOCKER) container inspect $(CONTAINER) > /dev/null || \
		$(DOCKER) run --rm -td -v "$(PWD)":"$(SRC_DIR)" -w /$(SRC_DIR) --name $(CONTAINER) $(IMAGE)

stop:
	$(DOCKER) stop $(CONTAINER)

