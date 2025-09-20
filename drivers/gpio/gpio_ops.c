// #include "xil_io.h"
// #include "xgpiops_hw.h"

#include "gpio_ops.h"

static volatile void __iomem *gpio_registers = NULL;

uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin) {
    uint32_t *dirm_x 	= (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  	= (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_ro_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA_RO__(bank));
	// REG - OPS
	*dirm_x &= ~(1 << pin);
    *oen_x &= ~(1 << pin);
    
	return *data_ro_x & (1 << pin);
}

void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val) {
    uint32_t *dirm_x = (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  = (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA__(bank));
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);

    if (val)
        *data_x |= (1 << pin);
    else 
        *data_x &= ~(1 << pin);
    
    return;
}


int map_gpio_reg(void) {
    /* define a pointer to map gpio register banks for a full page(?) */
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
	if (gpio_registers == NULL) {
		pr_alert("ERROR: failed to map GPIO memory\n");
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");

    return 0;
}

void unmap_gpio_reg(void) {
    iounmap(gpio_registers);
    return;
}

