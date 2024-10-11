cmake_minimum_required(VERSION 3.19)

project(TermLib CXX)

set(LIBRARY_NAME erlterm)

add_library(${LIBRARY_NAME} INTERFACE)

target_compile_features(${LIBRARY_NAME} INTERFACE cxx_std_20)

set(PROJECT_TARGET_COMPILE_OPTIONS
	-Werror -Wall -Wpedantic -Wextra
)
target_compile_options(${LIBRARY_NAME}
	INTERFACE
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

include(FetchContent)
FetchContent_Declare(
	glaze
	GIT_REPOSITORY https://github.com/stephenberry/glaze.git
	GIT_TAG v3.6.1
	GIT_SHALLOW TRUE
	PATCH_COMMAND git apply "${CMAKE_CURRENT_LIST_DIR}/thirdparty/glaze.diff"
)
FetchContent_MakeAvailable(glaze)

target_include_directories(${LIBRARY_NAME}
	INTERFACE
		include
)

target_sources(${LIBRARY_NAME}
	PUBLIC
		include/erlterm/core/defs.hpp

		include/erlterm/ei/ei.hpp
		include/erlterm/ei/read.hpp
		include/erlterm/ei/types.hpp

		include/erlterm/utils/cmp.hpp
		include/erlterm/read.hpp
)

target_link_libraries(${LIBRARY_NAME}
	INTERFACE
		Erlang::EI
		Erlang::Erlang

		glaze::glaze
)
