project(TermLib)
cmake_minimum_required(VERSION 3.19)

set(LIBRARY_NAME term)

add_library(${LIBRARY_NAME} STATIC)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

## ------------ Build Tests ---------- ##
option(TERMLIB_BUILD_TESTS "Build unit test" OFF)
##

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(TERMLIB_BUILD_TESTS)
	add_subdirectory(tests)
endif()

find_package(Erlang REQUIRED)

target_include_directories(${LIBRARY_NAME}
	PUBLIC
		include
)

target_sources(${LIBRARY_NAME}
	PUBLIC
		include/termlib/Exception.h
		include/termlib/TermBuilder.h
		include/termlib/TermParser.h

	PRIVATE
		src/Exception.cpp
		src/TermBuilder.cpp
		src/TermParser.cpp
)

target_link_libraries(${LIBRARY_NAME}
	PUBLIC
		Erlang::EI
		Erlang::Erlang
)

