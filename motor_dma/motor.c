#include "motor.h"

#include <stdlib.h>
#include <string.h>

#include "gpio/gpio.h"

struct motor
{
    gpio_pin fwd;
    gpio_pin back;
};

struct motor_group
{
    struct motor front_left;
    struct motor front_right;
    struct motor back_left;
    struct motor back_right;
    gpio_map     pin_state;
};

int motor_set_motor(struct motor* m, int pin_fwd, int pin_back);

struct motor_group* motor_make_group(const int* pins)
{
    struct motor_group* mg = malloc(sizeof(struct motor_group));
    if (mg == NULL) return NULL;
    memset(mg->pin_state, 0, sizeof(gpio_map));
    gpio_write(mg->pin_state);

    mg->front_left.fwd   = pins[0];
    mg->front_left.back  = pins[1];
    mg->front_right.fwd  = pins[2];
    mg->front_right.back = pins[3];
    mg->back_left.fwd    = pins[4];
    mg->back_left.back   = pins[5];
    mg->back_right.fwd   = pins[6];
    mg->back_right.back  = pins[7];
    return mg;
}

void motor_free_group(struct motor_group* mg)
{
    memset(mg->pin_state, 0, sizeof(gpio_map));
    gpio_write(mg->pin_state);
    free(mg);
}

void motor_forward(struct motor_group* mg)
{
    mg->pin_state[mg->front_left.fwd]   = GPIO_HI;
    mg->pin_state[mg->front_left.back]  = GPIO_LO;
    mg->pin_state[mg->front_right.fwd]  = GPIO_HI;
    mg->pin_state[mg->front_right.back] = GPIO_LO;
    mg->pin_state[mg->back_left.fwd]    = GPIO_HI;
    mg->pin_state[mg->back_left.back]   = GPIO_LO;
    mg->pin_state[mg->back_right.fwd]   = GPIO_HI;
    mg->pin_state[mg->back_right.back]  = GPIO_LO;

    gpio_write(mg->pin_state);
}

void motor_stop(struct motor_group* mg)
{
    mg->pin_state[mg->front_left.fwd]   = GPIO_LO;
    mg->pin_state[mg->front_left.back]  = GPIO_LO;
    mg->pin_state[mg->front_right.fwd]  = GPIO_LO;
    mg->pin_state[mg->front_right.back] = GPIO_LO;
    mg->pin_state[mg->back_left.fwd]    = GPIO_LO;
    mg->pin_state[mg->back_left.back]   = GPIO_LO;
    mg->pin_state[mg->back_right.fwd]   = GPIO_LO;
    mg->pin_state[mg->back_right.back]  = GPIO_LO;

    gpio_write(mg->pin_state);
}

void motor_reverse(struct motor_group* mg)
{
    mg->pin_state[mg->front_left.fwd]   = GPIO_LO;
    mg->pin_state[mg->front_left.back]  = GPIO_HI;
    mg->pin_state[mg->front_right.fwd]  = GPIO_LO;
    mg->pin_state[mg->front_right.back] = GPIO_HI;
    mg->pin_state[mg->back_left.fwd]    = GPIO_LO;
    mg->pin_state[mg->back_left.back]   = GPIO_HI;
    mg->pin_state[mg->back_right.fwd]   = GPIO_LO;
    mg->pin_state[mg->back_right.back]  = GPIO_HI;

    gpio_write(mg->pin_state);
}

// for turning, turn off the side we want to turn towards, turn on the opposite side

void motor_rotate_clockwise(struct motor_group* mg)
{
    mg->pin_state[mg->front_left.fwd]   = GPIO_HI;
    mg->pin_state[mg->front_left.back]  = GPIO_LO;
    mg->pin_state[mg->front_right.fwd]  = GPIO_LO;
    mg->pin_state[mg->front_right.back] = GPIO_LO;
    mg->pin_state[mg->back_left.fwd]    = GPIO_HI;
    mg->pin_state[mg->back_left.back]   = GPIO_LO;
    mg->pin_state[mg->back_right.fwd]   = GPIO_LO;
    mg->pin_state[mg->back_right.back]  = GPIO_LO;

    gpio_write(mg->pin_state);
}

void motor_rotate_counterwise(struct motor_group* mg)
{
    mg->pin_state[mg->front_left.fwd]   = GPIO_LO;
    mg->pin_state[mg->front_left.back]  = GPIO_LO;
    mg->pin_state[mg->front_right.fwd]  = GPIO_HI;
    mg->pin_state[mg->front_right.back] = GPIO_LO;
    mg->pin_state[mg->back_left.fwd]    = GPIO_LO;
    mg->pin_state[mg->back_left.back]   = GPIO_LO;
    mg->pin_state[mg->back_right.fwd]   = GPIO_HI;
    mg->pin_state[mg->back_right.back]  = GPIO_LO;

    gpio_write(mg->pin_state);
}

