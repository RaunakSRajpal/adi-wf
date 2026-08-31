CDIR		:= $(shell pwd)
usr_src		:= usrtest_dmapl.c
usr_obj		:= $(patsubst %.c,%.o, $(usr_src))

# ccflags-y := -I$(src)/xaxidma/include

export CROSS_COMPILE ARCH CC LD AR NM 
export STRIP READELF OBJCOP OBJDUMP

all:
		$(CC) $(usr_src) -o $(usr_obj)

clean: 
		rm -rf $(usr_obj)
