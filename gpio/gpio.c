#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gpio.h"

static const char*  GPIOMEM_PATH    = "/dev/gpiomem";
static const char*  RANGES_PATH     = "/proc/device-tree/soc/ranges";
static const size_t GPIO_MEM_LENGTH = 4096;
static const off_t  BCM2835_BASE    = 0x20000000;
static const off_t  GPIO_OFF        = 0x200000;
static const off_t  GPIO_ADDR_OFF   = 4;

static const uint8_t PIN_MODE_MASK = 0x7;

static uint8_t* gpiomem_ptr = NULL;

int gpio_open()
{
    off_t base_addr = BCM2835_BASE;

    int fd = open(RANGES_PATH, O_RDONLY | O_SYNC);
    // not an error if we don't get a valid address - we'll just use the default base
    if (fd > -1)
    {
        size_t bytes_read = pread(fd, &base_addr, sizeof(base_addr), GPIO_ADDR_OFF);
        if (bytes_read != sizeof(base_addr))
            base_addr = BCM2835_BASE;
        close(fd);
    }

    base_addr += GPIO_OFF;

    fd = open(GPIOMEM_PATH, O_RDWR | O_SYNC);
    if (fd < 0)
        return errno;

    gpiomem_ptr = mmap(NULL, GPIO_MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);

    close(fd);

    if (gpiomem_ptr == MAP_FAILED)
        return errno;
    else
        return 0;
}

int gpio_close()
{
    int ret = munmap(gpiomem_ptr, GPIO_MEM_LENGTH);
    if (ret != 0)
        return errno;
    else
        return 0;
}

void gpio_mode(uint8_t pin, enum gpio_direction dir)
{
    uint8_t fsel_reg = pin / 10;
    uint8_t shift    = (pin % 10) * 3;

    gpiomem_ptr[fsel_reg] = (gpiomem_ptr[fsel_reg] & ~(PIN_MODE_MASK << shift)) | ((uint8_t)dir << shift);
}

void gpio_write(uint8_t pin, enum gpio_value val)
{
    // depending on bank...
    // clear regiser: 10 or 11
    // set register: 7 or 8
    uint8_t reg = pin / 32 + 10 - (uint8_t)val * 3;

    gpiomem_ptr[reg] = 1 << (pin & 31);
}

enum gpio_value gpio_read(uint8_t pin)
{
    return gpiomem_ptr[pin / 32 + 13] & (1 << (pin & 31));
}

