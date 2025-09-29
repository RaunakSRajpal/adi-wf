BINARY		:= gpiopl
KERNEL		:= /home/raunak/wise-rsr/linux-adi
KMOD_DIR	:= $(shell pwd)
INCLUDEDIR	:= opt/Xilinx/Vitis/2024.2/data/embeddedsw/XilinxProcessorIPLib/drivers
usr_src		:= pl_gp_test.c
usr_obj		:= $(patsubst %.c,%.o, $(usr_src))
#EXTRA_CFLAGS	:= -I$(INCLUDEDIR)/gpiops_v3_13/src

obj-m := $(BINARY).o
$(BINARY)-objs := pl_gpio.o gpio_ops.o

ccflags-y := -I$(src)/include
# ccflags-y += -I$(INCLUDEDIR)/gpiops_v3_13/src
# ccflags-y += -I$(INCLUDEDIR)/common_v1_3/src
# ccflags-y += -I$(INCLUDEDIR)/axidma_v9_19

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


gpio_ops: gpio_ops.c gpio_ops.h
		make -C $(KERNEL) M=$(KMOD_DIR) gpio_ops.o

gpiopl: pl_gpio.c gpio_ops.c gpio_ops.h gpio_ioctl.h
		make -C $(KERNEL) M=$(KMOD_DIR) gpio_ops.o
		make -C $(KERNEL) M=$(KMOD_DIR) pl_gpio.o
		make -C $(KERNEL) M=$(KMOD_DIR) gpiopl.ko

all:
		make -C $(KERNEL) M=$(KMOD_DIR) modules
		$(CC) $(usr_src) -o $(usr_obj)

clean:
		make -C $(KERNEL) M=$(KMOD_DIR) clean
		rm -rf $(usr_obj)

