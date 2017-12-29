#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio.h"

int gpio_register_pin(enum gpio_pin pin) {
	char buffer[3];
	int bytes_written;
	int fd;
	int ret = 0;

	fd = open("sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		ret = -1;
		goto done;
	}
	
	bytes_written = snprintf(buffer, 3, "%d", pin);
	if (write(fd, buffer, bytes_written) < 0) {
		ret = -1;
		goto done;
	}

done:
	close(fd);
	return ret;
}

int gpio_unregister_pin(enum gpio_pin pin) {
	char buffer[3];
	int bytes_written;
	int fd;
	int ret = 0;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		ret = -1;
		goto done;
	}
	
	bytes_written = snprintf(buffer, 3, "%d", pin);
	if (write(fd, buffer, bytes_written) < 0) {
		ret = -1;
		goto done;
	}

done:
	close(fd);
	return ret;
}

int gpio_set_direction(enum gpio_pin pin, int direction) {
	int ret;
	char path[35]; // Largest possible value for the gpio direction path.
	int fd;

	snprintf(path, 35, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);

	if(fd < 0) {
		return -1;
	}
	
	if(direction == INPUT) {
		ret = write(fd, "in", 2);
	} else if(direction == OUTPUT) {
		ret = write(fd, "out", 3);
	} else {
		ret = -1;
	}

	close(fd);
	return ret;
}

int gpio_write(enum gpio_pin pin, int value) {
	int ret;
	char path[30]; // Largest possible value for the gpio direction path.
	int fd;

	snprintf(path, 30, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);

	if(fd < 0) {
		return -1;
	}
	
	if (value == OFF) {
		ret = write(fd, "0", 1);
	} else {
		ret = write(fd, "1", 1);
	}

	close(fd);
	return ret;
}

int gpio_read(enum gpio_pin pin) {
	int ret;
	char path[30]; // Largest possible value for the gpio direction path.
	char value_string[3];
	int fd;


	snprintf(path, 30, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);

	if(fd < 0) {
		return -1;
	}

	if(read(fd, value_string, 3) < 0) {
		ret = -1;
		goto done;
	}

	ret = atoi(value_string);

done:
	close(fd);
	return ret;
}
