#ifndef RNES_PPU_CONTROLLER_INCLUDED
#define RNES_PPU_CONTROLLER_INCLUDED

#include "defines.hpp"

namespace RNES {

    class PPUController {
    public:
        virtual Word readWord(Address t_address) = 0;
        virtual void writeWord(Address t_address, Word t_value) = 0; 

        virtual void sendVBlankNMI() = 0;
    }

}

#endif
