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

    ppu/surface_wrapper.cpp
    ppu/surface_wrapper.hpp
)

set_target_flags(core)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(core PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(core PUBLIC ${SDL2_LIBRARIES})

add_subdirectory(tests)
