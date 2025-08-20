#!bin/bash
set -e 

# ------------------------------------------------------------------------------
# The script takes 4 arguments in order: 
#       1>  Path to linux-adi directory (optional). 
# 
#       2>  Path to the devicetree file. This argument is required and 
#           the script throws an ERROR if not specified. 
# 
#       3>  Cross compiler used for building the config files (optional)
#           If the cross compiler is not specified the script uses Linaro
#           toolchains by default. Note, it will automatically install 
#           the required toolchains if it cannot locate any specified.
# 
#       4>  The config definition file to be built (optional). By default 
#           it makes kernel config for a zynq based project. 
# ------------------------------------------------------------------------------


LINUX_DIR=${1:-"linux-adi"}
DTFILE=$2
CROSS_COMPILE="$3"            # "/lib/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-"}
LOGFILE="./SETUP-kernel.log"

XVERSION="2024.2"

# ----------------------------------------------------------------
# Xilinx boards including zynq-adrv9361-z7035-bob are loctaed in
# the directory: $LINUX_DIR/arch/arm/boot/dts/xilinx/
# ----------------------------------------------------------------

error() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\tusage: $0 <linux-repo dir> <device-tree file> \n" | tee -a $LOGFILE 2>&1
    exit 1
}

status() {
    echo -e "[INFO]\t$1" | tee -a $LOGFILE 2>&1
}

ARCH=arm
IMG_NAME=uImage

# The board used below is generic and is not project specific. 
# As long as the board is a ZYNQ FPGA, use the configuration below.
CONFIG_GEN_FILE=${4:-"zynq_xcomm_adv7511_defconfig"}


## prep envinronment 
    source ~/.bashrc
    rm -rf $IMG_NAME $DTFILE
    
    rm -Rf sysmod_rootfs
    mkdir -p sysmod_rootfs
    mkdir -p sysmod_rootfs/usr
    
    touch $LOGFILE
    echo "Finding linux repo... " | tee $LOGFILE 2>&1 
    
    # Check for linux kernel source dir
    [ -d "$LINUX_DIR" ] || {
            status "ADI linux directory not found... installing from adi repository"
            git clone https://github.com/analogdevicesinc/linux.git \
                    --no-single-branch --depth=10 \
                    -- linux | tee -a $LOGFILE 2>&1
            mv -b linux/ linux-adi/
            status "linux repository cloned"
    }
    
    # Device-tree file
    if  [ -z "$DTFILE" ]; then 
            error "No device tree file specified" 
            exit 1
    else
            DTFILE="${DTFILE%.*}.dtb"
            CHECK_DTFILE="${DTFILE%.*}.dts"
            [ -f "$LINUX_DIR/arch/$ARCH/boot/dts/$CHECK_DTFILE" ] && \
                    status "Device tree file $DTFILE will be built" || \
                    error "$CHECK_DTFILE: Device tree file not found"
    fi
    
    # Cross-compiler GCC toolchains
    [ ! -z "$CROSS_COMPILE" ] && \
            if type "${CROSS_COMPILE}gcc" >/dev/null 2>&1 ; then 
                    status "Using cross-compile toolchain: $CROSS_COMPILE"
                    CC_CHECK='1'
            else
                    error "$CROSS_COPILE: specified toolchain not found"
            fi \
    || \
            if type "arm-linux-gnueabihf-gcc" >/dev/null 2>&1 ; then
                    CHECK_XILINX_TOOLS='1'
            elif type "arm-linux-gnueabi-gcc" >/dev/null 2>&1 ; then
                    CHECK_LINARO_TOOLS='1'
            fi
    
    if [ "$CHECK_XILINX_TOOLS" == '1' ] ; then
            source /opt/Xilinx/Vivado/$XVERSION/settings64.sh
            source /opt/Xilinx/Vitis/$XVERSION/settings64.sh
            status "Using AMD Xilinx toolchain for cross compilation"
            arm-linux-gnueabihf-gcc --version | tee -a $LOGFILE 2>&1
            status "Path to toolchain: $(which arm-linux-gnueabihf-gcc)"
            CROSS_COMPILE="arm-linux-gnueabihf-"
    elif [ "$CHECK_LINARO_TOOLS" == '1' ] ; then
            source /lib/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/
            status "Using Linaro-GCC toolchain for cross compilation"
            arm-linux-gnueabi-gcc --version | tee -a $LOGFILE 2>&1
            status "Path to toolchain: $(which arm-linux-gnueabihf-gcc)"
            CROSS_COMPILE="/lib/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-"
    elif [ "$CC_CHECK" == '1' ] ; then
            ${CROSS_COMPILE}gcc --version
            status "Path to toolchain: ${CROSS_COMPILE}gcc"
    else
            error "$CROSS_COMPILE: No GCC compiler found for arm cross-compilation"
    fi        
            # CHECK_XILINX_TOOLS=$(source /opt/Xilinx/Vivado/$XVERSION/settings.sh &>/dev/null)
            # CHECK_LINARO_TOOLS=$(source )
            # if  [ -z  ]; then
            #         status "Using arm-linux-gnueabihf-gcc for cross compilation"
            #         source /opt/Xilinx/Vivado/$XVERSION/settings.sh
            #         source /opt/Xilinx/Vitis/$XVERSION/settings.sh
            # elif [ -z  ]
            #     arm-linux-gnueabihf-gcc --version
            #     status "Using linaro toolchian for cross compilation"
    
    # Device type: Zynq(default), ZynqMP, Versal
    [ ! -z "$4" ] || \
            status "configuring kernel for default zynq settings"
    
    ### check if the following tools/libs are installed ###
    # - rsync
    # - u-boot-tools [mkimage utility]
    # - 

    export ARCH
    export CROSS_COMPILE


## Build Kernel and Devicetree
    cd $LINUX_DIR
    make clean && make mrproper
    rm -rf arch/$ARCH/boot/$IMG_NAME
    
    # generate .config file
    make $CONFIG_GEN_FILE | tee -a $LOGFILE 2>&1
    make menuconfig | tee -a $LOGFILE 2>&1              # This will load an interactive build menu. Select the config options.
    
    # make modules and kernel headers
    make -j8 modules | tee -a $LOGFILE 2>&1
    make modules_install INSTALL_MOD_PATH=../sysmod_rootfs/ | tee -a $LOGFILE 2>&1
    make headers_install INSTALL_HDR_PATH=../sysmod_rootfs/usr | tee -a $LOGFILE 2>&1
    
    # build kernel image
    make -j5 UIMAGE_LOADADDR=0x8000 $IMG_NAME | tee -a $LOGFILE 2>&1
    # diff .config.old .config

    # generate DTB
    make $DTFILE | tee -a $LOGFILE 2>&1

## package boot files
    cp -f arch/arm/boot/$IMG_NAME ../
    cp -f arch/arm/boot/dts/$DTFILE ../
    echo "boot files generated: $IMG_NAME $DTFILE"

