WS_CURRENT	:= $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/../..)
WS		:= $(or $(WS), $(WS_CURRENT))
include $(WS)/project_setup.env
include $(WS)/scripts/setup.env
export


BINARY		:= axidmapl
KERNEL		:= $(LINUX_DIR)		# /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)
#usr_src		:= usrtest_dmapl.c
#usr_obj		:= $(patsubst %.c,%.o, $(usr_src))

obj-m += $(BINARY).o
$(BINARY)-objs := dma_main.o dma_ctlops.o

ccflags-y := -I$(src)/include

CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
READELF		= $(CROSS_COMPILE)readelf
STRIP		= $(CROSS_COMPILE)strip

export CROSS_COMPILE ARCH CC LD AR NM 
export STRIP READELF OBJCOP OBJDUMP

all:
#		make -C $(KERNEL) M=$(KMOD_DIR)/xaxidma/src modules
		make -C $(KERNEL) M=$(KMOD_DIR) modules
#		$(CC) $(usr_src) -o $(usr_obj)

clean: 
#		make -C $(KERNEL) M=$(KMOD_DIR)/xaxidma/src clean
		make -C $(KERNEL) M=$(KMOD_DIR) clean
#		rm -rf $(usr_obj)
