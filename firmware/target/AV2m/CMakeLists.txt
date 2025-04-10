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
  drivers/device/sensors/accel/kx134_1211
  drivers/device/sensors/gyro/a3g4250d
  drivers/device/sensors/baro/bmp581
  drivers/device/flash/w25q128
  drivers/lora/sx1272
  drivers/gps/sam_m10q
  rtos/comms/loracomm
  rtos/data/gpsacquisition
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

add_executable(AV2m
  src/main.c
  src/devices.c
  src/tasks.c
  src/groundcomms.c
)
target_include_directories(AV2m
  PUBLIC 
    inc
)
target_compile_options(Australis-core PRIVATE
  ${DUMMY}
  -c -O0 -g3
)

target_link_libraries(AV2m Australis-firmware)
