cmake_minimum_required(VERSION 3.19)

project(TermLib CXX)

set(LIBRARY_NAME erlterm)

add_library(${LIBRARY_NAME} STATIC)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

set(PROJECT_TARGET_COMPILE_OPTIONS
	-Werror -Wall -Wpedantic -Wextra
)
target_compile_options(${LIBRARY_NAME}
	PRIVATE
		${PROJECT_TARGET_COMPILE_OPTIONS}
)

## ------------ Build Tests ---------- ##
option(TERMLIB_BUILD_TESTS "Build unit test" OFF)
option(TERMLIB_BUILD_EXAMPLES "Build examples" OFF)
##

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(TERMLIB_BUILD_TESTS)
	add_subdirectory(tests)
endif(TERMLIB_BUILD_TESTS)

if(TERMLIB_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(TERMLIB_BUILD_EXAMPLES)

find_package(Erlang REQUIRED)

add_subdirectory(thirdparty/glaze)

target_include_directories(${LIBRARY_NAME}
	PUBLIC
		include
)

target_sources(${LIBRARY_NAME}
	PUBLIC
		include/termlib/Exception.h
		include/termlib/TermBuilder.h
		include/termlib/TermParser.h

		include/erlterm/read.hpp

	PRIVATE
		src/Exception.cpp
		src/TermBuilder.cpp
		src/TermParser.cpp
)

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
		Erlang::EI
		Erlang::Erlang

		glaze::glaze
)
