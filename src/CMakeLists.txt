add_library(core STATIC
    assert.hpp
    defines.hpp

    cpu/cpu.cpp
    cpu/cpu_instructions.cpp
    cpu/cpu.hpp

    cpu/cpu_controller.hpp

    cpu/cpu_debugger.cpp
    cpu/cpu_debugger.hpp

    ppu/ppu.cpp
    ppu/ppu.hpp

    ppu/ppu_controller.hpp
)

set_target_flags(core)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
