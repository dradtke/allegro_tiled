# Includes the following functions
#  copy_data add_brew_package_path

# This is used for copying assets
# It allows for assets to be copied to the build dir
# This way, it allows for the example to run when
# the build dir wasn't the project root.
function (copy_data target name destination)
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

# This function is used to mimic pkg-config by manually
# setting the variables it would set for certain packages.
# Dependencies are located via *_HOME environment variables.
function (find_env_deps)
	if(DEFINED ENV{ALLEGRO_HOME})
		message("Using ALLEGRO_HOME: $ENV{ALLEGRO_HOME}")
		if(NOT DEFINED ENV{ALLEGRO_VERSION})
			message(FATAL_ERROR "ALLEGRO_HOME is defined, but ALLEGRO_VERSION is not.")
		endif()
		set(ALLEGRO_LIBRARY_DIRS "$ENV{ALLEGRO_HOME}\\bin" PARENT_SCOPE)
		set(ALLEGRO_INCLUDE_DIRS "$ENV{ALLEGRO_HOME}\\include" PARENT_SCOPE)
		set(ALLEGRO_LIBRARIES "-lallegro-$ENV{ALLEGRO_VERSION}-mt -lallegro_image-$ENV{ALLEGRO_VERSION}-mt" PARENT_SCOPE)
	else()
		message(FATAL_ERROR "ALLEGRO_HOME is not defined.")
	endif()

	if(DEFINED ENV{LIBXML_HOME})
		message("Using LIBXML_HOME: $ENV{LIBXML_HOME}")
		set(LIBXML_LIBRARY_DIRS "$ENV{LIBXML_HOME}\\bin" PARENT_SCOPE)
		set(LIBXML_INCLUDE_DIRS "$ENV{LIBXML_HOME}\\include" PARENT_SCOPE)
		set(LIBXML_LIBRARIES "-lxml2" PARENT_SCOPE)
	else()
		message(FATAL_ERROR "LIBXML_HOME is not defined.")
	endif()

	if(DEFINED ENV{ZLIB_HOME})
		message("Using ZLIB_HOME: $ENV{ZLIB_HOME}")
		set(ZLIB_LIBRARY_DIRS "$ENV{ZLIB_HOME}" PARENT_SCOPE)
		set(ZLIB_INCLUDE_DIRS "$ENV{ZLIB_HOME}\\include" PARENT_SCOPE)
		set(ZLIB_LIBRARIES "-lzlib1" PARENT_SCOPE)
	else()
		message(FATAL_ERROR "ZLIB_HOME is not defined.")
	endif()

	if(DEFINED ENV{GLIB_HOME})
		message("Using GLIB_HOME: $ENV{GLIB_HOME}")
		set(GLIB_LIBRARY_DIRS "$ENV{GLIB_HOME}\\lib" PARENT_SCOPE)
		set(GLIB_INCLUDE_DIRS "$ENV{GLIB_HOME}\\include\\glib-2.0" "$ENV{GLIB_HOME}\\lib\\glib-2.0\\include" PARENT_SCOPE)
		set(GLIB_LIBRARIES "-lglib-2.0" PARENT_SCOPE)
	else()
		message(FATAL_ERROR "GLIB_HOME is not defined.")
	endif()
endfunction()
