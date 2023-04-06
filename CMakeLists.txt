project(TermLib)
cmake_minimum_required(VERSION 3.14)

set(LIBRARY_NAME term)

add_library(${LIBRARY_NAME} STATIC)
add_library(${LIBRARY_NAME}Private INTERFACE)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

## ------------ Build Tests ---------- ##

option(TERMLIB_BUILD_TESTS "Build unit test" ON)

##

include (cmake/common.cmake)

list(APPEND CMAKE_MODULE_PATH "cmake")
find_package(Erlang REQUIRED)

target_include_directories(${LIBRARY_NAME}
	PUBLIC
		include
)

target_sources(${LIBRARY_NAME}
	PUBLIC
		include/termlib/TermBuilder.h
		include/termlib/TermParser.h

	PRIVATE
		src/TermBuilder.cpp
		src/TermParser.cpp
)

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
		Erlang::EI
		Erlang::Erlang
)

# Private interface
target_link_libraries(${LIBRARY_NAME}Private
	INTERFACE
		${LIBRARY_NAME}
)

if(TERMLIB_BUILD_TESTS)
	find_package(conan COMPONENTS gtest/1.11.0)
	add_subdirectory(tests)
endif(TERMLIB_BUILD_TESTS)
