#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#include "../drivers/gpio/gpio_ioctl.h"

#define TRUE 1
#define FALSE 0
#define EMIO_BASE 54

struct gpio_dev {
    int fd = -1;
    struct gpio_ioctl_pin gpb;
    struct gpio_ioctl_reg gpr;
}

int gpio_init(struct gpio_dev *dev);
int gpio_release(struct gpio_dev *dev);
int gpio_write(struct gpio_dev *dev, uint8_t bank, uint32_t reg_data);
uint32_t gpio_read(struct gpio_dev *dev, uint32_t reg_data);
int gpio_read_pin(struct gpio_dev *dev, uint8_t pin);
int gpio_write_pin(struct gpio_dev *dev, uint8_t pin, uint8_t data);

