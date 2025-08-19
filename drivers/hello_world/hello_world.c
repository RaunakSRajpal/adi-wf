#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h> 	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> 	/* O_ACCMODE */

#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("raunak rajpal<rsrajpal@bu.edu>");
MODULE_DESCRIPTION("Test hello world device drivers for Zynq-7035");
MODULE_VERSION("1.0");

/* Global variables/defines */
//----TODO----//


    /* Function declarations */
/* module function declarations */
static int  __init gpio_driver_init(void);
static void __exit gpio_driver_exit(void);
/* file operation functions */
ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset);
ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset);

static const struct proc_ops gpio_ops = {
    // .proc_op   =   gpio_open,
    // .proc_re   =   gpio_release,
    .proc_read   =   gpio_read,
    .proc_write   =   gpio_write
};


/* -------- Function definitions -------- */
ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
   	printk("GPIO_XOR: Reading Device\n");
	return copy_to_user(devbuf, "Welcome!", 8) ? 0 : 8;
}

ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
	printk("GPIO_XOR: Writin gto Device\n");
	return buf_size;
}

static int __init gpio_driver_init(void) {
	printk("initialising gpio driver!\n");
	return 0;
}

static void __exit gpio_driver_exit(void) {
	printk("killing process.. gpio driver released!\n");
	return ;
}


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

