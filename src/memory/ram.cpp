#include <fstream>

#include "assert.hpp"
#include "ram.hpp"

namespace RNES {

    RAM::RAM() : m_ram() {
        ;
    }

    RAM::RAM(const char* t_path) : m_ram() {
        std::ifstream fileStream(t_path, std::ios::binary | std::ios::in);
        ASSERT(fileStream.is_open(), "TODO: change this");
        fileStream.read(reinterpret_cast<char*>(m_ram.data()), ADDRESS_SPACE_SIZE);
    }

    Word RAM::readWord(Address t_address) const {
        return m_ram[t_address];
    }

    void RAM::writeWord(Address t_address, Word t_value) {
        m_ram[t_address] = t_value;
    }

    DWord RAM::readDWord(Address t_address) const {
        return (m_ram[t_address + 0U] << 0U) |
                (m_ram[t_address + 1U] << 8U);
    }

    void RAM::writeDWord(Address t_address, DWord t_value) {
        m_ram[t_address + 0U] = (t_value >> 0U) & 0xFF;
        m_ram[t_address + 1U] = (t_value >> 8U) & 0xFF;
    }

}
