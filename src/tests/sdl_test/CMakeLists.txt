add_executable(sdl_test EXCLUDE_FROM_ALL
    main.cpp
)

set(SDL_TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/sdl_test)

set_target_properties(sdl_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${SDL_TEST_OUTPUT_DIR}
)

target_link_libraries(sdl_test core)
set_target_flags(sdl_test)
