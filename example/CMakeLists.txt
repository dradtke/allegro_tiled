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
