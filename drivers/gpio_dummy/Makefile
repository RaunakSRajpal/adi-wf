BINARY		:= gpio_test
KERNEL		:= /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)
SYSROOT := /home/raunak/wise-rsr/sysmod_rootfs/usr/include

obj-m += $(BINARY).o
obj-m += usr_test.o

CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
READELF		= $(CROSS_COMPILE)readelf
STRIP		= $(CROSS_COMPILE)strip

CFLAGS := --sysroot=$(SYSROOT) -Wall -g
# CXXFLAGS := --sysroot=$(SYSROOT) -Wall -g
LDFLAGS := --sysroot=$(SYSROOT)

export CROSS_COMPILE ARCH CC LD AR NM CFLAGS LDFLAGS
export STRIP READELF OBJCOPY OBJDUMP

all:
		make -C $(KERNEL) M=$(KMOD_DIR) modules

clean: 
		make -C $(KERNEL) M=$(KMOD_DIR) clean
