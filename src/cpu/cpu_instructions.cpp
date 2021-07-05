#include "cpu.hpp"

namespace RNES { 

    bool signsMatch(Word t_v1, Word t_v2) {
        return (t_v1 & 0x80U) == (t_v2 & 0x80U);
    }

    DWord signExtend(Word t_value) {
        return (t_value & 0x80) ? (0xFF00U | t_value) : (0x0000 | t_value);
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
        m_st = stackPopWord() & 0b11001111; // mask out b flags

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
        setFlag(StatusFlag::OVERFLOW, value & 0b01000000);
        setFlag(StatusFlag::NEGATIVE, value & 0b10000000);

        m_pc += instructionSize(t_addressMode);
	}


    void CPU::instructionADC(AddressMode t_addressMode) {
        const Word arg = getWordArgument(t_addressMode);

        if (!getFlag(StatusFlag::DECIMAL)) {
            const Word oldAcc = m_acc;
            const DWord result = m_acc + arg + getFlag(StatusFlag::CARRY);

            m_acc = result & 0x00FFU;

            setFlag(StatusFlag::ZERO, m_acc == 0);
            setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);
            setFlag(StatusFlag::CARRY, result & 0xFF00);

            const bool overflow = (~(oldAcc ^ arg) & (oldAcc ^ m_acc)) & 0x80;
            setFlag(StatusFlag::OVERFLOW, overflow);
        }
        else {
            const Word accLo = (m_acc & 0x0F) >> 0;
            const Word accHi = (m_acc & 0xF0) >> 4;
            const Word argLo = (arg & 0x0F) >> 0;
            const Word argHi = (arg & 0xF0) >> 4;

            
            const Word sumLo = accLo + argLo + getFlag(StatusFlag::CARRY);
            const Word resultLo = sumLo % 10;
            const bool carryLo = (sumLo != resultLo);

            const Word sumHi = accHi + argHi + carryLo;
            const Word resultHi = sumHi % 10;
            const bool carryHi = (sumHi != resultHi);

            const Word result = (resultHi << 4) | (resultLo << 0);

            m_acc = result;

            setFlag(StatusFlag::ZERO, m_acc == 0);
            setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);
            setFlag(StatusFlag::CARRY, carryHi);

            // TODO: figure out what to do for the overflow flag here
        }

        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionSBC(AddressMode t_addressMode) {
        const Word arg = getWordArgument(t_addressMode);

        if (!getFlag(StatusFlag::DECIMAL)) {
            const Word oldAcc = m_acc;
            const Word argComp = ~arg;
            const DWord result = m_acc + argComp + getFlag(StatusFlag::CARRY);

            m_acc = result & 0x00FFU;

            setFlag(StatusFlag::ZERO, m_acc == 0);
            setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);
            setFlag(StatusFlag::CARRY, result & 0xFF00);

            const bool overflow = (~(oldAcc ^ argComp) & (oldAcc ^ m_acc)) & 0x80;
            setFlag(StatusFlag::OVERFLOW, overflow);
        }
        else {
            const Word accLo = (m_acc & 0x0F) >> 0;
            const Word accHi = (m_acc & 0xF0) >> 4;
            const Word argLo = (arg & 0x0F) >> 0;
            const Word argHi = (arg & 0xF0) >> 4;

            const Word diffLo = accLo - argLo - (1 - getFlag(StatusFlag::CARRY));
            const bool borrowLo = !!(diffLo & 0x80);
            const Word resultLo = borrowLo ? (diffLo + 10) : diffLo;

            const Word diffHi = accHi - argHi - borrowLo;
            const bool borrowHi = !!(diffHi & 0x80);
            const Word resultHi = borrowHi ? (diffHi + 10) : diffHi;

            const Word result = (resultHi << 4) | (resultLo << 0);

            m_acc = result;

            setFlag(StatusFlag::ZERO, m_acc == 0);
            setFlag(StatusFlag::NEGATIVE, m_acc & 0x80U);
            setFlag(StatusFlag::CARRY, !borrowHi);

            // TODO: overflow
        }

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
        stackPushDWord(m_pc + instructionSize(t_addressMode) - 1);
        m_pc = getAddressArgument(t_addressMode);
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
        generateBRK();
        m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionNOP(AddressMode t_addressMode) {
		m_pc += instructionSize(t_addressMode);
	}

    void CPU::instructionRTI(AddressMode t_addressMode) {
	    m_st = stackPopWord() & 0b11001111;
        m_pc = stackPopDWord();
	}

}
