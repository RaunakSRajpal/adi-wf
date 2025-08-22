#!/bin/bash
set -xe

WS="$(pwd)"
ADI_DIR="$WS/adi-hdl"
HDL_DIR="$ADI_DIR/hdl"
XVERSION="2024.2"


EVAL_BOARD=${1:-"adrv9361z7035"}
CARRIER=${2:-"ccbob_cmos"}
XSA_FILE="$HDL_DIR/projects/$EVAL_BOARD/$CARRIER/system_top.xsa"
UBOOT_FILE="$ADI_DIR/prep-BOOT/u-boot_zynq_adrv9361.elf"

BOOT_DIR="$ADI_DIR/prep-BOOT"
LOGFILE=$HDL_DIR/setup-HDL.log
BUILD_DIR="/$ADI_DIR/build_boot_bin"
OUTPUT_DIR="$ADI_DIR/output_boot_bin"

error() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\t\t usage: $0 \'eval_bd\' \'carrier_bd\' \n" | tee -a $LOGFILE 2>&1
    exit 1
}


## Test for arguments
    ### Test if HDL directory is present,, otherwise clone repo
    # -----TO DO------ #
    
    cd $HDL_DIR
    touch $LOGFILE
    
    if [ -z $EVAL_BOARD ]; then
     		error "ADI Evaluation board not specified"
    else
       [ -d "$HDL_DIR/projects/$EVAL_BOARD" ] || \
    		error "Evaluation board not found"
    fi
     
    if [ -z $CARRIER ]; then
    		error "ADI Carrier board not specified"
    else
       [ -d "$HDL_DIR/projects/$EVAL_BOARD/$CARRIER" ] || \
    		error "Carrier board not found"
    fi
    

## Setup environment
    cd $AD_DIR
    source /opt/Xilinx/Vivado/$XVERSION/settings64.sh

    export PATH=$PATH:/opt/Xilinx/Vivado/$XVERSION/bin:/opt/Xilinx/Vitis/$XVERSION/bin
    # export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/microbalaze/nt/bin
    export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/microbalaze/lin/bin
    export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/armr5/lin/gcc-arm-none-eabi/bin
    
    echo && which vivado | tee -a $LOGFILE 2>&1
    

## Build HDL project 
    # export ADI_IGNORE_VERSION_CHECK=1
    (
        cd $HDL_DIR/projects/$EVAL_BOARD/$CARRIER/ && \
        make -j5
    )

