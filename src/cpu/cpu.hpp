#ifndef RNES_CPU_6502_INCLUDED
#define RNES_CPU_6502_INCLUDED

#include <variant>
#include <set>
#include <memory>
#include <initializer_list>

#include "cpu_controller.hpp"
#include "defines.hpp"

namespace RNES::CPU {

    enum class AddressMode : size_t {
        IMPLICIT = 0,

        ACCUMULATOR,

        IMMEDIATE,

        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,

        RELATIVE,

        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,

        INDIRECT,

        INDEXED_INDIRECT,
        INDIRECT_INDEXED,

        NONE
    };
    static const size_t ADDRESS_MODE_COUNT = 13;

    enum class InstructionId : size_t {
        ADC = 0,
        AND = 1,
        ASL = 2,
        BCC = 3,
        BCS = 4,
        BEQ = 5,
        BIT = 6,
        BMI = 7,
        BNE = 8,
        BPL = 9,
        BRK = 10,
        BVC = 11,
        BVS = 12,
        CLC = 13,
        CLD = 14,
        CLI = 15,
        CLV = 16,
        CMP = 17,
        CPX = 18,
        CPY = 19,
        DEC = 20,
        DEX = 21,
        DEY = 22,
        EOR = 23,
        INC = 24,
        INX = 25,
        INY = 26,
        JMP = 27,
        JSR = 28,
        LDA = 29,
        LDX = 30,
        LDY = 31,
        LSR = 32,
        NOP = 33,
        ORA = 34,
        PHA = 35,
        PHP = 36,
        PLA = 37,
        PLP = 38,
        ROL = 39,
        ROR = 40,
        RTI = 41,
        RTS = 42,
        SBC = 43,
        SEC = 44,
        SED = 45,
        SEI = 46,
        STA = 47,
        STX = 48,
        STY = 49,
        TAX = 50,
        TAY = 51,
        TSX = 52,
        TXA = 53,
        TXS = 54,
        TYA = 55,
        NONE = 56
    };
    static const size_t INSTRUCTION_COUNT = static_cast<size_t>(InstructionId::NONE);

    struct InstructionInfo {
        InstructionId id;
        AddressMode addressMode;
    };

