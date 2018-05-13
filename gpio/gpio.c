#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio.h"

int gpio_register_pin(int pin_num, enum gpio_direction dir, gpio_pin* pin)
{
    // Largest possible length of a string we're dealing with
    char buffer[35];
    int bytes_written;
    int fd;
    int file_mode;
    int write_sts;

    /* tell the system we want to use this pin */

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0)
        return -1;

    bytes_written = snprintf(buffer, 3, "%d", pin_num);
    write_sts = write(fd, buffer, bytes_written);
    close(fd);

    if (write_sts < 0)
        return -1;

    /* now are we reading, or writing? */

    snprintf(buffer, 35, "/sys/class/gpio/gpio%d/direction", pin_num);
    fd = open(buffer, O_WRONLY);
    if(fd < 0)
        return -1;

    if(dir == GPIO_R) {
        write_sts = write(fd, "in", 2);
        file_mode = O_RDONLY;
    } else {
        write_sts = write(fd, "out", 3);
        file_mode = O_WRONLY;
    }

    close(fd);

    if(write_sts < 0)
        return -1;

    /* now let's grab the place we're reading/writing to/from */

    snprintf(buffer, 30, "/sys/class/gpio/gpio%d/value", pin_num);
    *pin = open(buffer, file_mode);

    if(*pin < 0)
        return -1;
    else
        return 0;
}

int gpio_unregister_pin(gpio_pin pin)
{
    char buffer[3];
    char file_name_buffer[4096];
    char conversion_buffer[35];
    int pin_number;
    int bytes_written;
    int fd;
    int write_sts;
    
    
    /* recover the pin number from the open file descriptor symlink */

    bytes_written = snprintf(conversion_buffer, 35, "/proc/self/fd/%d", pin);
    bytes_written = readlink(conversion_buffer, file_name_buffer, 4096);
    bytes_written = sscanf(file_name_buffer, "%*[^g]gpio/gpio/gpio%d%", &pin_number);
    if (!bytes_written) {
	printf("error calculating pin number\n");
	return;
    }
    
    close(pin);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0)
        return -1;

    bytes_written = snprintf(buffer, 3, "%d", pin_number);
    write_sts = write(fd, buffer, bytes_written);
    close(fd);

    if (write_sts < 0)
        return -1;
    else
        return 0;
}

int gpio_write(gpio_pin pin, enum gpio_value val)
{
    const char* lo = "0";
    const char* hi = "1";

    const char* val_str;

    if (val == GPIO_LO)
        val_str = lo;
    else
        val_str = hi;

    return write(pin, val_str, 1);
}

int gpio_read(gpio_pin pin)
{
    char value[3];
    lseek(pin, 0, SEEK_SET);
    if(read(pin, value, 3) < 0)
        return -1;
    return atoi(value);
}

