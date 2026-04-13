####################################################################
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Cmpany:   WISE Circuits Lab, Boston University
#
# Brief:    House-keeping tasks to keep other build scripts clean
####################################################################

# Global logfile path — set via argument or environment
if {[info exists env(LOGFILE)]} {
    set LOGFILE $env(LOGFILE)
} elseif {[llength $argv] >= 1} {
    set LOGFILE [lindex $argv 0] 
} else {
    set LOGFILE ""
}

# ----------------------------------------------------------------
# log_write: log messages to both stdout and logfile
# ----------------------------------------------------------------
proc log_write {msg} {
    global LOGFILE
    puts $msg
    if {$LOGFILE ne ""} {
        set fh [open $LOGFILE a]
        puts $fh $msg
        close $fh
    }
}

# ----------------------------------------------------------------
# Logging procedures/functions
# ----------------------------------------------------------------
proc usage {msg} {
    global argv0 LOGFILE
    log_write "\[ERROR\]\t$msg"
    log_write "\tusage: $argv0 <linux-repo dir> <device-tree file>"
    exit 1
}

proc error {msg detail} {
    log_write "\n\[ERROR\]\t$msg"
    log_write "\t\t$detail\n"
    exit 1
}

proc status {msg} {
    log_write "\[INFO\]\t$msg"
}

proc warning {msg detail} {
    log_write "\[WARNING\]\t$msg"
    log_write "\t$detail"
}

proc return_line {} {
    log_write ""
}

