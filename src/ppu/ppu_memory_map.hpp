#ifndef RNES_PPU_CONTROLLER_INCLUDED
#define RNES_PPU_CONTROLLER_INCLUDED

#include <array>
#include <memory>

#include "defines.hpp"
#include "ppu/chr_map.hpp"

namespace RNES::PPU {

    class PPUMemoryMap {
    public:
        explicit PPUMemoryMap(std::unique_ptr<CHRMap> t_chrMap);
        PPUMemoryMap(std::unique_ptr<CHRMap> t_chrMap, std::array<Word, 0x1000> t_internalVRam, std::array<Word, 0x20> t_paletteRamIndexes);

        Word readWord(Address t_address);
        void writeWord(Address t_address, Word t_value);

    private:
        std::array<Word, 0x1000> m_internalVRam;
        std::array<Word, 0x20> m_paletteRamIndexes;
        std::unique_ptr<CHRMap> m_chrMap;
    };

}

#endif
