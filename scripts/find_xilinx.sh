#!/bin/bash
set -e

# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Company:  WISE Circuits Lab, Boston University
# 
# Brief:    1> IF RUN STAND-ALONE:
#           Scans the SCC for Xilinx Vivado/Vitis(SDK) installations
#           and reports the bundled cross-compiler toolchains
#           available for each version, along with their supported 
#           target architectures.
# 
#           2> IF SOURCED:
#           Selects the specified Xilinx Vitis(SDK) installation on
#           SCC and bundled cross-compiler toolchains for the build.
#           If SCC_FALLBACK is set to 0(or unset), Vitis/SDK path 
#           selection is skipped and expects XVITIS to be set.
# ------------------------------------------------------------------


##  Initialise the script (Only if run standalone)
if ! (return 2>/dev/null) ; then

init_find_xilinx() {
        WS="$(dirname "$0")"
        LOGFILE="${WS}/LOG_find_xilinx.log"
        touch $LOGFILE && > $LOGFILE

        source ${WS}/../project_setup.env
        source ${WS}/setup.env
        . ${WS}/housekeeping.sh

        ARCH=arm
        OS=y    # [y/n]
}

fi


##  Select the Xilinx SDK installation path for the specified version
xver_sdk_sel() {

        status "Xilinx version: $XVERSION"

        case "$XVERSION" in
            2018.2) XVIVADO=${XVIVADO_18V2} && XVITIS=${XSDK_18V2}      ;;
            2019.1) XVIVADO=${XVIVADO_19V1} && XVITIS=${XSDK_19V1}      ;;
            2020.2) XVIVADO=${XVIVADO_20V2} && XVITIS=${XVITIS_20V2}   ;;
            2021.2) XVIVADO=${XVIVADO_21V2} && XVITIS=${XVITIS_21V2}   ;;
            2022.1) XVIVADO=${XVIVADO_22V1} && XVITIS=${XVITIS_22V1}   ;;
            2024.1) XVIVADO=${XVIVADO_24V1} && XVITIS=${XVITIS_24V1}    ;;
            ### Add support for newer xilinx versions here (Check setup.env) ###
            *)
                error "No existing path for Xilinx version $XVERSION available on SCC." \
                "Current SCC support for Xilinx tools is limited to versions: ${XSDK_VERSIONS}"
                ;;
        esac

        status "existing path for Xilinx Vivado Tools $XVERSION:\n\t\t$XVIVADO"
        status "existing path for Xilinx SDK Tools $XVERSION:\n\t\t$XVITIS"
        
        [ -f "$XVITIS/settings64.sh" ] && [ -d "$XVITIS/bin" ] && {
                status "Found Xilinx version $XVERSION tools"
                . ${XVITIS}/settings64.sh
                export PATH=$PATH:$XVIVADO/bin:$XVITIS/bin
        } || \
                error "Existing path for Xilinx $XVERSION SWE is incorrect.\n" \
                "Check env paths in file: [${WS}/scripts/setup.env]"

}


##  Select the cross-compiler for a given ARCH and SDK version
xarch_cc_sel() {

        if [ ${OS} == 'y' ] ; then
                case "$ARCH" in
                    arm)        CROSS_COMPILE=${XVITIS}/${XLNX_ARM_LNX_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_AARCH32_LNX_CC__PATH}
                                ;;
                    arm64)      CROSS_COMPILE=${XVITIS}/${XLNX_ARM64_LNX_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_AARCH64_LNX_CC__PATH}
                                ;;
                    armr5)      CROSS_COMPILE=${XVITIS}/${XLNX_ARMR5_ELF_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_ARMR5_NON_CC__PATH}
                                warning "CC: set to ELF EABI [${CROSS_COMPILE}]" \
                                "CC: ARM Cortex-R5 is meant to handle hard real-time, deterministic workloads NOT generic OS hosting."
                                ;;
                    microblaze) CROSS_COMPILE=${XVITIS}/${XLNX_MB_LNX_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_MB_LNX_CC__PATH}
                                ;;
                    riscv)      CROSS_COMPILE=${XVITIS}/${XLNX_RISCV_ELF_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_RISCV_NON_CC__PATH}
                                warning "CC: set to ELF EABI [${CROSS_COMPILE}]" \
                                "CC: Xilinx GCC support for RISC-V architecture is limited to ELF binaries"
                                ;;
                    ### Add Xilinx support for newer architectures here (Check setup.env) ###
                    *)
                        error "$ARCH: architecture not currently supported by Xilinx for linux GNU-EABI" \
                        "Supported architectures inlcude: $ARCHS_XLNX"
                        ;;
                esac
        elif [ ${OS} == 'n' ] ; then
                case "$ARCH" in
                    arm)        CROSS_COMPILE=${XVITIS}/${XLNX_ARM_NON_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_AARCH32_NON_CC__PATH}
                                ;;
                    arm64)      CROSS_COMPILE=${XVITIS}/${XLNX_ARM64_NON_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_AARCH64_NON_CC__PATH}
                                ;;
                    armr5)      CROSS_COMPILE=${XVITIS}/${XLNX_ARMR5_ELF_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_ARMR5_NON_CC__PATH}
                                ;;
                    microblaze) CROSS_COMPILE=${XVITIS}/${XLNX_MB_NON_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_MB_NON_CC__PATH}
                                ;;
                    riscv)      CROSS_COMPILE=${XVITIS}/${XLNX_RISCV_ELF_CC}
                                export PATH=$PATH:${XVITIS}/${XLNX_RISCV_NON_CC__PATH}
                                ;;
                    ### Add Xilinx support for newer architectures here (Check setup.env) ###
                    *)
                        error "$ARCH: architecture not currently supported by Xilinx for ELF (bare-metal) EABI" \
                        "Supported architectures inlcude: $ARCHS_XLNX"
                        ;;
                esac
        else
                error "OS: linvalid input" \
                "Only takes [y/n]: (bare-metal/RTOS - n; linux - y)"
        fi


        if type ${CROSS_COMPILE}gcc >/dev/null 2>&1 ; then
            # If the CC toolchain is verified, print out version, target info
                status "CC: Using toolchain: ${CROSS_COMPILE}"
                ${CROSS_COMPILE}gcc --version | tee -a $LOGFILE 2>&1

                printf "GCC target machine:\t" | tee -a $LOGFILE 2>&1
                ${CROSS_COMPILE}gcc -dumpmachine | tee -a $LOGFILE 2>&1

                printf "GCC version:\t" | tee -a $LOGFILE 2>&1
                ${CROSS_COMPILE}gcc -dumpversion | tee -a $LOGFILE 2>&1
        else
                warning "CC: GCC cross-compiler toolchain (Xilinx) not found for the arch: $ARCH" \
                "[${CROSS_COMPILE}gcc] not found"
        fi

}




##  MAIN: Runs IF segment only if the script is not being sourced

if ! (return 2>/dev/null) ; then
    # standalone
        init_find_xilinx
        for XVERSION in $XSDK_VERSIONS; do
            xver_sdk_sel

            OS=y
            for ARCH in $ARCHS_XLNX; do
                xarch_cc_sel
            done
            return_line

            OS=n
            for ARCH in $ARCHS_XLNX; do
                xarch_cc_sel
            done
            return_line
            return_line
        done
else
    # sourced
        [ "${SCC_FALLBACK:-0}" -eq 1 ] && xver_sdk_sel  # selects SCC fallback version support
        xarch_cc_sel
        export CROSS_COMPILE
fi



