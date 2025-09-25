/*******************************************
 * @file    dma_ops.h
 * @author  Raunak Rajpal (rsajpal@bu.edu)
 * 
 * @brief   captures the register operations
 *          required for DMA I/O
********************************************/

#ifndef DMA_OPS_H
#define DMA_OPS_H

#include <linux/types.h>
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

#define GPIO_PIN_MAX 118
#define GPIO_REG_SIZE 4

/**
 * DMA related device parameters and function prototypes
**/

#define MEM_BASE_ADDR		0x01000000
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x1000000)

#define TX_BD_SPACE_BASE	(MEM_BASE_ADDR)
#define TX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x00000FFF)
#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00001000)
#define RX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x00001FFF)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)


// static inline uint32_t gpio_pin_rd(uint8_t bank, uint8_t pin);

// static inline void gpio_pin_wr(uint8_t bank, uint8_t pin, uint8_t val);

int map_axi_dma(void);
void unmap_axi_dma(void);

// static inline void unmap_axi_dma(void) {
//     iounmap(axi_dma_baseptr);
//     return;
// }

#endif
