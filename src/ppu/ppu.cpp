#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "assert.hpp"
#include "ppu.hpp"

namespace RNES::PPU {

    static const size_t TILE_WIDTH = 8;
    static const size_t TILE_HEIGHT = 8;
    static const size_t TILE_SIZE = TILE_WIDTH * TILE_HEIGHT;

    static const size_t PATTERN_TABLE_TILE_COUNT = 512;

    static const std::array<RGBAPixel, 64> PALETTE_MAP = {{
        { 0x52, 0x52, 0x52, 0xff },
        { 0x1, 0x1a, 0x51, 0xff },
        { 0xf, 0xf, 0x65, 0xff },
        { 0x23, 0x6, 0x63, 0xff },
        { 0x36, 0x3, 0x4b, 0xff },
        { 0x40, 0x4, 0x26, 0xff },
        { 0x3f, 0x9, 0x4, 0xff },
        { 0x32, 0x13, 0x0, 0xff },
        { 0x1f, 0x20, 0x0, 0xff },
        { 0xb, 0x2a, 0x0, 0xff },
        { 0x0, 0x2f, 0x0, 0xff },
        { 0x0, 0x2e, 0xa, 0xff },
        { 0x0, 0x26, 0x2d, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0xa0, 0xa0, 0xa0, 0xff },
        { 0x1e, 0x4a, 0x9d, 0xff },
        { 0x38, 0x37, 0xbc, 0xff },
        { 0x58, 0x28, 0xb8, 0xff },
        { 0x75, 0x21, 0x94, 0xff },
        { 0x84, 0x23, 0x5c, 0xff },
        { 0x82, 0x2e, 0x24, 0xff },
        { 0x6f, 0x3f, 0x0, 0xff },
        { 0x51, 0x52, 0x0, 0xff },
        { 0x31, 0x63, 0x0, 0xff },
        { 0x1a, 0x6b, 0x5, 0xff },
        { 0xe, 0x69, 0x2e, 0xff },
        { 0x10, 0x5c, 0x68, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0xfe, 0xff, 0xff, 0xff },
        { 0x69, 0x9e, 0xfc, 0xff },
        { 0x89, 0x87, 0xff, 0xff },
        { 0xae, 0x76, 0xff, 0xff },
        { 0xce, 0x6d, 0xf1, 0xff },
        { 0xe0, 0x70, 0xb2, 0xff },
        { 0xde, 0x7c, 0x70, 0xff },
        { 0xc8, 0x91, 0x3e, 0xff },
        { 0xa6, 0xa7, 0x25, 0xff },
        { 0x81, 0xba, 0x28, 0xff },
        { 0x63, 0xc4, 0x46, 0xff },
        { 0x54, 0xc1, 0x7d, 0xff },
        { 0x56, 0xb3, 0xc0, 0xff },
        { 0x3c, 0x3c, 0x3c, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0xfe, 0xff, 0xff, 0xff },
        { 0xbe, 0xd6, 0xfd, 0xff },
        { 0xcc, 0xcc, 0xff, 0xff },
        { 0xdd, 0xc4, 0xff, 0xff },
        { 0xea, 0xc0, 0xf9, 0xff },
        { 0xf2, 0xc1, 0xdf, 0xff },
        { 0xf1, 0xc7, 0xc2, 0xff },
        { 0xe8, 0xd0, 0xaa, 0xff },
        { 0xd9, 0xda, 0x9d, 0xff },
        { 0xc9, 0xe2, 0x9e, 0xff },
        { 0xbc, 0xe6, 0xae, 0xff },
        { 0xb4, 0xe5, 0xc7, 0xff },
        { 0xb5, 0xdf, 0xe4, 0xff },
        { 0xa9, 0xa9, 0xa9, 0xff },
        { 0x0, 0x0, 0x0, 0xff },
        { 0x0, 0x0, 0x0, 0xff }
    }};

    PPU::PPU()
        : m_oam()
        , m_secondaryOAM()
        , m_controller(nullptr)

        , m_flags({ true, true })
        , m_registers({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })
        , m_spriteEvaluationState({ SpriteEvaluationCase::CASE_1, true, 0, 0, 0, 0 })
        , m_scanlineSprites()

        , m_lastUpdatedCycle(0)
        , m_currentCycle(0)

