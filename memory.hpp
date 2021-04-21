#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include <stddef.h>

namespace Emulator {

    using Word = uint8_t;
    using DWord = uint16_t;

    using Address = DWord;

    const size_t ADDRESS_SPACE_SIZE = 1ULL << (8ULL * sizeof(Address));

    class Memory {
    public:
        virtual ~Memory() {}

        virtual Word readWord(Address t_address) const = 0;
        virtual void writeWord(Address t_address, Word t_value) = 0;

        virtual DWord readDWord(Address t_address) const = 0;
        virtual void writeDWord(Address t_address, DWord t_value) = 0;
    };

}

#endif
