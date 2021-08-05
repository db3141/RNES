#ifndef RNES_PPU_INCLUDED
#define RNES_PPU_INCLUDED

#include <array>
#include <memory>

#include "defines.hpp"
#include "ppu_controller.hpp"
#include "surface_wrapper.hpp"

namespace RNES::PPU {

    static const size_t OAM_SIZE = 256;

    static const size_t OUTPUT_WIDTH = 256;
    static const size_t OUTPUT_HEIGHT = 240;

    class PPU {
    public:
        PPU();

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

        size_t m_lastUpdatedCycle;
        size_t m_currentCycle;

        SurfaceWrapper m_outputSurface;
    };

}

#endif
