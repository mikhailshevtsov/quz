cmake_minimum_required(VERSION 3.16)
project(quz)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    "meta_struct.hpp"
    "index.hpp"
    "string.hpp"
    "preprocessor.hpp"
)

add_library(quz INTERFACE ${SOURCES})
target_include_directories(quz INTERFACE .)

target_compile_features(quz INTERFACE cxx_std_20)

add_subdirectory(tests/)
target_link_libraries(quz_test PRIVATE quz)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)

target_link_libraries(quz_test PRIVATE gtest_main gmock_main)