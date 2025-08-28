#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define XGPIOPS_AXI 0x41210000

int interrupt_handler() {
    
    return 0;
}

int pin_rd(int *addr_space, int pin) {
    if (!pin)       { printf("ERROR: pin not specified\n");     return -1;}
    if (addr_space == NULL) {
        printf("ERROR: Address space cannot be NULL");
    }
    return addr_space[pin - 1];
}

int pin_wr(int *addr_space, int pin, bool state) {
    if (!pin)       { printf("ERROR: pin not specified\n");     return -1;}
    if (!state)     { printf("ERROR: pin state not specified\n");  return -1;}
    // if (state != 0 || 1) {
    //     printf("ERROR: pin state can only be HIGH or LOW\n");
    //     return -1;
    // }

    if (addr_space == NULL) {
        printf("ERROR: Address space cannot be NULL");
    }
    printf("addr base: %d\n", addr_space);

    addr_space[pin] = state;

    return 0;
}


int main () {
    printf("GPIO XOR function invoked.\n");
    int gpio_pin;
    
    // setup device file fifo
    int gpio_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (gpio_fd < 0) {
        printf("device file failed to open\n");
        return gpio_fd;
    }

    unsigned int* virtual_address = mmap(NULL, 65535, PROT_READ, MAP_SHARED, gpio_fd, XGPIOPS_AXI);
    printf("virtual_address: %d \n", virtual_address);
    if (virtual_address == MAP_FAILED) {
        perror("ERROR :\t mmap(): failed to allocate memory");
    }
    printf("dereferenced val: %d\n", *virtual_address);

    // read interrupt requests
    interrupt_handler();


    // configure gpio pins
    gpio_pin = 1;
    // pin_wr(virtual_address, gpio_pin, 1);
    int value = pin_rd(virtual_address, gpio_pin);
    printf("GPIO Pin %d: %x\n\n", gpio_pin, value);
    
    munmap(virtual_address, 65535);
    close(gpio_fd);
    return 0;
}

