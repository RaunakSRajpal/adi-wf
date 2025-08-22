#!/bin/bash
set -e

WS="$(pwd)"
ADI_DIR="$WS/adi-hdl-1"
HDL_DIR="$ADI_DIR/hdl"
XVERSION="2024.2"

proj_name="$1"
EVAL_BD=${2:-"adrv9361z7035"}
CARRIER=${3:-"ccbob_cmos"}
hdl_branch=${4:-"main"}
XSA_FILE="$HDL_DIR/projects/$EVAL_BD/$CARRIER/system_top.xsa"
UBOOT_FILE="$ADI_DIR/prep-BOOT/u-boot_zynq_adrv9361.elf"

BOOT_DIR="$ADI_DIR/prep-BOOT"
LOGFILE=$ADI_DIR/setup-HDL.log
BUILD_DIR="/$ADI_DIR/build_boot_bin"
OUTPUT_DIR="$ADI_DIR/output_boot_bin"

usage() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\t\t usage: $0 <eval_bd> <carrier_bd> \n" | tee -a $LOGFILE 2>&1
    exit 1
}

error() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE 2>&1
    exit 1
}

status() {
    echo -e "[INFO]\t$1" | tee -a $LOGFILE 2>&1
}


## Test for arguments
    [ -n $proj_name ] || \
            usage "Project name not specified"
    
    [ -n $EVAL_BD ] || \
     		usage "ADI Evaluation board not specified"
    
    [ -n $CARRIER ] || \
    		usage "ADI Carrier board not specified"
    
    mkdir -p $ADI_DIR/ && cd $ADI_DIR/
    touch $LOGFILE && \
        echo -e "Initialize ADI-HDL repository.\n \
             Setup required directories and envirnment variables\n" > $LOGFILE
    
    ### Test if HDL directory is present,, otherwise clone repo    
    [ -d "$ADI_DIR/hdl" ] || {
            status "ADI hdl directory not found... installing from \'analogdevicesinc\' repository"
            git clone https://github.com/analogdevicesinc/hdl.git \
                    --no-single-branch \
                    -- hdl | tee -a $LOGFILE 2>&1
            status "analogdevicesinc/hdl repository cloned"
            git checkout $hdl_branch
    }
    
    [ -d "$HDL_DIR/projects/$EVAL_BD" ] || \
    		error "Evaluation board not found"
    
    [ -d "$HDL_DIR/projects/$EVAL_BD/$CARRIER" ] || \
    		error "Carrier board not found"
    

## Setup environment
    cd $ADI_DIR
    source /opt/Xilinx/Vivado/$XVERSION/settings64.sh

    export PATH=$PATH:/opt/Xilinx/Vivado/$XVERSION/bin:/opt/Xilinx/Vitis/$XVERSION/bin
    # export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/microbalaze/nt/bin
    export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/microbalaze/lin/bin
    export PATH=$PATH:/opt/Xilinx/Vitis/$XVERSION/gnu/armr5/lin/gcc-arm-none-eabi/bin
    
    echo && which vivado | tee -a $LOGFILE 2>&1
    

## Build HDL project 
    # export ADI_IGNORE_VERSION_CHECK=1
    (
        cd $HDL_DIR/projects/$EVAL_BD/$CARRIER/ && \
        make -j5 | tee -a $LOGFILE 2>&1
        status "Project $EVAL_BD-$CARRIER: build complete"
        
        cd ../../
        mkdir -p $proj_name
        cp -r $EVAL_BD ./$proj_name        
    )
    
## Build ADI IP cores
    if [ "$EVAL_BD" == "library" ] ; then
    (
        cd $HDL_DIR/ && \
        make -C lbrary all | tee -a $LOGFILE 2>&1
        status "$HDL_DIR/library: build complete"
        ##TODO --
        #--- add library directory to Vivado IP repository
    )
    

