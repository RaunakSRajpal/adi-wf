<head>
  <style type="text/css">
    .tree {
      font-family: monospace;
      line-height: 1.5;
      padding: 0.8em 1em;
      background-color: transparent;
    }
    .tree a { text-decoration: none; }
    .DIR  { color: #6cb6ff; font-weight: bold; }
    .NORM { color: inherit; }
    .EXEC { color: #7ee787; }
    .LINK { color: aqua; }
  </style>
</head>



# adi-wf

This repository is a design workflow for building and deploying  specific projects based on evaluation boards supported by [Analog Devices, Inc](https://wiki.analog.com/resources/eval/user-guides#fmc_compatible_boards). The flow tools are designed for complete bring up of the evaluation board. This repo provides automation scripts and tools, HDL IP support, device drivers, and containerized build tooling.


## Table of Contents

- [Project Overview](#project-overview)
    <!-- - [Hardware Architecture](#hardware-architecture) -->
    <!-- - [Project Architecture](#project-architecture) -->
- [Build Workflow](#build-workflow)
    - [Getting Started](#getting-started)
    - [Steps](#steps)
- [Device Drivers](#device-drivers)
- [Containers](#containers)
- [Resources & References](#resources--references)
- [Related Repositories](#related-repositories)



## Project Overview

This repository supports the complete hardware/software co-design of an SDR system. The design integrates:

< -- REMOVE -- >
- **ADRV9361-Z7035 SoM** — ADI System-on-Module housing a Zynq-7000 SoC and an RF module (AD9361)
- **PZSDRCC-BRK-PCB-C** — ADI carrier board for programming and development
- **ORBGRAND PCB** — Houses the ORBGRAND decoder chip (swappable for future GRAND project revisions)

[--- FIX THIS ---]
- **Evaluation Board** — 
    - **dafuq**
- **Carrier Module/PCB** — 

Most of the supported evaluation boards have a single chip housing both the FPGA (programable logic or **PL**) and the hardened processor (processing system or **PS**) on the same die. This provides for the SoC to be incredibly versatile in 

### Architecture


> the *`hdl/`* and *`linux-adi/`* directories are cloned respectively from the [analogdevicesinc/**hdl**](https://github.com/analogdevicesinc/hdl) and [analogdevicesinc/**linux**](https://github.com/analogdevicesinc/linux) repos. These provide support for several SoM and FMC evaluation boards. Check the section on related [repos](#related-repositories) for more information.

More details are provided in the sections following [--].

After all the required builds are complete, the adi-wf root directory, which will be called workspace or **WS** should look like the directory tree shown below :

<body>
<pre class="tree">
<a class="DIR">adi-wf/</a>
├── <a class="DIR">BIN_pkg/</a>		------------------->	flow completion pkg
│   ├── <a class="DIR">[ required dirs ]</a>
│   └── <a class="NORM">[ required files ]</a>
├── <a class="DIR">build/</a>           ----------------------->	logs, build dirs for 
│   ├── <a class="DIR">logs</a>                                     individual projects
│   ├── <a class="DIR">project-1</a>
│   |   ├── <a class="DIR">BOOT</a>
│   |	├── <a class="DIR">sysmod_rootfs</a>
│   |   └── <a class="NORM">[ build files ]</a>
|   ...
|   ...
│   └── <a class="DIR">project-n</a>
│   	├── <a class="DIR">BOOT</a>
│   	├── <a class="DIR">sysmod_rootfs</a>
│       └── <a class="NORM">[ build files ]</a> 
├── <a class="DIR">containers/</a>		<!-- ---------------\>	containerized support -->
│   ├── <a class="NORM">README.md</a>
│   ├── <a class="NORM">sing_adiwf_ubuntu20.04.def</a>
│   └── <a class="EXEC">sing_adiwf_ubuntu20.04.v03.sif</a>
├── <a class="DIR">drivers/</a>		------------------->	device driver sources
├── <a class="DIR">hdl/</a>
├── <a class="DIR">lib/</a>	-------------------->	src files, provide C wrapper functions
├── <a class="DIR">linux-adi/</a>
├── <a class="DIR">pl-proj/</a>           ------------------------>     FPGA projects: custom RTL srcs 
│   ├── <a class="DIR">project-1</a>                -----|              and built bd IPs
│   |   ├── <a class="DIR">[eval_bd]_build</a>           |
│   |   ├── <a class="DIR">srcs</a>                      |
│   |   └── <a class="DIR">constr</a>                    |
│   └── <a class="DIR">project-2</a>                -----|
│   |   ├── <a class="DIR">[eval_bd]_build</a>
│   |   ├── <a class="DIR">srcs</a>
│   |   └── <a class="DIR">constr</a>
├── <a class="DIR">scripts/</a>		------------------->	automation scripts
│   ├── <a class="NORM">gen_boot-bin.sh</a>
│   ├── <a class="NORM">init-adi-hdl.sh</a>
│   ├── <a class="NORM">setup-uboot-proj.sh</a>
│   ├── <a class="NORM">setup.env</a>
|   ...
|   ...
│   └── <a class="NORM">update_gcc.sh</a>
├── <a class="NORM">LICENSE</a>
├── <a class="NORM">README.md</a>
├── <a class="NORM">project_setup.env</a>		------------------->	CONFIG project here
└── <a class="NORM">top_script.sh</a>	------------>	moves packaged files to mnt drive
</pre>
</body>

---



## Build Workflow

### Getting Started
If this is your first time cloning or building the repo, you can follow the steps below to the build and run a prototype project. For more advanced tooling check out [Steps](#steps).

1. **Configure the project**
    ```bash
    git
    ```

### Steps

1. **Clone the repository**
   ```bash
   git clone <repo-link>
   ```
   The `scripts/` directory contains all automation scripts, independent of board selection.

2. **Initialise the project directory**
   Sets up required IPs and generates the `.xpr` file for the target board.
   ```bash
   # Build IPs for a specific board or all boards (library directory)
   ./scripts/init_project.sh
   ```

3. **Open the project in Vivado**
   Modify the block design as needed. ADI-provided IPs can be incorporated into custom projects.

4. **Synthesize and implement**
   Run Synthesis → Implementation → Generate Bitstream → Export Hardware (`.xsa` file).

5. **Software development**
   Use the exported `.xsa` with:
   - **Vitis** — bare-metal software for the ARM processor
   - **Linux** — boot a full Linux distro on the device

6. **Generate boot binaries**
   ```bash
   ./scripts/gen-boot.sh
   ```
   An interactive menu allows kernel configuration tweaks for the AArch64-based platform.

7. **Flash the SD card**
   ```bash
   ./scripts/prep-sd.sh --device <device-name|uuid|mount-point>
   ```
   This script creates the correct partitions, root filesystem, and flashes all boot binaries.

8. **Boot the device**
   Insert the SD card and power on the board.

> **Containerization:** `.def` files for Singularity containers are in the [`containers/`](./containers/) directory. Build containers before running the workflows.


## Device Drivers

[-- TODO --]

## Containers

Container support is provided and strongly recommended for reproducible builds and to avoid host dependency conflicts. Build the container image before running the workflow scripts. Container definition files (`.def`) are located in the `containers/` directory. Information on conatainer dependencies and building them is explained in [containers](./containers/). 

```shell
singularity build --fakeroot <image-name>.sif containers/<definition-file>.def
```



## Resources & References

| Resource | Link |
|----------|------|
| Zynq-7000 SoC Overview | DS190-Zynq-7000-Overview |
| Zynq-7000 Technical Design Manual | UG585 |
| ADRV9361-Z7035 Quick Start Guide | [Link] |
| ADRV9361-Z7035 User Guide | — |
| System-Level Documentation | [Link] |
| HDL Documentation | [Link] |
| Petalinux UG1144 (NFS workaround) | Appendix-L, Ch7 |


## Related Repositories

### HDL
| Repo | Branch |
|------|--------|
| [analogdevicesinc/hdl](https://github.com/analogdevicesinc/hdl) | `main` |

### Linux & Build System
| Repo | Description |
|------|-------------|
| [analogdevicesinc/linux](https://github.com/analogdevicesinc/linux) | Linux kernel |
| [analogdevicesinc/buildroot](https://github.com/analogdevicesinc/buildroot) | Buildroot |
| [analogdevicesinc/adi-kuiper-gen](https://github.com/analogdevicesinc/adi-kuiper-gen) | ADI Kuiper image generator |
| [analogdevicesinc/linux_image_ADI-scripts](https://github.com/analogdevicesinc/linux_image_ADI-scripts) | Image scripts |
| [analogdevicesinc/arm-trusted-firmware](https://github.com/analogdevicesinc/arm-trusted-firmware) | ARM Trusted Firmware |

### Device Drivers
| Repo | Description |
|------|-------------|
| [analogdevicesinc/libiio](https://github.com/analogdevicesinc/libiio) | IIO library |
| [analogdevicesinc/iio-oscilloscope](https://github.com/analogdevicesinc/iio-oscilloscope) | IIO oscilloscope |
| [analogdevicesinc/pyadi-iio](https://github.com/analogdevicesinc/pyadi-iio) | Python IIO bindings |
| [analogdevicesinc/no-OS](https://github.com/analogdevicesinc/no-OS) | Bare-metal drivers |

### Petalinux / U-Boot
| Repo | Description |
|------|-------------|
| [analogdevicesinc/meta-adi](https://github.com/analogdevicesinc/meta-adi) | Yocto meta layer |
| [analogdevicesinc/u-boot](https://github.com/analogdevicesinc/u-boot) | U-Boot |
| [analogdevicesinc/u-boot-xlnx](https://github.com/analogdevicesinc/u-boot-xlnx) | Xilinx U-Boot |

### AD9361 Libraries
| Repo | Description |
|------|-------------|
| [analogdevicesinc/libad9361-iio](https://github.com/analogdevicesinc/libad9361-iio) | AD9361 IIO library |
| [analogdevicesinc/gnuradio](https://github.com/analogdevicesinc/gnuradio) | GNU Radio fork |
| [analogdevicesinc/gr-iio](https://github.com/analogdevicesinc/gr-iio) | GNU Radio IIO blocks |
| [analogdevicesinc/PicoZed-SDR](https://github.com/analogdevicesinc/PicoZed-SDR) | PicoZed SDR reference |
| [analogdevicesinc/ad936x-filter-wizard](https://github.com/analogdevicesinc/ad936x-filter-wizard) | Filter wizard |
| [analogdevicesinc/wiki-scripts](https://github.com/analogdevicesinc/wiki-scripts) | Wiki automation scripts |

---