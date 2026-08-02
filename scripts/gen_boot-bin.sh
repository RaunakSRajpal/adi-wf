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

WS="$(realpath "$(dirname $0)/../")"
LOGFILE=${WS}/build/logs/gen_BOOT.log
mkdir -p ${WS}/build/logs/
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
	[ -d $(dirname ${XSA_FILE}) ] || \
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
	(( XVIVADO_MISSING == 1 && XVITIS_MISSING == 1 )) && {
		SCC_FALLBACK=1
		export XILINXD_LICENSE_FILE=2100@XilinxLM.bu.edu
	}

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

		# extracted_content=$(python3 -c "import zipfile,sys; sys.stdout.write(zipfile.ZipFile('${XSA_FILE}').read('PATH_TO_FILE').decode())")
		# carrier=$(echo "$extracted_content" | grep -a "PATH_TO_FILE" | grep -oE "$(IFS='|'; echo "${patterns[*]}")")

		carrier="$(echo ${CARRIER} | grep -oE "$(IFS='|'; echo "${patterns[*]}")")"
		case  $carrier  in
		    zed)		uboot_elf="u-boot_zynq_zed.elf" ;;
		    ccfmc_*|ccbob_*)	uboot_elf="u-boot_zynq_adrv9361.elf" ;;
		    usrpe31x)		uboot_elf="u-boot-usrp-e310.elf" ;;
		    zc702)		uboot_elf="u-boot_zynq_zc702.elf" ;;
		    zc706)		uboot_elf="u-boot_zynq_zc706.elf" ;;
		    coraz7s)		uboot_elf="u-boot_zynq_coraz7.elf" ;;
		    *)
			error "ELF: u-boot.elf not found" \
			"!!!!! The specified carrier does not have a downloadable u-boot.elf file !!!!!"
			;;
		esac

		# Extract version info from the hdl branch name
		if [[ $hdl_branch =~ ([0-9]{4}_r[0-9]+) ]]; then
			boot_partition_location="${BASH_REMATCH[1]}"
			export UBOOT_FILE=${BOOT_DIR}/${uboot_elf}
			# echo "uboot_elf=${uboot_elf}" >> ${WS}/dump.env
		else
			error "ELF: Broken link. Not a valid file path URL" \
			"Found elf dump file for evaluation board but No version matched for hdl branch: $hdl_branch (file: ${WS}/scripts/setup.env)"
		fi

		[ -z $elf_dump_url_root ] && \
			error "ELF: Broken link. Not a valid file path URL" \
			"ELF dumpfile url is broken due to missing or incorrect env var 'elf_dump_url_root' (env var: ${WS}/scripts/setup.env: elf_dump_url_root=${elf_dump_url_root})"
		mkdir -p ${BOOT_DIR}

		status "Downloading $uboot_elf ..."
	        wget -O "${UBOOT_FILE}" ${elf_dump_url_root}/${boot_partition_location}/${uboot_elf} && {
			status "u-boot.elf file dowloaded at: ${UBOOT_FILE}"
			update_env_var ${WS}/project_setup.env "uboot_elf" ${uboot_elf}
		}
        else
		[ ! -f "$uboot_elf" ] && [ ! -f "$UBOOT_FILE" ] && \
			error "ELF: u-boot.elf file not found" \
			"$0: check the u-boot file path: ${WS}/project_setup.env. \n\t\t (files uboot_elf: ${uboot_elf} or UBOOT_FILE: ${UBOOT_FILE})"
		[ -f "$uboot_elf" ] && [ -f "$UBOOT_FILE" ] && \
			error "ELF: multiple core dumpfiles found, only one uboot.elf is supported" \
			"$0: multiple u-boot ELF files were located, creating filepath conflicts. Check files: \n\t\t (${uboot_elf}) \n\t\t (${UBOOTFILE})"

		[ -f "$uboot_elf" ] && {
			status "u-boot.elf file located: ${uboot_elf}"
			export UBOOT_FILE=${BOOT_DIR}/$(basename ${uboot_elf})
			cp -bu ${uboot_elf} $UBOOT_FILE
			update_env_var ${WS}/project_setup.env "uboot_elf" $(basename ${uboot_elf})
		}

		[ -f "$UBOOT_FILE" ] && \
			status "u-boot.elf file located: ${UBOOT_FILE}"
	fi



