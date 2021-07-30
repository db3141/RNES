#include <fstream>

#include "assert.hpp"
#include "cpu_test_controller.hpp"

namespace RNES::Test {

    CPUTestController::CPUTestController() : m_memory() {
        ;
    }

    CPUTestController::CPUTestController(const char* t_path) : m_memory() {
        std::ifstream fileStream(t_path, std::ios::binary | std::ios::in);
        ASSERT(fileStream.is_open(), "TODO: change this");
        fileStream.read(reinterpret_cast<char*>(m_memory.data()), CPU::ADDRESS_SPACE_SIZE);
    }

    Word CPUTestController::readWord(Address t_address) const {
        return m_memory[t_address];
    }

    void CPUTestController::writeWord(Address t_address, Word t_value) {
        m_memory[t_address] = t_value;
    }

    DWord CPUTestController::readDWord(Address t_address) const {
        return (m_memory[t_address + 0U] << 0U) |
                (m_memory[t_address + 1U] << 8U);
    }

    void CPUTestController::writeDWord(Address t_address, DWord t_value) {
        m_memory[t_address + 0U] = (t_value >> 0U) & 0xFF;
        m_memory[t_address + 1U] = (t_value >> 8U) & 0xFF;
    }

}
