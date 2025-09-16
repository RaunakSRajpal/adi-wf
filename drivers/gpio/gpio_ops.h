/*******************************************
 * @file gpio_ops.h
 * @author Raunak Rajpal (rsajpal@bu.edu)
 * @brief   captures the register operations
 *          required for GP I/O
********************************************/
#ifndef GPIO_OPS_H
#define GPIO_OPS_H

#include "xil_types.h"
// #include "xil_assert.h"
#include "xil_io.h"
#include "xgpiops_hw.h"

/**
 * 
 * 
**/

static inline uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin);

static inline void gpio_pin_wr(uint8_t bank, uint8_t pin);

#endif