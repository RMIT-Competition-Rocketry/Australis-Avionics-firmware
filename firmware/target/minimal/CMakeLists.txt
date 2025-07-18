cmake_minimum_required(VERSION 3.15)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ╔══════════════════════════════════════════════════════════╗
# ║                       DEPENDENCIES                       ║
# ╚══════════════════════════════════════════════════════════╝

include(FetchContent)

set(FETCHCONTENT_SOURCE_DIR_AUSTRALIS ${CMAKE_SOURCE_DIR}/australis)
FetchContent_Declare(Australis)

# ── AUSTRALIS EXTRA MODULES ─────────────────────────────────

set(AUSTRALIS_EXTRA_MODULES
  drivers/device/sensors/accel/ais3624dq
  drivers/device/sensors/gyro/iam_20380
  drivers/device/sensors/baro/lps22df
  drivers/device/flash/w25q128
  drivers/lora/sx1272
)

# ── AUSTRALIS CONFIG ────────────────────────────────────────

add_library(Australis-config INTERFACE)
target_include_directories(Australis-config SYSTEM
  INTERFACE
    inc
)

FetchContent_MakeAvailable(Australis)


# ╔══════════════════════════════════════════════════════════╗
# ║                        EXECUTABLE                        ║
# ╚══════════════════════════════════════════════════════════╝

add_executable(minimal
  src/main.c
  src/devices.c
  src/tasks.c
)
target_include_directories(minimal 
  PUBLIC 
    inc
)

target_link_libraries(minimal Australis-firmware)
