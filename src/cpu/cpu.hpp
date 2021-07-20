#ifndef RNES_CPU_6502_INCLUDED
#define RNES_CPU_6502_INCLUDED

#include <variant>
#include <set>
#include <memory>
#include <initializer_list>

#include "cpu_controller.hpp"

namespace RNES {

    class CPUDebugger;

    class CPU {
    public:
        friend CPUDebugger;

        CPU(Address t_programCounter=0x0000U);

        void setController(std::unique_ptr<CPUController> t_controller);

        bool executeInstruction();
        void printRegisters() const;

    private:
        //----- Defines -----//
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

        [[nodiscard]] size_t instructionSize(AddressMode t_mode) const;

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
