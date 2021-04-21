#ifndef RAM_INCLUDED
#define RAM_INCLUDED

#include <array>

#include "memory.hpp"

namespace Emulator {

    class RAM : public Memory {
    public:
        RAM();
        RAM(const char* t_path);

        Word readWord(Address t_address) const override;
        void writeWord(Address t_address, Word t_value) override;

        DWord readDWord(Address t_address) const override;
        void writeDWord(Address t_address, DWord t_value) override;

    private:
        std::array<Word, ADDRESS_SPACE_SIZE> m_ram;
    };

}

#endif
