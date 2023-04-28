#ifndef RNES_CPU_CONTROLLER_INCLUDED
#define RNES_CPU_CONTROLLER_INCLUDED

#include "defines.hpp"

namespace RNES::CPU {

    using Word = uint8_t;
    using DWord = uint16_t;

    using Address = DWord;

    const size_t ADDRESS_SPACE_SIZE = 1ULL << (8ULL * sizeof(Address));

    class CPUController {
    public:
        virtual ~CPUController() = default;

        [[nodiscard]] virtual Word readWord(Address t_address) const = 0;
        virtual void writeWord(Address t_address, Word t_value) = 0;

        [[nodiscard]] virtual DWord readDWord(Address t_address) const {
            return
                this->readWord(t_address + 0) << 0 |
                this->readWord(t_address + 1) << 8;
        }
    };

}

#endif
