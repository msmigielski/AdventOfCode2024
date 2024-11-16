cmake_minimum_required(VERSION 3.22)

project(AdventOfCode2024)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(third_party)

include(build_config)
enable_testing()

add_subdirectory(tasks)
