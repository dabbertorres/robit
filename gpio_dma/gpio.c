#include "gpio.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_ADDR (BCM2708_PERI_BASE + 0x200000)

#define GPIO_SET_OFF (0x1C / sizeof(uint32_t))
#define GPIO_CLR_OFF (0x28 / sizeof(uint32_t))
#define GPIO_LVL_OFF (0x34 / sizeof(uint32_t))

static volatile uint32_t* gpio_reg;

int gpio_init(gpio_dir_map map)
{
    int fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (fd < 0) return errno;

    gpio_reg = (volatile uint32_t*)mmap(0, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_ADDR);
    if ((int32_t)gpio_reg < 0) return errno;

    close(fd);

    for (uint32_t i = 0; i < GPIO_MAX_PINS / 10; i++)
    {
        uint32_t set_to = 0;
        for (int j = 0; j < 10; j++)
            set_to |= (uint32_t)map[i * 10 + j] << j * 3;
        *(gpio_reg + i) = 0;
        *(gpio_reg + i) = set_to;
    }

    return 0;
}

void gpio_deinit()
{
    for (uint32_t i = 0; i < GPIO_MAX_PINS / 10; i++)
        *(gpio_reg + i) = 0;
    munmap((void*)gpio_reg, sysconf(_SC_PAGESIZE));
}

void gpio_write(gpio_map map)
{
    uint32_t values = 0;
    for (uint32_t i = 0; i < GPIO_MAX_PINS; i++)
        values |= map[i] << i;
    *(gpio_reg + GPIO_SET_OFF) = values;
    *(gpio_reg + GPIO_CLR_OFF) = ~values;
}

void gpio_write_one(uint32_t pin, enum gpio_value val)
{
    switch (val)
    {
    case GPIO_LO:
        *(gpio_reg + GPIO_CLR_OFF) &= ~((uint32_t)1 << pin);
        break;

    case GPIO_HI:
        *(gpio_reg + GPIO_SET_OFF) |= (uint32_t)1 << pin;
        break;

    default:
        break;
    }
}

void gpio_read(gpio_map map)
{
    uint32_t val = *(gpio_reg + GPIO_LVL_OFF);

    for (uint32_t i = 0; i < GPIO_MAX_PINS; i++)
        map[i] = (val >> i) & 0x1;
}

enum gpio_value gpio_read_one(uint32_t pin)
{
    return (enum gpio_value)((*(gpio_reg + GPIO_LVL_OFF) >> pin) & 0x1);
}
