add_library(Australis-core OBJECT)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

target_sources(Australis-core 
  PRIVATE
    # -- SOURCE ---------------------------------------------
    src/AustralisCore.c
    src/devicelist.c
    src/tasklist.c
    src/state.c
    src/topic.c
    src/packet.c

    # -- RTOS -----------------------------------------------
    src/rtos/stateupdate.c
    src/rtos/flashwrite.c
    src/rtos/data/hdataacquisition.c
    src/rtos/data/ldataacquisition.c

    # -- PERIPHERAL DRIVERS ---------------------------------
    src/drivers/peripheral/adc.c
    src/drivers/peripheral/gpiopin.c
    src/drivers/peripheral/spi.c
    src/drivers/peripheral/tim.c
    src/drivers/peripheral/can.c
    src/drivers/peripheral/uart.c

    # -- SHELL ----------------------------------------------
    src/shell/shell.c
    src/shell/parser.c
    src/shell/mem.c
    src/shell/help.c
    src/shell/clear.c
)
target_link_options(Australis-core BEFORE PUBLIC
  -T${CMAKE_CURRENT_LIST_DIR}/core.ld
)

# ╔══════════════════════════════════════════════════════════╗
# ║                         INCLUDES                         ║
# ╚══════════════════════════════════════════════════════════╝

add_library(Australis-core_headers INTERFACE)
target_include_directories(Australis-core_headers
  INTERFACE
    inc
    inc/shell
    inc/rtos
    inc/rtos/data
    inc/drivers/peripheral
    inc/drivers/device
    inc/drivers/device/communication
    inc/drivers/device/storage
    inc/drivers/device/sensors
)
target_link_libraries(Australis-core_headers
  INTERFACE
    Startup_headers
    freertos_kernel
)

add_library(Australis-core_privateheaders INTERFACE)
target_include_directories(Australis-core_privateheaders
  INTERFACE
    inc/private
)
target_link_libraries(Australis-core_privateheaders
  INTERFACE
    Startup_headers
    freertos_kernel
)

target_link_libraries(Australis-core
  PUBLIC
    Australis-config
    Australis-core_headers
  PRIVATE
    Australis-core_privateheaders
)
