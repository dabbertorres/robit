#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>

#include "motor/motor.h"

#define FWRD_KEY 'w'
#define RVRS_KEY 's'
#define RIGHT_KEY 'd'
#define LEFT_KEY 'a'
#define STOP_KEY 0x20
#define EXIT_KEY 0x1b

int main()
{
    int pins[] = { 19, 26, 6, 13, 20, 21, 12, 7 };

    struct motor_group* robit = motor_make_group(pins);
    if(robit == NULL)
    {
        printf("something be broke\n");
        return 1;
    }

    int run = 1;

    while(run)
    {
        char input;
        scanf("%c", &input);

        switch(input)
        {
            case FWRD_KEY:
                motor_forward(robit);
                break;

            case STOP_KEY:
                motor_stop(robit);
                break;

            case RVRS_KEY:
                motor_reverse(robit);
                break;

            case RIGHT_KEY:
                motor_rotate_clockwise(robit);
                break;

            case LEFT_KEY:
                motor_rotate_counterwise(robit);
                break;

            case EXIT_KEY:
                run = 0;
                break;

            default:
                break;
        }
    }

    uint64_t freeRes = motor_free_group(robit);

    for(int i = 0; i < 8; ++i)
    {
        printf("pin #%d result(write: %llu, free: %llu)\n", i, (freeRes >> (8 * i) & 0x01), (freeRes >> (8 * i + 7) & 0x01));
    }

    return 0;
}

