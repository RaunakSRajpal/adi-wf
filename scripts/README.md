# Scripts

The provided scripts and tools can be used to automate many of the tasks associated with the workflow or individual builds.

## Build scripts
<!-- 
| Script | Description |
|-------------|-------------|
| setup-uboot-proj.sh | Clones ADI [**`linux`**]() repo and builds the linux kernel image and devicetree files.<br> Dependency <ul> <li> </ul> |
| init-adi-hdl.sh     | Clones ADI [**`hdl`**]() repo and builds the HDL board projects and other IPs |
| gen_boot-bin.sh     |  | -->


<table>
  <tr>
    <th>Script</th>
    <th>Description</th>
  </tr>

  <!-- setup-uboot-proj.sh -->
  <tr>
    <td><code>setup-uboot-proj.sh</code></td>
    <td>
      Clones ADI linux repo and builds the linux kernel image and devicetree files.
      <br><br>
      <b>Dependencies:</b>
      <ul>
        <li><b>Architecture:</b> <i>(required)</i> Both kernel image and device-tree depend on the selected architecture. [arm, arm64, x86, …]</li><br>
        <li><b>Xilinx Vivado/Vitis:</b> <i>(optional)</i> Required only for the dtc compiler and GCC cross-compiler. You can use alternatives. The container also comes with its own dtc and native cross-compiler.</li><br>
        <li><b>GCC Cross-compiler:</b> <i>(required)</i> You can provide the path to toolchains using <code>scripts/setup.env</code>. You can select from the ones on SCC (from host-gcc or xilinx), your host machine or bundled inside the container.</li>
      </ul>
    </td>
  </tr>

  <!-- init-adi-hdl.sh -->
  <tr>
    <td><code>init-adi-hdl.sh</code></td>
    <td>
      Clones ADI hdl repo and builds the HDL board projects and other IPs.
      <br><br>
      <b>Dependencies:</b>
      <ul>
        <li><b>Architecture:</b> <i>(required)</i> Synthesized HDL depends on the selected core architecture. [arm, arm64, x86, …]</li><br>
        <li><b>Xilinx Vivado/Vitis:</b> <i>(required)</i> Required for synthesis and implementation of the HDL design.</li><br>
        <li><b>GCC Cross-compiler:</b> <i>(required)</i> You can provide the path to toolchains using <code>scripts/setup.env</code>. You can select from the ones on SCC (from host-gcc or xilinx), your host machine or bundled inside the container.</li><br>
      </ul>
    </td>
  </tr>

  <!-- gen_boot-bin.sh -->
  <tr>
    <td><code>gen_boot-bin.sh</code></td>
    <td>
      Builds the boot binaries from the previously exported hardware file (.xsa or .hdf) HDL board projects and other IPs.
      <br><br>
      <b>Dependencies:</b>
      <ul>
        <li><b>Architecture:</b> <i>(required)</i> Both BOOT binaries and BIT file depend on the selected architecture. [arm, arm64, x86, …]</li><br>
        <li><b>Xilinx Vivado/Vitis:</b> <i>(required)</i> Required for xilinx extraction and packaging tools (bootgen, xsct).</li><br>
        <li><b>Hardware export file (XSA):</b> <i>(required)</i> You can provide the path to the XSA using <code>scripts/setup.env</code>. The name of the exported hardware file must be <code><b>system_top.xsa</b></code>. </li><br>
        <li><b>U-boot (.ELF) File:</b> <i>(optional)</i> You can introduce the ELF file using <code>project_setup.env</code>. If unspecified, it pulls the correct file and runs the build rpocess.</li><br>
      </ul>
    </td>
  </tr>

</table>

---


## Auxiliary tools


| Script            | Description   |
|-------------------|---------------|
| prep-SD.sh        | prepares BOOT and ROOT partitions in the bootable media (SDcard) and flashes the image and rootfs |
| find_xilinx.sh    | **IF RUN STAND-ALONE:** Scans the SCC for Xilinx Vivado/Vitis(SDK) installations and reports the bundled cross-compiler toolchains available for each version, along with their supported target architectures. <br><br> **IF SOURCED:** Selects the specified Xilinx Vitis(SDK) installation on SCC and bundled cross-compiler toolchains for the build. If SCC_FALLBACK is set to 0(or unset), Vitis/SDK path selection is skipped and expects XVITIS to be set. |
| housekeeping.sh   | logging and print-format functions:<br> error, status, warning, return_line |
| add_srcs.tcl      | includes all the srcs, testbench and constraints files into the vivado project and builds them (if needed) |
| gen_xsa.tcl       | exports hardware file(.xsa), including bitstream from existing vivado project |
