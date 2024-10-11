set(TESTS_NAME ${LIBRARY_NAME}UnitTests)

find_package(GTest REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${TESTS_NAME}
    # ParserIteratorTests.cpp
    # TermBuilderTests.cpp
    # TermParserTests.cpp
)

target_compile_features(${TESTS_NAME} PUBLIC cxx_std_20)

target_link_libraries(${TESTS_NAME}
    PRIVATE
        ${LIBRARY_NAME}

        GTest::gtest
        GTest::gtest_main
)
