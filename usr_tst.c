#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/ioctl.h>
// #include "gpio_ioctl.h"
#include "lib/pl_gpio.h"

struct gpio_dev dev;



int main() {

    int led = 4 + EMIO_BASE;
    int sw0 = 10 + EMIO_BASE;
    int sw1 = 11 + EMIO_BASE;
    long ret_chk;
    int bank = 2;

    struct gpio_ioctl_pin gpio_sw[2];
    struct gpio_ioctl_pin gpio_led;
    struct gpio_ioctl_pin gpio_xor;
    gpio_sw[0].pin = sw0;
    gpio_sw[1].pin = sw1;
    gpio_led.pin = led;
    gpio_xor.pin = -1;

    while (TRUE)
    {
        gpio_init(&dev);
        gpio_write(&dev, bank, 7);
        gpio_release(&dev);
        
        /* Test: XNOR gate- read AXI_GPIO(xor in pl) and send the complementary val to led */
        // unsigned int x = ioctl(fd, WR_DEV, &gpio_led);      
        // gpio_led.data = x ? 0 : 1;                                // xnor: the led.data val is reversed
        // printf("gpio_led.data: %d\nx: %d\n", gpio_led.data, x);
        // ret_chk = ioctl(fd, SET_PIN, &gpio_led);
        // printf("func return: %d\n", ret_chk);
        
        /* Test: blink led */
        // gpio_led.data = TRUE;
        // ret_chk = ioctl(fd, SET_PIN, &gpio_led);
        // printf("func return: %d\n", ret_chk);
        // usleep(1000000);
        // gpio_led.data = FALSE;
        // ret_chk = ioctl(fd, SET_PIN, &gpio_led);
        //printf("func return: %d\n", ret_chk);
        usleep(1000000);
    }
}

