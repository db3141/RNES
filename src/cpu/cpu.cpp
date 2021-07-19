#include <array>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "assert.hpp"
#include "cpu.hpp"

namespace RNES {
    
    DWord signExtendWord(Word t_value) {
        if (t_value & 0x80U) {
            return 0xFF00U | t_value;
        }
        else {
            return 0x0000U | t_value;
        }
    }

    CPU::CPU(std::shared_ptr<Memory> t_memory, Address t_programCounter)
        : m_memory(t_memory)
        , m_pc(t_programCounter)
        , m_sp(0x00)
        , m_acc(0x00)
        , m_x(0x00)
        , m_y(0x00)
        , m_st(0x00)
        , m_interruptFlags({ false, false, false })
    {
        ;
    }

    void CPU::printRegisters() const {
        std::cout
            << "PC:  0x" << std::hex << std::setw(4) << std::setfill('0') << (int)m_pc << " (0x" << (int)m_memory->readWord(m_pc) << ")\n"
            << "SP:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_sp  << '\n'
            << "ACC: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_acc << '\n'
            << "X:   0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_x   << '\n'
            << "Y:   0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_y   << '\n'
            << "ST:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_st  << "\n\n";
    }

    bool CPU::executeInstruction() {
        using Instruction = void (CPU::*)(AddressMode);

        struct InstructionInfo {
            Instruction instruction;
            AddressMode addressMode;
        };

        static const std::array<InstructionInfo, 256> INSTRUCTION_TABLE = {{
            { &CPU::instructionBRK, AddressMode::IMPLICIT },         /* 0x00 */
            { &CPU::instructionORA, AddressMode::INDEXED_INDIRECT }, /* 0x01 */
            { nullptr,              AddressMode::NONE },             /* 0x02 */
            { nullptr,              AddressMode::NONE },             /* 0x03 */
            { nullptr,              AddressMode::NONE },             /* 0x04 */
            { &CPU::instructionORA, AddressMode::ZERO_PAGE },        /* 0x05 */
            { &CPU::instructionASL, AddressMode::ZERO_PAGE },        /* 0x06 */
            { nullptr,              AddressMode::NONE },             /* 0x07 */
            { &CPU::instructionPHP, AddressMode::IMPLICIT },         /* 0x08 */
            { &CPU::instructionORA, AddressMode::IMMEDIATE },        /* 0x09 */
            { &CPU::instructionASL, AddressMode::ACCUMULATOR },      /* 0x0a */
            { nullptr,              AddressMode::NONE },             /* 0x0b */
            { nullptr,              AddressMode::NONE },             /* 0x0c */
            { &CPU::instructionORA, AddressMode::ABSOLUTE },         /* 0x0d */
            { &CPU::instructionASL, AddressMode::ABSOLUTE },         /* 0x0e */
            { nullptr,              AddressMode::NONE },             /* 0x0f */
            { &CPU::instructionBPL, AddressMode::RELATIVE },         /* 0x10 */
            { &CPU::instructionORA, AddressMode::INDIRECT_INDEXED }, /* 0x11 */
            { nullptr,              AddressMode::NONE },             /* 0x12 */
            { nullptr,              AddressMode::NONE },             /* 0x13 */
            { nullptr,              AddressMode::NONE },             /* 0x14 */
            { &CPU::instructionORA, AddressMode::ZERO_PAGE_X },      /* 0x15 */
            { &CPU::instructionASL, AddressMode::ZERO_PAGE_X },      /* 0x16 */
            { nullptr,              AddressMode::NONE },             /* 0x17 */
            { &CPU::instructionCLC, AddressMode::IMPLICIT },         /* 0x18 */
            { &CPU::instructionORA, AddressMode::ABSOLUTE_Y },       /* 0x19 */
            { nullptr,              AddressMode::NONE },             /* 0x1a */
            { nullptr,              AddressMode::NONE },             /* 0x1b */
            { nullptr,              AddressMode::NONE },             /* 0x1c */
            { &CPU::instructionORA, AddressMode::ABSOLUTE_X },       /* 0x1d */
            { &CPU::instructionASL, AddressMode::ABSOLUTE_X },       /* 0x1e */
            { nullptr,              AddressMode::NONE },             /* 0x1f */
            { &CPU::instructionJSR, AddressMode::ABSOLUTE },         /* 0x20 */
            { &CPU::instructionAND, AddressMode::INDEXED_INDIRECT }, /* 0x21 */
            { nullptr,              AddressMode::NONE },             /* 0x22 */
            { nullptr,              AddressMode::NONE },             /* 0x23 */
            { &CPU::instructionBIT, AddressMode::ZERO_PAGE },        /* 0x24 */
            { &CPU::instructionAND, AddressMode::ZERO_PAGE },        /* 0x25 */
            { &CPU::instructionROL, AddressMode::ZERO_PAGE },        /* 0x26 */
            { nullptr,              AddressMode::NONE },             /* 0x27 */
            { &CPU::instructionPLP, AddressMode::IMPLICIT },         /* 0x28 */
            { &CPU::instructionAND, AddressMode::IMMEDIATE },        /* 0x29 */
            { &CPU::instructionROL, AddressMode::ACCUMULATOR },      /* 0x2a */
            { nullptr,              AddressMode::NONE },             /* 0x2b */
            { &CPU::instructionBIT, AddressMode::ABSOLUTE },         /* 0x2c */
            { &CPU::instructionAND, AddressMode::ABSOLUTE },         /* 0x2d */
            { &CPU::instructionROL, AddressMode::ABSOLUTE },         /* 0x2e */
            { nullptr,              AddressMode::NONE },             /* 0x2f */
            { &CPU::instructionBMI, AddressMode::RELATIVE },         /* 0x30 */
            { &CPU::instructionAND, AddressMode::INDIRECT_INDEXED }, /* 0x31 */
            { nullptr,              AddressMode::NONE },             /* 0x32 */
            { nullptr,              AddressMode::NONE },             /* 0x33 */
            { nullptr,              AddressMode::NONE },             /* 0x34 */
            { &CPU::instructionAND, AddressMode::ZERO_PAGE_X },      /* 0x35 */
            { &CPU::instructionROL, AddressMode::ZERO_PAGE_X },      /* 0x36 */
            { nullptr,              AddressMode::NONE },             /* 0x37 */
            { &CPU::instructionSEC, AddressMode::IMPLICIT },         /* 0x38 */
            { &CPU::instructionAND, AddressMode::ABSOLUTE_Y },       /* 0x39 */
            { nullptr,              AddressMode::NONE },             /* 0x3a */
            { nullptr,              AddressMode::NONE },             /* 0x3b */
            { nullptr,              AddressMode::NONE },             /* 0x3c */
            { &CPU::instructionAND, AddressMode::ABSOLUTE_X },       /* 0x3d */
            { &CPU::instructionROL, AddressMode::ABSOLUTE_X },       /* 0x3e */
            { nullptr,              AddressMode::NONE },             /* 0x3f */
            { &CPU::instructionRTI, AddressMode::IMPLICIT },         /* 0x40 */
            { &CPU::instructionEOR, AddressMode::INDEXED_INDIRECT }, /* 0x41 */
            { nullptr,              AddressMode::NONE },             /* 0x42 */
            { nullptr,              AddressMode::NONE },             /* 0x43 */
            { nullptr,              AddressMode::NONE },             /* 0x44 */
            { &CPU::instructionEOR, AddressMode::ZERO_PAGE },        /* 0x45 */
            { &CPU::instructionLSR, AddressMode::ZERO_PAGE },        /* 0x46 */
            { nullptr,              AddressMode::NONE },             /* 0x47 */
            { &CPU::instructionPHA, AddressMode::IMPLICIT },         /* 0x48 */
            { &CPU::instructionEOR, AddressMode::IMMEDIATE },        /* 0x49 */
            { &CPU::instructionLSR, AddressMode::ACCUMULATOR },      /* 0x4a */
            { nullptr,              AddressMode::NONE },             /* 0x4b */
            { &CPU::instructionJMP, AddressMode::ABSOLUTE },         /* 0x4c */
            { &CPU::instructionEOR, AddressMode::ABSOLUTE },         /* 0x4d */
            { &CPU::instructionLSR, AddressMode::ABSOLUTE },         /* 0x4e */
            { nullptr,              AddressMode::NONE },             /* 0x4f */
            { &CPU::instructionBVC, AddressMode::RELATIVE },         /* 0x50 */
            { &CPU::instructionEOR, AddressMode::INDIRECT_INDEXED }, /* 0x51 */
            { nullptr,              AddressMode::NONE },             /* 0x52 */
            { nullptr,              AddressMode::NONE },             /* 0x53 */
            { nullptr,              AddressMode::NONE },             /* 0x54 */
            { &CPU::instructionEOR, AddressMode::ZERO_PAGE_X },      /* 0x55 */
            { &CPU::instructionLSR, AddressMode::ZERO_PAGE_X },      /* 0x56 */
            { nullptr,              AddressMode::NONE },             /* 0x57 */
            { &CPU::instructionCLI, AddressMode::IMPLICIT },         /* 0x58 */
            { &CPU::instructionEOR, AddressMode::ABSOLUTE_Y },       /* 0x59 */
            { nullptr,              AddressMode::NONE },             /* 0x5a */
            { nullptr,              AddressMode::NONE },             /* 0x5b */
            { nullptr,              AddressMode::NONE },             /* 0x5c */
            { &CPU::instructionEOR, AddressMode::ABSOLUTE_X },       /* 0x5d */
            { &CPU::instructionLSR, AddressMode::ABSOLUTE_X },       /* 0x5e */
            { nullptr,              AddressMode::NONE },             /* 0x5f */
            { &CPU::instructionRTS, AddressMode::IMPLICIT },         /* 0x60 */
            { &CPU::instructionADC, AddressMode::INDEXED_INDIRECT }, /* 0x61 */
            { nullptr,              AddressMode::NONE },             /* 0x62 */
            { nullptr,              AddressMode::NONE },             /* 0x63 */
            { nullptr,              AddressMode::NONE },             /* 0x64 */
            { &CPU::instructionADC, AddressMode::ZERO_PAGE },        /* 0x65 */
            { &CPU::instructionROR, AddressMode::ZERO_PAGE },        /* 0x66 */
            { nullptr,              AddressMode::NONE },             /* 0x67 */
            { &CPU::instructionPLA, AddressMode::IMPLICIT },         /* 0x68 */
            { &CPU::instructionADC, AddressMode::IMMEDIATE },        /* 0x69 */
            { &CPU::instructionROR, AddressMode::ACCUMULATOR },      /* 0x6a */
            { nullptr,              AddressMode::NONE },             /* 0x6b */
            { &CPU::instructionJMP, AddressMode::INDIRECT },         /* 0x6c */
            { &CPU::instructionADC, AddressMode::ABSOLUTE },         /* 0x6d */
            { &CPU::instructionROR, AddressMode::ABSOLUTE },         /* 0x6e */
            { nullptr,              AddressMode::NONE },             /* 0x6f */
            { &CPU::instructionBVS, AddressMode::RELATIVE },         /* 0x70 */
            { &CPU::instructionADC, AddressMode::INDIRECT_INDEXED }, /* 0x71 */
            { nullptr,              AddressMode::NONE },             /* 0x72 */
            { nullptr,              AddressMode::NONE },             /* 0x73 */
            { nullptr,              AddressMode::NONE },             /* 0x74 */
            { &CPU::instructionADC, AddressMode::ZERO_PAGE_X },      /* 0x75 */
            { &CPU::instructionROR, AddressMode::ZERO_PAGE_X },      /* 0x76 */
            { nullptr,              AddressMode::NONE },             /* 0x77 */
            { &CPU::instructionSEI, AddressMode::IMPLICIT },         /* 0x78 */
            { &CPU::instructionADC, AddressMode::ABSOLUTE_Y },       /* 0x79 */
            { nullptr,              AddressMode::NONE },             /* 0x7a */
            { nullptr,              AddressMode::NONE },             /* 0x7b */
            { nullptr,              AddressMode::NONE },             /* 0x7c */
            { &CPU::instructionADC, AddressMode::ABSOLUTE_X },       /* 0x7d */
            { &CPU::instructionROR, AddressMode::ABSOLUTE_X },       /* 0x7e */
            { nullptr,              AddressMode::NONE },             /* 0x7f */
            { nullptr,              AddressMode::NONE },             /* 0x80 */
            { &CPU::instructionSTA, AddressMode::INDEXED_INDIRECT }, /* 0x81 */
            { nullptr,              AddressMode::NONE },             /* 0x82 */
            { nullptr,              AddressMode::NONE },             /* 0x83 */
            { &CPU::instructionSTY, AddressMode::ZERO_PAGE },        /* 0x84 */
            { &CPU::instructionSTA, AddressMode::ZERO_PAGE },        /* 0x85 */
            { &CPU::instructionSTX, AddressMode::ZERO_PAGE },        /* 0x86 */
            { nullptr,              AddressMode::NONE },             /* 0x87 */
            { &CPU::instructionDEY, AddressMode::IMPLICIT },         /* 0x88 */
            { nullptr,              AddressMode::NONE },             /* 0x89 */
            { &CPU::instructionTXA, AddressMode::IMPLICIT },         /* 0x8a */
            { nullptr,              AddressMode::NONE },             /* 0x8b */
            { &CPU::instructionSTY, AddressMode::ABSOLUTE },         /* 0x8c */
            { &CPU::instructionSTA, AddressMode::ABSOLUTE },         /* 0x8d */
            { &CPU::instructionSTX, AddressMode::ABSOLUTE },         /* 0x8e */
            { nullptr,              AddressMode::NONE },             /* 0x8f */
            { &CPU::instructionBCC, AddressMode::RELATIVE },         /* 0x90 */
            { &CPU::instructionSTA, AddressMode::INDIRECT_INDEXED }, /* 0x91 */
            { nullptr,              AddressMode::NONE },             /* 0x92 */
            { nullptr,              AddressMode::NONE },             /* 0x93 */
            { &CPU::instructionSTY, AddressMode::ZERO_PAGE_X },      /* 0x94 */
            { &CPU::instructionSTA, AddressMode::ZERO_PAGE_X },      /* 0x95 */
            { &CPU::instructionSTX, AddressMode::ZERO_PAGE_Y },      /* 0x96 */
            { nullptr,              AddressMode::NONE },             /* 0x97 */
            { &CPU::instructionTYA, AddressMode::IMPLICIT },         /* 0x98 */
            { &CPU::instructionSTA, AddressMode::ABSOLUTE_Y },       /* 0x99 */
            { &CPU::instructionTXS, AddressMode::IMPLICIT },         /* 0x9a */
            { nullptr,              AddressMode::NONE },             /* 0x9b */
            { nullptr,              AddressMode::NONE },             /* 0x9c */
            { &CPU::instructionSTA, AddressMode::ABSOLUTE_X },       /* 0x9d */
            { nullptr,              AddressMode::NONE },             /* 0x9e */
            { nullptr,              AddressMode::NONE },             /* 0x9f */
            { &CPU::instructionLDY, AddressMode::IMMEDIATE },        /* 0xa0 */
            { &CPU::instructionLDA, AddressMode::INDEXED_INDIRECT }, /* 0xa1 */
            { &CPU::instructionLDX, AddressMode::IMMEDIATE },        /* 0xa2 */
            { nullptr,              AddressMode::NONE },             /* 0xa3 */
            { &CPU::instructionLDY, AddressMode::ZERO_PAGE },        /* 0xa4 */
            { &CPU::instructionLDA, AddressMode::ZERO_PAGE },        /* 0xa5 */
            { &CPU::instructionLDX, AddressMode::ZERO_PAGE },        /* 0xa6 */
            { nullptr,              AddressMode::NONE },             /* 0xa7 */
            { &CPU::instructionTAY, AddressMode::IMPLICIT },         /* 0xa8 */
            { &CPU::instructionLDA, AddressMode::IMMEDIATE },        /* 0xa9 */
            { &CPU::instructionTAX, AddressMode::IMPLICIT },         /* 0xaa */
            { nullptr,              AddressMode::NONE },             /* 0xab */
            { &CPU::instructionLDY, AddressMode::ABSOLUTE },         /* 0xac */
            { &CPU::instructionLDA, AddressMode::ABSOLUTE },         /* 0xad */
            { &CPU::instructionLDX, AddressMode::ABSOLUTE },         /* 0xae */
            { nullptr,              AddressMode::NONE },             /* 0xaf */
            { &CPU::instructionBCS, AddressMode::RELATIVE },         /* 0xb0 */
            { &CPU::instructionLDA, AddressMode::INDIRECT_INDEXED }, /* 0xb1 */
            { nullptr,              AddressMode::NONE },             /* 0xb2 */
            { nullptr,              AddressMode::NONE },             /* 0xb3 */
            { &CPU::instructionLDY, AddressMode::ZERO_PAGE_X },      /* 0xb4 */
            { &CPU::instructionLDA, AddressMode::ZERO_PAGE_X },      /* 0xb5 */
            { &CPU::instructionLDX, AddressMode::ZERO_PAGE_Y },      /* 0xb6 */
            { nullptr,              AddressMode::NONE },             /* 0xb7 */
            { &CPU::instructionCLV, AddressMode::IMPLICIT },         /* 0xb8 */
            { &CPU::instructionLDA, AddressMode::ABSOLUTE_Y },       /* 0xb9 */
            { &CPU::instructionTSX, AddressMode::IMPLICIT },         /* 0xba */
            { nullptr,              AddressMode::NONE },             /* 0xbb */
            { &CPU::instructionLDY, AddressMode::ABSOLUTE_X },       /* 0xbc */
            { &CPU::instructionLDA, AddressMode::ABSOLUTE_X },       /* 0xbd */
            { &CPU::instructionLDX, AddressMode::ABSOLUTE_Y },       /* 0xbe */
            { nullptr,              AddressMode::NONE },             /* 0xbf */
            { &CPU::instructionCPY, AddressMode::IMMEDIATE },        /* 0xc0 */
            { &CPU::instructionCMP, AddressMode::INDEXED_INDIRECT }, /* 0xc1 */
            { nullptr,              AddressMode::NONE },             /* 0xc2 */
            { nullptr,              AddressMode::NONE },             /* 0xc3 */
            { &CPU::instructionCPY, AddressMode::ZERO_PAGE },        /* 0xc4 */
            { &CPU::instructionCMP, AddressMode::ZERO_PAGE },        /* 0xc5 */
            { &CPU::instructionDEC, AddressMode::ZERO_PAGE },        /* 0xc6 */
            { nullptr,              AddressMode::NONE },             /* 0xc7 */
            { &CPU::instructionINY, AddressMode::IMPLICIT },         /* 0xc8 */
            { &CPU::instructionCMP, AddressMode::IMMEDIATE },        /* 0xc9 */
            { &CPU::instructionDEX, AddressMode::IMPLICIT },         /* 0xca */
            { nullptr,              AddressMode::NONE },             /* 0xcb */
            { &CPU::instructionCPY, AddressMode::ABSOLUTE },         /* 0xcc */
            { &CPU::instructionCMP, AddressMode::ABSOLUTE },         /* 0xcd */
            { &CPU::instructionDEC, AddressMode::ABSOLUTE },         /* 0xce */
            { nullptr,              AddressMode::NONE },             /* 0xcf */
            { &CPU::instructionBNE, AddressMode::RELATIVE },         /* 0xd0 */
            { &CPU::instructionCMP, AddressMode::INDIRECT_INDEXED }, /* 0xd1 */
            { nullptr,              AddressMode::NONE },             /* 0xd2 */
            { nullptr,              AddressMode::NONE },             /* 0xd3 */
            { nullptr,              AddressMode::NONE },             /* 0xd4 */
            { &CPU::instructionCMP, AddressMode::ZERO_PAGE_X },      /* 0xd5 */
            { &CPU::instructionDEC, AddressMode::ZERO_PAGE_X },      /* 0xd6 */
            { nullptr,              AddressMode::NONE },             /* 0xd7 */
            { &CPU::instructionCLD, AddressMode::IMPLICIT },         /* 0xd8 */
            { &CPU::instructionCMP, AddressMode::ABSOLUTE_Y },       /* 0xd9 */
            { nullptr,              AddressMode::NONE },             /* 0xda */
            { nullptr,              AddressMode::NONE },             /* 0xdb */
            { nullptr,              AddressMode::NONE },             /* 0xdc */
            { &CPU::instructionCMP, AddressMode::ABSOLUTE_X },       /* 0xdd */
            { &CPU::instructionDEC, AddressMode::ABSOLUTE_X },       /* 0xde */
            { nullptr,              AddressMode::NONE },             /* 0xdf */
            { &CPU::instructionCPX, AddressMode::IMMEDIATE },        /* 0xe0 */
            { &CPU::instructionSBC, AddressMode::INDEXED_INDIRECT }, /* 0xe1 */
            { nullptr,              AddressMode::NONE },             /* 0xe2 */
            { nullptr,              AddressMode::NONE },             /* 0xe3 */
            { &CPU::instructionCPX, AddressMode::ZERO_PAGE },        /* 0xe4 */
            { &CPU::instructionSBC, AddressMode::ZERO_PAGE },        /* 0xe5 */
            { &CPU::instructionINC, AddressMode::ZERO_PAGE },        /* 0xe6 */
            { nullptr,              AddressMode::NONE },             /* 0xe7 */
            { &CPU::instructionINX, AddressMode::IMPLICIT },         /* 0xe8 */
            { &CPU::instructionSBC, AddressMode::IMMEDIATE },        /* 0xe9 */
            { &CPU::instructionNOP, AddressMode::IMPLICIT },         /* 0xea */
            { nullptr,              AddressMode::NONE },             /* 0xeb */
            { &CPU::instructionCPX, AddressMode::ABSOLUTE },         /* 0xec */
            { &CPU::instructionSBC, AddressMode::ABSOLUTE },         /* 0xed */
            { &CPU::instructionINC, AddressMode::ABSOLUTE },         /* 0xee */
            { nullptr,              AddressMode::NONE },             /* 0xef */
            { &CPU::instructionBEQ, AddressMode::RELATIVE },         /* 0xf0 */
            { &CPU::instructionSBC, AddressMode::INDIRECT_INDEXED }, /* 0xf1 */
            { nullptr,              AddressMode::NONE },             /* 0xf2 */
            { nullptr,              AddressMode::NONE },             /* 0xf3 */
            { nullptr,              AddressMode::NONE },             /* 0xf4 */
            { &CPU::instructionSBC, AddressMode::ZERO_PAGE_X },      /* 0xf5 */
            { &CPU::instructionINC, AddressMode::ZERO_PAGE_X },      /* 0xf6 */
            { nullptr,              AddressMode::NONE },             /* 0xf7 */
            { &CPU::instructionSED, AddressMode::IMPLICIT },         /* 0xf8 */
            { &CPU::instructionSBC, AddressMode::ABSOLUTE_Y },       /* 0xf9 */
            { nullptr,              AddressMode::NONE },             /* 0xfa */
            { nullptr,              AddressMode::NONE },             /* 0xfb */
            { nullptr,              AddressMode::NONE },             /* 0xfc */
            { &CPU::instructionSBC, AddressMode::ABSOLUTE_X },       /* 0xfd */
            { &CPU::instructionINC, AddressMode::ABSOLUTE_X },       /* 0xfe */
            { nullptr,              AddressMode::NONE },             /* 0xff */
        }};

        handleInterrupts();

        const Word opcode = m_memory->readWord(m_pc);
        const InstructionInfo info = INSTRUCTION_TABLE[opcode];

        ASSERT(info.instruction != nullptr, "Invalid opcode");
        
        // call the function obtained from the table
        (this->*info.instruction)(info.addressMode);
        return false;
    }

