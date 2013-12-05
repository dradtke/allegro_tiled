# Includes the following functions
#  copy_data add_brew_package_path

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

# This function is used to add a homebrew package
# to the pkg-config path. This can currently only
# be run once .
function (add_homebrew_package_path)
	set(first true)
	foreach(f ${ARGN})
		execute_process(
			COMMAND brew --prefix ${f}
			OUTPUT_VARIABLE brew_path
			RESULT_VARIABLE brew_failed
		)
		if (NOT brew_failed)
			STRING(REGEX REPLACE "(\r?\n)+$" "" brew_path "${brew_path}")
			if (first)
				set(ENV{PKG_CONFIG_PATH} "${brew_path}/lib/pkgconfig")
				set(first false)
			else()
				set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${brew_path}/lib/pkgconfig")
			endif()
			MESSAGE("$ENV{PKG_CONFIG_PATH}")
		endif()
	endforeach()
endfunction()
