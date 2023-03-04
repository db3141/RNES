cmake_minimum_required(VERSION 3.16...3.26)

project(ReiNES VERSION 0.0.3 LANGUAGES CXX)

set(WARNING_FLAGS
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(SDL2 REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