    bool CPU::getFlag(StatusFlag t_flag) const {
        return !!(m_st & static_cast<Word>(t_flag));
    }

    void CPU::setFlag(StatusFlag t_flag, bool t_value) {
        if (t_value) {
            m_st |= static_cast<Word>(t_flag);
        }
        else {
            m_st &= ~static_cast<Word>(t_flag);
        }
    }

    size_t CPU::instructionSize(AddressMode t_mode) const {
        static const std::array<size_t, ADDRESS_MODE_COUNT> AMOUNT_TABLE {{
            1, // IMPLICIT

            1, // ACCUMULATOR

            2, // IMMEDIATE

            2, // ZERO_PAGE
            2, // ZERO_PAGE_X
            2, // ZERO_PAGE_Y

            2, // RELATIVE

            3, // ABSOLUTE
            3, // ABSOLUTE_X
            3, // ABSOLUTE_Y

            3, //INDIRECT

            2, // INDEXED_INDIRECT
            2  // INDIRECT_INDEXED
        }};

        return AMOUNT_TABLE[static_cast<size_t>(t_mode)];
    }


    CPU::WordReference CPU::getWordArgument(AddressMode t_mode) {
        switch(t_mode) {
            case AddressMode::IMPLICIT:
                ASSERT(false, "shouldn't be here");

            case AddressMode::ACCUMULATOR:
                return WordReference(*this, &m_acc);

            case AddressMode::IMMEDIATE:
                return WordReference(*this, m_pc + 1);

            case AddressMode::ZERO_PAGE:
                return WordReference(*this, m_memory->readWord(m_pc + 1));

            case AddressMode::ZERO_PAGE_X:
                return WordReference(*this, (m_memory->readWord(m_pc + 1) + m_x) % 0x0100U);

            case AddressMode::ZERO_PAGE_Y:
                return WordReference(*this, (m_memory->readWord(m_pc + 1) + m_y) % 0x0100U);

            case AddressMode::RELATIVE:
                ASSERT(false, "Relative addressing should use getAddressArgument");

            case AddressMode::ABSOLUTE:
                return WordReference(*this, m_memory->readDWord(m_pc + 1));

            case AddressMode::ABSOLUTE_X:
                return WordReference(*this, m_memory->readDWord(m_pc + 1) + m_x);

            case AddressMode::ABSOLUTE_Y:
                return WordReference(*this, m_memory->readDWord(m_pc + 1) + m_y);

            case AddressMode::INDIRECT:
                ASSERT(false, "Indirect addressing should use getAddressArgument");

            case AddressMode::INDEXED_INDIRECT: {
                const Address zeroPageAddress = (m_memory->readWord(m_pc + 1) + m_x) % 0x0100U;
                return WordReference(*this, m_memory->readDWord(zeroPageAddress));
            }

            case AddressMode::INDIRECT_INDEXED: {
                const Address zeroPageAddress = m_memory->readWord(m_pc + 1);
                return WordReference(*this, m_memory->readDWord(zeroPageAddress) + m_y);
            }

            default:
                ASSERT(false, "Invalid address mode");
        }

        throw std::exception();
    }