        , m_outputSurface(OUTPUT_WIDTH, OUTPUT_HEIGHT)
    {
        std::fill(m_scanlineSprites.begin(), m_scanlineSprites.end(), SpriteData{ 0xFF, 0xFF, 0xFF, 0xFF });
    }

    PPU::PPU(const char* t_oamFile) : PPU() {
        std::ifstream fileStream(t_oamFile, std::ios::in | std::ios::binary);
        ASSERT(fileStream.is_open(), "Failed to open");
        fileStream.read(reinterpret_cast<char*>(m_oam.data()), OAM_SIZE);
    }

    void PPU::setController(std::unique_ptr<PPUController> t_controller) {
        m_controller = std::move(t_controller);
    }

    void PPU::cycle() {
        const size_t scanline = m_currentCycle / 342;
        const size_t scanlineCycle = m_currentCycle % 342;

        if (scanline <= 239 || scanline == 261) {
            if (scanlineCycle == 0) {
                ;
            }
            else if (scanlineCycle <= 256) {
                //----- Sprite evaluation -----//
                if (scanlineCycle == 1) {
                    std::fill(m_secondaryOAM.begin(), m_secondaryOAM.end(), 0xFF);

                    // Reset sprite evaluation variables
                    m_spriteEvaluationState = { SpriteEvaluationCase::CASE_1, true, 0, 0, 0, 0 };

                    /*
                    std::cout << scanline << '\n';
                    for (size_t i = 0; i < 8; i++) {
                        std::cout << i << ": " << int(m_scanlineSprites[i].x) << ", " << int(m_scanlineSprites[i].y) << ", " << int(m_scanlineSprites[i].tileIndex) << ", " << int(m_scanlineSprites[i].attributes) << '\n';
                    }
                    std::cout << '\n';
                    //*/
                }
                else if (scanlineCycle >= 65) {
                    spriteEvaluationMain(scanline, scanlineCycle);
                }

                //----- Background -----//
                uint16_t& v = m_registers.v;
                uint16_t& t = m_registers.t;

                const size_t coarseXScroll = (v >> 0) & 0x001F;
                const size_t fineXScroll = m_registers.x;
                const size_t coarseYScroll = (v >> 5) & 0x001F;
                const size_t fineYScroll = (v >> 12) & 0x0007;

                const size_t nametableIndex = coarseYScroll * 32 + coarseXScroll;
                const uint16_t nametableAddress = 0x2000; // TODO: change this
                const uint8_t patternTableIndex = m_controller->readWord(nametableAddress + nametableIndex);

                const size_t tileXCoord = (fineXScroll + (scanlineCycle - 1)) % 8;
                const size_t tileYCoord = fineYScroll;

                const size_t paletteColourIndexLowBit = (m_controller->readWord(0x1000 + 16 * patternTableIndex + tileYCoord) >> (7 - tileXCoord)) & 1; // TODO: change the base address
                const size_t paletteColourIndexHighBit = (m_controller->readWord(0x1000 + 16 * patternTableIndex + tileYCoord + 8) >> (7 - tileXCoord)) & 1;
                const size_t paletteColourIndex = (paletteColourIndexHighBit << 1) | (paletteColourIndexLowBit << 0);

                const size_t attributeTableAddress = 0x23C0;
                const size_t attributeTableX = (scanlineCycle - 1) / 32;
                const size_t attributeTableY = scanline / 32;

                const size_t attributeTableHorizontal = ((scanlineCycle - 1) % 32) / 16; // get horizontal quadrant
                const size_t attributeTableVertical = (scanline % 32) / 16; // get vertical quadrant

                const size_t attributeTableQuadrant = 2 * attributeTableVertical + attributeTableHorizontal;
                ASSERT(attributeTableQuadrant < 4, "Out of range");

                const size_t paletteIndex = (m_controller->readWord(attributeTableAddress + 8 * attributeTableY + attributeTableX) >> (2 * attributeTableQuadrant)) & 0x03;
                ASSERT(paletteIndex < PALETTE_MAP.size(), "Out of range");

                if (scanline <= 240) {
                    RGBAPixel backgroundColour = { 0, 0, 0, 0 };
                    if (paletteColourIndex == 0) {
                        backgroundColour = PALETTE_MAP[m_controller->readWord(0x3F00)];
                    }
                    else {
                        backgroundColour = PALETTE_MAP[m_controller->readWord(0x3F00 + paletteColourIndex + 4 * paletteIndex)];
                    }

                    RGBAPixel foregroundColour = { 0, 0, 0, 0 };
                    // Loop backwards so sprite 0 is on top
                    for (int i = 7; i >= 1; i--) {
                        
                    }

                    m_outputSurface.setPixel(scanlineCycle - 1, scanline, backgroundColour.r, backgroundColour.g, backgroundColour.b, 0xFF);
                }


                // Increment coarse X scroll
                if (scanlineCycle % 8 == 0 && scanlineCycle != 0) {
                    if (coarseXScroll == 31) {
                        v &= ~0x001F; // set coarse x to 0
                        v ^= 0x0400; // switch horizontal nametable
                    }
                    else {
                        v++; // Increment coarse x (last 5 bits <= 30 so if we add 1 we at most get 31 which doesn't overflow into the next bits)
                    }
                }

                if (m_flags.render) {
                    // Increment Y scroll on cycle 256
                    if (scanlineCycle == 256) {
                        // Taken from https://wiki.nesdev.com/w/index.php?title=PPU_scrolling
                        if ((v & 0x7000) != 0x7000) {
                            v += 0x1000; // increment fine Y scroll
                        }
                        else {
                            v &= ~0x7000; // set fine Y to 0
                            int y = (v & 0x03E0) >> 5; // let y = coarse Y

                            if (y == 29) {
                                y = 0; // set coarse Y to 0
                                v ^= 0x0800; // switch vertical nametable
                            }
                            else if (y == 31) {
                                y = 0; // set coarse Y to 0
                            }
                            else {
                                y++; // increment coarse Y
                            }
                            v = (v & ~0x03E0) | (y << 5); // put coarse Y back into v
                        }
                    }
                    else if (scanlineCycle == 257) {
                        v = (v & 0xFBE0) | (t & ~0xFBE0); // copy X scroll from t to v
                    }

                    if (scanline == 261 && (280 <= scanlineCycle && scanlineCycle <= 304)) {
                        v = (v & 0x841F) | (t & ~0x841F); // copy horizontal bits from t to v
                    }
                }

            }
            else if (scanlineCycle <= 320) {
                //----- Background -----//

                //----- Sprite Evaluation -----//
                const size_t sprite = (scanlineCycle - 257) / 8;
                const size_t spriteCycle = (scanlineCycle - 257) % 8;
                switch (spriteCycle) {
                    case 0:
                        m_scanlineSprites[sprite].y = m_secondaryOAM[4 * sprite + 0];
                        break;
                    case 1:
                        m_scanlineSprites[sprite].tileIndex = m_secondaryOAM[4 * sprite + 1];
                        break;
                    case 2:
                        m_scanlineSprites[sprite].attributes = m_secondaryOAM[4 * sprite + 2];
                        break;
                    case 3:
                        m_scanlineSprites[sprite].x = m_secondaryOAM[4 * sprite + 3];
                        break;
                    default:
                        break;
                }
            }
            else if (scanlineCycle <= 336) {
                // TODO
            }
            else if (scanlineCycle <= 340) {
                // TODO
            }
        }
        else if (scanline == 240) {
            ;
        }
        else if (scanline == 241) {
            if (scanlineCycle == 1) {
                m_flags.vBlank = true;
                m_controller->sendVBlankNMI();
            }
        }
        //*
        for (size_t i = 0; i < 8; i++) {
            std::cout << std::hex << std::setw(2) << '(' << int(m_secondaryOAM[4*i + 0]) << ", " << int(m_secondaryOAM[4*i + 1]) << ", " << int(m_secondaryOAM[4*i + 2]) << ", " << int(m_secondaryOAM[4*i + 3]) << ") ";
        }
        std::cout << '\n';
        //*/

        m_currentCycle++;
    }

