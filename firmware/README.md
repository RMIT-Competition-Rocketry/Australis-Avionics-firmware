	
# Australis Firmware

![](/img/banner.png)

<div align="center">


*Project sources and build tooling for the Australis Firmware system*
</div>

---

## Table of Contents
<!-- mtoc-start -->
* [1. Source Structure](#source-structure)
  * [Key Components](#key-components)
  * [Modules](#modules)
  * [Hardware Targets](#hardware-targets)
* [2. Building the Firmware](#building-the-firmware)
  * [Prerequisites](#prerequisites)
  * [Build Steps](#build-steps)
  * [For Developers](#for-developers)
* [3. Deploying the Firmware](#deploying-the-firmware)
  * [Prerequisites](#prerequisites)
  * [Deployment Steps](#deployment-steps)
* [4. Contributing](#contributing)
<!-- mtoc-end -->

## Source Structure

The sources are organized as follows:

*   **`australis/`**: Contains the `Australis Firmware` components and test code, including:
    *   **`source/`**
	    * `core/`: Core firmware component linked to all sources that implement Australis
	    * `extra/`: Extra firmware component providing additional functionality with [modules](#modules)
	    * `system/`: System startup and platform dependant sources; Currently only supports STM32F439
	* **`test/`** 

*   **`target/`**:  Hardware target sources. Each subdirectory within `target/` represents a specific platform officially supported by Australis.

### Key Components

-   **Australis Core:**
    -   The core set of API definitions and tasks required for an Australis application. Any source code implementing `Australis Firmware` must link against this component.
-   **Australis Extra:**
    -   Additional [modules](#modules) that optionally provide extra functionality to the firmware core. Each `module` is dependant only on `Australis Core` and can be individually built and linked as necessary.
-   **System Sources:** 
  
### Modules
> [!IMPORTANT]
TODO: complete this section
  
### Hardware Targets

The `target/` directory contains specific configurations for different hardware platforms.  Each target defines:

###### Configuration Options 
Target specific `#defines` in `AustralisConfig.h`.

###### Peripheral Initialisation
All hardware peripherals (`CAN`, `SPI`, etc.) are initialised by the target and passed to any devices that require them during initialisation.

###### Device Initialisation
Each device required by the target must be appropriately initialised and registered in the core device list.

###### Firmware Entry Point
It is the responsibility of the target to initialise and run `Australis Core`. A target's source code must define the `main()` entry function and execute any code required by the above.

> [!NOTE]
See the `target/README.md` for more information about currently available targets (TODO: remember to fill this file out).
  
## Building the Firmware

The build process uses CMake to generate Makefiles, which are then used to compile and link the firmware.

### Prerequisites

*   **CMake (version 3.15 or higher):**  Used to generate the build system.
*   **ARM GCC Toolchain:** Required for compiling the ARM Cortex-M4 based firmware. Ensure the toolchain is installed and accessible in your system's PATH. Example of toolchain path: `/usr/bin/arm-none-eabi-gcc`.
*   **Make:** Used to execute the generated Makefiles.

### Build Steps
     
1.  **Set `HARDWARE_TARGET`:**
    - Define the `HARDWARE_TARGET` variable when invoking CMake. This specifies which hardware target in the `target/` directory will be built.

      ```shell
      cmake -DHARDWARE_TARGET=minimal -S . -B build
      ```

3.  **Build the Firmware:**
    - Navigate to the build directory and invoke `cmake`, or pass `cmake` the appropriate path to your build tree.

      ```bash
      cd build
      cmake --build
      ```
    
      or
    
      ```bash
      cmake --build path/to/build
      ```
        
### For Developers 
> [!IMPORTANT]
TODO: complete this section

## Deploying the Firmware

### Prerequisites
*   **JLink Commander:**  Used to operate the CPU and load binaries. It can be obtained from JLink's SDK suite [here](https://www.segger.com/downloads/jlink/)
*   **A valid Australis Target binary**

### Deployment Steps

1. **Prepare JLink Commander**
   - Connect the target hardware with a JLink debug probe.
   - Launch JLink Commander:
    
     ```bash
     JLinkExe
     ```

2. **Connect to Target Device**
   - In the JLink Commander prompt, connect to the target device and ensure the correct configuration is selected:
     
     ```shell
     Connect
     ```
     - Select the appropriate device (e.g., `STM32F439IIHx`).
     - Specify the target interface (e.g., `JTAG`).
     - Set the target interface speed (e.g., `4000 kHz`).

4. **Erase the Flash**
   - Clear all data already in the target's flash:
     
     ```shell
     Erase
     ```
     
5. **Flash the Firmware**
   - Load the binary into the target device:
     
     ```shell
     LoadFile path/to/firmware.elf
     ```
     - Replace `path/to/firmware.elf` with the path to the desired binary.

6. **Reset the Target**
   - Reset the target device to execute the firmware:
     
     ```shell
     Reset
     ```
7. **Exit JLink Commander**

---
    
## Contributing
> [!IMPORTANT]
TODO: complete this section
