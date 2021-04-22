#include <iostream>
#include <iomanip>

#include "cpu.hpp"

namespace Emulator { 

    bool signsMatch(Word t_v1, Word t_v2) {
        return (t_v1 & 0x80U) == (t_v2 & 0x80U);
    }

	void CPU::instructionLDA(AddressMode t_addressMode) {
		const Word value = getWordArgument(t_addressMode);
	    m_acc = value;

	    setFlag(StatusFlag::ZERO, m_acc == 0);
	    setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

	    m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionLDX(AddressMode t_addressMode) {
		const Word value = getWordArgument(t_addressMode);
        m_x = value;

        setFlag(StatusFlag::ZERO, m_x == 0);
        setFlag(StatusFlag::NEGATIVE, m_x & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionLDY(AddressMode t_addressMode) {
        const Word value = getWordArgument(t_addressMode);
        m_y = value;

        setFlag(StatusFlag::ZERO, m_y == 0);
        setFlag(StatusFlag::NEGATIVE, m_y & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSTA(AddressMode t_addressMode) {
		WordReference word = getWordArgument(t_addressMode);
        word = m_acc;

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSTX(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
        word = m_x;

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSTY(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
        word = m_y;

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionTAX(AddressMode t_addressMode) {
		m_x = m_acc;

        setFlag(StatusFlag::ZERO, m_x == 0);
        setFlag(StatusFlag::NEGATIVE, m_x & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionTAY(AddressMode t_addressMode) {
        m_y = m_acc;

        setFlag(StatusFlag::ZERO, m_y == 0);
        setFlag(StatusFlag::NEGATIVE, m_y & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionTXA(AddressMode t_addressMode) {
        m_acc = m_x;

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionTYA(AddressMode t_addressMode) {
        m_acc = m_y;

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionTSX(AddressMode t_addressMode) {
        m_x = m_sp;

        setFlag(StatusFlag::ZERO, m_x == 0);
        setFlag(StatusFlag::NEGATIVE, m_x & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionTXS(AddressMode t_addressMode) {
        m_sp = m_x;

        setFlag(StatusFlag::ZERO, m_sp == 0);
        setFlag(StatusFlag::NEGATIVE, m_sp & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionPHA(AddressMode t_addressMode) {
        stackPushWord(m_acc);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionPHP(AddressMode t_addressMode) {
        const Word value = m_st | Word(StatusFlag::B1) | Word(StatusFlag::B2);
        stackPushWord(value);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionPLA(AddressMode t_addressMode) {
        m_acc = stackPopWord();

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionPLP(AddressMode t_addressMode) {
        m_st = stackPopWord();

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionAND(AddressMode t_addressMode) {
		const Word value = getWordArgument(t_addressMode);
        m_acc &= value;

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionEOR(AddressMode t_addressMode) {
        const Word value = getWordArgument(t_addressMode);
        m_acc ^= value;

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);
        
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionORA(AddressMode t_addressMode) {
        const Word value = getWordArgument(t_addressMode);
        m_acc |= value;

        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBIT(AddressMode t_addressMode) {
		const Word value = getWordArgument(t_addressMode);
        const Word result = m_acc & value;

        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::OVERFLOW, result & 0x40U);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionADC(AddressMode t_addressMode) {
        const Word v1 = m_acc;
		const Word v2 = getWordArgument(t_addressMode);

        const Word v3 = v1 + getFlag(StatusFlag::CARRY);

        const DWord result = v2 + v3;
        m_acc = result & 0x00FFU;

        setFlag(StatusFlag::CARRY, result > 0xFFU);
        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        const bool overflow =
            ((v1 == 0x7F) && getFlag(StatusFlag::CARRY)) ||
            (signsMatch(v2, v3) && !signsMatch(v2, m_acc));
        setFlag(StatusFlag::OVERFLOW, overflow);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSBC(AddressMode t_addressMode) {
        const Word v1 = m_acc;
        const Word v2 = getWordArgument(t_addressMode);

        const Word v3 = v1 - !getFlag(StatusFlag::CARRY);

        const DWord result = v3 - v2;
        m_acc = result & 0x00FFU;

        setFlag(StatusFlag::CARRY, result > 0xFFU);
        setFlag(StatusFlag::ZERO, m_acc == 0);
        setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);

        const bool overflow =
            ((v1 == 0) && !getFlag(StatusFlag::CARRY)) ||
            (!signsMatch(v2, v3) && !signsMatch(v3, m_acc));
        setFlag(StatusFlag::OVERFLOW, overflow);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCMP(AddressMode t_addressMode) {
        const Word v1 = m_acc;
		const Word v2 = getWordArgument(t_addressMode);

        const Word result = v1 - v2;

        setFlag(StatusFlag::CARRY, v1 >= v2);
        setFlag(StatusFlag::ZERO, v1 == v2);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCPX(AddressMode t_addressMode) {
        const Word v1 = m_x;
		const Word v2 = getWordArgument(t_addressMode);

        const Word result = v1 - v2;

        setFlag(StatusFlag::CARRY, v1 >= v2);
        setFlag(StatusFlag::ZERO, v1 == v2);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCPY(AddressMode t_addressMode) {
        const Word v1 = m_y;
		const Word v2 = getWordArgument(t_addressMode);

        const Word result = v1 - v2;

        setFlag(StatusFlag::CARRY, v1 >= v2);
        setFlag(StatusFlag::ZERO, v1 == v2);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionINC(AddressMode t_addressMode) {
		WordReference word = getWordArgument(t_addressMode);
        const Word result = word + 1;

        word = result;

        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionINX(AddressMode t_addressMode) {
		m_x++;

        setFlag(StatusFlag::ZERO, m_x == 0);
        setFlag(StatusFlag::NEGATIVE, m_x & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionINY(AddressMode t_addressMode) {
        m_y++;

        setFlag(StatusFlag::ZERO, m_y == 0);
        setFlag(StatusFlag::NEGATIVE, m_y & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionDEC(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
        const Word result = word - 1;

        word = result;

        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionDEX(AddressMode t_addressMode) {
        m_x--;

        setFlag(StatusFlag::ZERO, m_x == 0);
        setFlag(StatusFlag::NEGATIVE, m_x & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionDEY(AddressMode t_addressMode) {
        m_y--;

        setFlag(StatusFlag::ZERO, m_y == 0);
        setFlag(StatusFlag::NEGATIVE, m_y & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionASL(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
		const Word value = word;
        const Word result = value << 1;

        word = result;

        setFlag(StatusFlag::CARRY, value & 0x80U);
        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionLSR(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
		const Word value = word;
        const Word result = value >> 1;

        word = result;

        setFlag(StatusFlag::CARRY, value & 0x01U);
        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionROL(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
		const Word value = word;
        const Word result = (value << 1) | getFlag(StatusFlag::CARRY);

        word = result;

        setFlag(StatusFlag::CARRY, value & 0x80U);
        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionROR(AddressMode t_addressMode) {
        WordReference word = getWordArgument(t_addressMode);
		const Word value = word;
        const Word result = (value >> 1) | (getFlag(StatusFlag::CARRY) << 7);

        word = result;

        setFlag(StatusFlag::CARRY, value & 0x01U);
        setFlag(StatusFlag::ZERO, result == 0);
        setFlag(StatusFlag::NEGATIVE, result & 0x80U);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionJMP(AddressMode t_addressMode) {
		const Address address = getAddressArgument(t_addressMode);
        m_pc = address;
	}

    void CPU::instructionJSR(AddressMode t_addressMode) {
		const Address address = getAddressArgument(t_addressMode);
        stackPushDWord(address);

        m_pc = address;
	}

    void CPU::instructionRTS(AddressMode t_addressMode) {
        m_pc = stackPopDWord() + instructionSize(t_addressMode);
	}


    void CPU::instructionBCC(AddressMode t_addressMode) {
		const Address branchAddress = getAddressArgument(t_addressMode);

        if (!getFlag(StatusFlag::CARRY)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBCS(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (getFlag(StatusFlag::CARRY)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBEQ(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (getFlag(StatusFlag::ZERO)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBMI(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (getFlag(StatusFlag::NEGATIVE)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBNE(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (!getFlag(StatusFlag::ZERO)) {
            m_pc = branchAddress;
        }

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBPL(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (!getFlag(StatusFlag::NEGATIVE)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBVC(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (!getFlag(StatusFlag::OVERFLOW)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionBVS(AddressMode t_addressMode) {
        const Address branchAddress = getAddressArgument(t_addressMode);

        if (getFlag(StatusFlag::OVERFLOW)) {
            m_pc = branchAddress;
        }
        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionCLC(AddressMode t_addressMode) {
		setFlag(StatusFlag::CARRY, 0);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCLD(AddressMode t_addressMode) {
		setFlag(StatusFlag::DECIMAL, 0);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCLI(AddressMode t_addressMode) {
		setFlag(StatusFlag::INTERRUPT_DISABLE, 0);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionCLV(AddressMode t_addressMode) {
		setFlag(StatusFlag::OVERFLOW, 0);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSEC(AddressMode t_addressMode) {
		setFlag(StatusFlag::CARRY, 1);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSED(AddressMode t_addressMode) {
		setFlag(StatusFlag::DECIMAL, 1);
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSEI(AddressMode t_addressMode) {
		setFlag(StatusFlag::INTERRUPT_DISABLE, 1);
        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionBRK(AddressMode t_addressMode) {
		stackPushDWord(m_pc);
        stackPushWord(m_st | 0b00110000); // Set b flags

        m_pc = m_memory->readDWord(0xFFFE);
	}

    void CPU::instructionNOP(AddressMode t_addressMode) {
		m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionRTI(AddressMode t_addressMode) {
	    m_st = stackPopWord() & 0b11001111;
        m_pc = stackPopDWord();
	}

}
