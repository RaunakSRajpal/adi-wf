#include "xil_io.h"
#include "xgpiops_hw.h"
#include "gpio_ops.h"

static volatile void __iomem *gpio_registers = NULL;

static inline void map_gpio_reg() {
    /* define a pointer to map gpio register banks for a full page*/
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
	if (gpio_registers == NULL) {
		pr_alert("ERROR: failed to map GPIO memory\n");
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");

    return;
}

static inline void unmap_gpio_reg() {
    iounmap(gpio_registers);
}

static inline uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin) {}

static inline void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val) {}

