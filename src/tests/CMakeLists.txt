set(TEST_SUBDIRECTORIES
    cpu_test
    ppu_test
)

foreach(dir ${TEST_SUBDIRECTORIES})
    add_subdirectory(${dir})
endforeach()
