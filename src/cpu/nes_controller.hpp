#ifndef RNES_NES_CONTROLLER_INCLUDED
#define RNES_NES_CONTROLLER_INCLUDED

#include <array>
#include <memory>

#include "defines.hpp"
#include "cpu/cpu_controller.hpp"

namespace RNES {

    class NESController : public CPU::CPUController {
    public:
        explicit NESController(std::unique_ptr<CPU::CPUController> t_cpuMapper);
        ~NESController() override = default;

        [[nodiscard]] Word readWord(Address t_address) const override;
        void writeWord(Address t_address, Word t_value) override;

    private:
        std::array<Word, 0x0800> m_internalRAM;
        std::unique_ptr<CPU::CPUController> m_cpuMapper;
    };

}

#endif
