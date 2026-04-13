####################################################################
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Cmpany:   WISE Circuits Lab, Boston University
# 
# Brief:    Includes all source and constraint files in their 
#           respective filesets. 
#           Expects to be run on the project root directory
####################################################################


# Parse and check argument/env variables passed from shell
    source housekeeping.tcl

    # parse arguments
    if {[llength $argv] < 1} {
        status "No arguments provided"
        status "Reading shell environment variable: HDL_PROJ"

        if {![info exists env(HDL_PROJ)]} {
            error "No project directory provided." \
            "Usage: add_srcs.tcl -tclargs <HDL-PROJECT-ROOT-DIR>"
            exit 1
        }

        set proj_dir $env(HDL_PROJ)
        set srcs_dir $proj_dir/srcs
        set constr_dir $proj_dir/constr
    } else {
        set proj_dir [lindex $argv 0]
    }

    if {![file isdirectory $proj_dir]} {
        error "Vivado Project(.xpr) directory not found: $proj_dir" \
        "HDL_PROJ: required to export from shell env"
        return
    }


    # Vivado build directory
    set hdl_build_dir [glob -nocomplain $proj_dir/*_build]

    if {[llength $hdl_build_dir] == 0} {
        error "Vivado Project(.xpr) build directory not found: $hdl_build_dir" \
        "hdl_build: must be exported to project directory after HDL build chain is complete"
        return
    } elseif {[llength $hdl_build_dir] > 1} {
        error "Multiple build projects located in: $proj_dir" \
        "Only one Vivado build supported per project directory"
        return
    }

    status "HDL build directory located: $hdl_build_dir"


    # RTL sources/constr directory
    set no_srcs 0; set no_constr 0
    if {![file isdirectory $srcs_dir]} {
        warning "No source directory found" \
        "custom RTL must be packaged inside \[$proj_dir/srcs\] directory"
        set no_srcs 1
    }
    if {![file isdirectory $constr_dir]} {
        warning "No constraints directory found" \
        "custom constraint files(xdc/sdc) must be packaged inside \[$proj_dir/constr\] directory"
        set no_constr 1
    }


    # .xpr file
    set xpr_file [glob -nocomplain $hdl_build_dir/*.xpr]     # Path to the .xpr project file

    if {[llength $xpr_file] == 0} {
        error "No .xpr project file found in: $hdl_build_dir"
        return
    } elseif {[llength $xpr_file] > 1} {
        error "Multiple .xpr project files found in: $hdl_build_dir" \
        "Only one Vivado project file(.xpr) supported per vivado-build directory"
        return
    }

    status "HDL build directory located: $hdl_build_dir"
    open_project [lindex $xpr_file 0]


    # Recursively find all source files
    if {!$no_srcs} {
        set src_files [glob -nocomplain -recurse \
            $srcs_dir/*.v       \
            $srcs_dir/*.sv      \
            $srcs_dir/*.vhd     \
            $srcs_dir/*.vhdl    \
            $srcs_dir/*.xdc     \
            $srcs_dir/*.sdc     \
        ]

        if {[llength $src_files] == 0} {
            warning "No source files found in: $srcs_dir" \
            ""
        }
    }   
    


# Add files to the project
    set constr_files {}
    set tb_files {}
    set des_src_files {}

    # filter out tb/constr files
    foreach f $src_files {
        set ext [file extension $f]
        set tail [file tail $f]
        if {$ext eq ".xdc" || $ext eq ".sdc"} {
            lappend constr_files $f
        } elseif {[regexp {.*_tb\..*} $tail]} {
            lappend tb_files $f
        } else {
            lappend des_src_files $f
        }
    }


    # constraint files (XDC/SDC)
    lappend constr_files [glob $constr_dir/*{.xdc,.sdc}]
    add_files -fileset constrs_1 $constr_files
    status "Added constraint files: $constr_files"

    # design and tb source files
    add_files -fileset sources_1 $des_src_files
    add_files -fileset sim_1 $tb_files
    status "Added source files: $des_src_files"
    status "Added simulation files: $tb_files"



# Update compile order and save project
    update_compile_order -fileset sources_1
    update_compile_order -fileset sim_1
    update_compile_order -fileset constrs_1

    save_project_as [lindex $xpr_file 0]
    status "Project saved: [lindex $xpr_file 0]"

exit 0

