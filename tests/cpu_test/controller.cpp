#include <fstream>

#include "assert.hpp"
#include "controller.hpp"

namespace RNES::Test {

    CPUTestController::CPUTestController() : m_memory() {
        ;
    }

    CPUTestController::CPUTestController(const char* t_path) : m_memory() {
        std::ifstream fileStream(t_path, std::ios::binary | std::ios::in);
        ASSERT(fileStream.is_open(), "Could not open file"); // TODO: change this
        fileStream.read(reinterpret_cast<char*>(m_memory.data()), CPU::ADDRESS_SPACE_SIZE);
    }

    Word CPUTestController::readWord(Address t_address) const {
        return m_memory[t_address];
    }

    void CPUTestController::writeWord(Address t_address, Word t_value) {
        m_memory[t_address] = t_value;
    }

}
