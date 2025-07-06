	
# Australis Firmware Targets

![](/img/banner.png)

<div align="center">


*Officially supported Australis target sources*
</div>

---
## Table of Contents
<!-- mtoc-start -->
* [1. Target Structure](#target-structure)
* [2. Supported Targets](#supported-targets)
  * [Adding a New Target](#adding-a-new-target)
<!-- mtoc-end -->

## Target Structure

This directory contains board-specific configurations and code for different hardware platforms that the Australis firmware can be deployed on. Each subdirectory within `target/` represents a unique hardware target.

Each hardware target subdirectory should contain the following:

*   **`CMakeLists.txt`**:  This file defines how to build the firmware for the specific target. It sets compiler flags, links necessary libraries, and defines the executable.  It's the entry point for CMake when building for that target.
*   **`src/`**: Contains source code specific to the target hardware. This may include:
    *   `main.c`: The main application entry point, likely containing initialization routines and task scheduling.
    *   `devices.c`:  Device driver initialization and configuration tailored to the specific hardware.
    *   `tasks.c`: Task definitions that handle different functionalities
*   **`inc/`**: Header files providing definitions and interfaces for the target-specific source code. This directory often includes header files for custom drivers or board-specific peripherals.
*   **`stm32_flash.ld`**: A linker script that tells the linker how to arrange the compiled code and data in memory for the target device's flash and RAM. Defines memory regions and section placement.
*   **`README.md`** (Optional):  A README file providing more details about the specific target, including setup instructions, known issues, and supported features.

## Supported Targets

*   **`minimal/`**: A minimal hardware target configuration. Includes the src and inc for main, devices, and tasks.c files. Contains CMakeLists.txt and uses an STM32F439IIHx microcontroller.

### Adding a New Target

To add support for a new hardware target:

1.  Create a new subdirectory within the `target/` directory, naming it after your target (e.g., `my_new_board/`).
2.  Create a `CMakeLists.txt` file for your target.  Use the existing `target/minimal/CMakeLists.txt` as a template.  **Crucially**, ensure this file:
    *   Defines a target name.
    *   Specifies the necessary source files (`src/*.c`).
    *   Includes any required header files (`inc/`).
    *   Links against the `Australis-firmware` library.
    *   Sets appropriate compiler and linker flags for your target device (likely in the `toolchain.cmake` file).
3.  Create `src/` and `inc/` directories within your target directory.
4.  Implement your target-specific code in the `src/` directory. This will likely involve:
    *   Initializing peripherals in `devices.c`.
    *   Configuring FreeRTOS tasks in `tasks.c`.
    *   Writing the main application loop in `main.c`.
5.  Create any needed header files in the `inc/` directory.

## TODO

*   Add more detailed explanations of the hardware target subdirectory structure.
*   Provide more comprehensive descriptions for each currently available target.
*   Document the process of creating custom device drivers.
