#include <iostream>

#include "assert.hpp"
#include "ppu.hpp"

namespace RNES {

    PPU::PPU()
        : m_oam()
        , m_controller(nullptr)
    {
        ;
    }

    void PPU::setController(std::unique_ptr<PPUController> t_controller) {
        m_controller = std::move(t_controller);
    }

    void PPU::render() {
        // Draw pattern table
        for (size_t i = 0; i < 256; i++) {
            if (i % 16 == 0) {
                std::cout << '\n';
            }

            for (size_t j = 0; j < 8; j++) {
                const size_t lowByte = m_controller->readWord(16 * i + j);
                const size_t highByte = m_controller->readWord(16 * i + j + 8);

                for (size_t k = 0; k < 8; k++) {
                    const size_t colorIndex = ((lowByte >> (7 - k)) & 0x01) | ((highByte >> (6 - k)) & 0x02);
                    switch (colorIndex) {
                        case 0:
                            std::cout << ' ';
                            break;
                        case 1:
                            std::cout << "\u2591";
                            break;
                        case 2:
                            std::cout << "\u2592";
                            break;
                        case 3:
                            std::cout << "\u2593";
                            break;
                        default:
                            ASSERT(false, "shouldn't be here");
                            break;
                    }
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }
    }

}
