/********************************************************************************
 * @file    dma_main.c
 * @author  Raunak Rajpal (rsajpal@bu.edu)
 * 
 * @brief   Entry point to the device driver for PL - DMA control, read/write
 *          transactons and user i/o commands
*********************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>    /* size_t */
#include <linux/fs.h>
#include <linux/fcntl.h>

#include "dma_ioctl.h"
#include "dma_ops.h"


#define DRV_NAME "axidmapl"





