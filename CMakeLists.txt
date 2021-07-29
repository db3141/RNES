cmake_minimum_required(VERSION 3.16)

project(RNES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Dependencies
find_package(SDL2 REQUIRED)


# Build configurations
set(ALLOWABLE_BUILD_TYPES "Debug" "Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${ALLOWABLE_BUILD_TYPES})
set(DEFAULT_BUILD_TYPE Debug)

set(DEBUG_FLAGS "-Og")
set(RELEASE_FLAGS "-O3")

option(USE_ADDRESS_SANITIZER ON)
option(USE_UB_SANITIZER ON)
option(USE_THREAD_SANITIZER OFF)

if(NOT GENERATOR_IS_MULTI_CONFIG)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "No build type provided. Defaulting to ${DEFAULT_BUILD_TYPE}")
        set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "" FORCE)
    endif()

    if(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWABLE_BUILD_TYPES)
        message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not a valid build type")
    endif()
endif()

set(ENABLED_WARNINGS "-Wall" "-Wextra" "-Wpedantic")
option(WARNINGS_AS_ERRORS ON)

if(WARNINGS_AS_ERRORS)
    message(STATUS "Warnings will be treated as errors")
    list(APPEND ENABLED_WARNINGS "-Werror")
endif()


# Sets the corresponding build type flags for the target
function(set_target_flags target)
    message(STATUS "Adding flags to target '${target}'")
    message(STATUS "Enabled warnings: ${ENABLED_WARNINGS}")
    target_compile_options(${target} PRIVATE 
        $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${RELEASE_FLAGS}>
    )
    target_compile_options(${target} PRIVATE ${ENABLED_WARNINGS})

    if(USE_ADDRESS_SANITIZER)
        target_compile_options(${target} PRIVATE "-fsanitize=address")
        target_link_options(${target} PRIVATE "-fsanitize=address")
    endif()

    if(USE_UB_SANITIZER)
        target_compile_options(${target} PRIVATE "-fsanitize=undefined")
        target_link_options(${target} PRIVATE "-fsanitize=undefined")
    endif()

    if(USE_THREAD_SANITIZER)
        target_compile_options(${target} PRIVATE "-fsanitize=thread")
        target_link_options(${target} PRIVATE "-fsanitize=thread")
    endif()

endfunction()

add_subdirectory("src")
