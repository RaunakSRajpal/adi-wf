#!/bin/bash
set -e 

# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Company:  WISE Circuits Lab, Boston University
# 
# Brief:
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
#       4>  TODO--update??The config definition file to be built (optional). 
#            By default it makes kernel config for a zynq based project. 
# ------------------------------------------------------------------


WS="$(dirname "$0")/../"
LOGFILE="${WS}/build/logs/LOG_build_kernel.log"
touch $LOGFILE && > $LOGFILE



## prep envinronment 
        source ~/.bashrc
        source ${WS}/project_setup.env
        source ${WS}/scripts/setup.env
        . ${WS}/scripts/housekeeping.sh

        rm -rf $IMG_NAME $DTFILE

        rm -Rf $SYSMOD_DIR
        mkdir -p $SYSMOD_DIR
        mkdir -p $SYSMOD_DIR/usr


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
                        # export PATH=$PATH:$XVITIS/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin
                        XVITIS_MISSING=0
                } || {
                        XVITIS_MISSING=1
                        warning "XVITIS: specified path not found" \
                        "$0: could not locate the installation path for Xilinx Vitis/SDK $XVERSION [${XVITIS}]: check setup.env"
                }
        } || XVITIS_MISSING=1

        
        # Check for linux kernel source directory
        status "Finding linux repo... "
        [ -d "$LINUX_DIR" ] || {
                status "ADI linux directory not found..."
                printf "Clone from ADI repository? [Y/n]: "
                read -r response
                case "$response" in
                    [nN]|[nN][oO])
                        status "Skipping clone... Exiting" \
                        "Check path [${LINUX_DIR}]: project_setup.env"
                        error "$0: ${LINUX_DIR} not found"
                        exit 1
                        ;;
                    *)
                        status "Cloning from ADI repository..."
                        git clone https://github.com/analogdevicesinc/linux.git \
                            --no-single-branch --depth=10 \
                            -- linux | tee -a $LOGFILE 2>&1
                        mv -b linux/ linux-adi/
                        status "Linux repository cloned"
                        git checkout $lnx_branch
                        ;;
                esac
        }
        

        # Device-tree file
        [ -z "$DTFILE" ] && \
                error "dtc: No device tree file specified" \
                "DTC: check project_setup.env [dt path: $DTFILE]"

        DTFILE="${DTFILE%.*}.dtb"
        CHECK_DTFILE="${DTFILE%.*}.dts"
        [ -f "$LINUX_DIR/arch/$ARCH/boot/dts/$CHECK_DTFILE" ] || \
                error "$CHECK_DTFILE: Device tree file not found" \
                "DTC: check project_setup.env [dt path: $DTFILE]"
        
        status "DTC: Device tree file $DTFILE will be built"

        
        # Cross-compiler GCC/ELF toolchains
        [ ! -z "$CROSS_COMPILE" ] && \
                if type "${CROSS_COMPILE}gcc" >/dev/null 2>&1 ; then
                        CC_CHECK=1
                        # If the CC toolchain is verified, print out version, target info
                        printf "GCC target machine:\t" | tee -a $LOGFILE 2>&1
                        ${CROSS_COMPILE}gcc -dumpmachine | tee -a $LOGFILE 2>&1
                        
                        printf "GCC version:\t" | tee -a $LOGFILE 2>&1
                        ${CROSS_COMPILE}gcc -dumpversion | tee -a $LOGFILE 2>&1

                        status "CC: Using toolchain: ${CROSS_COMPILE}"
                        ${CROSS_COMPILE}gcc --version | tee -a $LOGFILE 2>&1
                else
                        error "$CROSS_COMPILE: specified CC toolchain not found" \
                        "CC: check setup.env [CC path: ${CROSS_COMPILE}]"
                fi
        || {
                # If VITIS is missing, use VIVADO to set cross-compiler
                # If both VIVADO & VITIS missing, fallback to versions supported by SCC
                [ "${XVITIS_MISSING}" -eq 1 ] && {
                        [ "${XVIVADO_MISSING}" -eq 0 ] && XVITIS=$XVIVADO || SCC_FALLBACK=1
                }

                . ${WS}/scripts/find_xilinx.sh
        }


        # Device type: Zynq(default), ZynqMP, Versal
        [ ! -z "$PART_FAM" ] && {
                case "$PART_FAM" in
                    [zZ][yY][nN][qQ])
                        # The board used below is generic and not project specific. 
                        # As long as the board is a ZYNQ FPGA, use the configuration below.
                        CONFIG_GEN_FILE="zynq_xcomm_adv7511_defconfig"
                        status "configuring kernel for default zynq settings"
                        ;;
                    [zZ][yY][nN][qQ][mM][pP])
                    # TODO: ADD THE CORRECT CONFIG_DEF FILES
                        CONFIG_GEN_FILE="adi_zynqmp_defconfig"
                        status "configuring kernel for default zynqMP settings"
                        ;;
                    [vV][eE][rR][sS][aA][lL])
                    # TODO: ADD THE CORRECT CONFIG_DEF FILES
                        CONFIG_GEN_FILE=""
                        status "configuring kernel for default versal settings"
                        ;;
                    *)
                        warning "Yo.. WTFFF u doinn..."
                        ;;
                esac
        } || {
                CONFIG_GEN_FILE="zynq_xcomm_adv7511_defconfig"
                status "configuring kernel for default zynq settings"
        }


        # Check for dtc (either Vitis or u-boot bin)
        type dtc >/dev/null 2>&1 || \
                error "DTC: device tree compiler (dtc) not found" \
                "setup.env: check if u-boot-tools (or Xilinx toolchains) are installed and set correct path"
        

    ### TODO(?): check if the following tools/libs are installed ###
        # - rsync
        # - u-boot-tools [mkimage utility]

        export ARCH
        export CROSS_COMPILE
        export XVERSION



## Build Kernel and Devicetree
        cd $LINUX_DIR
        make clean && make mrproper
        rm -rf arch/$ARCH/boot/$IMG_NAME
        
        # generate .config file
        make $CONFIG_GEN_FILE | tee -a $LOGFILE 2>&1
        make menuconfig 2>&1    # TODO: Automate?       # This will load an interactive build menu. Select the config options.
        
        # make modules and kernel headers
        make -j8 modules | tee -a $LOGFILE 2>&1
        make modules_install INSTALL_MOD_PATH=$SYSMOD_DIR/ | tee -a $LOGFILE 2>&1
        make headers_install INSTALL_HDR_PATH=$SYSMOD_DIR/usr | tee -a $LOGFILE 2>&1
        
        # build kernel image
        make -j5 UIMAGE_LOADADDR=0x8000 $IMG_NAME | tee -a $LOGFILE 2>&1
        # diff .config.old .config

        # generate DTB
        make $DTFILE | tee -a $LOGFILE 2>&1

## package boot files
        mkdir -p $PKG_BOOT/${proj_name}

        cp -f arch/arm/boot/$IMG_NAME $PKG_BOOT && \
        cp -f arch/arm/boot/dts/$DTFILE $PKG_BOOT && return_line && \
        status "BUILD SUCCESS: boot files generated: $IMG_NAME $DTFILE"

