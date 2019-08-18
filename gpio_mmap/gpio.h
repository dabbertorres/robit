#pragma once

#include <stdint.h>

#define GPIO_MAX_PINS 30

typedef int gpio_pin;

enum gpio_direction
{
    GPIO_R = 0, // read
    GPIO_W = 1, // write
};

// gpio_dir_map is for configuring the direction of GPIO pins.
typedef enum gpio_direction gpio_dir_map[GPIO_MAX_PINS];

enum gpio_value
{
    GPIO_LO = 0,
    GPIO_HI = 1,
};

// gpio_map is for configuring the values of GPIO pins.
typedef enum gpio_value gpio_map[GPIO_MAX_PINS];

int gpio_init(gpio_dir_map map);
int gpio_deinit();

void gpio_write(gpio_map map);
void gpio_write_one(uint32_t pin, enum gpio_value val);

void            gpio_read(gpio_map map);
enum gpio_value gpio_read_one(uint32_t pin);

