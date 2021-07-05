add_library(core STATIC
    cpu/cpu.cpp
    cpu/cpu_instructions.cpp
    cpu/cpu.hpp

    cpu/cpu_debugger.cpp
    cpu/cpu_debugger.hpp

    memory/ram.cpp
    memory/ram.hpp
    memory/memory.hpp
)

message("${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(frontend)
