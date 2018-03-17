#include "motor.h"

#include <stdlib.h>

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
};

int motor_set_motor(struct motor* m, int pin_fwd, int pin_back);

struct motor_group* motor_make_group(int* pins)
{
    int res;

    struct motor_group* mg = malloc(sizeof(struct motor_group));

    struct motor* ms = &mg->front_left;

    for(int i = 0; i < 4; ++i)
    {
        res = motor_set_motor(ms++, pins[i * 2], pins[i * 2 + 1]);
        if(res < 0)
            goto fail;
    }

    return mg;

fail:
    do
    {
        --ms;
        gpio_unregister_pin(ms->fwd);
        gpio_unregister_pin(ms->back);
    }
    while(ms != &mg->front_left);

    free(mg);
    return NULL;
}

uint64_t motor_free_group(struct motor_group* mg)
{
    uint64_t res = 0;

    struct motor* m = &mg->front_left;

    for(int i = 0; i < 4; ++i, ++m)
    {
        res |= gpio_write(m->fwd, GPIO_LO) * -1 << (8 * i);
        res |= gpio_write(m->back, GPIO_LO) * -1 << (8 * (i + 1));

        res |= gpio_unregister_pin(m->fwd) * -1 << (8 * i + 7);
        res |= gpio_unregister_pin(m->back) * -1 << (8 * (i + 1) + 7);
    }

    free(mg);
    return res;
}

int motor_set_motor(struct motor* m, int pin_fwd, int pin_back)
{
    int res;

    res = gpio_register_pin(pin_fwd, GPIO_W, &m->fwd);
    if(res < 0)
        return res;

    res = gpio_register_pin(pin_back, GPIO_W, &m->back);
    if(res < 0)
        gpio_unregister_pin(m->fwd);

    return res;
}

int motor_forward(struct motor_group* mg)
{
    gpio_write(mg->front_left.fwd, GPIO_HI);
    gpio_write(mg->front_left.back, GPIO_LO);

    gpio_write(mg->front_right.fwd, GPIO_HI);
    gpio_write(mg->front_right.back, GPIO_LO);

    gpio_write(mg->back_left.fwd, GPIO_HI);
    gpio_write(mg->back_left.back, GPIO_LO);

    gpio_write(mg->back_right.fwd, GPIO_HI);
    gpio_write(mg->back_right.back, GPIO_LO);

    // TODO errors
    return 0;
}

int motor_stop(struct motor_group* mg)
{
    gpio_write(mg->front_left.fwd, GPIO_LO);
    gpio_write(mg->front_left.back, GPIO_LO);

    gpio_write(mg->front_right.fwd, GPIO_LO);
    gpio_write(mg->front_right.back, GPIO_LO);

    gpio_write(mg->back_left.fwd, GPIO_LO);
    gpio_write(mg->back_left.back, GPIO_LO);

    gpio_write(mg->back_right.fwd, GPIO_LO);
    gpio_write(mg->back_right.back, GPIO_LO);

    // TODO errors
    return 0;

}

int motor_reverse(struct motor_group* mg)
{
    gpio_write(mg->front_left.fwd, GPIO_LO);
    gpio_write(mg->front_left.back, GPIO_HI);

    gpio_write(mg->front_right.fwd, GPIO_LO);
    gpio_write(mg->front_right.back, GPIO_HI);

    gpio_write(mg->back_left.fwd, GPIO_LO);
    gpio_write(mg->back_left.back, GPIO_HI);

    gpio_write(mg->back_right.fwd, GPIO_LO);
    gpio_write(mg->back_right.back, GPIO_HI);

    // TODO errors
    return 0;
}

// for turning, turn off the side we want to turn towards, turn on the opposite side

int motor_rotate_clockwise(struct motor_group* mg)
{
    gpio_write(mg->front_left.fwd, GPIO_HI);
    gpio_write(mg->front_left.back, GPIO_LO);

    gpio_write(mg->front_right.fwd, GPIO_LO);
    gpio_write(mg->front_right.back, GPIO_LO);

    gpio_write(mg->back_left.fwd, GPIO_HI);
    gpio_write(mg->back_left.back, GPIO_LO);

    gpio_write(mg->back_right.fwd, GPIO_LO);
    gpio_write(mg->back_right.back, GPIO_LO);

    return 0;
}

int motor_rotate_counterwise(struct motor_group* mg)
{
    gpio_write(mg->front_left.fwd, GPIO_LO);
    gpio_write(mg->front_left.back, GPIO_LO);

    gpio_write(mg->front_right.fwd, GPIO_HI);
    gpio_write(mg->front_right.back, GPIO_LO);

    gpio_write(mg->back_left.fwd, GPIO_LO);
    gpio_write(mg->back_left.back, GPIO_LO);

    gpio_write(mg->back_right.fwd, GPIO_HI);
    gpio_write(mg->back_right.back, GPIO_LO);

    return 0;
}

