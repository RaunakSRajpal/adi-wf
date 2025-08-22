#!/bin/bash
set -e

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

usage() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\t\t usage: $0 <eval_bd> <carrier_bd> \n" | tee -a $LOGFILE 2>&1
    exit 1
}

error() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    exit 1
}

status() {
    echo -e "[INFO]\t$1" | tee -a $LOGFILE 2>&1
}


## Test for arguments
    ### Test if HDL directory is present
    [ -d $HDL_DIR ] || \
            error "hdl directory not found..\n\t\t \
                        try: bash init-adi-hdl.sh <eval_bd> <carrier_bd>"
    
    cd $HDL_DIR
    touch $LOGFILE
    
    if [ -z $EVAL_BOARD ]; then
     		usage "ADI Evaluation board not specified"
    else
       [ -d "$HDL_DIR/projects/$EVAL_BOARD" ] || \
    		error "Evaluation board not found"
    fi
     
    if [ -z $CARRIER ]; then
    		usage "ADI Carrier board not specified"
    else
       [ -d "$HDL_DIR/projects/$EVAL_BOARD/$CARRIER" ] || \
    		error "Carrier board not found"
    fi

 
## Build BOOT.BIN
    rm -Rf $BUILD_DIR $OUTPUT_DIR
    mkdir -p $OUTPUT_DIR
    mkdir -p $BUILD_DIR
    
    ### Prep SD Card
    [ -d $BOOT_DIR ] || \
    		error "BOOT directory not found!"
    
    [ -f "$BOOT_DIR/fsbl.elf" ] || \
    		error "fsbl.elf: file not found!"
    [ -f "$BOOT_DIR"/u-boot*.elf ] || \
    		error "u-boot.elf: file not found!"
    [ -f "$BOOT_DIR/zynq.bif" ] || \
    		error "zynq.bif: file not found!"
    [ -f "$BOOT_DIR"/uImage* ] || \
    		error "uImage: binary file not found!"
    
    cp $XSA_FILE $BUILD_DIR/
    cp $XSA_FILE $OUTPUT_DIR/
    cp $UBOOT_FILE $OUTPUT_DIR/
    
    ### Generate SDcard files
    echo "Generated all files for boot" | tee $LOGFILE 2>&1
    
    ### Create create_fsbl_project.tcl file used by xsct to create the fsbl.
    echo "hsi open_hw_design `basename $XSA_FILE`" > $BUILD_DIR/create_fsbl_project.tcl
    echo 'set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]' >> $BUILD_DIR/create_fsbl_project.tcl
    echo 'platform create -name hw0 -hw system_top.xsa -os standalone -out ./build/sdk -proc $cpu_name' >> $BUILD_DIR/create_fsbl_project.tcl
    echo 'platform generate' >> $BUILD_DIR/create_fsbl_project.tcl
    
    FSBL_PATH="$BUILD_DIR/build/sdk/hw0/export/hw0/sw/hw0/boot/fsbl.elf"
    SYSTEM_TOP_BIT_PATH="$BUILD_DIR/build/sdk/hw0/hw/system_top.bit"
    
    ### Build fsbl.elf
    (
	cd $BUILD_DIR
	xsct create_fsbl_project.tcl
    )
    cp -f $FSBL_PATH $OUTPUT_DIR/fsbl.elf
    cp -f $SYSTEM_TOP_BIT_PATH $OUTPUT_DIR/system_top.bit
    cp -f $BOOT_DIR/zynq.bif $OUTPUT_DIR/zynq.bif
    
    ### Build BOOT.BIN
    (
	cd $OUTPUT_DIR
	bootgen -arch zynq -image $BOOT_DIR/zynq.bif -o BOOT.BIN -w
    )
    
    echo "BOOT.BIN created!" | tee $LOGFILE 2>&1

    #---- build linux script ----
    # bash ../setup-uboot-proj.sh linux-adi/ xilinx/zynq-adrv9361-z7035-bob-cmos.dtb


