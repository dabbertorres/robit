#include <errno.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "sonar/sonar.h"

#define TRIG_PIN 23
#define ECHO_PIN 24

#define LEFT 0
#define CENTER 1
#define RIGHT 2

struct { int trig; int echo; } pin_pairs[] = { { 23, 24 }, { 25, 8 }, { 3, 4 } };

static int exit = 0;
void sig_handler(int sig)
{
    exit = 1;
}

int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    sonar_init();

    double dists[3] = { 0.0, 0.0, 0.0 };
    struct sonar* sensors[3] = { NULL, NULL, NULL };

    for (int i = 0; i < 3; i++)
    {
        sensors[i] = sonar_make(pin_pairs[i].trig, pin_pairs[i].echo, &dists[i]);
        if (sensors[i] == NULL)
        {
            printf("failed to create sonar\n");
            return 1;
        }
    }

    struct timespec sleepTime = { .tv_sec = 2, .tv_nsec = 0 };

    while(exit == 0)
    {
        nanosleep(&sleepTime, NULL);

        sonar_get_all(sensors, 3);

        for (int i = 0; i < 3; i++)
            printf("#%d = %4.2f cm\n", i, dists[i]);
        printf("\n");
    }

    for(int i = 0; i < 3; i++)
        sonar_free(sensors[i]);

    sonar_deinit();
}
