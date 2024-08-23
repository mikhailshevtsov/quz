cmake_minimum_required(VERSION 3.16)
project(quz)

set(SOURCES
    quz.hpp
    quz_index.hpp
    quz_string.hpp
)

add_library(quz INTERFACE ${SOURCES})
target_include_directories(quz INTERFACE .)