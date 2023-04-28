#include "test_chr_map.hpp"
#include "assert.hpp"

namespace RNES::PPU {

    TestCHRMap::TestCHRMap() : m_memory({0}) {

    }

    Word TestCHRMap::readWord(Address t_address) {
        ASSERT(t_address < 0x2000, "Invalid address");
        return m_memory[t_address];
    }

    void TestCHRMap::writeWord(Address t_address, Word t_word) {
        ASSERT(t_address < 0x2000, "Invalid address");
        m_memory[t_address] = t_word;
    }

}