#ifndef RNES_MAPPER0_INCLUDED
#define RNES_MAPPER0_INCLUDED

#include <array>
#include <vector>

#include "defines.hpp"
#include "mapper.hpp"

namespace RNES::Mapper {

    class CPUMapper0 : public CPU::CPUController {
    public:
        explicit CPUMapper0(std::vector<uint8_t> t_prgRom);
        ~CPUMapper0() override = default;

        [[nodiscard]] Word readWord(Address t_address) const override;
        void writeWord(Address t_address, Word t_value) override;

    private:
        const std::vector<uint8_t> m_prgRom;
        std::array<uint8_t, 0x2000> m_prgRam{};
    };

    class CHRMapper0 : public PPU::CHRMap {
    public:
        explicit CHRMapper0(std::array<Word, 0x2000> t_chrRom);

        Word readWord(Address t_address) override;
        void writeWord(Address t_address, Word t_value) override;
    private:
        std::array<Word, 0x2000> m_chrRom;
    };

    Mapper createMapper0(std::vector<uint8_t> t_prgRom, std::vector<uint8_t> t_chrRom);

}

#endif
