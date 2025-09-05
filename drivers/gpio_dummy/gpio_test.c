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
// #include <string.h>

/* ---------------- Global variables/macros --------------- */
#define XGPIOPS_BASE_ADDR       0xE000A000
#define XGPIOPS_DATA__(X)       0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        0x00000208 + (X*4 << 1)

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 32

#define PROCFS_NAME "gpio_driver"
#define PROCFS_BUFMAX_SIZE 1024
static uint32_t *gpio_registers = NULL;
static struct proc_dir_entry *gpio_proc = NULL;
static char databuf[PROCFS_BUFMAX_SIZE];
static unsigned long databuf_size = 0;

/********************************************************** */

/* ---------------- Function declarations ----------------- */

static void gpio_on(unsigned int bank, unsigned int pin);
static void gpio_off(unsigned int bank, unsigned int pin);
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
    .proc_read    =   gpio_read,
    .proc_write   =   gpio_write
};


/* ----------------- Function definitions ----------------- */

static void gpio_on(unsigned int bank, unsigned int pin) {
    unsigned int *dirm_x = gpio_registers + XGPIOPS_DIRM__(bank);
    unsigned int *oen_x  = gpio_registers + XGPIOPS_OEN__(bank);
    unsigned int *data_x = gpio_registers + XGPIOPS_DATA__(bank);
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x |= (1 << pin);
    return;
}

static void gpio_off(unsigned int bank, unsigned int pin) {
    unsigned int *dirm_x = gpio_registers + XGPIOPS_DIRM__(bank);
    unsigned int *oen_x  = gpio_registers + XGPIOPS_OEN__(bank);
    unsigned int *data_x = gpio_registers + XGPIOPS_DATA__(bank);
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);
    *data_x &= ~(1 << pin);
    return;
}

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Reading Device\n");
	// sprintf("", );

    return (*offset || copy_to_user(devbuf, "Welcome!", 8)) ? 0 : 8;
}

static ssize_t gpio_write(struct file *file, const char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Writing to Device\n");
	databuf_size = buf_size;

	/* buffer overflow */
	if (databuf_size >= PROCFS_BUFMAX_SIZE)
		databuf_size = PROCFS_BUFMAX_SIZE - 1;

	/* check if device buffer gets successfully passed from user to k-space */
    int ret_val = copy_from_user(databuf, devbuf, databuf_size);
    if (ret_val) {
		pr_alert("ERROR: buffer overflow: %d bytes failed\n", ret_val);
		return -1;
	}

	databuf[databuf_size] = '\0';
	*offset += databuf_size;
	printk("%s: writing %ld bytes to kernel buffer: %s\n", PROCFS_NAME, databuf_size, databuf);
	
	/* scan and check the buffer entry from user */
	int pin, value;
	if (sscanf(databuf, "(%d,%d)", &pin, &value) != 2) {
		printk("ERROR: %s: Inproper data format\n", PROCFS_NAME);
		return databuf_size;
	}

	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", PROCFS_NAME);
		return databuf_size;
	}

	/* function call to turn on/off gpio pin */
	if (value)
		gpio_on((uint32_t)(pin / GPIO_REG_SIZE), (uint32_t)(pin % GPIO_REG_SIZE));
	else
		gpio_off((uint32_t)(pin / GPIO_REG_SIZE), (uint32_t)(pin % GPIO_REG_SIZE));

    // printk("data input: %s\n", databuf);

    return buf_size;
}

static int __init gpio_driver_init(void) {
	printk("%s: initialising driver...\n", PROCFS_NAME);
	/* clear data buffer for kernel entry */
	memset(databuf, 0x0, sizeof(databuf));
	
	/* define a pointer to map gpio register banks for a full page*/
	gpio_registers = (uint32_t*)ioremap(XGPIOPS_BASE_ADDR, PAGE_SIZE);
	if (gpio_registers == NULL) {
		printk("ERROR: %s: failed to map GPIO memory\n", PROCFS_NAME);
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");
	
	/* create an entry in the proc-fs */
	gpio_proc = proc_create(PROCFS_NAME, 0666, NULL, &gpio_ops);
	if (gpio_proc == NULL) {
		pr_alert("ERROR: %s: Failed to initialize process\n", PROCFS_NAME);
	    return -ENOMEM;
	}

	pr_info("%s: process succesfully created\n", PROCFS_NAME);
	return 0;
}

static void __exit gpio_driver_exit(void) {
	iounmap(gpio_registers);
	proc_remove(gpio_proc);
	pr_info("%s: process removed\n", PROCFS_NAME);
	return;
}

/********************************************************** */


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rsrajpal@bu.edu");
MODULE_DESCRIPTION("Test gpio device drivers for Zynq-7000 series SoCs");
MODULE_VERSION("1.0");
