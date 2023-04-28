add_library(core STATIC
        assert.hpp
        defines.hpp
        error_or.hpp

        cpu/cpu.hpp
        cpu/cpu_controller.hpp
        cpu/cpu_debugger.hpp
        cpu/cpu.cpp
        cpu/cpu_debugger.cpp
        cpu/cpu_instructions.cpp
        cpu/nes_controller.hpp
        cpu/nes_controller.cpp

        ppu/ppu_controller.hpp
        ppu/ppu.hpp
        ppu/surface_wrapper.hpp
        ppu/ppu.cpp
        ppu/surface_wrapper.cpp

        mapper/mapper.hpp
        mapper/mapper.cpp
        mapper/mapper0.cpp ppu/ppu_controller.cpp ppu/chr_map.hpp mapper/mapper0.hpp)

# Use C++ 20 and disable extensions
target_compile_features(core PUBLIC cxx_std_20)
set_target_properties(core PROPERTIES CXX_EXTENSIONS ON)

target_compile_options(core PRIVATE ${WARNING_FLAGS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core PRIVATE SDL2::SDL2)

# Actual program target
add_executable(app
        app/main.cpp
        )

target_compile_features(app PUBLIC cxx_std_20)
set_target_properties(app PROPERTIES CXX_EXTENSIONS ON)

target_compile_options(app PRIVATE ${WARNING_FLAGS})
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(app PRIVATE SDL2::SDL2)
target_link_libraries(app PRIVATE core)

