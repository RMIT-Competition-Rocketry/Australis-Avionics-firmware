# Australis Avionics Firmware
![](./img/banner.png)

## Table of Contents
<!-- mtoc-start -->

* [Requirements](#requirements)
  * [Project Includes](#project-includes)
* [Getting Started](#getting-started)
  * [Building the Project](#building-the-project)
    * [Windows](#windows)
    * [Linux/MacOS](#linuxmacos)
    * [Docker](#docker)
  * [Code Guidelines](#code-guidelines)
* [Acknowledgements](#acknowledgements)
  * [Key Contributors](#key-contributors)
  * [Special Thanks](#special-thanks)

<!-- mtoc-end -->

## Requirements

To successfully install and contribute to the project, ensure you have the following prerequisites installed:

- Git
- Keil uVision IDE (windows only) or alternative editor and GDB debugger
- [GCC for arm (arm-none-eabi-gcc)](https://developer.arm.com/downloads/-/gnu-rm)
- Latest version of Australis Avionics libraries found [here](https://github.com/RMIT-AURC-Team/AuroraV-Avionics-lib/releases)

#### Project Includes

In order to build the project you must ensure the correct include paths are added for the compiler to recognise the headers. At minimum the following must be included:

```shell
/path/to/Australis-Avionics-firmware/Australis-Avionics/Core/Inc
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
/path/to/Australis-Avionics-firmware/Australis-Avionics/Drivers/CMSIS/Include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Drivers/CMSIS/Device/ST/STM32F4xx/Include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
/path/to/Australis-Avionics-firmware/Australis-Avionics/Drivers/CMSIS/Device/ST/STM32F4xx/Include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Drivers/CMSIS/Include
/path/to/Australis-Avionics-firmware/Australis-Avionics/Drivers/STM32F4xx_HAL_Driver/Inc
/path/to/Australis-Avionics-lib/inc
/path/to/Australis-Avionics-lib/inc/DSP/Include
/path/to/Australis-Avionics-lib/inc/DSP/PrivateInclude
/path/to/Australis-Avionics-lib/inc/CORE/Include
```

If running Tracealyzer also make sure to include these headers:

```shell
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/TraceRecorder/config
/path/to/Australis-Avionics-firmware/Australis-Avionics/Middlewares/Third_Party/TraceRecorder/include
```
> [!IMPORTANT]
> Additional includes may also be necessary, in particular any directories nested in ```/Australis-Avionics/Core/Inc/``` are required.

## Getting Started

### Building the Project
Before making any changes to the project source it is recommended to ensure the environment is correctly set-up. 

To start, the compiler toolchain ```arm-none-eabi-gcc``` should be installed and visible to whatever build system is intended to be used.

---

#### Windows
For Windows systems a Keil uVision project is provided in ```/Australis-Avionics/MDK-ARM/``` as ```Australis-Avionics.uvprojx```. 

This project is already configured for building for and debugging on the target platform, however maintenance for this platform is likely to be behind -- one should double check all sources and includes prior to beginning of development to minimise difficulties.

---

#### Linux/MacOS

A CMake profile is available for building the project from the command line across other platforms. To start, make sure the toolchain path is correct in ```/Australis-Avionics/toolchain.cmake```, navigate to ```/Australis-Avionics/Build/``` and run the following:

```shell
cmake ..
cmake --build .
```

Compile time flags can be defined with CMake prior to building. These flags are declared in ```/Australis-Avionics/CMakeLists.txt``` and can be passed at build time.

Once built, the compiled binary will be available as ```/Australis-Avionics/Build/Australis-firmware``` and a ```.map``` file will be generated.

> [!TIP]
> Debugging over JTAG on Linux and MacOS systems can be achieved with [JLink GDB Server](https://www.segger.com/products/debug-probes/j-link/tools/j-link-gdb-server/about-j-link-gdb-server/) and any choice of GDB debugger. Documentation on how to set-up and use the GDB server is available [here](https://kb.segger.com/J-Link_GDB_Server)

---

#### Docker

This repository additionally provides a ```Dockerfile``` for building an Arch Linux based Docker to host containers on. This image includes the base tooling requirements for development and debugging as well as the contents of the repository.

To get started with the Docker image you must first clone this repository; ensure Docker is installed on the host system, then run the following from the root directory of the repo, substituting ```{name}``` with any choice of name for the container:

```shell
docker build -t australis/dev:latest .           # Build image from the supplied Dockerfile
docker run --name {name} -it australis/dev:latest # Run a container and attach 
```

> [!TIP]
> If you intend to deploy and debug over JLink from within the Docker container you may additionally pass the ```--device``` flag to ```docker run```
> e.g. ```docker run --name avionics --device=/dev/ttyUSB0 -it australis/dev:latest```

---

### Code Guidelines
Specification for coding standard and best practices for this project can be found on the [wiki](https://github.com/s3785111/Australis-Avionics-firmware/wiki/Code-Guidelines)

## Acknowledgements
### Key Contributors
| Name              | Role                           |
|:------------------|:-------------------------------|
| Matthew Ricci     | Principal firmware developer   |
| William Houlahan  | Initial driver implementations |
| Benjamin Wilsmore | Initial driver implementations |

### Special Thanks
Other members of the Aurora V Avionics team:
- Hugo Begg 
- Jonathan Chandler
- Jeremy Timotius
- Lucas Webb 

Thank you to Aurora & Legacy project team leads Patrick Underwood and Brodie Alexander for providing the opportunity and environment to work on these rockets as part of the team, and thank you to everyone who helped make them a reality!

![](./img/footer.png)

---
