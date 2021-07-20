#ifndef RNES_PPU_INCLUDED
#define RNES_PPU_INCLUDED

#include <array>

#include "ppu_controller.hpp"

namespace RNES {

    static const size_t OAM_SIZE = 256;
    
    class PPU {
    public:
        PPU();

        void setController(std::unique_ptr<PPUController> t_controller);

        void render();
    private:
        std::array<Word, OAM_SIZE> m_oam;
        std::unique_ptr<PPUController> m_controller;
    };

}

#endif
