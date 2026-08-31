# adi-wf<br><pre><a class="NORM">Analog Devices Workflow</a></pre>

This repository is a design workflow for building and deploying  specific projects based on evaluation boards supported by [Analog Devices, Inc](https://wiki.analog.com/resources/eval/user-guides#fmc_compatible_boards). The flow tools are designed for complete bring up of the evaluation board. This repo provides automation scripts and tools, HDL IP support, device drivers, and containerized build tooling.


## Table of Contents

- [Project Overview](#project-overview)
    <!-- - [Hardware Architecture](#hardware-architecture) -->
    <!-- - [Project Architecture](#project-architecture) -->
- [Build Workflow](#build-workflow)
    - [Getting Started](#getting-started)
    - [Advanced Tooling](#advanced-tooling)
- [Device Drivers](#device-drivers)
- [Containers](#containers)
- [Resources & References](#resources--references)
- [Related Repositories](#related-repositories)



## Project Overview

Most of the supported evaluation boards have a single chip housing both an FPGA (**programable logic** or **PL**) and a hardened processor (**processing system** or **PS**) on the same die. This provides for the SoC to be incredibly versatile in handling a wide range of tasks. The **PS** can support a **linux** kernel and application sw (or run a **baremetal/RTOS** on the PS), while the **PL** handles time-critical, parallelizable, and dedicated workloads. The two subsystems communicate over high-bandwidth internal interconnects (**AXI protocol**).


This repository supports the complete hardware/software co-design of an SoM system. The project design integrates:

<!-- < -- REMOVE -- >
- **ADRV9361-Z7035 SoM** — ADI System-on-Module housing a Zynq-7000 SoC and an RF module (AD9361)
- **PZSDRCC-BRK-PCB-C** — ADI carrier board for programming and development
- **ORBGRAND PCB** — Houses the ORBGRAND decoder chip (swappable for future GRAND project revisions) -->

[--- FIX THIS ---]
- **Evaluation Board** — 
    - **dafuq**
- **Carrier Module/PCB** — 


[-- insert image --]
```
┌─────────────────────────────────┐
│       ADRV9361-Z7035 SoM        │  ← Evaluation Board (SoC + RF)
│   Zynq-7035 (PS + PL) + AD9361  │
└────────────────┬────────────────┘
                 │
┌────────────────▼────────────────┐
│     PZSDRCC-BRK-PCB-C           │  ← Carrier Board (I/O expansion)
└────────────────┬────────────────┘
                 │
┌────────────────▼────────────────┐
│             DUT PCB             │  ← Decoder Board (swappable)
└─────────────────────────────────┘
```


### Architecture
The system is designed in a way to leverage existing support from ADI and Xilinx, and also be able to integrate your custom RTL and/or kernel software on top. 
The workflow is controlled and configured by two dotenv files, **[`project_setup.env`](./project_setup.env)** and **[`setup.env`](./scripts/setup.env)** (located in scripts/ dir). Every script and tool in the repo sources its configs from these, hence they **must remain static** for the complete workflow. 


  -  **project_setup.env** : All user configurations for the build processes must be set from this file.
  -  **setup.env** : This contains all the system level paths, directory structure, and dependency source paths.

> **NOTE :** It is advised **NOT** to edit `setup.env` file, since this can break the build flow. Edit only if you're changing certain dependency and/or system paths for your host machine. More information on how to edit is provided inside the file.


#### **__Project :__**

The workflow operates on individual **_"projects"_**, which are defined inside `project_setup.env`. Each project encapsulates both the PL based hardware and its supporting firmware (or baremetal) code. The final binaries are available in `BIN_pkg/<my_project>` directory once the build is complete.

The HDL board projects, once built, can be found inside **`pl-hdl/<my_project>/`**. The built project directory must be named to end in ``<*_build>``. Custom RTL should be added in the same directory (**`pl-hdl/<my_project>/`**) inside dir named `srcs` and constraint files inside `constr`. Check the [WS tree](#ws-tree-) for complete directory tree.

> If you are running the workflow for the first time directly go to [Getting Started](#getting-started), or checkout [Advanced Tooling](#advanced-tooling) if you need detailed explanation of the build process.

<!-- More details are provided in the sections following [--]. -->

After all the required builds are complete, the adi-wf root directory, which will be called workspace or **WS** should look like the directory tree shown below. The 

#### **__WS Tree :__**

<body>
<pre class="tree">
<a class="DIR"><b>adi-wf/</b></a>
├── <a class="DIR"><b>BIN_pkg/</b></a>		------------------->	flow completion pkg
│   ├── <a class="DIR"><b>project-1</b></a>
│   |   └── <a class="NORM">[ pkg files ]</a>
|   └── <a class="DIR"><b>project-2</b></a>
│       └── <a class="NORM">[ pkg files ]</a>
├── <a class="DIR"><b>build/</b></a>           ----------------------->	logs, build dirs for 
│   ├── <a class="DIR"><b>logs</b></a>                                     each project
│   ├── <a class="DIR"><b>project-1</b></a>
│   |   ├── <a class="DIR"><b>BOOT</b></a>
│   |	├── <a class="DIR"><b>sysmod_rootfs</b></a>
│   |   └── <a class="NORM">[ build files ]</a>
|   ...
│   └── <a class="DIR"><b>project-n</b></a> 
├── <a class="DIR"><b>containers/</b></a>		<!-- ---------------\>	containerized support -->
│   ├── <a class="NORM">README.md</a>
│   ├── <a class="NORM">sing_adiwf_ubuntu20.04.def</a>
│   └── <a class="EXEC">sing_adiwf_ubuntu20.04.v03.sif</a>
├── <a class="DIR"><b>drivers/</b></a>		------------------->	device driver sources
├── <a class="DIR"><b>hdl/</b></a>
├── <a class="DIR"><b>lib/</b></a>	-------------------->	src files, provide C wrapper functions
├── <a class="DIR"><b>linux-adi/</b></a>
├── <a class="DIR"><b>pl-proj/</b></a>           ---------------+-------->     FPGA projects: custom RTL srcs 
│   ├── <a class="DIR"><b>project-1</b></a>                -----|              and built bd IPs
│   |   ├── <a class="DIR"><b>[eval_bd]_build</b></a>           |
│   |   ├── <a class="DIR"><b>srcs</b></a>                      |
│   |   └── <a class="DIR"><b>constr</b></a>                    |
│   └── <a class="DIR"><b>project-2</b></a>                -----|
│   |   ├── <a class="DIR"><b>[eval_bd]_build</b></a>
│   |   ├── <a class="DIR"><b>srcs</b></a>
│   |   └── <a class="DIR"><b>constr</b></a>
├── <a class="DIR"><b>scripts/</b></a>		------------------->	automation scripts
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
<br>

All the build logs, directories, and temp files can be found inside the [build/](./build) directory. All the tools and automation support are provided inside the [scripts/](./scripts/) directory. Similarly check the [drivers/](./drivers/) dir for device drivers bundled with provided PL devices.


> the *`hdl/`* and *`linux-adi/`* directories are cloned respectively from the [analogdevicesinc/**hdl**](https://github.com/analogdevicesinc/hdl) and [analogdevicesinc/**linux**](https://github.com/analogdevicesinc/linux) repos. These provide support for several SoM and FMC evaluation boards. Check the section on related [repos](#related-repositories) for more information.


## Build Workflow

### Getting Started
If this is your first time cloning or building the repo, you can follow the steps below to the build and run a zynq-prototype project. For a more advanced tooling guide check out [Advanced Tooling](#advanced-tooling).

1.  **Clone the repository**
    ```shell
    git clone <repo-link>
    ```

2. **Configure the project**\
    Open the environment file **`project_setup.env`** and provide the folllowing configs :
    ```shell
    EVAL_BD="adrv9361z7035"
    CARRIER="ccbob_cmos"
    PART_FAM="zynq"
    ARCH="arm"

    proj_name="new_project"
    XVERSION="2024.1"

    DTFILE="xilinx/zynq-adrv9361-z7035-bob.dts"
    uboot_elf="u-boot_zynq_adrv9361.elf"
    ```
    This will configure the settings for a **zynq-7000** board, with **ARM(aarch32)** architecture and linux kernel bring-up. Set **XVERSION** to your Xilinx installation version.

3.  **Run the Build scripts**
    > Run the scripts in the below fixed order :
    > -   **setup-uboot-proj.sh —** initializes the linux dir. Builds the kernel image and device-tree
    > -   **init-adi-proj.sh —** initializes the ADI HDL repo. Builds all the required PL based IPs for the board
    > -   **gen_boot-bin.sh —** generates the BOOT Binaries for the boot-partition

    ```shell
    bash scripts/setup-uboot-proj.sh
    bash scripts/init-adi-hdl.sh
    bash scripts/gen_boot-bin.sh
    ```

    > **NOTE:** If you are facing dependency errors/conflicts on host machine, run this step inside the provided [containers](./containers/) (Need to be built first). 

4.  **Flash drive**\
    If the build process ran successfully to completion, you should see the following files packaged inside the directory **`BIN_pkg`**.
    -   BOOT.BIN
    -   devicetree.dtb
    -   uImage(.ub) or zImage
    +   **rootfs/**
    *   uEnv.txt
    *   
    
    Copy or move the directory **rootfs** in the ROOT partition and the other 3 files in the BOOT partition of the flash drive/SD-card.

### Advanced Tooling

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


## Related ADI Repositories

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
---

> [CSS render: ignore] 
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