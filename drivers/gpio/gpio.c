#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */

#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rsrajpal@bu.edu");
MODULE_DESCRIPTION("Test gpio device drivers for Zynq-7035");
MODULE_VERSION("1.0");

/* Global variables/defines */
//----TODO----//
#define XGPIOPS_BASE_ADDR   (uint32_t*)0xE000A000
#define XGPIOPS_DATA_(X)    (uint32_t*)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO_(X) (uint32_t*)0x00000060 + (X*4)
#define XGPIOPS_DIRM_(X)    (uint32_t*)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN_(X)     (uint32_t*)0x00000208 + (X*4 << 1)

static unsigned int *gpio_registers = NULL;
static struct proc_dir_entry *gpio_proc = NULL;
static char databuf[1025] = {0};


/* ---------------- Function declarations ---------------- */

static int gpio_on(unsigned int pin);
static int gpio_off(unsigned int pin);
/* module functions */
static int  __init gpio_driver_init(void);
static void __exit gpio_driver_exit(void);
/* device file-ops functions */
// static size_t gpio_open(struct inode *inode, struct file *file);
// static size_t gpio_release(struct inode *inode, struct file *file);
static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset);
static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset);

static const struct proc_ops gpio_ops = {
    // .proc_op   =   gpio_open,
    // .proc_re   =   gpio_release,
    .proc_read   =   gpio_read,
    .proc_write   =   gpio_write
};


/* ----------------- Function definitions ----------------- */

static int gpio_on(unsigned int pin) {
    unsigned int *dirm_2 = gpio_registers + (uint32_t*)XGPIOPS_DIRM_(2);
    unsigned int *data_2 = gpio_registers + (uint32_t*)XGPIOPS_DATA_RO_(2);
    // TODO -- REG OPS
    *dirm_2 |= (1 << pin);
    *data_2 
    return 0;
}

static int gpio_off(unsigned int pin) {
    // TODO -- REG OPS
    return 0;
}

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Reading Device\n");

    return copy_to_user(devbuf, "Welcome!", 8) ? 0 : 8;
}

static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Writing to Device\n");
    int ret;
    ret = copy_from_user(databuf, devbuf, 8);
    if (ret)
        return 0;
        
    printk("data input: %s\n", databuf);

    return buf_size;
}

static int __init gpio_driver_init(void) {
	printk("initialising gpio driver!\n");
	
	memset(databuf, 0x0, sizeof(databuf));
	
	gpio_registers = (unsigned int*)ioremap(XGPIOPS_BASE_ADDR, PAGE_SIZE);
	if (gpio_registers == NULL)
	{
		printk("Failed to map GPIO memory to driver\n");
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");
	
	// create an entry in the proc-fs
	gpio_proc = proc_create("gpio_driver", 0666, NULL, &gpio_ops);
	if (gpio_proc == NULL) {
	    return -1;
	}

	return 0;
}

static void __exit gpio_driver_exit(void) {
	printk("killing process.. gpio driver released!\n");
	iounmap(gpio_registers);
	proc_remove(gpio_proc);
	return;
}


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