## Build BOOT.BIN
	rm -Rf $BUILD_BOOT_DIR $OUTPUT_DIR
	mkdir -p $OUTPUT_DIR
	mkdir -p $BUILD_BOOT_DIR


	# Create create_fsbl_project.tcl file used by xsct to create the fsbl.
	###---------    DO NOT EDIT THIS    ---------###
	echo "hsi open_hw_design `basename $XSA_FILE`" > $BUILD_BOOT_DIR/create_fsbl_project.tcl
	echo 'set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
	echo 'platform create -name hw0 -hw system_top.xsa -os standalone -out ./build/sdk -proc $cpu_name' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
	echo 'platform generate' >> $BUILD_BOOT_DIR/create_fsbl_project.tcl
        
	FSBL_PATH="$BUILD_BOOT_DIR/build/sdk/hw0/export/hw0/sw/hw0/boot/fsbl.elf"
	SYSTEM_TOP_BIT_PATH="$BUILD_BOOT_DIR/build/sdk/hw0/hw/system_top.bit"
	###-------------------------------------------###


	# Create zynq.bif file used by bootgen
	###---------    DO NOT EDIT THIS    ---------###
	echo 'the_ROM_image:' > $BOOT_DIR/zynq.bif
	echo '{' >> $BOOT_DIR/zynq.bif
	echo '[bootloader] fsbl.elf' >> $BOOT_DIR/zynq.bif
	echo 'system_top.bit' >> $BOOT_DIR/zynq.bif
	echo 'u-boot.elf' >> $BOOT_DIR/zynq.bif
	echo '}' >> $BOOT_DIR/zynq.bif
	###-------------------------------------------###


	# Build fsbl.elf
	cp -f $XSA_FILE $BUILD_BOOT_DIR/
	(
		cd $BUILD_BOOT_DIR
		xvfb-run --auto-servernum xsct create_fsbl_project.tcl
	) || error "fsbl build failed. Aborting..." \
		"xsct: xilinx command-line tool requires Xvfb dependency (dir ${BUILD_BOOT_DIR})"
	status "xsct: bootloader file fsbl.elf created"


	# Build BOOT.BIN
	cp -f $XSA_FILE $OUTPUT_DIR/
	cp -f $UBOOT_FILE $OUTPUT_DIR/u-boot.elf
	cp -f $FSBL_PATH $OUTPUT_DIR/fsbl.elf
	cp -f $SYSTEM_TOP_BIT_PATH $OUTPUT_DIR/system_top.bit
	cp -f $BOOT_DIR/zynq.bif $OUTPUT_DIR/zynq.bif

	(
		cd $OUTPUT_DIR
		bootgen -arch zynq -image $BOOT_DIR/zynq.bif -o BOOT.BIN -w
	) || \
		error "bootgen failed. Aborting..." \
		"bootgen: requires xilinx toolchains"
	status "BOOT.BIN created!"



## Generate SDcard files
	status "Generated all files for build-boot: $(ls -w0 ${OUTPUT_DIR})" | tee $LOGFILE 2>&1
	status "Files for bootable media can be found in directory: ${PKG_BOOT}" | tee $LOGFILE 2>&1

	mkdir -p $PKG_BOOT/
	cp -f ${OUTPUT_DIR}/BOOT.BIN $PKG_BOOT/


    #---- build linux script ----
    # bash ../setup-uboot-proj.sh linux-adi/ xilinx/zynq-adrv9361-z7035-bob-cmos.dtb
