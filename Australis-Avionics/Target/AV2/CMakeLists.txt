
# ╔══════════════════════════════════════════════════════════╗
# ║                AUSTRALIS V2 HARDWARE TARGET              ║
# ╚══════════════════════════════════════════════════════════╝

# Add current directory to executable for includes
target_include_directories(Australis-firmware PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add hardware target specific sources to executable
target_sources(Australis-firmware PUBLIC
  ./devices.c
)
