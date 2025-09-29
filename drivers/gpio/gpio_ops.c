// #include "xil_io.h"
// #include "xgpiops_hw.h"

#include "gpio_ops.h"

static volatile void __iomem *gpio_registers = NULL;
static volatile void __iomem *gpio_axi_reg = NULL;


uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin) {
    uint32_t* dirm_x 	= (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t* oen_x  	= (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t* data_ro_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA_RO__(bank));
	// REG - OPS
	*dirm_x &= ~(1 << pin);
    *oen_x &= ~(1 << pin);
    
    int ret = *data_ro_x & (1 << pin);
    // printk("%d, %d, %d \t0x%x\n", ret, bank, pin, gpio_registers);
    // printk("0x%x 0x%x 0x%x\n", dirm_x, oen_x, data_ro_x);
    // printk("0x%x 0x%x 0x%x\n", *dirm_x, *oen_x, *data_ro_x);
    
	return ret;
}

void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val) {
    uint32_t* dirm_x = (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t* oen_x  = (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t* data_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA__(bank));
    // REG - OPS
    *dirm_x |= (1 << pin);
    *oen_x |= (1 << pin);

    if (val)
        *data_x |= (1 << pin);
    else 
        *data_x &= ~(1 << pin);
    
    // printk("%d, %d \t0x%x\n", bank, pin, gpio_registers);
    // printk("0x%x 0x%x 0x%x\n", dirm_x, oen_x, data_x);
    // printk("0x%x 0x%x 0x%x\n", *dirm_x, *oen_x, *data_x);
    
    return;
}


int map_gpio_reg(void) {
    /* define a pointer to map gpio register banks for a full page(?) */
	gpio_registers = ioremap(XGPIOPS_BASE_ADDR, GPIO_REG_SIZE);
	if (gpio_registers == NULL) {
		pr_alert("ERROR: failed to map GPIO memory: 0x%x\n", gpio_registers);
		return -1;
	}
	
	printk("%s: Successfully mapped in GPIO memory at: 0x%x\n", "gpiopl", gpio_registers);

    return 0;
}

void unmap_gpio_reg(void) {
    iounmap(gpio_registers);
    return;
}



int map_gpio_axi(void) {
    /* define a pointer to map pl-gpio axi interconnect for a full page */
	gpio_axi_reg = (uint32_t*)ioremap(XGPIOPS_AXI_BASE_ADDR, PAGE_SIZE);
	if (gpio_axi_reg == NULL) {
		pr_alert("ERROR: failed to map GPIO_S_AXI memory: 0x%x\n",  gpio_axi_reg);
		return -1;
	}
	
	printk("%s: Successfully mapped in GPIO_S_AXI memory at: 0x%x\n", "gpiopl", gpio_axi_reg);

    return 0;
}

void unmap_gpio_axi(void) {
    iounmap(gpio_axi_reg);
    return;
}

uint32_t xor_axi_gpio_rd(void) {
    return *(uint32_t*)gpio_axi_reg;
}


