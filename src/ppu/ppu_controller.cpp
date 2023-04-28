#include "assert.hpp"
#include "ppu_controller.hpp"

namespace RNES::PPU {

    PPUController::PPUController(std::unique_ptr<CHRMap> t_chrMap) : PPUController(std::move(t_chrMap), {0}, {0}) {

    }

    PPUController::PPUController(std::unique_ptr<CHRMap> t_chrMap, std::array<Word, 0x1000> m_internalVRam, std::array<Word, 0x20> t_paletteRamIndexes)
            : m_internalVRam(m_internalVRam), m_paletteRamIndexes(t_paletteRamIndexes), m_chrMap(std::move(t_chrMap)) {

    }

    Word PPUController::readWord(Address t_address) {
        if (t_address < 0x2000) {
            return m_chrMap->readWord(t_address);
        } else if (t_address < 0x3EFF) {
            // TODO: allow for different mirroring modes
            return m_internalVRam[(t_address - 0x2000) % 0x1000];
        } else if (t_address < 0x4000) {
            return m_paletteRamIndexes[(t_address - 0x3F00) % 0x20];
        } else {
            ASSERT(false, "Should not be here");
        }
    }

    void PPUController::writeWord(RNES::Address t_address, RNES::Word t_value) {
        if (t_address < 0x2000) {
            m_chrMap->writeWord(t_address, t_value);
        } else if (t_address < 0x3EFF) {
            // TODO: allow for different mirroring modes
            m_internalVRam[(t_address - 0x2000) % 0x1000] = t_value;
        } else if (t_address < 0x4000) {
            m_paletteRamIndexes[(t_address - 0x3F00) % 0x20] = t_value;
        }
    }

}
