#include <iostream>

#include "assert.hpp"
#include "ppu.hpp"

namespace RNES {

    static const size_t TILE_WIDTH = 8;
    static const size_t TILE_HEIGHT = 8;
    static const size_t TILE_SIZE = TILE_WIDTH * TILE_HEIGHT;

    static const size_t PATTERN_TABLE_TILE_COUNT = 512;

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

        struct Tile {
            std::array<uint8_t, TILE_SIZE> colors;

            uint8_t getColor(size_t t_row, size_t t_column) {
                return colors[TILE_WIDTH * t_row + t_column];
            }
        };    

        std::array<Tile, PATTERN_TABLE_TILE_COUNT> tiles;

        for (size_t i = 0; i < tiles.size(); i++) {
            Tile tile;

            for (size_t j = 0; j < TILE_HEIGHT; j++) {
                const size_t lowByte = m_controller->readWord(16 * i + j);
                const size_t highByte = m_controller->readWord(16 * i + j + 8);

                for (size_t k = 0; k < TILE_WIDTH; k++) {
                    const uint8_t lowBit = (lowByte >> (7 - k)) & 0b00000001;
                    const uint8_t highBit = (highByte >> (7 - k)) & 0b00000001;

                    const uint8_t color = (highBit << 1) | lowBit;
                    tile.colors[8*j + k] = color;
                }
            }
            tiles[i] = tile;
        }

        for (size_t i = 0; i < tiles.size(); i++) {
            for (size_t r = 0; r < TILE_HEIGHT; r++) {
                for (size_t c = 0; c < TILE_WIDTH; c++) {
                    switch (tiles[i].getColor(r, c)) {
                    case 0:
                        std::cout << "  ";
                        break;
                    case 1:
                        std::cout << "\u2591\u2591";
                        break;
                    case 2:
                        std::cout << "\u2592\u2592";
                        break;
                    case 3:
                        std::cout << "\u2593\u2593";
                        break;
                    default:
                        ASSERT(false, "shouldn't be here");
                        break;
                    }
                }
                std::cout << '\n';
            }
            std::cout << "----------------\n";
        }

    }

}
