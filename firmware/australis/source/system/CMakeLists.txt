add_library(Startup OBJECT)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

# Include startup assembly as separate library to avoid target compile
# options being added to the assembler flags.
add_library(Startup_asm OBJECT 
  src/startup_stm32f4xx.s
)

target_sources(Startup
  PUBLIC
    src/system_stm32f4xx.c
)

# ╔══════════════════════════════════════════════════════════╗
# ║                         INCLUDES                         ║
# ╚══════════════════════════════════════════════════════════╝

add_library(Startup_headers INTERFACE)
add_library(Startup_linker INTERFACE stm32_flash.ld)

target_include_directories(Startup_headers 
  INTERFACE 
    inc
    ${FIRMWARE_LIB_ROOT}/inc
    ${FIRMWARE_LIB_ROOT}/inc/CORE
    ${FIRMWARE_LIB_ROOT}/inc/CORE/Include
    ${FIRMWARE_LIB_ROOT}/inc/DSP
    ${FIRMWARE_LIB_ROOT}/inc/DSP/Include
    ${FIRMWARE_LIB_ROOT}/inc/DSP/Include/dsp
    ${FIRMWARE_LIB_ROOT}/inc/DSP/PrivateInclude
)

target_link_libraries(Startup
  PUBLIC
    Startup_headers
    Startup_linker
  PRIVATE
    $<TARGET_OBJECTS:Startup_asm>
)
