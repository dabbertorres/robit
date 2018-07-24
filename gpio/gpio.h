#pragma once

typedef int gpio_pin;

enum gpio_direction
{
    GPIO_R = 0,
    GPIO_W = 1,
};

enum gpio_value
{
    GPIO_LO = 0,
    GPIO_HI = 1,
};

// if successful, 0 is returned. Otherwise, returns errno
int gpio_open();
int gpio_close();

void gpio_mode(uint8_t pin, enum gpio_direction dir);

void gpio_write(uint8_t pin, enum gpio_value val);
enum gpio_value gpio_read(uint8_t pin);

