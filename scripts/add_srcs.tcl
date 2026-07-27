####################################################################
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Cmpany:   WISE Circuits Lab, Boston University
# 
# Brief:    Includes all source and constraint files in their 
#           respective filesets. Expects to be run on the project 
#           root directory. All source files must be inside srcs dir
#           and constraints in constr dir.
# 
# Usage:    add_src.tcl <project_dir>
#   project_dir		(REQ)	project root dir
####################################################################


# Parse and check argument/env variables passed from shell
    source $env(WS)/scripts/housekeeping.tcl

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

    } else {
        set proj_dir [lindex $argv 0]
    }

    if {![file isdirectory $proj_dir]} {
        error "Vivado Project(.xpr) directory not found: $proj_dir" \
        "HDL_PROJ: required to export from shell env"
        return
    }
	set srcs_dir $proj_dir/srcs
	set constr_dir $proj_dir/constr


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
    set no_srcs_dir 0; set no_constr_dir 0
    if {![file isdirectory $srcs_dir]} {
        warning "No source directory found" \
        "custom RTL must be packaged inside \[$proj_dir/srcs\] directory"
        set no_srcs_dir 1
    }
    if {![file isdirectory $constr_dir]} {
        warning "No constraints directory found" \
        "custom constraint files(xdc/sdc) must be packaged inside \[$proj_dir/constr\] directory"
        set no_constr_dir 1
    }


    # .xpr vivado project file
    set xpr_files [glob -nocomplain $hdl_build_dir/*.xpr];		# Path to the .xpr project file

    if {[llength $xpr_files] == 0} {
        error "No .xpr project file found in: $hdl_build_dir"
        return
    } elseif {[llength $xpr_files] > 1} {
        error "Multiple .xpr project files found in: $hdl_build_dir" \
        "Only one Vivado project file(.xpr) supported per vivado-build directory\n\t\t (Found: $xpr_files)"
        return
    }

    set xpr_file [lindex $xpr_files 0]
	set xpr_filename [file tail $xpr_file]
    status "Vivado project file(.xpr) located \n\t\t(file $xpr_file)"
    # status "Opening Vivado project: $xpr_filename \n\t\t(file: $xpr_file)"
    # open_project $xpr_file


    # Recursively find all source files
    if {!$no_srcs_dir} {
        set src_files [glob_recursive $srcs_dir {
			*.v		\
			*.sv	\
			*.vhd	\
			*.vhdl	\
			*.xdc	\
			*.sdc	\
		}]

        if {[llength $src_files] == 0} {
            warning "No source files found in: $srcs_dir" \
            "custom RTL must be packaged inside \[$proj_dir/srcs\] directory\n\t\t (Found: $src_files)"
        }
    }


    # Recursively find all constraint files (XDC/SDC)
    if {!$no_constr_dir} {
        lappend constr_files {*}[glob -nocomplain -directory $constr_dir -- *.xdc]
        lappend constr_files {*}[glob -nocomplain -directory $constr_dir -- *.sdc]
    }



# Add files to the project
    set constr_files {}
    set tb_files {}
    set des_src_files {}

    # filter src/tb/constr files
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


    # Include source files in Vivado project (.xpr)
    if {[llength $des_src_files] == 0} {
        warning "pldev_srcs: No files found." \
        "No recognised source files were found at: $srcs_dir"
    } else {
        add_files -fileset sources_1 $des_src_files
        status "Include source files: $des_src_files"
    }

    if {[llength $tb_files] == 0} {
        warning "pldev_sim: No files found." \
        "No recognised testbench(_tb) files were found at: $srcs_dir"
    } else {
        add_files -fileset sim_1 $tb_files
        status "Include simulation files: $tb_files"
    }

    if {[llength $constr_files] == 0} {
        warning "pldev_constrs: No files found." \
        "No recognised constraint files were found at: $srcs_dir or $constr_dir"
    } else {
        status "I got till here!! $constr_files"
        add_files -fileset constrs_1 $constr_files
        status "Include constraint files: $constr_files"
    }



# Update compile order and clean exit
    status "Opening Vivado project: $xpr_filename \n\t\t(file: $xpr_file)"
    open_project $xpr_file
    update_compile_order -fileset sources_1
    update_compile_order -fileset sim_1
    # update_compile_order -fileset constrs_1

    # save_project
    status "Project saved: $xpr_file"
    close_project

exit 0

