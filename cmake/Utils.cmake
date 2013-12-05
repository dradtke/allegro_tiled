# Includes the following functions
#  copy_data

# This is used for copying assets
# It allows for assets to be copied to the build dir
# This way, it allows for the example to run when
# the build dir wasn't the project root.
function(copy_data target name destination)
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
        return()
    endif()

    file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*)
    
    add_custom_target(${target} ALL DEPENDS ${files})

    foreach(file ${files})
        add_custom_command(
            OUTPUT "${destination}/${file}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
            COMMAND "${CMAKE_COMMAND}" -E copy
                    "${CMAKE_CURRENT_SOURCE_DIR}/${file}" "${destination}/${file}"
            )
    endforeach()
endfunction()
