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

int gpio_register_pin(int pin_num, enum gpio_direction dir, gpio_pin* pin);
int gpio_unregister_pin(gpio_pin pin);
int gpio_write(gpio_pin pin, enum gpio_value val);
int gpio_read(gpio_pin pin);

