#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define EMIO_BASE 54

ssize_t xgpio_readreg(int file_desc, char *buf, int pin) {
    char *buf_o;
    sprintf(buf_o, "|%d:", pin);
    ssize_t nbytes_o = write(file_desc,buf_o,sizeof(buf_o));
    if (nbytes_o <= 0) {
        printf("fildes %d: failed to send pin to kernel\n");
    }
    
    ssize_t nbytes_i = read(file_desc,buf,sizeof(buf));
    // printf("no. of bytes read: %d\n", nbytes);
    return nbytes_i;
}

ssize_t xgpio_writereg(int file_desc, char *buf, int pin, int value) {
    //snprintf(buf, sizeof(buf), "(%d,%d)", pin, value);
    sprintf(buf, "(%d,%d)", pin, value);
    //char t_buf0[5], t_buf1[2];
    //sprintf(t_buf0, "%d", pin);
    //sprintf(t_buf1, "%d", value);
    //printf("(%s,%s)", t_buf0, t_buf1);
    //itoa(pin, t_buf0, 10);
    //itoa(value, t_buf0, 10);
    //strcpy(buf, "(");
    //strcat(buf, t_buf0);
    //strcat(buf, ",");
    //strcat(buf, t_buf1);
    //strcat(buf, ")");
    printf("%s: buffer size: %d\n", buf, strlen(buf));
    ssize_t nbytes = write(file_desc,buf,strlen(buf)-1);
    return nbytes;
}

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
    ssize_t ret_chk;

    while (TRUE)
    {
        //xgpio_readreg(fd, rd_buffer, sw0);
        //printf("sw[0]: %s", rd_buffer);
        //xgpio_readreg(fd, rd_buffer, sw1);
        //printf("sw[1]: %s", rd_buffer);

        ret_chk = xgpio_writereg(fd, wr_buffer, led, TRUE);
        printf("bytes written: %d\n", ret_chk);
        usleep(500000);
        ret_chk = xgpio_writereg(fd, wr_buffer, led, FALSE);
        printf("bytes written: %d\n", ret_chk);
        usleep(500000);
    }

    return 0;
}
