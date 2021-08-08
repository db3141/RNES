#ifndef RNES_PPU_INCLUDED
#define RNES_PPU_INCLUDED

#include <array>
#include <memory>

#include "defines.hpp"
#include "ppu_controller.hpp"
#include "surface_wrapper.hpp"

namespace RNES::PPU {

    static const size_t OAM_SIZE = 256;
    static const size_t OAM_SECONDARY_SIZE = 32;

    static const size_t OUTPUT_WIDTH = 256;
    static const size_t OUTPUT_HEIGHT = 240;

    class PPU {
    public:
        PPU();
        PPU(const char* t_oamFile);

        void cycle();

        uint8_t readPPUStatus();
        uint8_t readOAMData();
        uint8_t readPPUData();

        void writePPUCTRL(uint8_t t_value);
        void writePPUMask(uint8_t t_value);
        void writeOAMAddress(uint8_t t_value);
        void writeOAMData(uint8_t t_value);
        void writePPUScroll(uint8_t t_value);
        void writePPUAddress(uint8_t t_value);
        void writePPUData(uint8_t t_value);

        void writeOAMByte(uint8_t t_index, uint8_t t_value);

        void setController(std::unique_ptr<PPUController> t_controller);

        [[nodiscard]] SDL_Surface* getScreenOutput();
    private:
        std::array<Word, OAM_SIZE> m_oam;
        std::array<Word, OAM_SECONDARY_SIZE> m_secondaryOAM;
        std::unique_ptr<PPUController> m_controller;

        struct {
            bool vBlank;
            bool render;
        } m_flags;

        struct {
            uint8_t ppuCtrl;
            uint8_t ppuMask;
            uint8_t ppuStatus;
            uint8_t oamAddr;
            uint8_t oamData;
            uint8_t ppuScrollX;
            uint8_t ppuScrollY;
            uint16_t ppuAddr;
            uint8_t ppuData;

            /* Diagram modified from https://wiki.nesdev.com/w/index.php?title=PPU_scrolling
             *
             *  _yyy NN YYYYY XXXXX
             *   ||| || ||||| +++++-- coarse X scroll
             *   ||| || +++++-------- coarse Y scroll
             *   ||| ++-------------- nametable select
             *   +++----------------- fine Y scroll
             *
             */
            uint16_t v; // Current VRAM address
            uint16_t t; // Temporary VRAM address
            uint8_t x; // Fine x scroll
            bool w; // First or second write toggle
        } m_registers;


        enum class SpriteEvaluationCase {
            CASE_1,
            CASE_1A_1,
            CASE_1A_2,
            CASE_1A_3,

            CASE_2,

            CASE_3,
            CASE_3A_1,
            CASE_3A_2,
            CASE_3A_3,

            CASE_4,
        };

        struct {
            SpriteEvaluationCase eCase;
            bool writeFlag; // register for when secondary OAM can be written to
            uint8_t temp; // temporary register for copying to secondary OAM
            uint8_t n, m; // index registers
            uint8_t secondaryOAMCount; // number of elements written to OAM
        } m_spriteEvaluationState;

        struct SpriteData {
            uint8_t x, y;
            uint8_t tileIndex;
            uint8_t attributes;
        };
        std::array<SpriteData, 8> m_scanlineSprites; // sprite data of sprites to be drawn on the next scanline

        size_t m_lastUpdatedCycle;
        size_t m_currentCycle;

        SurfaceWrapper m_outputSurface;

        //----- Methods -----//
        void spriteEvaluationMain(size_t t_scanline, size_t t_scanlineCycle);
        void writeSecondaryOAM(size_t t_address, uint8_t t_value);
        uint8_t readOAMWord(size_t t_n, size_t t_m);
    };

}

#endif
