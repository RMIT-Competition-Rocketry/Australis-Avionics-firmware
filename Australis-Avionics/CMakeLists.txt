cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Australis-Avionics LANGUAGES C ASM)

# Define the firmware core root directory
set(FIRMWARE_CORE_ROOT ${CMAKE_SOURCE_DIR}/Code/core CACHE PATH "" FORCE)

# Define the firmware extra root directory
set(FIRMWARE_EXTRA_ROOT ${CMAKE_SOURCE_DIR}/Code/extra CACHE PATH "" FORCE)

# Define hardware target root directory
set(HARDWARE_TARGET_ROOT ${CMAKE_SOURCE_DIR}/Target CACHE PATH "" FORCE)

# Define hardware target to default as AV2
set(HARDWARE_TARGET "AV2" CACHE STRING "")

# Output to Build/bin/<target> directory
set(BINARY_ROOT ${CMAKE_BINARY_DIR}/bin/${HARDWARE_TARGET})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_ROOT})

# Define name for hardware target executable
set(HARDWARE_TARGET_EXECUTABLE "Australis-firmware_${HARDWARE_TARGET}")

# Create executable target
add_executable(${HARDWARE_TARGET_EXECUTABLE})

# Include hardware specific target options in build
if (HARDWARE_TARGET STREQUAL "A3")
    add_subdirectory(${HARDWARE_TARGET_ROOT}/A3)
elseif (HARDWARE_TARGET STREQUAL "AV2")
    add_subdirectory(${HARDWARE_TARGET_ROOT}/AV2)
endif()

# ────────────── Options for Tracealyzer and Data ────────────

option(ENABLE_TRACEALYZER  "Enable Tracealyzer support" OFF)
option(ENABLE_FLIGHT_TEST  "Enable flight test mode"    OFF)
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
# ║                         INCLUDES                         ║
# ╚══════════════════════════════════════════════════════════╝

