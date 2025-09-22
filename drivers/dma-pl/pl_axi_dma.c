#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h> /* size_t */
#include <linux/fs.h>
#include <linux/fcntl.h>

#include "dma_ioctl.h"
#include "gpio_ops.h"


#define DRV_NAME "axidmapl"

/* ---------------- File-Operation declarations ----------------- */

/* device file-ops functions */
static int gpio_open(struct inode *inode, struct file *file);
static int gpio_release(struct inode *inode, struct file *file);
static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset);
static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations gpio_ops = {
    .open    =   gpio_open,
    .release =   gpio_release,
    .read    =   gpio_read,
	.unlocked_ioctl = gpio_ioctl
};

static struct class *cls;
/**************************************************************** */



static int gpio_open(struct inode *inode, struct file *file) {
    pr_info("%s: Major: %d Minor: %d \t file->f_pos: %lld\n", DRV_NAME, imajor(inode), iminor(inode), file->f_pos);
    pr_info("%s: device file mode: 0x%x \t flags: 0x%x\n", DRV_NAME, file->f_mode);

    return 0;
}

static int gpio_release(struct inode *inode, struct file *file) {
    pr_info("%s: device file closed\n", DRV_NAME);
    return 0;
}

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    /* buffer overflow */
    unsigned int databuf_size = 0;
    // TODO: initialise databuf_size
    if (databuf_size >= DEVFS_BUFMAX_SIZE)
		databuf_size = DEVFS_BUFMAX_SIZE - 1;
    
    pr_info("%s: Reading from device file %s:\t%s\n", DRV_NAME, DEVICE_FILE_NAME, DEVICE_PATH);
    
    /** TODO:
     * fix: missing actual read function
    **/

    return databuf_size;
}

static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct gpio_ioctl databuf;
    unsigned int databuf_size = sizeof(databuf);

    switch (cmd)
    {
        int ret_val;
        case RD_DEV:
            // gpio_read(file, );
            break;

        case WR_DEV:
            // gpio_write(file, );
            break;

        case GET_PIN:
            ret_val = copy_from_user(&databuf, (struct gpio_ioctl *)arg, databuf_size);
            if (ret_val) {
	        	pr_alert("ERROR: %s: buffer overflow: %d bytes failed\n", DRV_NAME, ret_val);
	        	return -1;
	        }

            ret_val = getPinVal(databuf.pin);
            if (ret_val < 0) {
                pr_alert("ERROR: %s: Undefined pin value: %d\n", DRV_NAME, ret_val);
            }

            return ret_val;

        case SET_PIN:
            ret_val = copy_from_user(&databuf, (struct gpio_ioctl *)arg, databuf_size);
            if (ret_val) {
	        	pr_alert("ERROR: %s: buffer overflow: %d bytes failed\n", DRV_NAME, ret_val);
	        	return -1;
	        }

	        ret_val = setPinVal(databuf.pin, databuf.data);
	        if (ret_val < 0) {
	            pr_alert("ERROR: %s: Undefined pin value: %d\n", DRV_NAME, ret_val);
	        }

            return ret_val;

        default:
            pr_err("%s: Invalid i/o control signal. Aborting device operation\n", DRV_NAME);
            return -1;
    }
    
    return databuf_size;
}


static int  __init gpio_driver_init(void) {
    printk("%s: initialising driver...\n", DRV_NAME);

    // dev_t gp_dev;

    /* Register the character device */
    int ret_val = register_chrdev(MAJOR_NUM, DRV_NAME, &gpio_ops);
    if (ret_val < 0) {
		pr_err("ERROR: %s: Failed to initialize char device\n", DRV_NAME);
	    return ret_val;
	}

    pr_info("%s: device %s registered succesfully at %s\n", DRV_NAME, DEVICE_FILE_NAME, DEVICE_PATH);

    cls = class_create(DRV_NAME);
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);                                                           

    map_axi_dma();

    return 0;
}

static void __exit gpio_driver_exit(void) {
    unmmap_axi_dma();

    device_destroy(cls, MKDEV(MAJOR_NUM, 0));  
    class_destroy(cls);
    unregister_chrdev(MAJOR_NUM, DRV_NAME);

	// dev_t gp_dev = MKDEV(ioctl_major, 0);
	// cdev_del(&ioctl_gpio_dev);
	// unregister_chrdev_region(gp_dev,dev_count);

	pr_info("%s: process removed\n", DRV_NAME);

    return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rsrajpal@bu.edu");
MODULE_DESCRIPTION("DMA device driver for HP ports, Zynq-7000 series SoCs");
MODULE_VERSION("1.0");
