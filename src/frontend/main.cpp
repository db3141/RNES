#include "cpu/cpu.hpp"
#include "cpu/cpu_debugger.hpp"
#include "memory/ram.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: <file>\n";
        return 1;
    }

    auto ramController = std::make_shared<RNES::RAM>(argv[1]);
    RNES::CPU cpu(ramController, 0x0400U);

    RNES::CPUDebugger debugger(cpu);
    debugger.start();

    return 0;
}
