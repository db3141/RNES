cmake_minimum_required(VERSION 3.16)

project(RNES)

set(CMAKE_CXX_STANDARD 20)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 0)
set(${PROJECT_NAME}_VERSION_PATCH 0)

set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENABLED_WARNINGS "-Wall" "-Wextra" "-Wpedantic")
add_compile_options(${ENABLED_WARNINGS})
option(WARNINGS_AS_ERRORS ON)

if(WARNINGS_AS_ERRORS)
    add_compile_options("-Werror")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options("-O3")
else()
    message("Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

add_subdirectory("src")