    void PPU::spriteEvaluationMain(size_t t_scanline, size_t t_scanlineCycle) {

        switch (m_spriteEvaluationState.eCase) {
            case SpriteEvaluationCase::CASE_1:
                if (t_scanlineCycle % 2 == 1) {
                    m_spriteEvaluationState.temp = readOAMWord(m_spriteEvaluationState.n, 0);
                }
                else if (m_spriteEvaluationState.secondaryOAMCount < 8) {
                    writeSecondaryOAM(4 * m_spriteEvaluationState.secondaryOAMCount, m_spriteEvaluationState.temp);

                    const uint8_t y = m_spriteEvaluationState.temp;
                    if (y <= t_scanline && t_scanline <= y + 8) {
                        m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_1A_1;
                    }
                    else {
                        m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_2;
                    }
                }
                else {
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_2;
                }
                break;

            case SpriteEvaluationCase::CASE_1A_1:
                if (t_scanlineCycle % 2 == 1) {
                    m_spriteEvaluationState.temp = readOAMWord(m_spriteEvaluationState.n, 1);
                }
                else {
                    writeSecondaryOAM(4 * m_spriteEvaluationState.secondaryOAMCount + 1, m_spriteEvaluationState.temp);
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_1A_2;
                }
                break;

            case SpriteEvaluationCase::CASE_1A_2:
                if (t_scanlineCycle % 2 == 1) {
                    m_spriteEvaluationState.temp = readOAMWord(m_spriteEvaluationState.n, 2);
                }
                else {
                    writeSecondaryOAM(4 * m_spriteEvaluationState.secondaryOAMCount + 2, m_spriteEvaluationState.temp);
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_1A_3;
                }
                break;

            case SpriteEvaluationCase::CASE_1A_3:
                if (t_scanlineCycle % 2 == 1) {
                    m_spriteEvaluationState.temp = readOAMWord(m_spriteEvaluationState.n, 3);
                }
                else {
                    writeSecondaryOAM(4 * m_spriteEvaluationState.secondaryOAMCount + 3, m_spriteEvaluationState.temp);
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_2;
                    m_spriteEvaluationState.secondaryOAMCount++;
                }
                break;

            case SpriteEvaluationCase::CASE_2:
                m_spriteEvaluationState.n = (m_spriteEvaluationState.n + 1) % 64;
                if (m_spriteEvaluationState.n == 0) {
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_4;
                }
                else if (m_spriteEvaluationState.secondaryOAMCount < 8) {
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_1;
                }
                else if (m_spriteEvaluationState.secondaryOAMCount == 8) {
                    m_spriteEvaluationState.writeFlag = false;
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3;
                }
                else {
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3;
                }
                break;

            case SpriteEvaluationCase::CASE_3:
                if (t_scanlineCycle % 2 == 1) {
                    const uint8_t y = readOAMWord(m_spriteEvaluationState.n, m_spriteEvaluationState.m);

                    if (y <= t_scanline && t_scanline <= y + 8) {
                        m_registers.ppuStatus |= 0x20; // set sprite overflow flag
                        m_spriteEvaluationState.m = (m_spriteEvaluationState.m + 1) % 4;
                        if (m_spriteEvaluationState.m == 0) {
                            m_spriteEvaluationState.n = (m_spriteEvaluationState.n + 1) % 64;
                        }
                        m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3A_1;
                    }
                    else {
                        m_spriteEvaluationState.n = (m_spriteEvaluationState.n + 1) % 64;
                        m_spriteEvaluationState.m = (m_spriteEvaluationState.m + 1) % 4;

                        if (m_spriteEvaluationState.n == 0) {
                            m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_4;
                        }
                        else {
                            m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3;
                        }
                    }
                }
                break;

            case SpriteEvaluationCase::CASE_3A_1:
                if (t_scanlineCycle % 2 == 1) {
                    readOAMWord(m_spriteEvaluationState.n, m_spriteEvaluationState.m); // unused read

                    m_spriteEvaluationState.m = (m_spriteEvaluationState.m + 1) % 4;
                    if (m_spriteEvaluationState.m == 0) {
                        m_spriteEvaluationState.n++;
                    }
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3A_2;
                }
                break;

            case SpriteEvaluationCase::CASE_3A_2:
                if (t_scanlineCycle % 2 == 1) {
                    readOAMWord(m_spriteEvaluationState.n, m_spriteEvaluationState.m); // unused read

                    m_spriteEvaluationState.m = (m_spriteEvaluationState.m + 1) % 4;
                    if (m_spriteEvaluationState.m == 0) {
                        m_spriteEvaluationState.n++;
                    }
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_3A_3;
                }
                break;

            case SpriteEvaluationCase::CASE_3A_3:
                if (t_scanlineCycle % 2 == 1) {
                    readOAMWord(m_spriteEvaluationState.n, m_spriteEvaluationState.m); // unused read

                    m_spriteEvaluationState.m = (m_spriteEvaluationState.m + 1) % 4;
                    if (m_spriteEvaluationState.m == 0) {
                        m_spriteEvaluationState.n++;
                    }
                    m_spriteEvaluationState.eCase = SpriteEvaluationCase::CASE_4;
                }
                break;

            case SpriteEvaluationCase::CASE_4:
                if (t_scanlineCycle % 2 == 1) {
                    readOAMWord(m_spriteEvaluationState.n, 0); // unused read
                    m_spriteEvaluationState.n++;
                }
                break;
        }
    }

