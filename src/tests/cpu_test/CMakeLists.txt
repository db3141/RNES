add_executable(cpu_test EXCLUDE_FROM_ALL
    main.cpp
    
    cpu_test_controller.cpp
    cpu_test_controller.hpp
)

set(CPU_TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/cpu_test)

set_target_properties(cpu_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CPU_TEST_OUTPUT_DIR}
)

target_link_libraries(cpu_test core)
set_target_flags(cpu_test)

# Copy test binary to build directory
add_custom_command(TARGET cpu_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/6502_functional_test.bin
    ${CPU_TEST_OUTPUT_DIR}/6502_functional_test.bin
)
