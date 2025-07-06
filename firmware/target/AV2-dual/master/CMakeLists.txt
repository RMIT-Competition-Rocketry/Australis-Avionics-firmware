cmake_minimum_required(VERSION 3.15)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(MAP_DIR ${CMAKE_CURRENT_BINARY_DIR}/map)

# ╔══════════════════════════════════════════════════════════╗
# ║                       DEPENDENCIES                       ║
# ╚══════════════════════════════════════════════════════════╝

include(FetchContent)

set(FETCHCONTENT_SOURCE_DIR_AUSTRALIS ${CMAKE_SOURCE_DIR}/australis)
FetchContent_Declare(Australis)

# ── AUSTRALIS EXTRA MODULES ─────────────────────────────────

set(AUSTRALIS_EXTRA_MODULES
  drivers/sensors/accel/kx134_1211
  drivers/sensors/gyro/a3g4250d
  drivers/sensors/baro/bmp581
  drivers/flash/w25q128
  drivers/lora/sx1272
  drivers/gps/sam_m10q
  publications/lora
  publications/can
  publications/gpsacquisition
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

add_executable(AV2-dual_master
  src/main.c
  src/devices.c
  src/tasks.c
  src/statelogic.c
  src/groundcomms.c
)
target_include_directories(AV2-dual_master
  PUBLIC 
    inc
)
target_compile_options(Australis-core PRIVATE
  -c -O0 -g3
)

file(MAKE_DIRECTORY ${MAP_DIR})
target_link_libraries(AV2-dual_master Australis-firmware)
target_link_options(AV2-dual_master
  PUBLIC
    LINKER:-Map=${MAP_DIR}/AV2-dual_master.map
)
