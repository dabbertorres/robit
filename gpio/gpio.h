#pragma once

struct gpio_pin;

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

int gpio_register_pin(int pin_num, enum gpio_direction dir, struct gpio_pin* pin);
int gpio_unregister_pin(struct gpio_pin* pin);
int gpio_write(struct gpio_pin* pin, enum gpio_value val);
int gpio_read(struct gpio_pin* pin);

