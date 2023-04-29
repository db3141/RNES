#include "nes_controller.hpp"

namespace RNES {

    NESController::NESController(std::unique_ptr<CPU::CPUMemoryMap> t_cpuMapper)
            : m_internalRAM({0}), m_cpuMapper(std::move(t_cpuMapper)) {
        ;
    }

    Word NESController::readWord(RNES::Address t_address) const {
        if (t_address < 0x2000) {
            return m_internalRAM[t_address % 0x0800]; // first 0x0800 bytes are mirrored
        } else if (t_address < 0x4000) {
            // TODO: PPU registers
            return -1;
        } else if (t_address < 0x4018) {
            // TODO: APU and IO registers
            return -1;
        } else if (t_address < 0x4020) {
            // TODO: other APU and IO stuff
            return -1;
        } else {
            return m_cpuMapper->readWord(t_address);
        }
    }

    void NESController::writeWord(Address t_address, Word t_value) {
        if (t_address < 0x2000) {
            m_internalRAM[(t_address % 0x0800)] = t_value;
        } else if (t_address < 0x4000) {
            // TODO: PPU registers
        } else if (t_address < 0x4018) {
            // TODO: APU and IO registers
        } else if (t_address < 0x4020) {
            // TODO: other APU and IO stuff
        } else {
            m_cpuMapper->writeWord(t_address, t_value);
        }
    }

}
