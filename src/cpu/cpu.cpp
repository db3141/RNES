#include <array>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "assert.hpp"
#include "cpu.hpp"

namespace RNES::CPU {

    DWord signExtendWord(Word t_value) {
        if (t_value & 0x80U) {
            return 0xFF00U | t_value;
        }
        else {
            return 0x0000U | t_value;
        }
    }

    CPU::CPU(Address t_programCounter)
        : m_controller(nullptr)
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

    void CPU::setController(std::unique_ptr<CPUController> t_controller) {
        m_controller = std::move(t_controller);
    }

    void CPU::printRegisters() const {
        std::cout
            << "PC:  0x" << std::hex << std::setw(4) << std::setfill('0') << (int)m_pc << " (0x" << (int)m_controller->readWord(m_pc) << ")\n"
            << "SP:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_sp  << '\n'
            << "ACC: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_acc << '\n'
            << "X:   0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_x   << '\n'
            << "Y:   0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_y   << '\n'
            << "ST:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)m_st  << "\n\n";
    }

    bool CPU::executeInstruction() {
        using Instruction = void (CPU::*)(AddressMode);

        static const std::array<Instruction, INSTRUCTION_COUNT + 1> INSTRUCTION_POINTERS {
            &CPU::instructionADC,
            &CPU::instructionAND,
            &CPU::instructionASL,
            &CPU::instructionBCC,
            &CPU::instructionBCS,
            &CPU::instructionBEQ,
            &CPU::instructionBIT,
            &CPU::instructionBMI,
            &CPU::instructionBNE,
            &CPU::instructionBPL,
            &CPU::instructionBRK,
            &CPU::instructionBVC,
            &CPU::instructionBVS,
            &CPU::instructionCLC,
            &CPU::instructionCLD,
            &CPU::instructionCLI,
            &CPU::instructionCLV,
            &CPU::instructionCMP,
            &CPU::instructionCPX,
            &CPU::instructionCPY,
            &CPU::instructionDEC,
            &CPU::instructionDEX,
            &CPU::instructionDEY,
            &CPU::instructionEOR,
            &CPU::instructionINC,
            &CPU::instructionINX,
            &CPU::instructionINY,
            &CPU::instructionJMP,
            &CPU::instructionJSR,
            &CPU::instructionLDA,
            &CPU::instructionLDX,
            &CPU::instructionLDY,
            &CPU::instructionLSR,
            &CPU::instructionNOP,
            &CPU::instructionORA,
            &CPU::instructionPHA,
            &CPU::instructionPHP,
            &CPU::instructionPLA,
            &CPU::instructionPLP,
            &CPU::instructionROL,
            &CPU::instructionROR,
            &CPU::instructionRTI,
            &CPU::instructionRTS,
            &CPU::instructionSBC,
            &CPU::instructionSEC,
            &CPU::instructionSED,
            &CPU::instructionSEI,
            &CPU::instructionSTA,
            &CPU::instructionSTX,
            &CPU::instructionSTY,
            &CPU::instructionTAX,
            &CPU::instructionTAY,
            &CPU::instructionTSX,
            &CPU::instructionTXA,
            &CPU::instructionTXS,
            &CPU::instructionTYA,
            nullptr
        };

        handleInterrupts();

        const Word opcode = m_controller->readWord(m_pc);
        const InstructionInfo info = INSTRUCTION_TABLE[opcode];

        ASSERT(info.id != InstructionId::NONE, "Invalid opcode");

        const Instruction instructionPointer = INSTRUCTION_POINTERS[static_cast<size_t>(info.id)];

        // call the function obtained from the table
        (this->*instructionPointer)(info.addressMode);

        return false;
    }

