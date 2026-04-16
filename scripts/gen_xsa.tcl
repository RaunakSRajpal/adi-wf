####################################################################
# Author:   Raunak Rajpal (rsrajpal@bu.edu)
# Cmpany:   WISE Circuits Lab, Boston University
# 
# Brief:    Exports hardware file (including bitstream) from Vivado.
# 			Expects to be run on the project root directory. 
# 
# Usage: 	gen_xsa.tcl <project_dir> -f <xsa_file_path> 
# 	project_dir		(REQ)	project root dir
# 	-f	  			(OPT)	forced; overwrite .xsa file
# 	xsa_file_path 	(OPT)	path to the exported .xsa file
####################################################################


# Parse and check argument/env variables passed from shell
    source housekeeping.tcl

    # Parse arguments
    if {[llength $argv] < 1} {
        status "No arguments provided"
        status "Reading shell environment variable: HDL_PROJ"

        if { ![info exists env(HDL_PROJ)] } {
            error "No project directory provided." \
            "Usage: add_srcs.tcl -tclargs <HDL-PROJECT-ROOT-DIR>"
        }
        set proj_dir $env(HDL_PROJ)

		if { ![info exists env(XSA_FILE)] } {
            set xsa_path ""
        }
		set xsa_path $env(XSA_FILE)
		
		set run_flags [list -f]
        set srcs_dir $proj_dir/srcs
        set constr_dir $proj_dir/constr
    } else {
        set proj_dir [lindex $argv 0]
		set run_flags [lindex $argv 1]
		set xsa_path [lindex $argv 2]
		if { [llength $run_flags] != 0 && [lindex $run_flags 0] ne "-f" } {
			error "unrecognized option" \
			"Options: -f \t\t force run. Overwrites existing .xsa file"
		}
    }

    if { ![file isdirectory $proj_dir] } {
        error "Vivado Project(.xpr) directory not found: $proj_dir" \
        "HDL_PROJ: required to export from shell env"
        return
    }


	# Vivado build directory
    set hdl_build_dir [glob -nocomplain $proj_dir/*_build]

    if { [llength $hdl_build_dir] == 0 } {
        error "Vivado Project(.xpr) build directory not found: $hdl_build_dir" \
        "hdl_build: must be exported to project directory after HDL build chain is complete"
        return
    } elseif {[llength $hdl_build_dir] > 1} {
        error "Multiple build projects located in: $proj_dir" \
        "Only one Vivado build supported per project directory"
        return
    }

    status "HDL build directory located: $hdl_build_dir"


	# check flags for overwriting .xsa
	if { [file exists $xsa_path] && [lindex $run_flags 0] ne "-f" } {
	    error "XSA_FILE: .xsa file already exists: $xsa_path; If you wish to overwrite existing file, use the \"-f\" option." \
		"Usage: vivado -source gen_xsa.tcl -tclargs <project_dir> -f (optional) <xsa_file_path> (optional)"
		return
	}


	# .xpr vivado project file
    set xpr_files [glob -nocomplain "$hdl_build_dir/*.xpr"]     # Path to the .xpr project file

    if { [llength $xpr_files] == 0 } {
        error "No .xpr project file found in: $hdl_build_dir" \
		"$0: HDL board IPs must need tpo be built before running this script."
        return
    } elseif {[llength $xpr_files] > 1} {
        error "Multiple .xpr project files found in: $hdl_build_dir" \
        "Only one Vivado project file(.xpr) supported per vivado-build directory\n\t\t Found: $xpr_files"
        return
    }

	set xpr_file [lindex $xpr_files 0]
    status "Vivado project file(.xpr) located: $xpr_file"
	set vivado_proj_name [file rootname [file tail $xpr_file]]
	status "Opening Vivado project: $xpr_file"

    open_project $xpr_file



# Run synthesis --> implementation --> bitstream
	# Check if bitstream already exists, else run implementation
	set runs [get_runs impl_1]
	if { [get_property PROGRESS $runs] != "100%" || 
	     [get_property STATUS $runs] != "route_design Complete!" } {
	    status "Running synthesis..."
	    launch_runs synth_1 -jobs 4
	    wait_on_run synth_1

	    status "Running implementation..."
	    launch_runs impl_1 -jobs 4
	    wait_on_run impl_1
	}

	set bit_files [glob -nocomplain "${proj_dir}/${vivado_proj_name}.runs/impl_1/*.bit"]
	if { [llength $bit_files] == 0 } {
	    status "Generating bitstream..."
	    launch_runs impl_1 -to_step write_bitstream -jobs 4
	    wait_on_run impl_1
	}



# Export hardware (.xsa) including bitstream
	if { [llength $xsa_path] == 0 } {
		set xsa_path "${proj_dir}/${vivado_proj_name}/system_top.xsa"
	}
	status "Exporting hardware to: $xsa_path"

	write_hw_platform -fixed -include_bit -force -file $xsa_path

	if { ![file exists $xsa_path] } {
	    puts "ERROR: XSA export failed, file not found: $xsa_path"
	    exit 1
	}

	status "Hardware file successfully exported: $xsa_path"
	close_project

