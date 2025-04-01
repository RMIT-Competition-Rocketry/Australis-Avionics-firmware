<!-- Document Start -->

![](./img/banner.png)

<div align="center">

## FreeRTOS Based Firmware For Australis Series Flight Computers

[![Static Badge](https://img.shields.io/badge/documentation%20-%20Wiki%20-%20dodgerblue?style=flat&logo=gitbook)](https://github.com/s3785111/Australis-Avionics-firmware/wiki)
[![Static Badge](https://img.shields.io/badge/documentation%20-%20API%20Reference%20-%20dodgerblue?style=flat&logo=gitbook)](https://rmit-competition-rocketry.github.io/Australis-Avionics-firmware/)
[![Static Badge](https://img.shields.io/badge/documentation%20-%20Hardware%20%28Members%20Only%29%20-%20dodgerblue?style=flat&logo=gitbook)](https://github.com/RMIT-Competition-Rocketry/AV2-Hardware)

[![Static Badge](https://img.shields.io/badge/%20-%20projects%20-%20grey?style=flat&logo=git&logoColor=white)](https://github.com/s3785111/Australis-Avionics-firmware/projects?query=is%3Aopen)
[![Static Badge](https://img.shields.io/badge/%20-%20pull%20requests%20-%20grey?style=flat&logo=github&logoColor=white)](https://github.com/s3785111/Australis-Avionics-firmware/pulls)

<details>
<summary>Want to submit an issue?</summary>
<p float='right'><br>
<a href='https://github.com/s3785111/Australis-Avionics-firmware/issues/new?template=bug_report.md'><img src='https://img.shields.io/badge/report%20issue%20-%20Bug%20-%20tomato?style=flat'/></a>
<a href='https://github.com/RMIT-Competition-Rocketry/Australis-Avionics-firmware/issues/new?template=feature-proposal.md'><img src='https://img.shields.io/badge/report%20issue%20-%20Feature%20-%20tomato?style=flat'/></a>
<a href='https://github.com/RMIT-Competition-Rocketry/Australis-Avionics-firmware/issues/new?template=refactor-declaration.md'><img src='https://img.shields.io/badge/report%20issue%20-%20Refactor%20-%20tomato?style=flat'/></a>
</p> 
</details>

---

*Operating avionics hardware for winning rocket of Technical Excellence Award and runner's up overall in AURC 2024* - Aurora V

*Powering IREC & AURC 2025 entries for avionics subsystem* - Legacy III
</div>

## Table of Contents
<!-- mtoc-start -->
* [1. Overview](#overview)
* [2. Documentation](#documentation)
* [3. Getting Started](#getting-started)
  *   [Docker Environment](#docker-environment) 
* [4. Contributing](#contributing)
* [5. Acknowledgements](#acknowledgements)
  * [Key Contributors](#key-contributors)
  * [Special Thanks](#special-thanks)
<!-- mtoc-end -->

## Overview

This repository contains the firmware for the Australis series flight computers, developed using FreeRTOS. The firmware is designed to run on various hardware platforms, with support for STM32F439 microcontrollers. The project includes source code, build tooling, and documentation to facilitate development and deployment.

## Documentation

For detailed information about the project, including build instructions, code guidelines, and hardware targets, refer to the following resources:

- [Project Wiki](https://github.com/s3785111/Australis-Avionics-firmware/wiki): Comprehensive documentation about the project.
- [API Reference](https://rmit-competition-rocketry.github.io/Australis-Avionics-firmware/): Detailed API documentation for the project.
- [Firmware README](firmware/README.md): Detailed information about the firmware source structure, build process, and target platforms.
  
## Getting Started

For a detailed guide to get started with the project, follow the instructions in the [Firmware README](firmware/README.md). This includes information on setting up the build environment, building the firmware, and running it on supported hardware platforms.

### Docker Environment
> [!IMPORTANT]
TODO: Populate this section

## Contributing

Contributions to the project are welcome. Please refer to the [contributing guidelines](firmware/README.md#contributing) in the firmware README for more information on how to contribute.

## Acknowledgements

### Key Contributors
| Name              | Role                           |
|:------------------|:-------------------------------|
| Matthew Ricci     | Principal firmware developer   |
| William Houlahan  | Initial driver implementations |
| Benjamin Wilsmore | Initial driver implementations |

### Special Thanks
Other members of the Aurora V Avionics team:
- Hugo Begg - *avionics hardware*
- Jonathan Chandler - *ground control station*
- Jeremy Timotius - *data analysis*
- Lucas Webb - *ground control station*

Thank you to Aurora & Legacy project team leads Patrick Underwood and Brodie Alexander for providing the opportunity and environment to work on these rockets as part of the team, and thank you to everyone who helped make them a reality!

![](./img/footer.png)
