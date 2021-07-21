add_executable(ppu_test EXCLUDE_FROM_ALL
    main.cpp
    
    ppu_test_controller.cpp
    ppu_test_controller.hpp
)

set(PPU_TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/ppu_test)

set_target_properties(ppu_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PPU_TEST_OUTPUT_DIR}
)

target_link_libraries(ppu_test core)
set_target_flags(ppu_test)

# Copy test binary to build directory
add_custom_command(TARGET ppu_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/ppu_dump.bin
    ${PPU_TEST_OUTPUT_DIR}/ppu_dump.bin
)
