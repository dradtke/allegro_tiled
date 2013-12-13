cmake_minimum_required(VERSION 2.6)
project(allegro_tiled)

# Allows us to get CMake stuff from other places
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Utility functions
include(Utils)

# Options
option(WANT_EXAMPLE "build the example" true)

# Use pkg-config for all deps
find_package(PkgConfig)

# Note: I've set this to not just run for APPLE
# just in case someone's using linuxbrew
add_homebrew_package_path(libxml2 zlib)

if(APPLE)
	pkg_check_modules(
		ALLEGRO REQUIRED
		allegro-5.0
		allegro_main-5.0
		allegro_image-5.0
	)
else()
	pkg_check_modules(
		ALLEGRO REQUIRED
		allegro-5.0
		allegro_image-5.0
	)
endif()

pkg_check_modules(
	GLIB REQUIRED
	glib-2.0
)

pkg_check_modules(
	LIBXML REQUIRED
	libxml-2.0
)

pkg_check_modules(
	ZLIB REQUIRED
	zlib
)

link_directories(
	${ALLEGRO_LIBRARY_DIRS}
	${GLIB_LIBRARY_DIRS}
	${LIBXML_LIBRARY_DIRS}
	${ZLIB_LIBRARY_DIRS}
)

include_directories(
	${ALLEGRO_INCLUDE_DIRS}
	${GLIB_INCLUDE_DIRS}
	${LIBXML_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include
)

file(GLOB allegro_tiled_headers include/allegro5/*.h)
file(GLOB allegro_tiled_sources src/*.c)

# Generate pc files
configure_file(
	misc/allegro_tiled-5.pc.in
	misc/allegro_tiled-5.pc
	@ONLY
)

configure_file(
	misc/allegro_tiled-5.pc.in
	misc/allegro_tiled-5.0.pc
	@ONLY
)

# Actually build the library
add_library(
	allegro_tiled
	${allegro_tiled_sources}
)

# Make sure the library is linked against everything we need
target_link_libraries(
	allegro_tiled
	${ALLEGRO_LIBRARIES}
	${GLIB_LIBRARIES}
	${LIBXML_LIBRARIES}
	${ZLIB_LIBRARIES}
)

install(
	TARGETS allegro_tiled
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(
	FILES ${allegro_tiled_headers}
	DESTINATION include/allegro5
)

install(
	FILES misc/allegro_tiled-5.pc misc/allegro_tiled-5.0.pc
	DESTINATION lib/pkgconfig
)

# Build the example if the option is enabled
if (WANT_EXAMPLE)
	add_subdirectory(example)
endif()
