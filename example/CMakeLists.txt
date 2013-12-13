# NOTE: This file is not meant to be called directly.
# It will be called from the main CMakeLists.txt file
# when the example needs to be built.

copy_data(example_data data .)

file(GLOB example_sources src/*.c)
add_executable(
	example
	${example_sources}
)

target_link_libraries(
	example
	allegro_tiled
)
