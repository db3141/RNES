#include <iostream>
#include <iomanip>
#include <string>

#include "cpu.hpp"
#include "ram.hpp"

int main(int argc, char* argv[]) {
    auto ramController = std::make_shared<Emulator::RAM>("6502_functional_test.bin");
    Emulator::CPU cpu(ramController, 0x0400U);

    Emulator::cpuDebug(cpu);

    return 0;
}
