#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */

#include <asm/io.h>

/* ---------------- Global variables/defines ---------------- */
#define XGPIOPS_BASE_ADDR       (uint32_t*)0xE000A000
#define XGPIOPS_DATA__(X)       (uint32_t*)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    (uint32_t*)0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       (uint32_t*)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        (uint32_t*)0x00000208 + (X*4 << 1)

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 32

#define PROCFS_NAME "gpio_driver"
#define PROCFS_BUF_SIZE 1024
static unsigned int *gpio_registers = NULL;
static struct proc_dir_entry *gpio_proc = NULL;
static char databuf[PROCFS_BUF_SIZE] = {0};

/********************************************************** */

/* ---------------- Function declarations ----------------- */

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

/********************************************************** */


static const struct proc_ops gpio_ops = {
    // .proc_op   =   gpio_open,
    // .proc_re   =   gpio_release,
    .proc_read   =   gpio_read,
    .proc_write   =   gpio_write
};


/* ----------------- Function definitions ----------------- */

static int gpio_on(unsigned int bank, unsigned int pin) {
    unsigned int *dirm_x = gpio_registers + (uint32_t*)XGPIOPS_DIRM__(bank);
    unsigned int *oen_x  = gpio_registers + (uint32_t*)XGPIOPS_OEN__(bank);
    unsigned int *data_x = gpio_registers + (uint32_t*)XGPIOPS_DATA_RO__(bank);
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x |= (1 << pin);
}

static int gpio_off(unsigned int bank, unsigned int pin) {
    unsigned int *dirm_x = gpio_registers + (uint32_t*)XGPIOPS_DIRM__(bank);
    unsigned int *oen_x  = gpio_registers + (uint32_t*)XGPIOPS_OEN__(bank);
    unsigned int *data_x = gpio_registers + (uint32_t*)XGPIOPS_DATA_RO__(bank);
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x &= ~(1 << pin);
}

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Reading Device\n");
	static char read_databuf[1025] = {0};

    return (*offset || copy_to_user(devbuf, "Welcome!", 8)) ? 0 : 8;
}

static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Writing to Device\n");
	int pin;
	bool value;
	int ret_val;

	if (buf_size >= PROCFS_BUF_SIZE)
		buf_size = PROCFS_BUF_SIZE - 1;

    ret_val = copy_from_user(databuf, devbuf, buf_size);

    if (ret_val) {
		printk("ERROR: buffer overflow: %d\n", ret_val);
		return -1;
	}
	else {
		printk("%s: writing %d bytes to kernel buffer\n", PROCFS_NAME, ret_val);
		return 0;
	}

	/* scan and check the buffer entry from user */
	if (sscanf(databuf, "%d,%d", &pin, &value) != 2) {
		printk("ERROR: %s: Inproper data format\n", PROCFS_NAME);
		return buf_size;
	}

	if (pin > GPIO_PIN_MAX - 1) {
		printk("ERROR: %s: Undefined pin value", PROCFS_NAME)
		return buf_size;
	}

	if (value)
		gpio_on((pin / GPIO_REG_SIZE), (pin % GPIO_REG_SIZE));
	else (value)
		gpio_off((pin / GPIO_REG_SIZE), (pin % GPIO_REG_SIZE));

    // printk("data input: %s\n", databuf);

    return buf_size;
}

static int __init gpio_driver_init(void) {
	printk("initialising driver: %s...\n", PROCFS_NAME);
	/* clear data buffer for kernel entry */
	memset(databuf, 0x0, sizeof(databuf));
	
	/* define a pointer to map gpio register banks for a full page*/
	gpio_registers = (unsigned int*)ioremap(XGPIOPS_BASE_ADDR, PAGE_SIZE);
	if (gpio_registers == NULL) {
		printk("ERROR: driver %s: failed to map GPIO memory\n", PROCFS_NAME);
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");
	
	/* create an entry in the proc-fs */
	gpio_proc = proc_create(PROCFS_NAME, 0666, NULL, &gpio_ops);
	if (gpio_proc == NULL) {
		printk("ERROR: process %s: Failed to initialize\n", PROCFS_NAME);
	    return -1;
	}

	printk("%s: process succesfully created\n", PROCFS_NAME);

	return 0;
}

static void __exit gpio_driver_exit(void) {
	printk("killing process.. %s removed\n", PROCFS_NAME);
	iounmap(gpio_registers);
	proc_remove(gpio_proc);
	return;
}

/********************************************************** */


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rsrajpal@bu.edu");
MODULE_DESCRIPTION("Test gpio device drivers for Zynq-7035");
MODULE_VERSION("1.0");