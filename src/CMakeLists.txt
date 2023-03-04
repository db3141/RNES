add_library(core STATIC 
    assert.hpp
    defines.hpp

    cpu/cpu.hpp
    cpu/cpu_controller.hpp
    cpu/cpu_debugger.hpp
    cpu/cpu.cpp
    cpu/cpu_debugger.cpp
    cpu/cpu_instructions.cpp

    ppu/ppu_controller.hpp
    ppu/ppu.hpp
    ppu/surface_wrapper.hpp
    ppu/ppu.cpp
    ppu/surface_wrapper.cpp
)

# Use C++ 20 and disable extensions
target_compile_features(core PUBLIC cxx_std_20)
set_target_properties(core PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(core PRIVATE ${WARNING_FLAGS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core PRIVATE SDL2::SDL2)
