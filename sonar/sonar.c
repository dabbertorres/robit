#include "sonar.h"

#include <time.h>
#include <stdlib.h>

#include "gpio/gpio.h"

#define MICRO_TO_NANO(t) (t * 1000)
#define NANO_TO_MICRO(t) ((double)t / 1000.0)
#define BASE_TO_MICRO(t) (t * 1000000)

#define TIMESPEC_TO_MICRO(t) ((double)BASE_TO_MICRO(t.tv_sec) + NANO_TO_MICRO(t.tv_nsec))

#define TRIG_TIME MICRO_TO_NANO(10)

struct sonar
{
    gpio_pin trig;
    gpio_pin echo;
};

struct sonar* sonar_make(int pin_trig, int pin_echo)
{
    struct sonar* s = malloc(sizeof(struct sonar));

    int res = gpio_register_pin(pin_trig, GPIO_W, &s->trig);
    if(res < 0)
        goto fail;

    res = gpio_register_pin(pin_echo, GPIO_R, &s->echo);
    if(res < 0)
        goto fail_out;

    return s;

fail_out:
    gpio_unregister_pin(s->trig);

fail:
    free(s);
    return NULL;
}

void sonar_free(struct sonar* s)
{
    gpio_unregister_pin(s->trig);
    gpio_unregister_pin(s->echo);
    free(s);
}

double sonar_test(struct sonar* s, int* trig_err, int* echo_err, int* time_err)
{
    *trig_err = gpio_write(s->trig, GPIO_HI);
    if (*trig_err < 0)
        return 0.0;

    // module is triggered by a 10 microsecond high
    struct timespec ts = { .tv_sec = 0, .tv_nsec = TRIG_TIME };
    *time_err = nanosleep(&ts, NULL);
    if (*time_err == -1)
        return 0.0;

    *trig_err = gpio_write(s->trig, GPIO_LO);
    if (*trig_err < 0)
        return 0.0;

    // wait for module to complete sending pulse
    while((*echo_err = gpio_read(s->echo)) <= 0);

    // measure the length of the echo
    struct timespec start, end;

    *time_err = clock_gettime(CLOCK_REALTIME, &start);
    if (*time_err == -1)
        return 0.0;

    while((*echo_err = gpio_read(s->echo)) > 0);

    *time_err = clock_gettime(CLOCK_REALTIME, &end);
    if (*time_err == -1)
        return 0.0;

    // microseconds / 58 = centimeters
    double us = TIMESPEC_TO_MICRO(end) - TIMESPEC_TO_MICRO(start);
    return us / 58.0;
}

double sonar_trigger(struct sonar* s)
{
    gpio_write(s->trig, GPIO_HI);

    // module is triggered by a 10 microsecond high
    struct timespec ts = { .tv_sec = 0, .tv_nsec = TRIG_TIME };
    nanosleep(&ts, NULL);

    gpio_write(s->trig, GPIO_LO);

    // wait for module to complete sending pulse
    while(gpio_read(s->echo) <= 0);

    // measure the length of the echo
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    while(gpio_read(s->echo) > 0);
    clock_gettime(CLOCK_REALTIME, &end);

    // microseconds / 58 = centimeters
    double us = TIMESPEC_TO_MICRO(end) - TIMESPEC_TO_MICRO(start);
    return us / 58.0;
}
