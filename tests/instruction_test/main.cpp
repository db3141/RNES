#include <iostream>
#include <iomanip>
#include <string>

#include <M6502/cpu/cpu.hpp>
#include <M6502/cpu/debugger/cpu_debugger.hpp>
#include <M6502/ram/ram.hpp>

int main(int argc, char* argv[]) {
    auto ramController = std::make_shared<Emulator::RAM>("tests/6502_functional_test.bin");
    Emulator::CPU cpu(ramController, 0x0400U);

    Emulator::CPUDebugger debugger(cpu);
    debugger.start();

    return 0;
}
