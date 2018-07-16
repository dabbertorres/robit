#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "motor/motor.h"
#include "sonar/sonar.h"

#define NUM_SENSORS 3

#define LEFT 0
#define CENTER 1
#define RIGHT 2

// valuewas picked off the top of my head
#define MIN_SENSOR_DIST 10.0

enum state
{
    STATE_FWD,
    STATE_REV_TO_CLEAR,
    STATE_LEFT_TO_CLEAR_RIGHT,
    STATE_RIGHT_TO_CLEAR_LEFT,
    STATE_STOP,
};

// size of state stack picked off the top of my head - probably PLENTY big
enum state state[64] = { STATE_FWD };
int curr_state = 0;
#define STATE_STACK_SIZE (int)((sizeof(state) / sizeof(state[0])))

const int sensor_pins[] = { 14, 2, 15, 3, 18, 4 };
const int motor_pins[] = { 19, 26, 6, 13, 20, 21, 12, 7 };

static volatile sig_atomic_t run = 1;

void signal_handler(int sig) { run = 0; }

int main()
{
    srand(time(NULL));

    struct sigaction sig_act;
    sig_act.sa_handler = signal_handler;
    sigaction(SIGINT, &sig_act, NULL);

    // distances are provided in centimeters
    double sensor_dists[NUM_SENSORS] = { 0.0 };
    struct sonar* sensors[NUM_SENSORS] = { NULL };

    sonar_init();

    for (int i = 0; i < NUM_SENSORS; i++)
    {
        sensors[i] = sonar_make(sensor_pins[i * 2], sensor_pins[i * 2 + 1], &sensor_dists[i]);
        if (sensors[i] == NULL)
        {
            fprintf(stderr, "sonar #%d broken\n", i);
            return 1;
        }
    }

    struct motor_group* motors = motor_make_group(motor_pins);
    if(motors == NULL)
    {
        fprintf(stderr, "motors broken\n");
        return 1;
    }

    while(run != 0)
    {
        // 300 millisecond (in nanoseconds)
        struct timespec sleep_time = { .tv_sec = 0, .tv_nsec = 300 * 1000000 };
        nanosleep(&sleep_time, NULL);

        sonar_get_all(sensors, sizeof(sensors) / sizeof(sensors[0]));

        switch(state[curr_state])
        {
            case STATE_FWD:
                // something in the way?
                if (sensor_dists[CENTER] <= MIN_SENSOR_DIST)
                {
                    // can we turn to avoid?
                    if (sensor_dists[LEFT] > MIN_SENSOR_DIST)
                    {
                        state[++curr_state] = STATE_LEFT_TO_CLEAR_RIGHT;
                    }
                    else if (sensor_dists[RIGHT] > MIN_SENSOR_DIST)
                    {
                        state[++curr_state] = STATE_RIGHT_TO_CLEAR_LEFT;
                    }
                    else
                    {
                        // alright, we can't turn to avoid. back up until all sensors are clear, then do a random turn
                        state[++curr_state] = rand() % 2 == 0 ? STATE_LEFT_TO_CLEAR_RIGHT : STATE_RIGHT_TO_CLEAR_LEFT;
                        state[++curr_state] = STATE_REV_TO_CLEAR;
                    }
                }
                else
                {
                    // motor onwards to victory!
                    motor_forward(motors);
                }
                break;

            case STATE_REV_TO_CLEAR:
                if (sensor_dists[CENTER] > MIN_SENSOR_DIST)
                    curr_state--;
                // rearview is for chumps. any dimwits behind me need to look out!
                else
                    motor_reverse(motors);
                break;

            case STATE_LEFT_TO_CLEAR_RIGHT:
                // if we're clear, go to previous state
                if (sensor_dists[RIGHT] > MIN_SENSOR_DIST)
                    curr_state--;
                // if something is blocking our turn, let's back up a bit before continuing (3 point turn)
                else if (sensor_dists[LEFT] <= MIN_SENSOR_DIST)
                    state[++curr_state] = STATE_REV_TO_CLEAR;
                else
                    motor_rotate_counterwise(motors);
                break;

            case STATE_RIGHT_TO_CLEAR_LEFT:
                // if we're clear, go to previous state - otherwise keep turning
                if (sensor_dists[LEFT] > MIN_SENSOR_DIST)
                    curr_state--;
                // if something is blocking our turn, let's back up a bit before continuing (3 point turn)
                else if (sensor_dists[RIGHT] <= MIN_SENSOR_DIST)
                    state[++curr_state] = STATE_REV_TO_CLEAR;
                else
                    motor_rotate_clockwise(motors);
                break;

            default:
                // weird state? stop in our tracks, and try to go to a previous state
                fprintf(stderr, "ended up in a weird state: %d - ", state[curr_state]);
            case STATE_STOP:
                fprintf(stderr, "stopping!\n");
                motor_stop(motors);
                curr_state--;
                break;
        }

        if (curr_state < 0)
        {
            curr_state = 0;
            fprintf(stderr, "stack underflow!\n");
        }
        else if (curr_state >= STATE_STACK_SIZE)
        {
            curr_state = STATE_STACK_SIZE - 1;
            fprintf(stderr, "stack overflow!\n");
        }
    }

    motor_free_group(motors);

    for (int i = 0; i < NUM_SENSORS; i++)
        sonar_free(sensors[i]);

    sonar_deinit();

    return 0;
}

