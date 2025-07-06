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
  drivers/sensors/accel/ais3624dq
  drivers/sensors/gyro/iam_20380
  drivers/sensors/baro/lps22df
  drivers/flash/w25q128
  publications/can
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

add_executable(AV2-dual_supervisor
  src/main.c
  src/devices.c
  src/tasks.c
  src/statelogic.c
)
target_include_directories(AV2-dual_supervisor
  PUBLIC 
    inc
)
target_compile_options(Australis-core PRIVATE
  ${DUMMY}
  -c -O0 -g3
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/map)
target_link_libraries(AV2-dual_supervisor Australis-firmware)
target_link_options(AV2-dual_supervisor
  PUBLIC
    LINKER:-Map=${MAP_DIR}/AV2-dual_supervisor.map
)
