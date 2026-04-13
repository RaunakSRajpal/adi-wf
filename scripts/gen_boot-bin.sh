#!/bin/bash
set -e

# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Company:  WISE Circuits Lab, Boston University
# 
# Brief:    Generates boot binaries for supported SoC/FMC boards.
#           XSA file must be exported prior run.
# ------------------------------------------------------------------

# EVAL_BD=${1:-"adrv9361z7035"}
# CARRIER=${2:-"ccbob_cmos"}
# XSA_FILE=${3:-"$HDL_DIR/projects/$EVAL_BD/$CARRIER/system_top.xsa"}

WS="$(dirname "$0")/../"
LOGFILE=${ADI_DIR}/build/logs/setup-BOOT.log
touch $LOGFILE && > $LOGFILE

# UBOOT_FILE="${BOOT_DIR}/u-boot_zynq_adrv9361.elf"



## prep environment 
        source ~/.bashrc
        source ${WS}/project_setup.env
        source ${WS}/scripts/setup.env
        . ${WS}/scripts/housekeeping.sh


        # Test if project space exists
        [ -z $proj_name ] && \
                error "$0: Project name not specified" \
                "proj_name: All HDL projects must be defined in project_setup.env [${WS}/project_setup.env]"
        status "$0: using project space $proj_name [${HDL_PROJ}]"

        [ -d ${HDL_PROJ}/ ] || \
        	error "$0: ADI project board not found" \
                "EVAL_BD/CARRIER: must be defined in project_setup.env [${WS}/project_setup.env]"


        # HDL board build directory
        [ -d ${HDL_PROJ}/${EVAL_BD}-${CARRIER}-${proj_name}_build ] || \
                error "hdl project directory not found" \
                "$0: HDL_PROJ: All hdl projects must be initialised prior to building boot binaries"
        # cd ${HDL_PROJ}/${EVAL_BD}-${CARRIER}-${proj_name}_build


        # Architecture
        [ -z $ARCH ] && \
                error "ARCH: No target architecture specified" \
                "project_setup.env: you must specify the target machine architecture"


        # Check for Xilinx dependencies
        [ -z $XVERSION ] && \
                error "XVERSION: No xilinx version specified" \
                "project_setup.env: you must specify the version of the xilinx tools being used (host or SCC)"
        
        [ ! -z "$XVIVADO" ] && {
                [ -f "$XVIVADO/settings64.sh" ] && {
                        . ${XVIVADO}/settings64.sh
                        XVIVADO_MISSING=0
                } || {
                        XVIVADO_MISSING=1
                        warning "XVIVADO: specified path not found" \
                        "$0: could not locate the installation path for Xilinx Vivado $XVERSION [${XVIVADO}]: check setup.env"
                }
        } || XVIVADO_MISSING=1

        [ ! -z "$XVITIS" ] && {
                [ -f "$XVITIS/settings64.sh" ] && {
                        . ${XVITIS}/settings64.sh
                        XVITIS_MISSING=0
                } || {
                        XVITIS_MISSING=1
                        warning "XVITIS: specified path not found" \
                        "$0: could not locate the installation path for Xilinx Vitis/SDK $XVERSION [${XVITIS}]: check setup.env"
                }
        } || XVITIS_MISSING=1

        #   If either one of VITIS or VIVADO is missing, terminate with error handler
        #       If both VIVADO & VITIS are missing, fallback to versions supported by SCC
        (( (XVIVADO_MISSING ^ XVITIS_MISSING) == 1 )) && \
                error "$0: Xilinx Vivado or Vitis(SDK) $XVERSION installation directory not found" \
                "setup.env: Both XVIVADO and XVITIS are essential HDL build requirements. check README.md for more information. \n\t\tXVIVADO: [${XVIVADO}]\n\t\tXVITIS: [${XVITIS}]"
        (( XVIVADO_MISSING == 1 && XVITIS_MISSING == 1 )) && \
                SCC_FALLBACK=1

        . ${WS}/scripts/find_xilinx.sh
        return_line && which vivado | tee -a $LOGFILE 2>&1


        # XSA file
        case "$XSA_FILE" in
            *.xsa)
                [ -f "$XSA_FILE" ] || \
                        error "ERROR: XSA file not found" \
                        "$0: [${XSA_FILE}]: check hardware export(.xsa) file path in setup.env: [${WS}/scripts/setup.env]"
                status "XSA file found: ${XSA_FILE}"
                ;;
            *)
                error "ERROR: ${XSA_FILE} is not a .xsa file" \
                "$0: [${XSA_FILE}]: check hardware export(.xsa) file path in setup.env: [${WS}/scripts/setup.env]"
                ;;
        esac
        

        # u-boot (.elf) file
        if [ -z "$uboot_elf" ] || [ "$uboot_elf" = "download" ]; then
                patterns=("zed" "ccfmc_*" "ccbob_*" "usrpe31x" "zc702" "zc706" "coraz7s")

	        carrier=$(unzip -p $XSA_FILE | grep -a "PATH_TO_FILE" | grep -oE "$(IFS='|'; echo "${patterns[*]}")")
	        case  $carrier  in
	            zed)                uboot_elf="u-boot_zynq_zed.elf" ;;
	            ccfmc_*|ccbob_*)	uboot_elf="u-boot_zynq_adrv9361.elf" ;;
	            usrpe31x)		uboot_elf="u-boot-usrp-e310.elf" ;;
	            zc702)		uboot_elf="u-boot_zynq_zc702.elf" ;;
	            zc706)		uboot_elf="u-boot_zynq_zc706.elf" ;;
	            coraz7s)		uboot_elf="u-boot_zynq_coraz7.elf" ;;
	            *)
	        	error "ELF: u-boot.elf not found" \
                        "!!!!! The specified carrier does not have a downloadable u-boot.elf file !!!!!"
	        esac
        
	        boot_partition_location=${XVERSION//./_r}
                export UBOOT_FILE=${BOOT_DIR}/${uboot_elf}

	        echo "Downloading $uboot_elf ..."
	        wget -O "${UBOOT_FILE}" https://swdownloads.analog.com/cse/boot_partition_files/uboot/$boot_partition_location/$uboot_elf
        else
                [ -f "$uboot_elf" ] || \
                        error "ELF: u-boot.elf file not found" \
                        "$0: check the u-boot file path in project_setup.env: uboot_elf: [${uboot_elf}]"
                
                status "u-boot.elf file located: ${uboot_elf}"
                export UBOOT_FILE=${BOOT_DIR}/$(basename ${uboot_elf})
                cp -bu ${uboot_elf} $UBOOT_FILE
        fi



## Build BOOT.BIN
        rm -Rf $BUILD_BOOT_DIR $OUTPUT_DIR
        mkdir -p $OUTPUT_DIR
        mkdir -p $BUILD_BOOT_DIR


        # Create create_fsbl_project.tcl file used by xsct to create the fsbl.
                ### DO NOT EDIT THIS ###
        echo "hsi open_hw_design `basename $XSA_FILE`" > $BUILD_BOOT_DIR/create_fsbl_project.tcl
        echo 'set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
        echo 'platform create -name hw0 -hw system_top.xsa -os standalone -out ./build/sdk -proc $cpu_name' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
        echo 'platform generate' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
        
        FSBL_PATH="$BUILD_BOOT_DIR/build/sdk/hw0/export/hw0/sw/hw0/boot/fsbl.elf"
        SYSTEM_TOP_BIT_PATH="$BUILD_BOOT_DIR/build/sdk/hw0/hw/system_top.bit"


        # Create zynq.bif file used by bootgen
                ### DO NOT EDIT THIS ###
        echo 'the_ROM_image:' > $BOOT_DIR/zynq.bif
        echo '{' >> $BOOT_DIR/zynq.bif
        echo '[bootloader] fsbl.elf' >> $BOOT_DIR/zynq.bif
        echo 'system_top.bit' >> $BOOT_DIR/zynq.bif
        echo 'u-boot.elf' >> $BOOT_DIR/zynq.bif
        echo '}' >> $BOOT_DIR/zynq.bif


        # Build fsbl.elf
        cp -f $XSA_FILE $BUILD_BOOT_DIR/
        (
        	cd $BUILD_BOOT_DIR
        	xsct create_fsbl_project.tcl
        )


        # Build BOOT.BIN
        cp -f $XSA_FILE $OUTPUT_DIR/
        cp -f $UBOOT_FILE $OUTPUT_DIR/u-boot.elf
        cp -f $FSBL_PATH $OUTPUT_DIR/fsbl.elf
        cp -f $SYSTEM_TOP_BIT_PATH $OUTPUT_DIR/system_top.bit
        cp -f $BOOT_DIR/zynq.bif $OUTPUT_DIR/zynq.bif

        (
        	cd $OUTPUT_DIR
        	bootgen -arch zynq -image $BOOT_DIR/zynq.bif -o BOOT.BIN -w
        ) && \
        echo "BOOT.BIN created!" | tee $LOGFILE 2>&1



## Generate SDcard files
        status "Generated all files for boot: ${OUTPUT_DIR}" | tee $LOGFILE 2>&1
        status "Files for bootable media can be found in directory: ${PKG_BOOT}" | tee $LOGFILE 2>&1

        mkdir -p $PKG_BOOT/${proj_name}
        cp -f ${OUTPUT_DIR}/BOOT.BIN $PKG_BOOT


    #---- build linux script ----
    # bash ../setup-uboot-proj.sh linux-adi/ xilinx/zynq-adrv9361-z7035-bob-cmos.dtb
