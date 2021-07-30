#include "cpu/cpu.hpp"
#include "cpu/cpu_debugger.hpp"
#include "cpu_test_controller.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: <file>\n";
        return 1;
    }

    auto controller = std::make_unique<RNES::Test::CPUTestController>(argv[1]);
    RNES::CPU::CPU cpu(0x0400U);
    cpu.setController(std::move(controller));

    RNES::CPU::CPUDebugger debugger(cpu);
    debugger.start();

    return 0;
}