    Address CPU::getAddressArgument(AddressMode t_mode) {
        switch(t_mode) {
            case AddressMode::IMPLICIT:
                ASSERT(false, "shouldn't be here");

            case AddressMode::ACCUMULATOR:
                ASSERT(false, "Accumulator addressing should use getWordArgument");

            case AddressMode::IMMEDIATE:
                ASSERT(false, "Immediate addressing should use getWordArgument");

            case AddressMode::ZERO_PAGE:
                return m_memory->readWord(m_pc + 1);

            case AddressMode::ZERO_PAGE_X:
                return (m_memory->readWord(m_pc + 1) + m_x) % 0x0100U;

            case AddressMode::ZERO_PAGE_Y:
                return (m_memory->readWord(m_pc + 1) + m_y) % 0x0100U;

            case AddressMode::RELATIVE:
                return m_pc + signExtendWord(m_memory->readWord(m_pc + 1));

            case AddressMode::ABSOLUTE:
                return m_memory->readDWord(m_pc + 1);

            case AddressMode::ABSOLUTE_X:
                return m_memory->readDWord(m_pc + 1) + m_x;

            case AddressMode::ABSOLUTE_Y:
                return m_memory->readDWord(m_pc + 1) + m_y;

            case AddressMode::INDIRECT:
                return m_memory->readDWord(m_memory->readDWord(m_pc + 1));

            case AddressMode::INDEXED_INDIRECT: {
                const Address zeroPageAddress = (m_memory->readWord(m_pc + 1) + m_x) % 0x0100U;
                return m_memory->readDWord(zeroPageAddress);
            }

            case AddressMode::INDIRECT_INDEXED: {
                const Address zeroPageAddress = m_memory->readWord(m_pc + 1);
                return m_memory->readDWord(zeroPageAddress) + m_y;
            }

            default:
                ASSERT(false, "Invalid address mode");
                exit(EXIT_FAILURE);
        }
    }

