#include <iostream>

#include "ppu/ppu.hpp"
#include "ppu_test_controller.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: <file>\n";
        return 1;
    }

    auto ppuController = std::make_unique<RNES::PPUTestController>(argv[1]);
    RNES::PPU ppu;
    ppu.setController(std::move(ppuController));
    ppu.render();

    return 0;
}
