#ifndef RNES_PPU_TEST_CONTROLLER
#define RNES_PPU_TEST_CONTROLLER

#include "ppu_controller.hpp"

namespace RNES {

    static const size_t PPU_ADDRESS_SPACE_SIZE = 0x4000;

    class PPUTestContoller : public PPUController {
    public:
        PPUTestController();

        Word readWord(Address t_address) override;
        void writeWord(Address t_address, Word t_value) override;

        void sendVBlankNMI() override;

    private:
        std::array<Word, PPU_ADDRESS_SPACE_SIZE> m_memory;
    }

}

#endif
