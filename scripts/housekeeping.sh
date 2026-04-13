#!/bin/bash
# ------------------------------------------------------------------
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# 
# Brief:    House-keeping tasks to keep other build scripts clean
# ------------------------------------------------------------------

if ! (return 2>/dev/null); then
    # if run standalone
    LOGFILE="${1:-$LOGFILE}"
    printf "LOGFILE: $1\t$0\n"
fi

usage() {
    echo -e "[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    echo -e "\tusage: $0 <linux-repo dir> <device-tree file> \n" | tee -a $LOGFILE 2>&1
    exit 1
}

error() {
    printf "\n[ERROR]\t$1\n" | tee -a $LOGFILE >&2
    printf "\t\t$2\n\n" | tee -a $LOGFILE >&2
    exit 1
}

status() {
    printf "[INFO]\t$1\n" | tee -a $LOGFILE 2>&1
}

warning() {
    printf "[WARNING]\t$1\n" | tee -a $LOGFILE 2>&1
    printf "\t\t$2\n" | tee -a $LOGFILE 2>&1
}

return_line() {
    printf "\n" | tee -a $LOGFILE 2>&1
}
