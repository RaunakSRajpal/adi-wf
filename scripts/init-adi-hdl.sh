#!/bin/bash
set -e

# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Company:  WISE Circuits Lab, Boston University
# 
# Brief:    Builds HDL project for supported SoC/FMC boards from ADI
#           and sources them inside a vivado project.
# 
#           Can also be used to build all ADI IPs and package them
#           inside the Vivado IP repository (set EVAL_BD="library")
# ------------------------------------------------------------------

# proj_name="$1"
# EVAL_BD=${2:-"adrv9361z7035"}
# CARRIER=${3:-"ccbob_cmos"}
# hdl_branch=${4:-"hdl_2023_r2"}        # "main"}

WS="$(dirname "$0")/../"
LOGFILE=${ADI_DIR}/build/logs/setup-HDL.log
touch $LOGFILE && > $LOGFILE



## prep environment
        source ~/.bashrc
        source ${WS}/project_setup.env
        source ${WS}/scripts/setup.env
        . ${WS}/scripts/housekeeping.sh

        # mkdir -p $ADI_DIR/ && cd $ADI_DIR/


        # Check if project space exists
        [ -z $proj_name ] && \
                error "$0: Project name not specified" \
                "proj_name: All HDL projects must be defined in project_setup.env [${WS}/project_setup.env]"
        status "$0: using project space $proj_name [${HDL_PROJ}]"

        [ ! -d "${HDL_PROJ}" ] && \
                warning "Directory not found: ${proj_name} -- creating it now"
        mkdir -p "${HDL_PROJ}"


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


        # Check for HDL source directory
        status "Finding ADI-HDL repo... "
        [ -d "$HDL_DIR" ] || {
                status "ADI HDL directory not found..."
                printf "Clone from ADI repository? [Y/n]: "
                read -r response
                case "$response" in
                    [nN]|[nN][oO])
                        status "Skipping clone... Exiting" \
                        "Check path [${HDL_DIR}]: project_setup.env"
                        error "$0: ${HDL_DIR} not found"
                        exit 1
                        ;;
                    *)
                        status "Cloning from ADI repository..."
                        git clone https://github.com/analogdevicesinc/hdl.git \
                            --no-single-branch --depth=10 \
                            -- hdl | tee -a $LOGFILE 2>&1
                        # mv -b hdl/ hdl-adi/
                        status "HDL repository cloned"
                        git checkout $hdl_branch
                        ;;
                esac
        }

        
        # HDL board build directory
        [ -d "${HDL_DIR}/projects/$EVAL_BD/$CARRIER" ] || \
        	error "$0: ADI project board not found" \
                "EVAL_BD/CARRIER: must be defined in project_setup.env [${WS}/project_setup.env]"
        


## Build HDL project 
        export ADI_IGNORE_VERSION_CHECK=1
        (
                cd ${XPR_ROOT} && \
                make -j5 | tee -a $LOGFILE 2>&1
                status "Project board: $EVAL_BD-$CARRIER: build complete"

	        mkdir -p ${HDL_PROJ}/srcs
                mkdir -p ${HDL_PROJ}/constr
                export ${HDL_PROJ}

                # this extracts the directory name of the hdl-build
                # cp -r $(dirname "${XSA_FILE}") ${HDL_PROJ} && \
                # HDL_BUILD_DIR=${HDL_PROJ}/$(basename $(dirname "${XSA_FILE}"))
                cp -r ${XPR_ROOT} ${HDL_PROJ} && \
                mv -b ${HDL_PROJ}/$(basename ${XPR_ROOT}) ${HDL_PROJ}/${EVAL_BD}-${CARRIER}-${proj_name}_build
                export $LOGFILE

                # run tcl script to load srcs in .xpr project
                vivado -mode batch \
                        -nolog -nojournal \
                        -source add_srcs.tcl \
                        -tclargs ${HDL_PROJ}
                
                # run tcl script to export hardware(.xsa) file
                # \\ TODO: \\
        )


## Build ADI IP cores
        if [ "$EVAL_BD" == "library" ] ; then (
                cd ${HDL_DIR} && \
                make -C lbrary all | tee -a $LOGFILE 2>&1
                status "$HDL_DIR/library: build complete"
                ## -- TODO --
                #--- tcl: add library directory to Vivado IP repository
                # run tcl script to load srcs in .xpr project
                # vivado -mode batch \
                #         -nolog -nojournal \
                #         -source <>.tcl \
                #         -tclargs ${HDL_BUILD_DIR}
        )




# bootstrapping a bash/tcl script 
    # #!/bin/sh
    # the next line restarts using tclsh \
    # exec tclsh "$0" "$@"