    static const std::array<InstructionInfo, 256> INSTRUCTION_TABLE = {{
        { InstructionId::BRK,   AddressMode::IMPLICIT },         /* 0x00 */
        { InstructionId::ORA,   AddressMode::INDEXED_INDIRECT }, /* 0x01 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x02 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x03 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x04 */
        { InstructionId::ORA,   AddressMode::ZERO_PAGE },        /* 0x05 */
        { InstructionId::ASL,   AddressMode::ZERO_PAGE },        /* 0x06 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x07 */
        { InstructionId::PHP,   AddressMode::IMPLICIT },         /* 0x08 */
        { InstructionId::ORA,   AddressMode::IMMEDIATE },        /* 0x09 */
        { InstructionId::ASL,   AddressMode::ACCUMULATOR },      /* 0x0a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x0b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x0c */
        { InstructionId::ORA,   AddressMode::ABSOLUTE },         /* 0x0d */
        { InstructionId::ASL,   AddressMode::ABSOLUTE },         /* 0x0e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x0f */
        { InstructionId::BPL,   AddressMode::RELATIVE },         /* 0x10 */
        { InstructionId::ORA,   AddressMode::INDIRECT_INDEXED }, /* 0x11 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x12 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x13 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x14 */
        { InstructionId::ORA,   AddressMode::ZERO_PAGE_X },      /* 0x15 */
        { InstructionId::ASL,   AddressMode::ZERO_PAGE_X },      /* 0x16 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x17 */
        { InstructionId::CLC,   AddressMode::IMPLICIT },         /* 0x18 */
        { InstructionId::ORA,   AddressMode::ABSOLUTE_Y },       /* 0x19 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x1a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x1b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x1c */
        { InstructionId::ORA,   AddressMode::ABSOLUTE_X },       /* 0x1d */
        { InstructionId::ASL,   AddressMode::ABSOLUTE_X },       /* 0x1e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x1f */
        { InstructionId::JSR,   AddressMode::ABSOLUTE },         /* 0x20 */
        { InstructionId::AND,   AddressMode::INDEXED_INDIRECT }, /* 0x21 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x22 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x23 */
        { InstructionId::BIT,   AddressMode::ZERO_PAGE },        /* 0x24 */
        { InstructionId::AND,   AddressMode::ZERO_PAGE },        /* 0x25 */
        { InstructionId::ROL,   AddressMode::ZERO_PAGE },        /* 0x26 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x27 */
        { InstructionId::PLP,   AddressMode::IMPLICIT },         /* 0x28 */
        { InstructionId::AND,   AddressMode::IMMEDIATE },        /* 0x29 */
        { InstructionId::ROL,   AddressMode::ACCUMULATOR },      /* 0x2a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x2b */
        { InstructionId::BIT,   AddressMode::ABSOLUTE },         /* 0x2c */
        { InstructionId::AND,   AddressMode::ABSOLUTE },         /* 0x2d */
        { InstructionId::ROL,   AddressMode::ABSOLUTE },         /* 0x2e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x2f */
        { InstructionId::BMI,   AddressMode::RELATIVE },         /* 0x30 */
        { InstructionId::AND,   AddressMode::INDIRECT_INDEXED }, /* 0x31 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x32 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x33 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x34 */
        { InstructionId::AND,   AddressMode::ZERO_PAGE_X },      /* 0x35 */
        { InstructionId::ROL,   AddressMode::ZERO_PAGE_X },      /* 0x36 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x37 */
        { InstructionId::SEC,   AddressMode::IMPLICIT },         /* 0x38 */
        { InstructionId::AND,   AddressMode::ABSOLUTE_Y },       /* 0x39 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x3a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x3b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x3c */
        { InstructionId::AND,   AddressMode::ABSOLUTE_X },       /* 0x3d */
        { InstructionId::ROL,   AddressMode::ABSOLUTE_X },       /* 0x3e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x3f */
        { InstructionId::RTI,   AddressMode::IMPLICIT },         /* 0x40 */
        { InstructionId::EOR,   AddressMode::INDEXED_INDIRECT }, /* 0x41 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x42 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x43 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x44 */
        { InstructionId::EOR,   AddressMode::ZERO_PAGE },        /* 0x45 */
        { InstructionId::LSR,   AddressMode::ZERO_PAGE },        /* 0x46 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x47 */
        { InstructionId::PHA,   AddressMode::IMPLICIT },         /* 0x48 */
        { InstructionId::EOR,   AddressMode::IMMEDIATE },        /* 0x49 */
        { InstructionId::LSR,   AddressMode::ACCUMULATOR },      /* 0x4a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x4b */
        { InstructionId::JMP,   AddressMode::ABSOLUTE },         /* 0x4c */
        { InstructionId::EOR,   AddressMode::ABSOLUTE },         /* 0x4d */
        { InstructionId::LSR,   AddressMode::ABSOLUTE },         /* 0x4e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x4f */
        { InstructionId::BVC,   AddressMode::RELATIVE },         /* 0x50 */
        { InstructionId::EOR,   AddressMode::INDIRECT_INDEXED }, /* 0x51 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x52 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x53 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x54 */
        { InstructionId::EOR,   AddressMode::ZERO_PAGE_X },      /* 0x55 */
        { InstructionId::LSR,   AddressMode::ZERO_PAGE_X },      /* 0x56 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x57 */
        { InstructionId::CLI,   AddressMode::IMPLICIT },         /* 0x58 */
        { InstructionId::EOR,   AddressMode::ABSOLUTE_Y },       /* 0x59 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x5a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x5b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x5c */
        { InstructionId::EOR,   AddressMode::ABSOLUTE_X },       /* 0x5d */
        { InstructionId::LSR,   AddressMode::ABSOLUTE_X },       /* 0x5e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x5f */
        { InstructionId::RTS,   AddressMode::IMPLICIT },         /* 0x60 */
        { InstructionId::ADC,   AddressMode::INDEXED_INDIRECT }, /* 0x61 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x62 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x63 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x64 */
        { InstructionId::ADC,   AddressMode::ZERO_PAGE },        /* 0x65 */
        { InstructionId::ROR,   AddressMode::ZERO_PAGE },        /* 0x66 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x67 */
        { InstructionId::PLA,   AddressMode::IMPLICIT },         /* 0x68 */
        { InstructionId::ADC,   AddressMode::IMMEDIATE },        /* 0x69 */
        { InstructionId::ROR,   AddressMode::ACCUMULATOR },      /* 0x6a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x6b */
        { InstructionId::JMP,   AddressMode::INDIRECT },         /* 0x6c */
        { InstructionId::ADC,   AddressMode::ABSOLUTE },         /* 0x6d */
        { InstructionId::ROR,   AddressMode::ABSOLUTE },         /* 0x6e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x6f */
        { InstructionId::BVS,   AddressMode::RELATIVE },         /* 0x70 */
        { InstructionId::ADC,   AddressMode::INDIRECT_INDEXED }, /* 0x71 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x72 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x73 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x74 */
        { InstructionId::ADC,   AddressMode::ZERO_PAGE_X },      /* 0x75 */
        { InstructionId::ROR,   AddressMode::ZERO_PAGE_X },      /* 0x76 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x77 */
        { InstructionId::SEI,   AddressMode::IMPLICIT },         /* 0x78 */
        { InstructionId::ADC,   AddressMode::ABSOLUTE_Y },       /* 0x79 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x7a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x7b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x7c */
        { InstructionId::ADC,   AddressMode::ABSOLUTE_X },       /* 0x7d */
        { InstructionId::ROR,   AddressMode::ABSOLUTE_X },       /* 0x7e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x7f */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x80 */
        { InstructionId::STA,   AddressMode::INDEXED_INDIRECT }, /* 0x81 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x82 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x83 */
        { InstructionId::STY,   AddressMode::ZERO_PAGE },        /* 0x84 */
        { InstructionId::STA,   AddressMode::ZERO_PAGE },        /* 0x85 */
        { InstructionId::STX,   AddressMode::ZERO_PAGE },        /* 0x86 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x87 */
        { InstructionId::DEY,   AddressMode::IMPLICIT },         /* 0x88 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x89 */
        { InstructionId::TXA,   AddressMode::IMPLICIT },         /* 0x8a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x8b */
        { InstructionId::STY,   AddressMode::ABSOLUTE },         /* 0x8c */
        { InstructionId::STA,   AddressMode::ABSOLUTE },         /* 0x8d */
        { InstructionId::STX,   AddressMode::ABSOLUTE },         /* 0x8e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x8f */
        { InstructionId::BCC,   AddressMode::RELATIVE },         /* 0x90 */
        { InstructionId::STA,   AddressMode::INDIRECT_INDEXED }, /* 0x91 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x92 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x93 */
        { InstructionId::STY,   AddressMode::ZERO_PAGE_X },      /* 0x94 */
        { InstructionId::STA,   AddressMode::ZERO_PAGE_X },      /* 0x95 */
        { InstructionId::STX,   AddressMode::ZERO_PAGE_Y },      /* 0x96 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x97 */
        { InstructionId::TYA,   AddressMode::IMPLICIT },         /* 0x98 */
        { InstructionId::STA,   AddressMode::ABSOLUTE_Y },       /* 0x99 */
        { InstructionId::TXS,   AddressMode::IMPLICIT },         /* 0x9a */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x9b */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x9c */
        { InstructionId::STA,   AddressMode::ABSOLUTE_X },       /* 0x9d */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x9e */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0x9f */
        { InstructionId::LDY,   AddressMode::IMMEDIATE },        /* 0xa0 */
        { InstructionId::LDA,   AddressMode::INDEXED_INDIRECT }, /* 0xa1 */
        { InstructionId::LDX,   AddressMode::IMMEDIATE },        /* 0xa2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xa3 */
        { InstructionId::LDY,   AddressMode::ZERO_PAGE },        /* 0xa4 */
        { InstructionId::LDA,   AddressMode::ZERO_PAGE },        /* 0xa5 */
        { InstructionId::LDX,   AddressMode::ZERO_PAGE },        /* 0xa6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xa7 */
        { InstructionId::TAY,   AddressMode::IMPLICIT },         /* 0xa8 */
        { InstructionId::LDA,   AddressMode::IMMEDIATE },        /* 0xa9 */
        { InstructionId::TAX,   AddressMode::IMPLICIT },         /* 0xaa */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xab */
        { InstructionId::LDY,   AddressMode::ABSOLUTE },         /* 0xac */
        { InstructionId::LDA,   AddressMode::ABSOLUTE },         /* 0xad */
        { InstructionId::LDX,   AddressMode::ABSOLUTE },         /* 0xae */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xaf */
        { InstructionId::BCS,   AddressMode::RELATIVE },         /* 0xb0 */
        { InstructionId::LDA,   AddressMode::INDIRECT_INDEXED }, /* 0xb1 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xb2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xb3 */
        { InstructionId::LDY,   AddressMode::ZERO_PAGE_X },      /* 0xb4 */
        { InstructionId::LDA,   AddressMode::ZERO_PAGE_X },      /* 0xb5 */
        { InstructionId::LDX,   AddressMode::ZERO_PAGE_Y },      /* 0xb6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xb7 */
        { InstructionId::CLV,   AddressMode::IMPLICIT },         /* 0xb8 */
        { InstructionId::LDA,   AddressMode::ABSOLUTE_Y },       /* 0xb9 */
        { InstructionId::TSX,   AddressMode::IMPLICIT },         /* 0xba */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xbb */
        { InstructionId::LDY,   AddressMode::ABSOLUTE_X },       /* 0xbc */
        { InstructionId::LDA,   AddressMode::ABSOLUTE_X },       /* 0xbd */
        { InstructionId::LDX,   AddressMode::ABSOLUTE_Y },       /* 0xbe */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xbf */
        { InstructionId::CPY,   AddressMode::IMMEDIATE },        /* 0xc0 */
        { InstructionId::CMP,   AddressMode::INDEXED_INDIRECT }, /* 0xc1 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xc2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xc3 */
        { InstructionId::CPY,   AddressMode::ZERO_PAGE },        /* 0xc4 */
        { InstructionId::CMP,   AddressMode::ZERO_PAGE },        /* 0xc5 */
        { InstructionId::DEC,   AddressMode::ZERO_PAGE },        /* 0xc6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xc7 */
        { InstructionId::INY,   AddressMode::IMPLICIT },         /* 0xc8 */
        { InstructionId::CMP,   AddressMode::IMMEDIATE },        /* 0xc9 */
        { InstructionId::DEX,   AddressMode::IMPLICIT },         /* 0xca */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xcb */
        { InstructionId::CPY,   AddressMode::ABSOLUTE },         /* 0xcc */
        { InstructionId::CMP,   AddressMode::ABSOLUTE },         /* 0xcd */
        { InstructionId::DEC,   AddressMode::ABSOLUTE },         /* 0xce */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xcf */
        { InstructionId::BNE,   AddressMode::RELATIVE },         /* 0xd0 */
        { InstructionId::CMP,   AddressMode::INDIRECT_INDEXED }, /* 0xd1 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xd2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xd3 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xd4 */
        { InstructionId::CMP,   AddressMode::ZERO_PAGE_X },      /* 0xd5 */
        { InstructionId::DEC,   AddressMode::ZERO_PAGE_X },      /* 0xd6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xd7 */
        { InstructionId::CLD,   AddressMode::IMPLICIT },         /* 0xd8 */
        { InstructionId::CMP,   AddressMode::ABSOLUTE_Y },       /* 0xd9 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xda */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xdb */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xdc */
        { InstructionId::CMP,   AddressMode::ABSOLUTE_X },       /* 0xdd */
        { InstructionId::DEC,   AddressMode::ABSOLUTE_X },       /* 0xde */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xdf */
        { InstructionId::CPX,   AddressMode::IMMEDIATE },        /* 0xe0 */
        { InstructionId::SBC,   AddressMode::INDEXED_INDIRECT }, /* 0xe1 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xe2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xe3 */
        { InstructionId::CPX,   AddressMode::ZERO_PAGE },        /* 0xe4 */
        { InstructionId::SBC,   AddressMode::ZERO_PAGE },        /* 0xe5 */
        { InstructionId::INC,   AddressMode::ZERO_PAGE },        /* 0xe6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xe7 */
        { InstructionId::INX,   AddressMode::IMPLICIT },         /* 0xe8 */
        { InstructionId::SBC,   AddressMode::IMMEDIATE },        /* 0xe9 */
        { InstructionId::NOP,   AddressMode::IMPLICIT },         /* 0xea */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xeb */
        { InstructionId::CPX,   AddressMode::ABSOLUTE },         /* 0xec */
        { InstructionId::SBC,   AddressMode::ABSOLUTE },         /* 0xed */
        { InstructionId::INC,   AddressMode::ABSOLUTE },         /* 0xee */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xef */
        { InstructionId::BEQ,   AddressMode::RELATIVE },         /* 0xf0 */
        { InstructionId::SBC,   AddressMode::INDIRECT_INDEXED }, /* 0xf1 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xf2 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xf3 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xf4 */
        { InstructionId::SBC,   AddressMode::ZERO_PAGE_X },      /* 0xf5 */
        { InstructionId::INC,   AddressMode::ZERO_PAGE_X },      /* 0xf6 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xf7 */
        { InstructionId::SED,   AddressMode::IMPLICIT },         /* 0xf8 */
        { InstructionId::SBC,   AddressMode::ABSOLUTE_Y },       /* 0xf9 */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xfa */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xfb */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xfc */
        { InstructionId::SBC,   AddressMode::ABSOLUTE_X },       /* 0xfd */
        { InstructionId::INC,   AddressMode::ABSOLUTE_X },       /* 0xfe */
        { InstructionId::NONE,  AddressMode::NONE },             /* 0xff */
    }};

