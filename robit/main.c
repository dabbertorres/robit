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
    int pins[] = { 12, 16, 20, 21, 6, 13, 19, 26 };

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
                motor_reverse(robit);
                break;

            case LEFT_KEY:
                motor_reverse(robit);
                break;

            case EXIT_KEY:
                run = 0;
                break;

            default:
                break;
        }
    }

    motor_free_group(robit);

    return 0;
}

