#include "assert.hpp"
#include "cpu/cpu_controller.hpp"
#include "ppu/ppu_controller.hpp"

#include <array>
#include <utility>
#include <vector>

namespace RNES::Mapper {
    class CPUMapper0 : public CPU::CPUController {
    public:
        CPUMapper0(std::vector<uint8_t> t_prgRom, bool t_isFamilyBasic);
        ~CPUMapper0() override = default;

        [[nodiscard]] Word readWord(Address t_address) const override;
        void writeWord(Address t_address, Word t_value) override;

    private:
        const std::vector<uint8_t> m_prgRom;
        const bool m_isFamilyBasic;
        std::array<uint8_t, 0x2000> m_prgRam{};
    };

    class PPUMapper0 : public PPU::PPUController {
    public:

    private:

    };

    CPUMapper0::CPUMapper0(std::vector<uint8_t> t_prgRom, bool t_isFamilyBasic)
            : m_prgRom(std::move(t_prgRom)), m_isFamilyBasic(t_isFamilyBasic), m_prgRam({0}) {

        ASSERT(m_prgRom.size() == 0x2000 || m_prgRom.size() == 0x4000, "Invalid PRG-ROM size");
    }

    RNES::Word CPUMapper0::readWord(RNES::Address t_address) const {
        ASSERT(t_address >= 0x6000, "Invalid Address");
        if (t_address < 0x8000) {
            return (m_isFamilyBasic) ? (m_prgRam[t_address - 0x6000]) : (0);
        }

        return m_prgRom[(t_address - 0x8000) % m_prgRom.size()];
    }

    void CPUMapper0::writeWord(RNES::Address t_address, RNES::Word t_value) {
        ASSERT(t_address >= 0x6000, "Invalid Address");
        if (t_address < 0x8000 && m_isFamilyBasic) {
            m_prgRam[t_address - 0x6000] = t_value;
        }
    }

}