target_include_directories(${HARDWARE_TARGET_EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/Code/system
    ${FIRMWARE_CORE_ROOT}/inc
    ${FIRMWARE_CORE_ROOT}/inc/shell
    ${FIRMWARE_CORE_ROOT}/inc/rtos
    ${FIRMWARE_CORE_ROOT}/inc/rtos/comms
    ${FIRMWARE_CORE_ROOT}/inc/rtos/data
    ${FIRMWARE_CORE_ROOT}/inc/drivers/peripheral
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device/flash
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device/sensors
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device/sensors/accel
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device/sensors/baro
    ${FIRMWARE_CORE_ROOT}/inc/drivers/device/sensors/gyro
    ${CMAKE_SOURCE_DIR}/Lib/inc
    ${CMAKE_SOURCE_DIR}/Lib/inc/CORE
    ${CMAKE_SOURCE_DIR}/Lib/inc/CORE/Include
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/Include
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/Include/dsp
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/PrivateInclude
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/CMSIS_RTOS
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/config
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/include
)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

target_sources(${HARDWARE_TARGET_EXECUTABLE} PRIVATE
    # -- System ---------------------------------------------
    ${CMAKE_SOURCE_DIR}/Code/system/system_stm32f4xx.c

    # -- Source ---------------------------------------------
    ${FIRMWARE_CORE_ROOT}/src/main.c
    ${FIRMWARE_CORE_ROOT}/src/devicelist.c
    ${FIRMWARE_CORE_ROOT}/src/tasklist.c

    # -- Shell ----------------------------------------------
    ${FIRMWARE_CORE_ROOT}/src/shell/shell.c
    ${FIRMWARE_CORE_ROOT}/src/shell/launch.c
    ${FIRMWARE_CORE_ROOT}/src/shell/mem.c
    ${FIRMWARE_CORE_ROOT}/src/shell/baro.c
    ${FIRMWARE_CORE_ROOT}/src/shell/help.c

    # -- RTOS -----------------------------------------------
    ${FIRMWARE_CORE_ROOT}/src/rtos/stateupdate.c
    ${FIRMWARE_CORE_ROOT}/src/rtos/flashwrite.c
    ${FIRMWARE_CORE_ROOT}/src/rtos/comms/usbcomm.c
    ${FIRMWARE_CORE_ROOT}/src/rtos/data/hdataacquisition.c
    ${FIRMWARE_CORE_ROOT}/src/rtos/data/ldataacquisition.c

    # -- PERIPHERAL DRIVERS ---------------------------------
    ${FIRMWARE_CORE_ROOT}/src/drivers/peripheral/gpiopin.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/peripheral/spi.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/peripheral/can.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/peripheral/uart.c

    # -- DEVICE DRIVERS -------------------------------------
    ${FIRMWARE_CORE_ROOT}/src/drivers/device/flash/w25q128.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/device/sensors/accel/kx134_1211.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/device/sensors/gyro/a3g4250d.c
    ${FIRMWARE_CORE_ROOT}/src/drivers/device/sensors/baro/bmp581.c

    # -- FreeRTOS -------------------------------------------
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/Source/event_groups.c
)

# ──────────────────── TRACEALYZER SOURCES ──────────────────

set(TRACEALYZER_SOURCES
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/SEGGER_RTT.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcAssert.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcCounter.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcDependency.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcDiagnostics.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcEntryTable.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcError.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcEvent.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcExtension.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcHardwarePort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcHeap.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcInternalEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcInterval.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcISR.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcKernelPort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcMultiCoreEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcObject.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcPrint.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcRunnable.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcSnapshotRecorder.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcStackMonitor.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcStateMachine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcStaticBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcStreamingRecorder.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcStreamPort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcString.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcTask.c
    ${CMAKE_SOURCE_DIR}/Middlewares/TraceRecorder/trcTimestamp.c
)

# ──────────────────────── DATA SOURCES ─────────────────────

set(DATA_SOURCES
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/accel/accelX.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/accel/accelY.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/accel/accelZ.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/baro/press.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/gyro/gyroX.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/gyro/gyroY.c
    ${CMAKE_SOURCE_DIR}/Test/dummy/data/gyro/gyroZ.c
)

# ╔══════════════════════════════════════════════════════════╗
# ║                        EXECUTABLE                        ║
# ╚══════════════════════════════════════════════════════════╝

# ───────────────────────── LIBRARIES ───────────────────────

# Include startup assembly as separate library to avoid target compile
# options being added to the assembler flags.
add_library(startup OBJECT ${CMAKE_SOURCE_DIR}/Code/system/startup_stm32f4xx.s)

# Import existing avionics static library
add_library(libavionics STATIC IMPORTED)
set_property(TARGET libavionics 
    PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Lib/bin/libavionics.a
)

# ────────────────────────── OPTIONS ────────────────────────

if(ENABLE_FLIGHT)
    set(FLIGHT -DFLIGHT CACHE BOOL "")
    set(ENABLE_TRACEALYZER OFF) 
    set(ENABLE_DUMMY OFF)
    set(ENABLE_DEBUG OFF)
    set(ENABLE_FLIGHT_TEST OFF)
endif()

if(ENABLE_TRACEALYZER)
    target_sources(${HARDWARE_TARGET_EXECUTABLE} PRIVATE ${TRACEALYZER_SOURCES})
    set(TRACEALYZER -DTRACE CACHE BOOL "")
endif()

if(ENABLE_DUMMY)
    target_sources(${HARDWARE_TARGET_EXECUTABLE} PRIVATE ${DATA_SOURCES})
    target_include_directories(${HARDWARE_TARGET_EXECUTABLE} PRIVATE 
        ${CMAKE_SOURCE_DIR}/Test/dummy/data/accel
        ${CMAKE_SOURCE_DIR}/Test/dummy/data/baro
        ${CMAKE_SOURCE_DIR}/Test/dummy/data/gyro
    )
    set(DUMMY -DDUMMY CACHE BOOL "")
endif()

if(ENABLE_DEBUG)
    set(DEBUG -DDEBUG CACHE BOOL "")
endif()

if(ENABLE_FLIGHT_TEST)
    set(FLIGHT_TEST -DFLIGHT_TEST CACHE BOOL "")
endif()

# ─────────────────────────── TARGET ────────────────────────

# Link in startup assembly and libavionics
target_link_libraries(${HARDWARE_TARGET_EXECUTABLE} 
    $<TARGET_OBJECTS:startup> 
    libavionics
)

target_compile_options(${HARDWARE_TARGET_EXECUTABLE} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_DEFS} ${C_FLAGS}>
    ${FLIGHT_TEST}
    ${TRACEALYZER}
    ${DEBUG}
    ${DUMMY}
    ${FLIGHT}
    -c -O0 
)
target_link_options(${HARDWARE_TARGET_EXECUTABLE} PRIVATE
    -Wl,-Map=${BINARY_ROOT}/${HARDWARE_TARGET_EXECUTABLE}.map
)

# ╔══════════════════════════════════════════════════════════╗
# ║                        POST-BUILD                        ║
# ╚══════════════════════════════════════════════════════════╝

add_custom_command(TARGET ${HARDWARE_TARGET_EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo                       "                                        "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "           Build Configuration          "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "                                        "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Option       Variable              Value"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "----------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Flight test  ENABLE_FLIGHT_TEST    ${ENABLE_FLIGHT_TEST}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Tracealyzer  ENABLE_TRACEALYZER    ${ENABLE_TRACEALYZER}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Debug        ENABLE_DEBUG          ${ENABLE_DEBUG}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Dummy        ENABLE_DUMMY          ${ENABLE_DUMMY}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Flight       ENABLE_FLIGHT         ${ENABLE_FLIGHT}"
    COMMAND ${CMAKE_COMMAND} -E echo                       "                                        "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --green   "For Target: ${HARDWARE_TARGET_EXECUTABLE}"
    COMMAND ${CMAKE_COMMAND} -E echo                       "                                        "
)
