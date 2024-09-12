cmake_minimum_required(VERSION 3.16)
project(quz)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    "quz/meta_struct.hpp"
    "quz/index.hpp"
    "quz/string.hpp"
    "quz/preprocessor.hpp"
)

add_library(quz INTERFACE ${SOURCES})
target_include_directories(quz INTERFACE "quz/")
target_compile_features(quz INTERFACE cxx_std_20)

add_subdirectory(tests/)