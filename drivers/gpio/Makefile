BINARY		:= pl_gpio_main
KERNEL		:= /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)

obj-m += $(BINARY).o
$(BINARY)-objs := gpio_ops.o pl_gpio.o

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
