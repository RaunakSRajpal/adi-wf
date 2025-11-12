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

int gpio_init(struct gpio_dev *dev) {
    dev->fd = open("/dev/gpiopl_dev", O_RDWR);

    if (dev->fd < 0) {
        printf("%s: Failed to open device file: %d\n", DEVICE_FILE_NAME, dev->fd);
        return 1;
    }
    
    return 0;
}

int gpio_release(struct gpio_dev *dev) {
    close(dev->fd);
}

int gpio_write(struct gpio_dev *dev, uint8_t bank, uint32_t reg_data) {
    if(dev->fd < 0) {
        printf("%s: device file not open: %d\n", DEVICE_FILE_NAME, dev->fd);
        return 1;
    }

    // write(dev->fd, reg_gpbank, sizeof(reg_gpbank));

    dev->gpr.bank = bank;
    dev->gpr.data = reg_data;
    int ret = ioctl(dev->fd, WR_DEV, &dev->gpr);

    if (ret < 0) {
        printf("%s: Write to device file failed: %d\n", DEVICE_FILE_NAME, ret);
        return 1;
    }

    return 0;
};

uint32_t gpio_read(struct gpio_dev *dev, uint32_t reg_data) {
    if(dev->fd < 0) {
        printf("%s: device file not open: %d\n", DEVICE_FILE_NAME, dev->fd);
        return 1;
    }

    // read(dev->fd, reg_gpbank, sizeof(reg_gpbank));

    int ret = ioctl(dev->fd, RD_DEV, &dev->gpr);
    if (ret < 0) {
        printf("%s: Write to device file failed: %d\n", DEVICE_FILE_NAME, ret);
        return 1;
    }
    dev->gpr.data = ret;

    return reg_data;
}

int gpio_read_pin(struct gpio_dev *dev, uint8_t pin) {
    if(dev->fd < 0) {
        printf("%s: device file not open: %d\n", DEVICE_FILE_NAME, dev->fd);
        return 1;
    }

    int ret = ioctl(dev->fd, GET_PIN, &dev->gpb);
    if (ret < 0) {
        printf("%s: Write to device file failed: %d\n", DEVICE_FILE_NAME, ret);
        return 1;
    }
    dev->gpb.data = ret;
    
    return 0;
}

int gpio_write_pin(struct gpio_dev *dev, uint8_t pin, uint8_t data) {
    if(dev->fd < 0) {
        printf("%s: device file not open: %d\n", DEVICE_FILE_NAME, dev->fd);
        return 1;
    }

    dev->gpb.pin = pin;
    dev->gpb.data = data;
    int ret = ioctl(dev->fd, SET_PIN, &dev->gpb);

    if (ret < 0) {
        printf("%s: Write to device file failed: %d\n", DEVICE_FILE_NAME, ret);
        return 1;
    }
    
    return 0;
}

