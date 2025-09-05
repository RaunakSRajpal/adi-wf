#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */

#include <asm/io.h>
// #include <string.h>

/* ---------------- Global variables/macros --------------- */
#define XGPIOPS_BASE_ADDR       (uint32_t)0xE000A000
#define XGPIOPS_DATA__(X)       (uint32_t)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    (uint32_t)0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       (uint32_t)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        (uint32_t)0x00000208 + (X*4 << 1)

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 4

#define PROCFS_NAME "gpio_driver"
#define PROCFS_BUFMAX_SIZE 1024
static volatile void __iomem *gpio_registers = NULL;
static struct proc_dir_entry *gpio_proc = NULL;
static char databuf[PROCFS_BUFMAX_SIZE];
static unsigned long databuf_size = 0;
static int pin_io = 1;
static int pin;

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

static ssize_t gpio_read(struct file *file, char __user *devbuf, size_t buf_size, loff_t *offset) {
    printk("GPIO_XOR: Reading Device\n");
	int bank, value;
	char buf;

	/* set pin, bank values for MIO/EMIO */
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", PROCFS_NAME);
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
	pr_info("%s: read process: %s\n", PROCFS_NAME, file->f_path.dentry->d_name.name);
	*offset += len;

    return len;
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
	int bank, value;
	if (sscanf(databuf, "(%d,%d)", &pin, &value) != 2) {
	    if (sscanf(databuf, "|%d:", &pin) != 2) {
	        printk("ERROR: %s: Inproper data format\n", PROCFS_NAME);
		    return databuf_size;
	    }
	    pin_io = 0;
	}

    if (pin_io) {
	/* set pin, bank values for MIO/EMIO */
	if (pin >= GPIO_PIN_MAX) {
		printk("ERROR: %s: Undefined pin value", PROCFS_NAME);
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
	}

    // printk("data input: %s\n", databuf);
    return databuf_size;
}

static int __init gpio_driver_init(void) {
	printk("%s: initialising driver...\n", PROCFS_NAME);
	/* clear data buffer for kernel entry */
	memset(databuf, 0x0, sizeof(databuf));
	
	/* define a pointer to map gpio register banks for a full page*/
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
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