    void CPU::generateIRQ() {
        m_interruptFlags.irq = true;
    }
        
    void CPU::generateBRK() {
        m_interruptFlags.brk = true;
    }
        
    void CPU::generateNMI() {
        m_interruptFlags.nmi = true;
    }

    void CPU::handleInterrupts() {
        if (m_interruptFlags.nmi) {
            m_interruptFlags.nmi = false;
            ASSERT(false, "TODO: implement this");
        }
        else if (m_interruptFlags.brk) {
            m_interruptFlags.brk = false;

            stackPushDWord(m_pc + 1); // size of BRK is 1
            stackPushWord(m_st | 0b00110000);

            setFlag(StatusFlag::INTERRUPT_DISABLE, true);

            std::cout << std::hex << m_memory->readDWord(0xFFFE) << '\n';
            m_pc = m_memory->readDWord(0xFFFE);           
        }
        else if (m_interruptFlags.irq && getFlag(StatusFlag::INTERRUPT_DISABLE)) {
            m_interruptFlags.irq = false;
            ASSERT(false, "TODO: implement this");
        }
    }

    void CPU::stackPushWord(Word t_word) {
        m_memory->writeWord(m_sp + 0x0100U, t_word);
        m_sp--;
    }

    Word CPU::stackPopWord() {
        m_sp++;
        return m_memory->readWord(m_sp + 0x0100U);
    }

