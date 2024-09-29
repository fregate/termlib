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
