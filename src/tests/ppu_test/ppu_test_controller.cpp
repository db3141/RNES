#include <fstream>

#include "assert.hpp"
#include "ppu_test_controller.hpp"

namespace RNES {

    PPUTestController::PPUTestController() : m_memory() {
        ;
    }

    PPUTestController::PPUTestController(const char* t_file) : m_memory() {
        std::ifstream fileStream(t_file, std::ios::binary | std::ios::in);
        ASSERT(fileStream.is_open(), "Couldn't find file");
        fileStream.read(reinterpret_cast<char*>(m_memory.data()), PPU_ADDRESS_SPACE_SIZE);
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
