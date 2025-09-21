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
#define XGPIOPS_DIRM__(X)       (uint32_t)0x00000204 + (X*4 << 4)
#define XGPIOPS_OEN__(X)        (uint32_t)0x00000208 + (X*4 << 4)

#define XGPIOPS_AXI_BASE_ADDR   (uint32_t)0x41210000


uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin);
void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val);
int map_gpio_reg(void);
void unmap_gpio_reg(void);

int map_gpio_axi(void);
void unmap_gpio_axi(void);
uint32_t xor_axi_gpio_rd(void);

#endif

