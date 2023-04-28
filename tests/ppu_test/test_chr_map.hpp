#ifndef RNES_TEST_CHR_MAP_HPP
#define RNES_TEST_CHR_MAP_HPP

#include <array>

#include "ppu/chr_map.hpp"

namespace RNES::PPU {

    class TestCHRMap : public CHRMap {
    public:
        TestCHRMap();

        Word readWord(Address t_address) override;
        void writeWord(Address t_address, Word t_word) override;

    private:
        std::array<Word, 0x2000> m_memory;
    };

}


#endif
