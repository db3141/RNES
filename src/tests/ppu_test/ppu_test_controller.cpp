#include "assert.hpp"
#include "ppu_test_controller.hpp"

namespace RNES {

    PPUTestController::PPUTestController() : m_memory() {
        ;
    }

    Word PPUTestController::readWord(Address t_address) {
        return m_memory[t_address];
    }

    void PPUTestController::writeWord(Address t_address, Word t_value) {
        m_memory[t_address] = t_value;
    }

    void PPUTestController::sendVBlankNMI() {
        ASSERT(false, "Not implemented");
    }

}
