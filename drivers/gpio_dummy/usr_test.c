#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void xgpio_readreg(int file_desc, char *buf, int pin) {
    ssize_t nbytes = read(file_desc,buf,sizeof(buf));
    // printf("no. of bytes read: %d\n", nbytes);
    return;
}

void xgpio_writereg(int file_desc, char *buf, int pin, int value) {
    sprintf(buf, "(%d,%d)", pin, value);
    ssize_t nbytes = write(file_desc,buf,sizeof(buf));
    return;
}

int main() {

    int fd = open("/proc/gpio_proc", O_RDWR);
    if (fd < 0) {
        perror("Failed to open file\n\n", fd);
        return 1;
    }

    char rd_buffer[1024];
    char wr_buffer[1024];
    int led = 4;

    while (true)
    {
        // xgpio_readreg(fd, rd_buffer, 8);
        // printf("sw[0]: %s", rd_buffer);
        // xgpio_readreg(fd, rd_buffer, 9);
        // printf("sw[0]: %s", rd_buffer);
        xgpio_writereg(fd, wr_buffer, led, true);
        usleep(500000);
        xgpio_writereg(fd, wr_buffer, led, false);
        usleep(500000);
    }

    return 0;
}
