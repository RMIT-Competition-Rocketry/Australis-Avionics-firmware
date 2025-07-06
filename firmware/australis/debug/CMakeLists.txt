if(ENABLE_DEBUG)
    add_library(Australis-debug OBJECT)

    target_link_libraries(Australis-debug
        PUBLIC
            Australis-core_headers
            Australis-extra_headers
        PRIVATE
            Australis-core
            Australis-core_privateheaders
    )

    target_link_libraries(Australis-firmware
        PRIVATE
            Australis-debug
    )

    target_sources(Australis-debug
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_adcvsense.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_accelerometer.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_barometer.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_gyroscope.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_launch.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_state.c
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_stack.c
    )

    if(TARGET Australis-extra_sx1272)
    target_sources(Australis-debug
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/debug_sx1272.c
    )
    endif()

    target_sources(Australis-firmware 
        PUBLIC
            $<TARGET_OBJECTS:Australis-debug>
    )
endif()