    [[nodiscard]] constexpr size_t instructionSize(AddressMode t_mode) {
        const std::array<size_t, ADDRESS_MODE_COUNT> AMOUNT_TABLE {{
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

    class CPUDebugger;

    class CPU {
    public:
        friend CPUDebugger;

        CPU(Address t_programCounter=0x0000U);

        void setController(std::unique_ptr<CPUController> t_controller);

        bool executeInstruction();
        void cycle();
        void printRegisters() const;


    private:
        //----- Defines -----//

        enum class StatusFlag : Word {
            CARRY             = 0b00000001,
            ZERO              = 0b00000010,
            INTERRUPT_DISABLE = 0b00000100,
            DECIMAL           = 0b00001000,
            B1                = 0b00010000,
            B2                = 0b00100000,
            OVERFLOW          = 0b01000000,
            NEGATIVE          = 0b10000000
        };

        class WordReference {
        public:
            WordReference(CPU& t_cpu, Word* t_word);
            WordReference(CPU& t_cpu, Address t_address);

            ~WordReference() = default;

            WordReference(const WordReference&) = delete;
            void operator=(const WordReference&) = delete;

            operator Word() const;
            void operator=(Word t_value);

        private:
            CPU& m_cpu;
            std::variant<Word*, Address> m_location;
        };

        //----- Members -----//
        std::unique_ptr<CPUController> m_controller;

        Address m_pc;
        Word m_sp;
        Word m_acc;
        Word m_x;
        Word m_y;
        Word m_st;

        struct {
            bool irq;
            bool brk;
            bool nmi;
        } m_interruptFlags;

        //----- Private methods -----//
        [[nodiscard]] bool getFlag(StatusFlag t_flag) const;
        void setFlag(StatusFlag t_flag, bool t_value);

        [[nodiscard]] WordReference getWordArgument(AddressMode t_mode);
        [[nodiscard]] Address getAddressArgument(AddressMode t_mode);

        //----- Interrupts -----//
        void generateIRQ();
        void generateBRK();
        void generateNMI();

        void handleInterrupts();

        //----- Stack -----//
        void stackPushWord(Word t_word);
        Word stackPopWord();

        void stackPushDWord(DWord t_dword);
        DWord stackPopDWord();


        //----- Instructions -----//
        void instructionLDA(AddressMode t_addressMode);
        void instructionLDX(AddressMode t_addressMode);
        void instructionLDY(AddressMode t_addressMode);
        void instructionSTA(AddressMode t_addressMode);
        void instructionSTX(AddressMode t_addressMode);
        void instructionSTY(AddressMode t_addressMode);

        void instructionTAX(AddressMode t_addressMode);
        void instructionTAY(AddressMode t_addressMode);
        void instructionTXA(AddressMode t_addressMode);
        void instructionTYA(AddressMode t_addressMode);

        void instructionTSX(AddressMode t_addressMode);
        void instructionTXS(AddressMode t_addressMode);
        void instructionPHA(AddressMode t_addressMode);
        void instructionPHP(AddressMode t_addressMode);
        void instructionPLA(AddressMode t_addressMode);
        void instructionPLP(AddressMode t_addressMode);

        void instructionAND(AddressMode t_addressMode);
        void instructionEOR(AddressMode t_addressMode);
        void instructionORA(AddressMode t_addressMode);
        void instructionBIT(AddressMode t_addressMode);

        void instructionADC(AddressMode t_addressMode);
        void instructionSBC(AddressMode t_addressMode);
        void instructionCMP(AddressMode t_addressMode);
        void instructionCPX(AddressMode t_addressMode);
        void instructionCPY(AddressMode t_addressMode);

        void instructionINC(AddressMode t_addressMode);
        void instructionINX(AddressMode t_addressMode);
        void instructionINY(AddressMode t_addressMode);
        void instructionDEC(AddressMode t_addressMode);
        void instructionDEX(AddressMode t_addressMode);
        void instructionDEY(AddressMode t_addressMode);

        void instructionASL(AddressMode t_addressMode);
        void instructionLSR(AddressMode t_addressMode);
        void instructionROL(AddressMode t_addressMode);
        void instructionROR(AddressMode t_addressMode);

        void instructionJMP(AddressMode t_addressMode);
        void instructionJSR(AddressMode t_addressMode);
        void instructionRTS(AddressMode t_addressMode);

        void instructionBCC(AddressMode t_addressMode);
        void instructionBCS(AddressMode t_addressMode);
        void instructionBEQ(AddressMode t_addressMode);
        void instructionBMI(AddressMode t_addressMode);
        void instructionBNE(AddressMode t_addressMode);
        void instructionBPL(AddressMode t_addressMode);
        void instructionBVC(AddressMode t_addressMode);
        void instructionBVS(AddressMode t_addressMode);

        void instructionCLC(AddressMode t_addressMode);
        void instructionCLD(AddressMode t_addressMode);
        void instructionCLI(AddressMode t_addressMode);
        void instructionCLV(AddressMode t_addressMode);
        void instructionSEC(AddressMode t_addressMode);
        void instructionSED(AddressMode t_addressMode);
        void instructionSEI(AddressMode t_addressMode);

        void instructionBRK(AddressMode t_addressMode);
        void instructionNOP(AddressMode t_addressMode);
        void instructionRTI(AddressMode t_addressMode);
    };

}

#endif