    uint8_t PPU::readOAMWord(size_t t_n, size_t t_m) {
        //std::cout << "OAM[" << 4 * t_n + t_m << "] = " << std::hex << std::setw(2) << std::setfill('0') << int(m_oam[4 * t_n + t_m]) << '\n';
        return m_oam[4 * t_n + t_m];
    }

    void PPU::writeSecondaryOAM(size_t t_address, uint8_t t_value) {
        if (m_spriteEvaluationState.writeFlag) {
            m_secondaryOAM[t_address] = t_value;
        }
    }

    uint8_t PPU::readPPUStatus() {
        const uint8_t result = m_registers.ppuStatus;
        m_registers.ppuStatus &= 0x7F;
        m_registers.w = 0; // reset address latch
        return result;
    }

    uint8_t PPU::readOAMData() {
        const size_t scanlineCycle = m_currentCycle % 342;

        // When clearing secondary OAM return 0xFF
        if (1 <= scanlineCycle && scanlineCycle <= 64) {
            return 0xFF;
        }
        else {
            return m_oam[m_registers.oamAddr];
        }
    }

    uint8_t PPU::readPPUData() {
        const uint8_t result = m_controller->readWord(m_registers.v);
        if (m_registers.ppuCtrl & 0x04) {
            m_registers.v += 32;
        }
        else {
            m_registers.v += 1;
        }
        return result;
    }

