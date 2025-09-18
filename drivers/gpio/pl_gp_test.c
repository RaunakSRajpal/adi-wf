#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "gpio_ioctl.h"

#define TRUE 1
#define FALSE 0

#define EMIO_BASE 54

// ssize_t xgpio_readreg(int file_desc, char *buf, int pin) {
//     ssize_t nbytes = read(file_desc,buf,sizeof(buf));
//     // printf("no. of bytes read: %d\n", nbytes);
//     return nbytes;
// }

// ssize_t xgpio_writereg(int file_desc, char *buf, int pin, int value) {
//     sprintf(buf, "(%d,%d)", pin, value);
//     printf("buffer size: %d\n", sizeof(buf));
//     ssize_t nbytes = write(file_desc,buf,sizeof(buf));
//     return nbytes;
// }

int main() {

    int fd = open("/proc/gpio_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open file\n");
        return 1;
    }

    char rd_buffer[1024];
    char wr_buffer[1024];
    
    int led = 4 + EMIO_BASE;
    int sw0 = 8 + EMIO_BASE;
    int sw1 = 9 + EMIO_BASE;
    long ret_chk;

    struct gpio_ioctl gpio_sw[2];
    struct gpio_ioctl gpio_led;
    gpio_sw[0].pin = sw0;
    gpio_sw[1].pin = sw1;
    gpio_led.pin = led;

    while (TRUE)
    {
        // xgpio_readreg(fd, rd_buffer, sw0);
        // printf("sw[0]: %s", rd_buffer);
        // xgpio_readreg(fd, rd_buffer, sw1);
        // printf("sw[1]: %s", rd_buffer);

        ret_chk = ioctl(fd, GET_PIN, &gpio_sw[0]);
        printf("gpio read: %d\n", ret_chk);
        ret_chk = ioctl(fd, GET_PIN, &gpio_sw[1]);
        printf("gpio read: %d\n", ret_chk);

        gpio_led.data = TRUE;
        ret_chk = ioctl(fd, SET_PIN, &gpio_led);
        printf("func return: %d\n", ret_chk);
        usleep(500000);
        gpio_led.data = FALSE;
        ret_chk = ioctl(fd, SET_PIN, &gpio_led);
        printf("func return: %d\n", ret_chk);
        usleep(500000);

        // ret_chk = xgpio_writereg(fd, wr_buffer, led, TRUE);
        // printf("bytes written: %d\n", ret_chk);
        // usleep(500000);
        // ret_chk = xgpio_writereg(fd, wr_buffer, led, FALSE);
        // printf("bytes written: %d\n", ret_chk);
        // usleep(500000);
    }

    return 0;
}
