cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Australis-Avionics LANGUAGES C ASM)

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

set(HEADERS
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/Inc/can
    ${CMAKE_SOURCE_DIR}/Core/Inc/rtos
    ${CMAKE_SOURCE_DIR}/Core/Inc/rtos/tasks
    ${CMAKE_SOURCE_DIR}/Core/Inc/rtos/tasks/comms
    ${CMAKE_SOURCE_DIR}/Core/Inc/rtos/tasks/data
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/flash
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/lora
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/sensors
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/sensors/accel
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/sensors/baro
    ${CMAKE_SOURCE_DIR}/Core/Inc/spi/sensors/gyro
    ${CMAKE_SOURCE_DIR}/Core/Inc/uart
    ${CMAKE_SOURCE_DIR}/Core/Inc/uart/gps
    ${CMAKE_SOURCE_DIR}/Core/Inc/uart/usb
    ${CMAKE_SOURCE_DIR}/Core/Inc/shell
    ${CMAKE_SOURCE_DIR}/Lib/inc
    ${CMAKE_SOURCE_DIR}/Lib/inc/CORE
    ${CMAKE_SOURCE_DIR}/Lib/inc/CORE/Include
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/Include
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/Include/dsp
    ${CMAKE_SOURCE_DIR}/Lib/inc/DSP/PrivateInclude
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/config
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/include
)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

set(SOURCES
    # -- System ---------------------------------------------
    ${CMAKE_SOURCE_DIR}/MDK-ARM/System/system_stm32f4xx.c

    # -- Source ---------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_SOURCE_DIR}/Core/Src/drivers.c
    ${CMAKE_SOURCE_DIR}/Core/Src/devicelist.c
    ${CMAKE_SOURCE_DIR}/Core/Src/devices.c

    # -- Shell ----------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/shell/launch.c
    ${CMAKE_SOURCE_DIR}/Core/Src/shell/mem.c
    ${CMAKE_SOURCE_DIR}/Core/Src/shell/baro.c
    ${CMAKE_SOURCE_DIR}/Core/Src/shell/help.c

    # -- RTOS -----------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/freertos.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/stateupdate.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/comms/loracomm.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/comms/usbcomm.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/data/flashwrite.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/data/gpsacquisition.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/data/hdataacquisition.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/data/ldataacquisition.c
    ${CMAKE_SOURCE_DIR}/Core/Src/rtos/tasks/comms/payloadcomm.c

    # -- CAN ------------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/can/can.c

    # -- SPI ------------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/spi.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/flash/w25q128.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/lora/sx1272.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/sensors/sensors.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/sensors/accel/kx134_1211.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/sensors/gyro/a3g4250d.c
    ${CMAKE_SOURCE_DIR}/Core/Src/spi/sensors/baro/bmp581.c

    # -- UART -----------------------------------------------
    ${CMAKE_SOURCE_DIR}/Core/Src/uart/uart.c
    ${CMAKE_SOURCE_DIR}/Core/Src/uart/gps/gps.c
    ${CMAKE_SOURCE_DIR}/Core/Src/uart/usb/shell.c

    # -- FreeRTOS -------------------------------------------
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
)

# ──────────────────── TRACEALYZER SOURCES ──────────────────

set(TRACEALYZER_SOURCES
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/SEGGER_RTT.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcAssert.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcCounter.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcDependency.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcDiagnostics.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcEntryTable.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcError.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcEvent.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcExtension.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcHardwarePort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcHeap.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcInternalEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcInterval.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcISR.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcKernelPort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcMultiCoreEventBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcObject.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcPrint.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcRunnable.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcSnapshotRecorder.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcStackMonitor.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcStateMachine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcStaticBuffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcStreamingRecorder.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcStreamPort.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcString.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcTask.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/TraceRecorder/trcTimestamp.c
)

# ──────────────────────── DATA SOURCES ─────────────────────

set(DATA_SOURCES
    ${CMAKE_SOURCE_DIR}/Data/accelX.c
    ${CMAKE_SOURCE_DIR}/Data/accelY.c
    ${CMAKE_SOURCE_DIR}/Data/accelZ.c
    ${CMAKE_SOURCE_DIR}/Data/press.c
    ${CMAKE_SOURCE_DIR}/Data/gyroX.c
    ${CMAKE_SOURCE_DIR}/Data/gyroY.c
    ${CMAKE_SOURCE_DIR}/Data/gyroZ.c
)

# ╔══════════════════════════════════════════════════════════╗
# ║                        EXECUTABLE                        ║
# ╚══════════════════════════════════════════════════════════╝

# ───────────────────────── LIBRARIES ───────────────────────

# Include startup assembly as separate library to avoid target compile
# options being added to the assembler flags.
add_library(startup ${CMAKE_SOURCE_DIR}/MDK-ARM/System/startup_stm32f4xx.s)

add_library(avionics STATIC IMPORTED)
set_property(TARGET avionics PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Lib/bin/libavionics.a)

# ────────────────────────── OPTIONS ────────────────────────

if(ENABLE_FLIGHT)
    set(FLIGHT -DFLIGHT CACHE BOOL "" FORCE)
    set(ENABLE_TRACEALYZER OFF CACHE BOOL "" FORCE)
    set(ENABLE_DUMMY OFF CACHE BOOL "" FORCE)
    set(ENABLE_DEBUG OFF CACHE BOOL "" FORCE)
    set(ENABLE_FLIGHT_TEST OFF CACHE BOOL "" FORCE)
endif()

if(ENABLE_TRACEALYZER)
    list(APPEND SOURCES ${TRACEALYZER_SOURCES})
    set(TRACEALYZER -DTRACE CACHE BOOL "" FORCE)
endif()

if(ENABLE_DUMMY)
    list(APPEND SOURCES ${DATA_SOURCES})
    list(APPEND HEADERS ${CMAKE_SOURCE_DIR}/Data)
    set(DUMMY -DDUMMY CACHE BOOL "" FORCE)
endif()

if(ENABLE_DEBUG)
    set(DEBUG -DDEBUG CACHE BOOL "" FORCE)
endif()

if(ENABLE_FLIGHT_TEST)
    set(FLIGHT_TEST -DFLIGHT_TEST CACHE BOOL "" FORCE)
endif()

# ─────────────────────────── TARGET ────────────────────────

add_executable(Australis-firmware.elf ${SOURCES})
target_link_libraries(Australis-firmware.elf startup avionics)

target_include_directories(Australis-firmware.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/System
    ${HEADERS}
)
target_compile_options(Australis-firmware.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_DEFS} ${C_FLAGS}>
    ${FLIGHT_TEST}
    ${TRACEALYZER}
    ${DEBUG}
    ${DUMMY}
    ${FLIGHT}
    -c -O0 
)
target_link_options(Australis-firmware.elf PRIVATE
    -Wl,-Map=Australis-firmware.map
    -o Australis-firmware.elf
)

# ╔══════════════════════════════════════════════════════════╗
# ║                        POST-BUILD                        ║
# ╚══════════════════════════════════════════════════════════╝

add_custom_command(TARGET Australis-firmware.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "           Build configuration          "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "                                        "
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Option       Variable              Value"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "----------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Flight test  ENABLE_FLIGHT_TEST    ${ENABLE_FLIGHT_TEST}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Tracealyzer  ENABLE_TRACEALYZER    ${ENABLE_TRACEALYZER}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Debug        ENABLE_DEBUG          ${ENABLE_DEBUG}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Dummy        ENABLE_DUMMY          ${ENABLE_DUMMY}"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "Flight       ENABLE_FLIGHT         ${ENABLE_FLIGHT}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
