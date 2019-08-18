#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "gpio_dma/gpio.h"
#include "motor_dma/motor.h"

#define FWRD_KEY 'w'
#define RVRS_KEY 's'
#define RIGHT_KEY 'd'
#define LEFT_KEY 'a'
#define STOP_KEY 0x20
#define EXIT_KEY 0x1b

void enable_raw_mode();
void disable_raw_mode();
void alloc_motor();
void free_motor();
void die(const char* s);
// void sig_act(int sig);

struct termios      orig_termios;
struct motor_group* robit;

int main()
{
    // struct sigaction sig_act;
    // sig_act.sa_handler = signal_handler;
    // sigaction(SIGINT, &sig_act, NULL);

    const int pins[] = {19, 26, 6, 13, 20, 21, 12, 7};

    gpio_dir_map dir_map;
    memset(dir_map, 0, sizeof(gpio_dir_map));

    for (uint32_t i = 0; i < sizeof(pins) / sizeof(int); i++)
        dir_map[pins[i]] = GPIO_W;

    gpio_init(dir_map);
    atexit(gpio_deinit);

    enable_raw_mode();
    alloc_motor(pins);

    int run = 1;

    while (run)
    {
        int  bytes;
        char input;
        if ((bytes = read(STDIN_FILENO, &input, 1)) == -1 && errno != EAGAIN) die("main: read");

        switch (input)
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

    return 0;
}

void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("enable_raw_mode: tcgetattr");
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    raw.c_cflag &= ~(CSIZE | PARENB);
    raw.c_cflag |= (CS8 | B19200);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("enable_raw_mode: tcsetattr");
}

void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("disable_raw_mode: tcsetattr");
}

void alloc_motor(const int* pins)
{
    robit = motor_make_group(pins);
    if (robit == NULL) die("alloc_motor: motor_make_group");

    atexit(free_motor);
}

void free_motor()
{
    motor_free_group(robit);
}

void die(const char* s)
{
    perror(s);
    exit(1);
}

// void sig_act(int sig) {}
