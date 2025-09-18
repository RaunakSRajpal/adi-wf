// #include "xil_io.h"
// #include "xgpiops_hw.h"

#include "gpio_ops.h"

static uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin) {
    uint32_t *dirm_x 	= (uint32_t*)(gpio_registers + XGPIOPS_DIRM__(bank));
    uint32_t *oen_x  	= (uint32_t*)(gpio_registers + XGPIOPS_OEN__(bank));
    uint32_t *data_ro_x = (uint32_t*)(gpio_registers + XGPIOPS_DATA_RO__(bank));
	// REG - OPS
	*dirm_x &= ~(1 << pin);
    *oen_x &= ~(1 << pin);
    
	return *data_ro_x & (1 << pin);
}

static void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val) {
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

