#ifndef RNES_TEST_CONTROLLER_INCLUDED
#define RNES_TEST_CONTROLLER_INCLUDED

#include <array>

#include "cpu/cpu_controller.hpp"

namespace RNES::Test {

    class CPUTestController : public CPU::CPUController {
    public:
        CPUTestController();
        CPUTestController(const char* t_path);

        Word readWord(Address t_address) const override;
        void writeWord(Address t_address, Word t_value) override;

        DWord readDWord(Address t_address) const override;
        void writeDWord(Address t_address, DWord t_value) override;

    private:
        std::array<Word, CPU::ADDRESS_SPACE_SIZE> m_memory;
    };

}

#endif
