cmake_minimum_required(VERSION 3.16)

project(RNES)

set(CMAKE_CXX_STANDARD 20)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 0)
set(${PROJECT_NAME}_VERSION_PATCH 0)

set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

set(ENABLED_WARNINGS "-Wall -Wextra -Wpedantic")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_BUILD_TYPE EQUAL "Debug")
    add_compile_options(${ENABLED_WARNINGS})
elseif(CMAKE_BUILD_TYPE EQUAL "Release")
    add_compile_options("-O3 ${ENABLED_WARNINGS}")
else()
    message("Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

add_subdirectory("src")
