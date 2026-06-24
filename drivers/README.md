\# Drivers



The provided scripts and tools can be used to automate many of the tasks associated with the workflow or individual builds.



\## Build Drivers

<!-- 

| Driver | Description |

&#x20;    |  | -->





<table>

&#x20; <tr>

&#x20;   <th>Driver</th>

&#x20;   <th>Description</th>

&#x20; </tr>



&#x20; <!-- dev-mem\_acc -->

&#x20; <tr>

&#x20;   <td><code>dev-mem\_acc</code></td>

&#x20;   <td>

&#x20;     Implements a DMA character device driver for ZYNQ-7000 High Performance Ports, allowing for fast data transfers between FPGA logic and memory.

&#x20;     <br><br>

&#x20;     <b>Dependencies:</b>

&#x20;     <ul>

&#x20;       <li><b>Architecture:</b> <i>(required)</i> ...</li><br>

&#x20;       <li><b>Xilinx Vivado/Vitis:</b> <i>(optional)</i> ...</li><br>

&#x20;       <li><b>GCC Cross-compiler:</b> <i>(required)</i> ...</li>

&#x20;     </ul>

&#x20;   </td>

&#x20; </tr>



&#x20; <!-- dma-pl -->

&#x20; <tr>

&#x20;   <td><code>dma-pl</code></td>

&#x20;   <td>

&#x20;     Handles the register‑level operations required for DMA-based I/O.

&#x20;     <br><br>

&#x20;     <b>Dependencies:</b>

&#x20;     <ul>

&#x20;       <li>**Architecture:</b> <i>** *(required)* </i>...</li><br>

&#x20;       <li>**Xilinx Vivado/Vitis:</b> <i>***(optional)* </i>...</li><br>

&#x20;       <li>**GCC Cross-compiler:**</b> <i>*(required)* </i>...</li>

&#x20;     </ul>

&#x20;   </td>

&#x20; </tr>



&#x20; <!-- dma\_irq -->

&#x20; <tr>

&#x20;   <td><code>dma\_irq</code></td>

&#x20;   <td>

&#x20;     Provides the driver entry point for PL, DMA management, read/write transactions, and user‑level I/O requests.

&#x20;     <br><br>

&#x20;     <b>Dependencies:</b>

&#x20;     <ul>

&#x20;       <li>**Architecture:</b> <i>***(required)* </i>...</li><br>

&#x20;       <li>**Xilinx Vivado/Vitis:</b> <i>***(optional)* </i>...</li><br>

&#x20;       <li>**GCC Cross-compiler:</b> <i>***(required)*</i>...</li>

&#x20;     </ul>

&#x20;   </td>

&#x20; </tr>



&#x20; <!-- gpio -->

&#x20; <tr>

&#x20;   <td><code>gpio</code></td>

&#x20;   <td>

&#x20;     Implements the register‑level interface and operations for GPIO. 

&#x20;     <br><br>

&#x20;     **Dependencies:**

&#x20;     <ul>

&#x20;       <li>**Architecture:**</b> <i>*(required)*</i>...</li><br>

&#x20;       <li>**Xilinx Vivado/Vitis:**</b> <i>*(optional)*</i>...</li><br>

&#x20;       <li>**GCC Cross-compiler:**</b> <i>*(required)*</i>...</li>

&#x20;     </ul>

&#x20;   </td>

&#x20; </tr>

&#x20; 

&#x20; <!-- gpio\_dummy -->

&#x20; <tr>

&#x20;   <td><code>gpio\_dummy</code></td>

&#x20;   <td>

&#x20;     Provides a test to validate GPIO module functionality ...

&#x20;     <br><br>

&#x20;     **Dependencies:**

&#x20;     <ul>

&#x20;       <li>**Architecture:**</b> <i>*(required)*</i>...</li><br>

&#x20;       <li>**Xilinx Vivado/Vitis:**</b> <i>*(optional)*</i>...</li><br>

&#x20;       <li>**GCC Cross-compiler:**</b> <i>*(required)*</i>...</li>

&#x20;     </ul>

&#x20;   </td>

&#x20; </tr>





</table>



\---





\## Auxiliary tools





