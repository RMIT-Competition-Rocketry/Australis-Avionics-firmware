set(MODULE_NAME Australis-extra_bmp581)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

add_library(${MODULE_NAME} OBJECT)
target_sources(${MODULE_NAME}
  PRIVATE
    src/bmp581.c
)

# ╔══════════════════════════════════════════════════════════╗
# ║                         INCLUDES                         ║
# ╚══════════════════════════════════════════════════════════╝

target_include_directories(${MODULE_NAME}
  PUBLIC
    inc/
)
target_include_directories(Australis-extra_headers
  INTERFACE
    inc/
)

target_link_libraries(${MODULE_NAME} 
  PUBLIC
    Australis-core_headers 
  PRIVATE
    Australis-core_privateheaders
)
