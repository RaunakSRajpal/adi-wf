#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h> /* size_t */
#include <linux/fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */

#include <asm/io.h>
// #include "gpiodev.h"

/* ---------------- Global variables/macros --------------- */
#define XGPIOPS_BASE_ADDR       (uint32_t)0xE000A000
#define XGPIOPS_DATA__(X)       (uint32_t)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    (uint32_t)0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       (uint32_t)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        (uint32_t)0x00000208 + (X*4 << 1)

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 4
#define MAJOR_NUM 100
 
#define IOCTL_SET_PIN _IOWR(MAJOR_NUM, 2, int)

#define DEVICE_NAME "gpio_driver_ioctl"
#define DEVFS_BUFMAX_SIZE 1024
static char databuf[DEVFS_BUFMAX_SIZE];
static unsigned long databuf_size = 0;

static volatile void __iomem *gpio_registers = NULL;

// static struct class *cls;
static unsigned int dev_count = 1;
static int ioctl_num = 0;
static unsigned int ioctl_major = 0;

static struct cdev ioctl_gpio_dev;
unsigned int pin;

/********************************************************** */

/* ---------------- Function declarations ----------------- */

static void gpio_on(unsigned int bank, unsigned int pin);
static void gpio_off(unsigned int bank, unsigned int pin);
/* module functions */
static int  __init gpio_driver_init(void);
static void __exit gpio_driver_exit(void);
/* device file-ops functions */
static size_t gpio_open(struct inode *inode, struct file *file);
static size_t gpio_release(struct inode *inode, struct file *file);
static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset);
static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset);
static ssize_t gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/********************************************************** */


static const struct file_operations gpio_ops = {
    .open    =   gpio_open,
    .release =   gpio_release,
    .read    =   gpio_read,
    .write   =   gpio_write,
	.unlocked_ioctl = gpio_ioctl
};


/* ----------------- Function definitions ----------------- */

static void gpio_on(unsigned int bank, unsigned int pin) {
    uint32_t *dirm_x = (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  = (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA__(bank));
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x |= (1 << pin);
    return;
}

static void gpio_off(unsigned int bank, unsigned int pin) {
    uint32_t *dirm_x = (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  = (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA__(bank));
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x &= ~(1 << pin);
    return;
}

static int gpio_reg_read(unsigned int bank, unsigned int pin) {
	uint32_t *dirm_x 	= (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  	= (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_ro_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA_RO__(bank));
	// REG - OPS
	*dirm_x &= ~(1 << pin);
    *oen_x &= ~(1 << pin);
	return *data_ro_x & (1 << pin);
}

/* This is called whenever a process attempts to open the device file */ 
static int gpio_open(struct inode *inode, struct file *file) 
{ 
    pr_info("device_open(%p)\n", file); 
 
    return 0;
}

static int gpio_release(struct inode *inode, struct file *file) 
{ 
    pr_info("device_release(%p,%p)\n", inode, file); 
 
    return 0;
}

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Reading Device\n");
	int bank, value;
	char buf;

	/* set pin, bank values for MIO/EMIO */
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", DEVICE_NAME);
		return databuf_size;
	} else if (pin > 0 && pin < 54)	{
		pin = pin % (GPIO_REG_SIZE * 8);
		bank = pin / (GPIO_REG_SIZE * 8);
	} else if (pin >= 54) {
		pin = (pin - 54) % (GPIO_REG_SIZE * 8);
		bank = 2 + (pin - 54) / (GPIO_REG_SIZE * 8);
	}

	/* read gpio register and pass to user buffer */
	value = gpio_reg_read(bank,pin);
	if (value)	buf = "1";
	else		buf = "0";
	int len = sizeof(buf);

	if (*offset >= len || copy_to_user(devbuf, buf, len)) {
		pr_alert("copy buffer to user space failed\n");
		return 0;
	}
	pr_info("%s: read process: %s\n", DEVICE_NAME, file->f_path.dentry->d_name.name);
	*offset += len;

    return len;
}

static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Writing to Device\n");
	databuf_size = buf_size;

	/* buffer overflow */
	if (databuf_size >= DEVFS_BUFMAX_SIZE)
		databuf_size = DEVFS_BUFMAX_SIZE - 1;

	/* check if device buffer gets successfully passed from user to k-space */
    int ret_val = copy_from_user(databuf, devbuf, databuf_size);
    if (ret_val) {
		pr_alert("ERROR: buffer overflow: %d bytes failed\n", ret_val);
		return -1;
	}

	databuf[databuf_size] = '\0';
	*offset += databuf_size;
	printk("%s: writing %ld bytes to kernel buffer: %s\n", DEVICE_NAME, databuf_size, databuf);
	
	/* scan and check the buffer entry from user */
	int pin, bank, value;
	if (sscanf(databuf, "(%d,%d)", &pin, &value) != 2) {
		printk("ERROR: %s: Inproper data format\n", DEVICE_NAME);
		return databuf_size;
	}

	/* set pin, bank values for MIO/EMIO */
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", DEVICE_NAME);
		return databuf_size;
	} else if (pin > 0 && pin < 54)	{
		pin = pin % (GPIO_REG_SIZE * 8);
		bank = pin / (GPIO_REG_SIZE * 8);
	} else if (pin >= 54) {
		pin = (pin - 54) % (GPIO_REG_SIZE * 8);
		bank = 2 + (pin - 54) / (GPIO_REG_SIZE * 8);
	}

	/* function call to turn on/off gpio pin */
	if (value)	gpio_on((uint32_t)bank, (uint32_t)pin);
	else		gpio_off((uint32_t)bank, (uint32_t)pin);

    // printk("data input: %s\n", databuf);
    return databuf_size;
}

