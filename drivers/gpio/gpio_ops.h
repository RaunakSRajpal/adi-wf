/*******************************************
 * @file gpio_ops.h
 * @author Raunak Rajpal (rsajpal@bu.edu)
 * @brief   captures the register operations
 *          required for GP I/O
********************************************/

#ifndef GPIO_OPS_H
#define GPIO_OPS_H

// #include "xil_types.h"
// #include "xil_assert.h"
// #include "xil_io.h"
// #include "xgpiops_hw.h"
#include <asm/io.h>
#include <linux/module.h>

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 4

/* Register definitons:
 * The mocros declared below use physical addresses specified from
 * Vivado Address Editor, post implementation of design
**/

#define XGPIOPS_BASE_ADDR       (uint32_t)0xE000A000
#define XGPIOPS_DATA__(X)       (uint32_t)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    (uint32_t)0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       (uint32_t)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        (uint32_t)0x00000208 + (X*4 << 1)

static volatile void __iomem *gpio_registers = NULL;


static uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin);
static void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val);

static inline int map_gpio_reg(void) {
    /* define a pointer to map gpio register banks for a full page(?) */
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
	if (gpio_registers == NULL) {
		pr_alert("ERROR: failed to map GPIO memory\n");
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");

    return 0;
}

static inline void unmap_gpio_reg(void) {
    iounmap(gpio_registers);
    return;
}

#endif
