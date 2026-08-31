WS_CURRENT	:= $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/../..)
WS		:= $(or $(WS), $(WS_CURRENT))
include $(WS)/project_setup.env
include $(WS)/scripts/setup.env
export


BINARY		:= hello_world		# gpio_xor-driver
KERNEL		:= $(LINUX_DIR)		# /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)

obj-m += $(BINARY).o

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
		make -C $(KERNEL) M=$(KMOD_DIR) modules

clean: 
		make -C $(KERNEL) M=$(KMOD_DIR) clean

