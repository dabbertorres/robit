CC := rpi3-cc
AR := rpi3-ar

CFLAGS := -Wall -Wextra -O2

SRC := gpio.c
OBJ := $(SRC:%.c=%.o)

gpio.a: $(OBJ)
	$(AR) rcs $@ $^

gpio.o: gpio.c gpio.h
	$(CC) -o $@ $(CFLAGS) -c $<