    void PPU::writePPUCTRL(uint8_t t_value) {
        m_registers.ppuCtrl = t_value;
        m_registers.t = (m_registers.t & 0b11110011'11111111) | ((t_value & 0b00000011) << 10);
    }

    void PPU::writePPUMask(uint8_t t_value) {
        m_registers.ppuMask = t_value;
    }

    void PPU::writeOAMAddress(uint8_t t_value) {
        m_registers.oamAddr = t_value;
    }

    void PPU::writeOAMData(uint8_t t_value) {
        // TODO: check if rendering before writing
        m_oam[m_registers.oamAddr] = t_value;
        m_registers.oamAddr++;
    }

    void PPU::writePPUScroll(uint8_t t_value) {
        if (m_registers.w == 0) {
            m_registers.ppuScrollX = t_value;

            m_registers.t = (m_registers.t & 0b11111111'11100000) | ((t_value & 0b11111000) >> 3);
            m_registers.x = t_value & 0b00000111;
        }
        else {
            m_registers.ppuScrollY = t_value;

            m_registers.t = (m_registers.t & 0b11111100'00011111) | ((t_value & 0b11111000) << 2);
            m_registers.t = (m_registers.t & 0b10001111'11111111) | ((t_value & 0b00000111) << 12);
        }
        m_registers.w = !m_registers.w;
    }

    void PPU::writePPUAddress(uint8_t t_value) {
        if (m_registers.w == 0) {
            m_registers.ppuAddr = (m_registers.ppuAddr & 0x00FF) | (t_value << 8);

            m_registers.t = (m_registers.t & 0b01000000'11111111) | ((t_value & 0b00111111) << 8); // MSB gets reset here (this is not a bug)
        }
        else {
            m_registers.ppuAddr = (m_registers.ppuAddr & 0xFF00) | (t_value << 0);

            m_registers.t = (m_registers.t & 0b11111111'00000000) | ((t_value & 0b11111111) << 0);
            m_registers.v = m_registers.t;
        }
        m_registers.w = !m_registers.w;
    }

    void PPU::writePPUData(uint8_t t_value) {
        // TODO: set internal register here
        m_controller->writeWord(m_registers.v, t_value);
        if (m_registers.ppuCtrl & 0x04) {
            m_registers.v += 32;
        }
        else {
            m_registers.v += 1;
        }
    }

    void PPU::writeOAMByte(uint8_t t_index, uint8_t t_value) {
        m_oam[t_index] = t_value;
    }

    SDL_Surface* PPU::getScreenOutput() {
        return m_outputSurface.getUnderlyingSurface();
    }

}
