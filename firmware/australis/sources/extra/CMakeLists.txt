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

  # ---------------------------------------------------
  add_library(${_MODULE_LIB} OBJECT)
  target_sources(${_MODULE_LIB}
    PUBLIC
      src/${_MODULE_PATH}/${_MODULE_NAME}.c
  )
  target_include_directories(${_MODULE_LIB}
    PUBLIC
      inc/${_MODULE_PATH}
      # TODO: Update GPS to implement API and remove this
      inc/drivers/gps
  )
  target_include_directories(Australis-extra_headers
    INTERFACE
      inc/${_MODULE_PATH}
  )
  # ---------------------------------------------------

  target_link_libraries(${_MODULE_LIB} 
    PUBLIC
      Australis-core_headers 
    PRIVATE
      Australis-core_privateheaders
  )
  target_link_libraries(Australis-extra INTERFACE ${_MODULE_LIB})
endforeach()


# ╔══════════════════════════════════════════════════════════╗
# ║                         INCLUDES                         ║
# ╚══════════════════════════════════════════════════════════╝

#target_include_directories(Australis-extra
#  PUBLIC
#    inc
#    inc/drivers/device/gps
#    inc/drivers/device/sensors
#    inc/drivers/device/sensors/accel
#    inc/drivers/device/sensors/baro
#    inc/drivers/device/sensors/gyro
#)
