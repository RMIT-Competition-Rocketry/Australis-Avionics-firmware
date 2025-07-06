cmake_minimum_required(VERSION 3.15)

if(NOT TARGET Australis-config)
  message(FATAL_ERROR " Australis-config target not specified. Please specify a cmake target that defines the include directory for AustralisConfig.h")
endif()


# Define the firmware source root directory
set(FIRMWARE_SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR}/sources)

# Define the firmware system root directory
set(FIRMWARE_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/lib)

# Define the firmware debug root directory
set(FIRMWARE_DEBUG_ROOT ${CMAKE_CURRENT_LIST_DIR}/debug)

# Define the firmware test root directory
set(FIRMWARE_TEST_ROOT ${CMAKE_CURRENT_LIST_DIR}/test)

# Define the middlewares root directory
set(MIDDLEWARES_ROOT ${CMAKE_CURRENT_LIST_DIR}/middlewares)


# Define the firmware core root directory
set(FIRMWARE_CORE_ROOT ${FIRMWARE_SOURCE_ROOT}/core)

# Define the firmware extra root directory
set(FIRMWARE_EXTRA_ROOT ${FIRMWARE_SOURCE_ROOT}/extra)

# Define the firmware system root directory
set(FIRMWARE_SYSTEM_ROOT ${FIRMWARE_SOURCE_ROOT}/system)


# ─────────────────────── Build Options ─────────────────────

option(ENABLE_FLIGHT       "Enable flight mode"         OFF)
option(ENABLE_DEBUG        "Enable debug mode"          OFF)
option(ENABLE_TEST         "Enable test mode"           OFF)
option(ENABLE_DUMMY        "Enable dummy test mode"     OFF)
option(ENABLE_TRACE        "Enable trace recording"     OFF)

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
      "$<TARGET_PROPERTY:Australis-config,INTERFACE_INCLUDE_DIRECTORIES>"
      sources
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
add_subdirectory(${FIRMWARE_DEBUG_ROOT})
add_subdirectory(${FIRMWARE_TEST_ROOT})

# -- LIBAVIONICS ---------------------------------------------

add_library(libavionics STATIC IMPORTED)
set_property(
  TARGET 
    libavionics 
  PROPERTY IMPORTED_LOCATION 
    ${FIRMWARE_LIB_ROOT}/bin/libavionics.a
)


# ── LINKING ─────────────────────────────────────────────────

# Link libraries to target
target_link_libraries(Australis-firmware
  PUBLIC
    Startup 
    Australis-extra
    Australis-extra_headers
    Australis-core_headers
    libavionics 
  PRIVATE
    Australis-core
    Australis-core_privateheaders
)
target_sources(Australis-firmware PUBLIC
  $<TARGET_OBJECTS:Australis-core>
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

if(ENABLE_TRACE)
  set(TRACE_INCLUDES
      ${MIDDLEWARES_ROOT}/TraceRecorder/config
      ${MIDDLEWARES_ROOT}/TraceRecorder/include
  )
  set(TRACE_SOURCES
      ${MIDDLEWARES_ROOT}/TraceRecorder/SEGGER_RTT.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcAssert.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcCounter.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcDependency.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcDiagnostics.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcEntryTable.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcError.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcEvent.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcEventBuffer.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcExtension.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcHardwarePort.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcHeap.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcInternalEventBuffer.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcInterval.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcISR.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcKernelPort.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcMultiCoreEventBuffer.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcObject.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcPrint.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcRunnable.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcSnapshotRecorder.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcStackMonitor.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcStateMachine.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcStaticBuffer.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcStreamingRecorder.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcStreamPort.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcString.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcTask.c
      ${MIDDLEWARES_ROOT}/TraceRecorder/trcTimestamp.c
  )
  # Add trace to Australis
  target_include_directories(Australis-core_headers INTERFACE ${TRACE_INCLUDES})

  # Add trace to FreeRTOS
  target_include_directories(freertos_config INTERFACE ${TRACE_INCLUDES})
  target_link_libraries(freertos_config INTERFACE Startup_headers)
  target_sources(freertos_kernel PRIVATE ${TRACE_SOURCES})
endif()

add_custom_command(TARGET Australis-firmware POST_BUILD
  COMMAND_EXPAND_LISTS
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "Build Target:                                          "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  ${HARDWARE_TARGET} ${SUBTARGET}                      "
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "Build Configuration:                                   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Option       Variable              Value             "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  ----------------------------------------             "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Debug        ENABLE_DEBUG          ${ENABLE_DEBUG}   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Test         ENABLE_TEST           ${ENABLE_TEST}    "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Dummy        ENABLE_DUMMY          ${ENABLE_DUMMY}   "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Flight       ENABLE_FLIGHT         ${ENABLE_FLIGHT}  "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "  Trace        ENABLE_TRACE          ${ENABLE_TRACE}   "
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red     "Australis Extra Modules:                               "
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow  "$<LIST:TRANSFORM,${AUSTRALIS_EXTRA_MODULES},PREPEND,  >"
  COMMAND ${CMAKE_COMMAND} -E echo                       "                                                       "
)
