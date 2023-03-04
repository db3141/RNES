# CPU Test
add_executable(cpu_test
    cpu_test/controller.hpp

    cpu_test/controller.cpp
    cpu_test/main.cpp
)

target_link_libraries(cpu_test PRIVATE core)

# PPU Test
add_executable(ppu_test
    ppu_test/controller.hpp

    ppu_test/controller.cpp
    ppu_test/main.cpp
)

target_link_libraries(ppu_test PRIVATE core)
target_link_libraries(ppu_test PRIVATE SDL2::SDL2)

# SDL Test
add_executable(sdl_test
    sdl_test/main.cpp
)

target_link_libraries(sdl_test PRIVATE SDL2::SDL2)
