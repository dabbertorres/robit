#include "motor.h"

#include <stdlib.h>

#include "gpio/gpio.h"

struct motor
{
    gpio_pin hi;
    gpio_pin lo;
};

struct motor_group
{
    struct motor front_left;
    struct motor front_right;
    struct motor back_left;
    struct motor back_right;
};

int motor_set_motor(struct motor* m, int pin_hi, int pin_lo);

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
        gpio_unregister_pin(ms->hi);
        gpio_unregister_pin(ms->lo);
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
        res |= gpio_write(m->hi, GPIO_LO) * -1 << (8 * i);
        res |= gpio_write(m->lo, GPIO_LO) * -1 << (8 * (i + 1));

        res |= gpio_unregister_pin(m->hi) * -1 << (8 * i + 7);
        res |= gpio_unregister_pin(m->lo) * -1 << (8 * (i + 1) + 7);
    }

    free(mg);
    return res;
}

int motor_set_motor(struct motor* m, int pin_hi, int pin_lo)
{
    int res;

    res = gpio_register_pin(pin_hi, GPIO_W, &m->hi);
    if(res < 0)
        return res;

    res = gpio_register_pin(pin_lo, GPIO_W, &m->lo);
    if(res < 0)
        gpio_unregister_pin(m->hi);

    return res;
}

int motor_forward(struct motor_group* mg)
{
    gpio_write(mg->front_left.hi, GPIO_HI);
    gpio_write(mg->front_left.lo, GPIO_LO);

    gpio_write(mg->front_right.hi, GPIO_HI);
    gpio_write(mg->front_right.lo, GPIO_LO);

    gpio_write(mg->back_left.hi, GPIO_HI);
    gpio_write(mg->back_left.lo, GPIO_LO);

    gpio_write(mg->back_right.hi, GPIO_HI);
    gpio_write(mg->back_right.lo, GPIO_LO);

    // TODO errors
    return 0;
}

int motor_stop(struct motor_group* mg)
{
    gpio_write(mg->front_left.hi, GPIO_LO);
    gpio_write(mg->front_left.lo, GPIO_LO);

    gpio_write(mg->front_right.hi, GPIO_LO);
    gpio_write(mg->front_right.lo, GPIO_LO);

    gpio_write(mg->back_left.hi, GPIO_LO);
    gpio_write(mg->back_left.lo, GPIO_LO);

    gpio_write(mg->back_right.hi, GPIO_LO);
    gpio_write(mg->back_right.lo, GPIO_LO);

    // TODO errors
    return 0;

}

int motor_reverse(struct motor_group* mg)
{
    gpio_write(mg->front_left.hi, GPIO_LO);
    gpio_write(mg->front_left.lo, GPIO_HI);

    gpio_write(mg->front_right.hi, GPIO_LO);
    gpio_write(mg->front_right.lo, GPIO_HI);

    gpio_write(mg->back_left.hi, GPIO_LO);
    gpio_write(mg->back_left.lo, GPIO_HI);

    gpio_write(mg->back_right.hi, GPIO_LO);
    gpio_write(mg->back_right.lo, GPIO_HI);

    // TODO errors
    return 0;
}

int motor_rotate_right(struct motor_group* mg)
{
    gpio_write(mg->front_left.hi, GPIO_HI);
    gpio_write(mg->front_left.lo, GPIO_LO);

    gpio_write(mg->front_right.hi, GPIO_LO);
    gpio_write(mg->front_right.lo, GPIO_HI);

    gpio_write(mg->back_left.hi, GPIO_HI);
    gpio_write(mg->back_left.lo, GPIO_LO);

    gpio_write(mg->back_right.hi, GPIO_LO);
    gpio_write(mg->back_right.lo, GPIO_HI);

    return 0;
}

int motor_rotate_left(struct motor_group* mg)
{
    gpio_write(mg->front_left.hi, GPIO_LO);
    gpio_write(mg->front_left.lo, GPIO_HI);

    gpio_write(mg->front_right.hi, GPIO_HI);
    gpio_write(mg->front_right.lo, GPIO_LO);

    gpio_write(mg->back_left.hi, GPIO_LO);
    gpio_write(mg->back_left.lo, GPIO_HI);

    gpio_write(mg->back_right.hi, GPIO_HI);
    gpio_write(mg->back_right.lo, GPIO_LO);

    return 0;
}

