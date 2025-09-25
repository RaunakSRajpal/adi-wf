/*******************************************
 * @file    dma_ops.c
 * @author  Raunak Rajpal (rsajpal@bu.edu)
 * 
 * @brief   captures the register operations
 *          required for DMA I/O
********************************************/

#include "dma_ops.h"


#define AXIDMA_MEM_SIZE   (64 * PAGE_SIZE)

static volatile void __iomem *axi_dma_baseptr = NULL;


int map_axi_dma(void) {
    /* define a base pointer to map PL-AxiDMA to a 64kB of DDR menory block */
	axi_dma_baseptr = (uint32_t*)ioremap(MEM_BASE_ADDR, AXIDMA_MEM_SIZE);
	if (axi_dma_baseptr == NULL) {
		pr_alert("ERROR: failed to map DMA_S_AXI memory: 0x%x\n",  axi_dma_baseptr);
		return XST_FAILURE;
	}
	
	printk("%s: Successfully mapped in DMA_S_AXI memory at: 0x%x\n", "gpiopl", axi_dma_baseptr);

    return XST_SUCCESS;
}

void unmap_axi_dma(void) {
    iounmap(axi_dma_baseptr);
    return;
}

