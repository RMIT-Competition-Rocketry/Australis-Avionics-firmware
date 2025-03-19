# ╔══════════════════════════════════════════════════════════╗
# ║                AUSTRALIS V2 HARDWARE TARGET              ║
# ╚══════════════════════════════════════════════════════════╝

# Add current directory to executable for includes as well 
# as any necessary extra includes
target_include_directories(${HARDWARE_TARGET_EXECUTABLE} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${FIRMWARE_EXTRA_ROOT}/inc/
  ${FIRMWARE_EXTRA_ROOT}/inc/rtos/
  ${FIRMWARE_EXTRA_ROOT}/inc/rtos/data
  ${FIRMWARE_EXTRA_ROOT}/inc/rtos/comms
  ${FIRMWARE_EXTRA_ROOT}/inc/drivers/lora
  ${FIRMWARE_EXTRA_ROOT}/inc/drivers/gps
)

# Add hardware target specific sources to executable 
target_sources(${HARDWARE_TARGET_EXECUTABLE} PUBLIC
  ${FIRMWARE_EXTRA_ROOT}/src/packets.c
  ${FIRMWARE_EXTRA_ROOT}/src/rtos/heartbeat.c
  ${FIRMWARE_EXTRA_ROOT}/src/rtos/comms/loracomm.c
  ${FIRMWARE_EXTRA_ROOT}/src/rtos/data/gpsacquisition.c
  ${FIRMWARE_EXTRA_ROOT}/src/drivers/lora/sx1272.c
  ${FIRMWARE_EXTRA_ROOT}/src/drivers/gps/sam_m10q.c
  src/devices.c
  src/tasks.c
)
