/*************************************************
 * @file    dma_ioctl.h
 * @author  Raunak Rajpal (rsajpal@bu.edu)
 * 
 * @brief   ioctl patrameters and definitions for
 *          character dma driver, PL-I/O device
**************************************************/

#ifndef GPIO_IOCTL_H
#define GPIO_IOCTL_H

#include <linux/ioctl.h>

struct dma_ioctl
{
    uint8_t pin;
    uint8_t data;
};


#define MAJOR_NUM 100

#define RD_DEV  _IOR(MAJOR_NUM, 0, int32_t *)
#define WR_DEV  _IOW(MAJOR_NUM, 1, int32_t *)
#define GET_PIN _IOWR(MAJOR_NUM, 2, struct gpio_ioctl *)
#define SET_PIN _IOW(MAJOR_NUM, 3, struct gpio_ioctl *)

/* The name of the device file */ 
#define DEVICE_FILE_NAME "axidma_dev"
#define DEVICE_PATH "/dev/axidma_dev"
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID      // check the correct use of this macro 
#define DEVFS_BUFMAX_SIZE 1024

#endif

