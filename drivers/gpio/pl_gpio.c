#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

// #include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h> /* size_t */
#include <linux/fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */

#include "gpio_ioctl.h"
#include "gpio_ops.h"
//#include "gpio_ops.c"

#define DRV_NAME "gpio-pl"

/* ---------------- File-Operation declarations ----------------- */

/* device file-ops functions */
static int gpio_open(struct inode *inode, struct file *file);
static int gpio_release(struct inode *inode, struct file *file);
static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset);
static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset);
static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations gpio_ops = {
    .open    =   gpio_open,
    .release =   gpio_release,
    .read    =   gpio_read,
    .write   =   gpio_write,
	.unlocked_ioctl = gpio_ioctl
};

static struct class *cls;



static uint32_t getPinVal(unsigned int pin) {
    unsigned int bank;

    /* set pin, bank values for MIO/EMIO */
    printk("pin num rx: %d\n", pin);
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", DRV_NAME);
		return -1;
	} else if (pin > 0 && pin < 54)	{
		pin = pin % (GPIO_REG_SIZE * 8);
		bank = pin / (GPIO_REG_SIZE * 8);
	} else if (pin >= 54) {
		pin = (pin - 54) % (GPIO_REG_SIZE * 8);
		bank = 2 + (pin - 54) / (GPIO_REG_SIZE * 8);
	}

    return gpio_pin_rd(bank,pin);
}

static int setPinVal(unsigned int pin, unsigned int val) {
    unsigned int bank;

    /* set pin, bank values for MIO/EMIO */
    printk("pin num rx: %d\n", pin);
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value: (%d,%d)", DRV_NAME, pin, val);
		return -1;
	} else if (pin > 0 && pin < 54)	{
		pin = pin % (GPIO_REG_SIZE * 8);
		bank = pin / (GPIO_REG_SIZE * 8);
	} else if (pin >= 54) {
		pin = (pin - 54) % (GPIO_REG_SIZE * 8);
		bank = 2 + (pin - 54) / (GPIO_REG_SIZE * 8);
	}

    gpio_pin_wr(bank,pin,val);
    return 0;
}


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
    
    /* TODO:
     * fix: the read function
     ***/
    // int retval = copy_to_user(devbuf, buf, databuf_size);
	// if (*offset >= databuf_size || retval) {
	//     pr_alert("copy buffer to user space failed with %d\n", retval);
    //     return 0;
	// }

    // databuf[databuf_size] = '\0';
    // *offset += databuf_size;
    // pr_info("%s: reading %ld bytes to kernel buffer: %s\n", DEVICE_NAME, databuf_size, databuf);

    return databuf_size;
}

static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
    /* buffer overflow */
    unsigned int databuf_size = buf_size;
	if (databuf_size >= DEVFS_BUFMAX_SIZE)
		databuf_size = DEVFS_BUFMAX_SIZE - 1;

    pr_info("%s: Writinging to device file %s:\t%s\n", DRV_NAME, DEVICE_FILE_NAME, DEVICE_PATH);
    
    /* TODO:
     * fix: the write function
     ***/
    /* check if device buffer gets successfully passed from user to k-space */
    // int ret_val = copy_from_user(databuf, (uint32_t *)devbuf, databuf_size);
    // if (ret_val) {
    //     pr_alert("ERROR: buffer overflow: %d bytes failed\n", ret_val);
    //     return -1;
	// }

	// databuf[databuf_size] = '\0';
	// *offset += databuf_size;
    // pr_info("%s: writing %ld bytes to kernel buffer: %s\n", DRV_NAME, databuf_size, databuf);

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
	        
	        pr_info("databuf->pin,val: %d,%d\n", databuf.pin, databuf.data);

            ret_val = getPinVal(databuf.pin);
            if (ret_val < 0) {
                pr_alert("ERROR: %s: Undefined pin value\n", DRV_NAME);
            }
            
            return ret_val;
        
        case SET_PIN:
            ret_val = copy_from_user(&databuf, (struct gpio_ioctl *)arg, databuf_size);
            if (ret_val) {
	        	pr_alert("ERROR: %s: buffer overflow: %d bytes failed\n", DRV_NAME, ret_val);
	        	return -1;
	        }
	        
	        pr_info("INFO: %s: databuf->pin,val: %d,%d\n", DRV_NAME, databuf.pin, databuf.data);
	        
	        ret_val = setPinVal(databuf.pin, databuf.data);
	        if (ret_val < 0) {
	            pr_alert("ERROR: %s: Undefined pin value2: %d\n", DRV_NAME, ret_val);
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

    /* Register the character device (at least try) */
    int ret_val = register_chrdev(MAJOR_NUM, DRV_NAME, &gpio_ops);
    if (ret_val < 0) {
		pr_err("ERROR: %s: Failed to initialize char device\n", DRV_NAME);
	    return ret_val;
	}

    pr_info("%s: device %s registered succesfully at %s\n", DRV_NAME, DEVICE_FILE_NAME, DEVICE_PATH);

    cls = class_create(DRV_NAME);
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);                                                           

    /* create an entry in the sys-fs */
	// int dev_chk = alloc_chrdev_region(&dev, 0, dev_count, DRV_NAME);
	// if (dev_chk == NULL) {
	// 	pr_error("ERROR: %s: Failed to initialize char device\n", DRV_NAME);
	//     return -ENOMEM;
	// }

	// ioctl_major = MAJOR(dev);
	// cdev_init(&ioctl_gpio_dev, gp_dev, dev_count);
	// if (gpio_dev == NULL) {
	// 	pr_error("ERROR: %s: Failed to initialize char device\n", DRV_NAME);
	//     unregister_chrdev_region(gp_dev, dev_count);
	// 	return -ENOMEM;
	// }

    printk("%s: ", DRV_NAME);
    map_gpio_reg();

    return 0;
}

static void __exit gpio_driver_exit(void) {
    unmap_gpio_reg();

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
MODULE_DESCRIPTION("Test gpio device drivers for Zynq-7000 series SoCs");
MODULE_VERSION("1.0");

