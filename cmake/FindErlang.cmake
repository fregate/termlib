include(FindPackageHandleStandardArgs)
include(CMakePrintHelpers)

function(execute_erlang IN_ERL_EXECUTABLE OUT_ERLROOT_DIR OUT_INCLUDE_DIR OUT_LIBRARY_DIR)
  EXECUTE_PROCESS(
    COMMAND ${IN_ERL_EXECUTABLE} -noshell -eval "io:format(\"~s\", [code:lib_dir()])" -s erlang halt
    OUTPUT_VARIABLE Erlang_OTP_LIB_DIR
  )

  EXECUTE_PROCESS(
    COMMAND ${IN_ERL_EXECUTABLE} -noshell -eval "io:format(\"~s\", [code:root_dir()])" -s erlang halt
    OUTPUT_VARIABLE Erlang_OTP_ROOT_DIR
  )

  EXECUTE_PROCESS(
    COMMAND ${IN_ERL_EXECUTABLE} -noshell -eval "io:format(\"~s\",[filename:basename(code:lib_dir('erl_interface'))])" -s erlang halt
    OUTPUT_VARIABLE Erlang_EI_DIR
  )

  SET(${OUT_ERLROOT_DIR} ${Erlang_OTP_ROOT_DIR} PARENT_SCOPE)
  SET(${OUT_INCLUDE_DIR} ${Erlang_OTP_LIB_DIR}/${Erlang_EI_DIR}/include PARENT_SCOPE)
  SET(${OUT_LIBRARY_DIR} ${Erlang_OTP_LIB_DIR}/${Erlang_EI_DIR}/lib PARENT_SCOPE)
endfunction(execute_erlang)

cmake_print_variables(ERLANG_ROOT ERLANG_EI_LIB ERLANG_EI_LIB)

if (NOT ERLANG_ROOT)
  if ((NOT ERLANG_EI_LIB) OR (NOT ERLANG_EI_INCLUDE))
    FIND_PROGRAM(ERLEX NAMES erl)
    execute_erlang(${ERLEX} Erlang_OTP_ROOT_DIR Erlang_EI_INCLUDE_DIRS Erlang_EI_LIBRARY_PATH)
  else()
    SET(Erlang_OTP_ROOT_DIR ${CMAKE_FIND_ROOT_PATH}/host/lib/erlang)
    SET(Erlang_EI_LIBRARY_PATH ${ERLANG_EI_LIB})
    SET(Erlang_EI_INCLUDE_DIRS ${ERLANG_EI_INCLUDE})
  endif()
else()
  execute_erlang(${ERLANG_ROOT}/bin/erl Erlang_OTP_ROOT_DIR Erlang_EI_INCLUDE_DIRS Erlang_EI_LIBRARY_PATH)
endif()

SET(Erlang_EI_LIBRARY_NAME   libei.a)
set(Erlang_FOUND ON)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Erlang
  Erlang_EI_LIBRARY_PATH
  Erlang_EI_INCLUDE_DIRS
  Erlang_FOUND
)

if(NOT TARGET Erlang::Erlang)
  add_library(Erlang::Erlang INTERFACE IMPORTED)
  set_target_properties(Erlang::Erlang
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Erlang_OTP_ROOT_DIR}/usr/include
    )
endif()

if(NOT TARGET Erlang::EI)
  add_library(erlang_ei STATIC IMPORTED)
  set_property(TARGET erlang_ei PROPERTY
    IMPORTED_LOCATION ${Erlang_EI_LIBRARY_PATH}/${Erlang_EI_LIBRARY_NAME}
  )

  add_library(Erlang::EI INTERFACE IMPORTED)
  set_property(TARGET Erlang::EI PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Erlang_EI_INCLUDE_DIRS}
  )
  set_property(TARGET Erlang::EI PROPERTY
    INTERFACE_LINK_LIBRARIES erlang_ei
  )
endif()
