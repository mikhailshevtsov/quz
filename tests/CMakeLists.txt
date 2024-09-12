cmake_minimum_required(VERSION 3.16)
project(quz_test)

set(TEST_SOURCES
    test_main.cpp)

add_executable(quz_test ${TEST_SOURCES})

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)

target_link_libraries(quz_test PRIVATE gtest_main gmock_main quz)
