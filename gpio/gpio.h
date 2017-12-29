#pragma once

#define INPUT 0
#define OUTPUT 1

#define OFF 0
#define ON 1

enum gpio_pin {
GPIO_0 = 0,
GPIO_1 = 1,
GPIO_2 = 2,
GPIO_3 = 3,
GPIO_7 = 7,
GPIO_8 = 8,
GPIO_9 = 9,
GPIO_10 = 10,
GPIO_11 = 11,
GPIO_14 = 14,
GPIO_15 = 15,
GPIO_17 = 17,
GPIO_18 = 18,
GPIO_21	= 21,
GPIO_22	= 22,
GPIO_23 = 23,
GPIO_24 = 24,
GPIO_25 = 25,
GPIO_27 = 27,
};

int gpio_register_pin(enum gpio_pin pin);
int gpio_unregister_pin(enum gpio_pin pin);
int gpio_set_direction(enum gpio_pin pin, int direction);
int gpio_write(enum gpio_pin pin, int value);
int gpio_read(enum gpio_pin pin);
