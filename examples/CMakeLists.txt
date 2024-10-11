cmake_minimum_required(VERSION 3.19)

project(ErlTermExamples CXX)

add_executable(read_term
	read_term.cpp
)

target_link_libraries(read_term
	erlterm
)

target_compile_options(read_term
	PRIVATE
		${PROJECT_TARGET_COMPILE_OPTIONS}
)

add_executable(write_term
	write_term.cpp
)

target_link_libraries(write_term
	erlterm
)

target_compile_options(write_term
	PRIVATE
		${PROJECT_TARGET_COMPILE_OPTIONS}
)
