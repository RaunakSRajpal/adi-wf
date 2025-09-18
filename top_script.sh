#!/bin/bash/
set -e

CC_TOOLCHAIN="arm-linux-gnueabihf-" # "/home/raunak/wise-rsr/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi" 
EVAL_BD="zynq-adrv9361-z7035-bob-cmos.dtb"
LINUX_DIR="linux-adi"

MNT_PT=$1
WS="$(pwd)"

source /opt/Xilinx/Vivado/2024.2/settings64.sh
source /opt/Xilinx/Vitis/2024.2/settings64.sh

# bash setup-uboot-proj.sh $LINUX_DIR xilinx/$EVAL_BD $CC_TOOLCHAIN

# ## build driver modules
# (
#     cd drivers/gpio_dummy/
#     make | tee err.log 2>&1
#     cd ../
#     
#     cd drivers/hello_world/
#     make | tee err.log 2>&1
# )

   
## boot
(
    rm -rf $MNT_PT/BOOT/uImage
    rm -rf $MNT_PT/BOOT/devicetree.dtb
    cp -f $WS/uImage $MNT_PT/BOOT
    cp -f $WS/$EVAL_BD $MNT_PT/BOOT/
    mv -b $MNT_PT/BOOT/$EVAL_BD $MNT_PT/BOOT/devicetree.dtb
)

## rootfs
(
    # kernel+headers
    rm -Rf $MNT_PT/rootfs/usr/local/src/linux-adi/
    cp -r $WS/$LINUX_DIR $MNT_PT/rootfs/usr/local/src/
    
    rm -Rf $MNT_PT/rootfs/usr/src/include
    cp -r $WS/sysmod_rootfs/usr/include $MNT_PT/rootfs/usr/src/
    
    # modules
    rm -Rf $MNT_PT/rootfs/usr/lib/modules/
    cp -r $WS/sysmod_rootfs/lib/modules/* $MNT_PT/rootfs/usr/lib/modules/
    ln -sfn /usr/local/src/$LINUX_DIR $MNT_PT/rootfs/usr/lib/modules/build
    
    # drivers
    rm -Rf $MNT_PT/rootfs/home/analog/drivers/
    cp -r $WS/drivers $MNT_PT/rootfs/home/analog/
)

sync

