cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Australis-Avionics LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)

#
# Put explanation of repo build here
#

set(FETCHCONTENT_QUIET FALSE)

if(NOT HARDWARE_TARGET)
  message(FATAL_ERROR 
    " Attempted to build without a defined hardware target.\n"
    " If you are trying to build from the Australis-Avionics-firmware repository, you must define HARDWARE_TARGET.")
endif()


if(NOT EXISTS target/${HARDWARE_TARGET})
  message(FATAL_ERROR 
    " Target directory ${HARDWARE_TARGET} not found. Please specify a valid target.")
endif()


# Build the selected target
add_subdirectory(target/${HARDWARE_TARGET})
