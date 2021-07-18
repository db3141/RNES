set(FRONTEND_PROJECT RNES)

add_executable(${FRONTEND_PROJECT} main.cpp)
target_link_libraries(${FRONTEND_PROJECT} core)

set_target_flags(${FRONTEND_PROJECT})