    void CPU::cycle() {
        // TODO
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

    CPU::WordReference CPU::getWordArgument(AddressMode t_mode) {
        switch(t_mode) {
            case AddressMode::IMPLICIT:
                ASSERT(false, "shouldn't be here");

            case AddressMode::ACCUMULATOR:
                return WordReference(*this, &m_acc);

            case AddressMode::IMMEDIATE:
                return WordReference(*this, m_pc + 1);

            case AddressMode::ZERO_PAGE:
                return WordReference(*this, m_controller->readWord(m_pc + 1));

            case AddressMode::ZERO_PAGE_X:
                return WordReference(*this, (m_controller->readWord(m_pc + 1) + m_x) % 0x0100U);

            case AddressMode::ZERO_PAGE_Y:
                return WordReference(*this, (m_controller->readWord(m_pc + 1) + m_y) % 0x0100U);

            case AddressMode::RELATIVE:
                ASSERT(false, "Relative addressing should use getAddressArgument");

            case AddressMode::ABSOLUTE:
                return WordReference(*this, m_controller->readDWord(m_pc + 1));

            case AddressMode::ABSOLUTE_X:
                return WordReference(*this, m_controller->readDWord(m_pc + 1) + m_x);

            case AddressMode::ABSOLUTE_Y:
                return WordReference(*this, m_controller->readDWord(m_pc + 1) + m_y);

            case AddressMode::INDIRECT:
                ASSERT(false, "Indirect addressing should use getAddressArgument");

            case AddressMode::INDEXED_INDIRECT: {
                const Address zeroPageAddress = (m_controller->readWord(m_pc + 1) + m_x) % 0x0100U;
                return WordReference(*this, m_controller->readDWord(zeroPageAddress));
            }

            case AddressMode::INDIRECT_INDEXED: {
                const Address zeroPageAddress = m_controller->readWord(m_pc + 1);
                return WordReference(*this, m_controller->readDWord(zeroPageAddress) + m_y);
            }

            default:
                ASSERT(false, "Invalid address mode");
        }
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
                return m_controller->readWord(m_pc + 1);

            case AddressMode::ZERO_PAGE_X:
                return (m_controller->readWord(m_pc + 1) + m_x) % 0x0100U;

            case AddressMode::ZERO_PAGE_Y:
                return (m_controller->readWord(m_pc + 1) + m_y) % 0x0100U;

            case AddressMode::RELATIVE:
                return m_pc + signExtendWord(m_controller->readWord(m_pc + 1));

            case AddressMode::ABSOLUTE:
                return m_controller->readDWord(m_pc + 1);

            case AddressMode::ABSOLUTE_X:
                return m_controller->readDWord(m_pc + 1) + m_x;

            case AddressMode::ABSOLUTE_Y:
                return m_controller->readDWord(m_pc + 1) + m_y;

            case AddressMode::INDIRECT:
                return m_controller->readDWord(m_controller->readDWord(m_pc + 1));

            case AddressMode::INDEXED_INDIRECT: {
                const Address zeroPageAddress = (m_controller->readWord(m_pc + 1) + m_x) % 0x0100U;
                return m_controller->readDWord(zeroPageAddress);
            }

            case AddressMode::INDIRECT_INDEXED: {
                const Address zeroPageAddress = m_controller->readWord(m_pc + 1);
                return m_controller->readDWord(zeroPageAddress) + m_y;
            }

            default:
                ASSERT(false, "Invalid address mode");
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

            m_pc = m_controller->readDWord(0xFFFE);
        }
        else if (m_interruptFlags.irq && getFlag(StatusFlag::INTERRUPT_DISABLE)) {
            m_interruptFlags.irq = false;
            ASSERT(false, "TODO: implement this");
        }
    }

    void CPU::stackPushWord(Word t_word) {
        m_controller->writeWord(m_sp + 0x0100U, t_word);
        m_sp--;
    }

    Word CPU::stackPopWord() {
        m_sp++;
        return m_controller->readWord(m_sp + 0x0100U);
    }

    void CPU::stackPushDWord(DWord t_dword) {
        m_controller->writeWord(m_sp + 0x0100U, (t_dword >> 8) & 0x00FFU);
        m_sp--;
        m_controller->writeWord(m_sp + 0x0100U, (t_dword >> 0) & 0x00FFU);
        m_sp--;
    }

    DWord CPU::stackPopDWord() {
        DWord result = 0;

        m_sp++;
        result |= m_controller->readWord(m_sp + 0x0100U) << 0;
        m_sp++;
        result |= m_controller->readWord(m_sp + 0x0100U) << 8;

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
            return m_cpu.m_controller->readWord(*addressPointer);
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
            m_cpu.m_controller->writeWord(*val, t_value);
        }
        else {
            ASSERT(false, "Tried to use invalid WordReference");
        }
    }

}