static ssize_t gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	unsigned int val = 0;
	int bank
	char buffer[DEVFS_BUFMAX_SIZE];

	switch (cmd) {
	case IOCTL_SET_PIN: {
		pin = arg;

		if (pin >= GPIO_PIN_MAX) {
			printk("ERROR: %s: Undefined pin value", DEVICE_NAME);
			return databuf_size;
		} else if (pin > 0 && pin < 54)	{
			pin = pin % (GPIO_REG_SIZE * 8);
			bank = pin / (GPIO_REG_SIZE * 8);
		} else if (pin >= 54) {
			pin = (pin - 54) % (GPIO_REG_SIZE * 8);
			bank = 2 + (pin - 54) / (GPIO_REG_SIZE * 8);
		}

		/* read gpio register and pass to user buffer */
		val = gpio_reg_read(bank,pin);
		if (val)	buffer = "1";
		else		buffer = "0";
		int len = sizeof(buffer);
		return val;
	}
	
	default:
		pr_alert("IOCTL: ioctl number does not match\n");
		val = -EFAULT;
		break;
	}

	return val;
}

static int __init gpio_driver_init(void) {
	printk("%s: initialising driver...\n", DEVICE_NAME);
	/* clear data buffer for kernel entry */
	memset(databuf, 0x0, sizeof(databuf));

	dev_t dev;
	
	/* define a pointer to map gpio register banks for a full page*/
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
	if (gpio_registers == NULL) {
		pr_alert("ERROR: %s: failed to map GPIO memory\n", DEVICE_NAME);
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");
	
	/* create an entry in the proc-fs */
	int dev_chk = alloc_chrdev_region(&dev, 0, dev_count, DEVICE_NAME);
	if (dev_chk == NULL) {
		pr_error("ERROR: %s: Failed to initialize char device\n", DEVICE_NAME);
	    return -ENOMEM;
	}

	ioctl_major = MAJOR(dev);
	cdev_init(&ioctl_gpio_dev, dev, dev_count);
	if (gpio_dev == NULL) {
		pr_error("ERROR: %s: Failed to initialize char device\n", DEVICE_NAME);
	    unregister_chrdev_region(dev, dev_count);
		return -ENOMEM;
	}

	pr_info("%s: device regitsered succesfully\n", DEVFS_BUFMAX_SIZE);
	return 0;
}

static void __exit gpio_driver_exit(void) {
	iounmap(gpio_registers);

	dev_t dev = MKDEV(ioctl_major, 0);
	cdev_del(&ioctl_gpio_dev);
	unregister_chrdev_region(dev,dev_count);

	pr_info("%s: process removed\n", DEVICE_NAME);
	return;
}

/********************************************************** */


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rsrajpal@bu.edu");
MODULE_DESCRIPTION("Test gpio device drivers for Zynq-7000 series SoCs");
MODULE_VERSION("1.0");
