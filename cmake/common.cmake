
set(thirdparty_DIR "${CMAKE_SOURCE_DIR}/thirdparty/cmake" CACHE STRING "thiredparty dir" FORCE)
set(conan_DIR "${CMAKE_SOURCE_DIR}/thirdparty/cmake" CACHE STRING "conan dir" FORCE)

if(CMAKE_VERSION VERSION_LESS 3.21)
  get_property(not_top DIRECTORY PROPERTY PARENT_DIRECTORY)
  if(NOT not_top)
    set(PROJECT_IS_TOP_LEVEL true)
  endif()
endif()
