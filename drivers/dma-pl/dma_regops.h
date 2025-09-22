/*******************************************
 * @file gpio_ops.h
 * @author Raunak Rajpal (rsajpal@bu.edu)
 * @brief   captures the register operations
 *          required for DMA I/O
********************************************/

#ifndef DMA_OPS_H
#define DMA_OPS_H

#include "xil_types.h"
// #include "xil_assert.h"
#include "xil_io.h"
#include "xgpiops_hw.h"

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 4

/**
 * 
**/

#define XGPIOPS_BASE_ADDR       (uint32_t)0xE000A000
#define XGPIOPS_DATA__(X)       (uint32_t)0x00000040 + (X*4)
#define XGPIOPS_DATA_RO__(X)    (uint32_t)0x00000060 + (X*4)
#define XGPIOPS_DIRM__(X)       (uint32_t)0x00000204 + (X*4 << 1)
#define XGPIOPS_OEN__(X)        (uint32_t)0x00000208 + (X*4 << 1)


static inline uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin);

static inline void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val);

static inline void map_gpio_reg();
static inline void unmap_gpio_reg();

#endif
