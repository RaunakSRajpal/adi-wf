#!/bin/bash
set -xe


WS="$(pwd)"
ADI_DIR="$WS/adi-hdl"
HDL_DIR="$ADI_DIR/hdl"
XVERSION="2024.2"


IMG_NAME=${1:-"$IMG_NAME"}
DEV_ID=$2

# BOOT_DIR="$ADI_DIR/prep-BOOT"
# LOGFILE=$HDL_DIR/setup-HDL.log
# BUILD_DIR="/$ADI_DIR/build_boot_bin"
# OUTPUT_DIR="$ADI_DIR/output_boot_bin"


IMGversion="2025-03-18"
DOWNLOAD_IMG="https://swdownloads.analog.com/cse/kuiper/image_${IMGversion}-ADI-Kuiper-full.zip"
DEVICE="sdd"

WS="$(dirname "$0")/../"
LOGFILE=${ADI_DIR}/build/logs/setup-HDL.log
touch $LOGFILE && > $LOGFILE


## Check/prep directories
    # --TODO--
    [ ! -z $IMG_NAME ] || \
    		erorr "No image specified"
    [ ! -z $DEV_ID ] || \
     		error "No device name or mount point specified"
    
    # find device from device-name/mount-point/UUID
    DEVFILE=$(lsblk -o KNAME,PATH,MODEL,MOUNTPOINT,UUID | grep $DEV_ID | awk {'print $2'}) 2>&1
    if [ -z $DEVFILE ]; then
    		error "$DEV_ID: Block device not found"
    else
    		echo "$DEV_ID Located at $DEVFILE"
    fi

## Download the image file and verify
   time wget $DOWNLOAD_IMG
   md5sum image_${IMGversion}-ADI-Kuiper-full.zip
   time unzip image_${IMGversion}-ADI-Kuiper-full.zip real
   md5sum ${IMGversion}-ADI-Kuiper-full.img | tee -a $LOGFILE 2>&1
    
## Write img to SD-device
   sudo umount /dev/$DEVICE
   time sudo dd \
   	bs=4194304 \
   	status=progress \
   	of=/dev/$DEVICE \
   	if=${IMGversions}-ADI-Kuiper-full.img
   sync
    
# verify the flashed img
   sudo umount /dev/$DEVICE
   sudo fsck -y /dev/$DEVICE | tee -a $LOGFILE 2>&1
    


