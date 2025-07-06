if(NOT SUBTARGET)
  message(FATAL_ERROR " ")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${SUBTARGET})
  message(FATAL_ERROR 
    " Subtarget directory ${SUBTARGET} not found. Please specify a valid subtarget.")
endif()

# Build the selected subtarget target
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${SUBTARGET})