    void CPU::stackPushDWord(DWord t_dword) {
        m_memory->writeWord(m_sp + 0x0100U, (t_dword >> 8) & 0x00FFU);
        m_sp--;
        m_memory->writeWord(m_sp + 0x0100U, (t_dword >> 0) & 0x00FFU);
        m_sp--;
    }

    DWord CPU::stackPopDWord() {
        DWord result = 0;

        m_sp++;
        result |= m_memory->readWord(m_sp + 0x0100U) << 0;
        m_sp++;
        result |= m_memory->readWord(m_sp + 0x0100U) << 8;

        return result;
    }

//--------------------------------------------------------------------------------------------------------------------

    CPU::WordReference::WordReference(CPU& t_cpu, Word* t_word)
        : m_cpu(t_cpu)
        , m_location(t_word)
    {
        ;
    }

    CPU::WordReference::WordReference(CPU& t_cpu, Address t_address)
        : m_cpu(t_cpu)
        , m_location(t_address)
    {
        ;
    }

    CPU::WordReference::operator Word() const {
        const auto wordDPointer = std::get_if<Word*>(&m_location);
        const auto addressPointer = std::get_if<Address>(&m_location);

        if (wordDPointer != nullptr) {
            return **wordDPointer;
        }
        else if (addressPointer != nullptr) {
            return m_cpu.m_memory->readWord(*addressPointer);
        }
        else {
            ASSERT(false, "Tried to use invalid WordReference");
        }
    }

    void CPU::WordReference::operator=(Word t_value) {
        if (Word** val = std::get_if<Word*>(&m_location)) {
            **val = t_value;
        }
        else if (Address* val = std::get_if<Address>(&m_location)) {
            m_cpu.m_memory->writeWord(*val, t_value);
        }
        else {
            ASSERT(false, "Tried to use invalid WordReference");
        }
    }

}
