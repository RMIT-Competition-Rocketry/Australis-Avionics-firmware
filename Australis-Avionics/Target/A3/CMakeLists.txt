
# ╔══════════════════════════════════════════════════════════╗
# ║                 AURORA III HARDWARE TARGET               ║
# ╚══════════════════════════════════════════════════════════╝

# Add current directory to executable for includes
target_include_directories(Australis-firmware_${HARDWARE_TARGET} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add hardware target specific sources to executable
target_sources(Australis-firmware_${HARDWARE_TARGET} PUBLIC
  ./devices.c
)
