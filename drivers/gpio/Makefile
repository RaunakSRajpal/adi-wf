# BINARY		:= gpio-pl
KERNEL		:= /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)

obj-m := gpiopl.o
gpiopl-objs := pl_gpio.o gpio_ops.o
# gpiopl-y += gpio_ops.o

ccflags-y := -I$(src)

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

#gpio_ops:
#		make -C $(KERNEL) M=$(KMOD_DIR) gpio_ops.o

gpiopl: pl_gpio.c gpio_ops.c gpio_ops.h gpio_ioctl.h
		make -C $(KERNEL) M=$(KMOD_DIR) gpio_ops.o
		make -C $(KERNEL) M=$(KMOD_DIR) pl_gpio.o
		make -C $(KERNEL) M=$(KMOD_DIR) gpiopl.ko

all:
		make -C $(KERNEL) M=$(KMOD_DIR) modules

clean:
		make -C $(KERNEL) M=$(KMOD_DIR) clean

