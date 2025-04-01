cmake_minimum_required(VERSION 3.15)

if(NOT TARGET Australis-config)
  message(FATAL_ERROR " Australis-config target not specified. Please specify a cmake target that defines the include directory for AustralisConfig.h")
endif()


# Define the firmware source root directory
set(FIRMWARE_SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR}/source)

# Define the firmware system root directory
set(FIRMWARE_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/lib)

# Define the firmware test root directory
set(FIRMWARE_TEST_ROOT ${CMAKE_CURRENT_LIST_DIR}/test)


# Define the firmware core root directory
set(FIRMWARE_CORE_ROOT ${FIRMWARE_SOURCE_ROOT}/core)

# Define the firmware extra root directory
set(FIRMWARE_EXTRA_ROOT ${FIRMWARE_SOURCE_ROOT}/extra)

# Define the firmware system root directory
set(FIRMWARE_SYSTEM_ROOT ${FIRMWARE_SOURCE_ROOT}/system)


# ─────────────────────── Build Options ─────────────────────

option(ENABLE_FLIGHT       "Enable flight mode"         OFF)
option(ENABLE_DEBUG        "Enable debug mode"          OFF)
option(ENABLE_DUMMY        "Enable dummy test mode"     OFF)

# ──────────────── Target Device Configuration ───────────────

set(DEVICE STM32F439IIHx)
set(FLASH_START 0x08000000)
set(FLASH_SIZE  0x00200000)
set(RAM_START   0x20000000)
set(RAM_SIZE    0x00030000)
set(RAM2_START  0x10000000)
set(RAM2_SIZE   0x00010000)

# ╔══════════════════════════════════════════════════════════╗
# ║                       DEPENDENCIES                       ║
# ╚══════════════════════════════════════════════════════════╝

# Bring in FreeRTOS kernel 
FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        main 
  GIT_PROGRESS   TRUE
)

# Use Australis' default FreeRTOSConfig.h if no target already exists
if(NOT TARGET freertos_config)
  message(WARNING 
    " No freertos_config target specified, defaulting to Australis provided configuration.\n"
    " NOTE: If you wish to use your own FreeRTOSConfig.h, it must be included in freertos_config.")

  # Add in config library
  add_library(freertos_config INTERFACE)
  
  target_include_directories(freertos_config SYSTEM
    INTERFACE
      source
  )
  target_compile_definitions(freertos_config
    INTERFACE
      projCOVERAGE_TEST=0
  )
endif()

# Select heap implementation
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

# Select the ARM Cortex M4 port
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

# Populate FreeRTOS kernel files
FetchContent_MakeAvailable(freertos_kernel)


# ╔══════════════════════════════════════════════════════════╗
# ║                        LIBRARIES                         ║
# ╚══════════════════════════════════════════════════════════╝

add_library(Australis-firmware STATIC)

# -- AUSTRALIS CORE ------------------------------------------

add_subdirectory(${FIRMWARE_SYSTEM_ROOT})
add_subdirectory(${FIRMWARE_CORE_ROOT})
add_subdirectory(${FIRMWARE_EXTRA_ROOT})

# -- LIBAVIONICS ---------------------------------------------

add_library(libavionics STATIC IMPORTED)
set_property(
  TARGET 
    libavionics 
  PROPERTY IMPORTED_LOCATION 
    ${FIRMWARE_LIB_ROOT}/bin/libavionics.a
)

# -- DATA SOURCES --------------------------------------------

if(ENABLE_DUMMY)
  # Create static library with data sources
  add_library(Australis-test STATIC 
    PRIVATE 
      ${FIRMWARE_TEST_ROOT}/dummy/data/accel/accelX.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/accel/accelY.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/accel/accelZ.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/baro/press.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/gyro/gyroX.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/gyro/gyroY.c
      ${FIRMWARE_TEST_ROOT}/dummy/data/gyro/gyroZ.c
  )
  # Add in test include directories
  target_include_directories(Australis-test 
    PUBLIC 
      ${FIRMWARE_TEST_ROOT}
  )
  set(DUMMY -DDUMMY CACHE BOOL "")
endif()

# ── LINKING ─────────────────────────────────────────────────

target_link_options(Australis-firmware 
  PUBLIC
    LINKER:-Map=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Australis-firmware.map
)

# Link libraries to target
target_link_libraries(Australis-firmware
  PUBLIC
    Australis-core
    Australis-extra
    Startup_headers
    libavionics 
  PRIVATE
    Startup 
)

# ╔══════════════════════════════════════════════════════════╗
# ║                        POST-BUILD                        ║
# ╚══════════════════════════════════════════════════════════╝

if(ENABLE_FLIGHT)
  set(FLIGHT -DFLIGHT CACHE BOOL "")
  set(ENABLE_DUMMY OFF)
  set(ENABLE_DEBUG OFF)
endif()

if(ENABLE_DEBUG)
  set(DEBUG -DDEBUG CACHE BOOL "")
endif()

add_custom_command(TARGET Australis-firmware POST_BUILD
  COMMAND_EXPAND_LISTS
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "Build Configuration:                                   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Option       Variable              Value             "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  ----------------------------------------             "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Debug        ENABLE_DEBUG          ${ENABLE_DEBUG}   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Dummy        ENABLE_DUMMY          ${ENABLE_DUMMY}   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Flight       ENABLE_FLIGHT         ${ENABLE_FLIGHT}  "
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "Australis Extra Modules:                               "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "$<LIST:TRANSFORM,${AUSTRALIS_EXTRA_MODULES},PREPEND,  >"
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
)
