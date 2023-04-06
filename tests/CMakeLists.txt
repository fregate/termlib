set(TESTS_NAME ${LIBRARY_NAME}UnitTests)

find_package(GTest REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${TESTS_NAME}
    ParserIteratorTests.cpp
    TermBuilderTests.cpp
)

set_target_properties(
    ${TESTS_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME ${TESTS_NAME}
    OUTPUT_NAME ${TESTS_NAME}
)

target_compile_features(${TESTS_NAME} PUBLIC cxx_std_17)

target_link_libraries(${TESTS_NAME}
    PRIVATE
    ${LIBRARY_NAME}

    GTest::gtest
    GTest::gtest_main
)
