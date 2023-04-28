#ifndef RNES_CHR_MAP_INCLUDED
#define RNES_CHR_MAP_INCLUDED

#include "defines.hpp"

namespace RNES::PPU {

    class CHRMap {
    public:
        virtual ~CHRMap() = default;

        virtual Word readWord(Address t_address) = 0;
        virtual void writeWord(Address t_address, Word t_value) = 0;
    };

}


#endif
