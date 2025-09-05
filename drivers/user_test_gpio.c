#include <stdio.h>
#include <stdlib.h>



int main() {

    int fd = open("/proc/gpio_proc", O_RDWR);
    if (fd < 0) {
        perror("Failed to open file\n\n", fd);
        return 1;
    }
    
    char buf[1025];
    ssize_t nbytes = read(fd,buf,sizeof(buf));
    printf("no. of bytes read: %d\n", nbytes);

    return 0;
}
