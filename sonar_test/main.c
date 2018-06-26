#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "sonar/sonar.h"

#define TRIG_PIN 23
#define ECHO_PIN 24

int main()
{
    struct sonar* sonar = sonar_make(TRIG_PIN, ECHO_PIN);
    if (sonar == NULL)
    {
        printf("failed to create sonar\n");
        return 1;
    }

    int trig_err, echo_err, time_err;
    double dist = sonar_test(sonar, &trig_err, &echo_err, &time_err);

    int exit_now = 0;
    int err = errno;

    if (trig_err < 0)
        printf("trig_err: %d\n", trig_err);

    if (echo_err < 0)
        printf("echo_err: %d\n", echo_err);

    if (time_err < 0)
        printf("time_err (%d): %s\n", err, strerror(err));

    if (exit_now)
    {
        sonar_free(sonar);
        return 1;
    }

    printf("test distance = %4.2f cm\n", dist);

    struct timespec sleepTime = { .tv_sec = 2, .tv_nsec = 0 };

    while(!feof(stdin))
    {
        nanosleep(&sleepTime, NULL);
        dist = sonar_trigger(sonar);

        printf("distance = %4.2f cm\n", dist);
    }

    sonar_free(sonar);
}
