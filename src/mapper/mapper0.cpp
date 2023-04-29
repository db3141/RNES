#include "assert.hpp"
#include "mapper.hpp"
#include "mapper0.hpp"
#include "ppu/ppu_memory_map.hpp"

#include <array>
#include <utility>
#include <vector>

namespace RNES::Mapper {

    CPUMapper0::CPUMapper0(std::vector<uint8_t> t_prgRom)
            : m_prgRom(std::move(t_prgRom)), m_prgRam({0}) {

        ASSERT(m_prgRom.size() == 0x4000 || m_prgRom.size() == 0x8000, "Invalid PRG-ROM size");
    }

    RNES::Word CPUMapper0::readWord(RNES::Address t_address) const {
        ASSERT(t_address >= 0x6000, "Invalid Address");
        if (t_address < 0x8000) {
            return m_prgRam[t_address - 0x6000];
        }

        return m_prgRom[(t_address - 0x8000) % m_prgRom.size()];
    }

    void CPUMapper0::writeWord(RNES::Address t_address, RNES::Word t_value) {
        ASSERT(t_address >= 0x6000, "Invalid Address");
        m_prgRam[t_address - 0x6000] = t_value;
    }


    CHRMapper0::CHRMapper0(std::array<Word, 0x2000> t_chrRom) : m_chrRom(t_chrRom) {

    }

    Word CHRMapper0::readWord(Address t_address) {
        return m_chrRom[t_address];
    }

    void CHRMapper0::writeWord(Address t_address, Word t_value) {
        m_chrRom[t_address] = t_value;
    }


    Mapper createMapper0(std::vector<uint8_t> t_prgRom, std::vector<uint8_t> t_chrRom) {
        ASSERT(t_chrRom.size() == 0x2000, "Invalid CHR-ROM size");

        std::array<Word, 0x2000> chrRom{0};

        for (size_t i = 0; i < chrRom.size(); i++) {
            chrRom[i] = t_chrRom[i];
        }

        Mapper result{};

        result.cpuController = std::make_unique<CPUMapper0>(std::move(t_prgRom));
        result.ppuController = std::make_unique<PPU::PPUMemoryMap>(std::make_unique<CHRMapper0>(chrRom));

        return result;
    }

}
