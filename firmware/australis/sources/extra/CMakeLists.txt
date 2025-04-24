add_library(Australis-extra INTERFACE)
add_library(Australis-extra_headers INTERFACE)

# ╔══════════════════════════════════════════════════════════╗
# ║                         SOURCES                          ║
# ╚══════════════════════════════════════════════════════════╝

foreach(_MODULE ${AUSTRALIS_EXTRA_MODULES})
  set(_MODULE_NAME $<PATH:GET_FILENAME,${_MODULE}>)
  set(_MODULE_PATH $<PATH:GET_PARENT_PATH,${_MODULE}>)

  cmake_path(GET _MODULE FILENAME _MODULE_NAME)
  set(_MODULE_LIB Australis-extra_${_MODULE_NAME})

  add_subdirectory(${_MODULE})

  target_sources(Australis-extra INTERFACE $<TARGET_OBJECTS:${_MODULE_LIB}>)
  target_link_libraries(Australis-extra INTERFACE ${_MODULE_LIB})
endforeach()
