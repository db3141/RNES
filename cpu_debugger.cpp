#include <iostream>
#include <iomanip>
#include <string>

#include "cpu.hpp"

namespace Emulator {

    std::array<const char*, 256> DECODED_OPCODES = {{
        "BRK IMPLICIT",
        "ORA INDIRECT",
        "invalid",
        "invalid",
        "invalid",
        "ORA ZERO_PAGE",
        "ASL IMMEDIATE",
        "invalid",
        "PHP IMPLICIT",
        "ORA IMMEDIATE",
        "ASL ACCUMULATOR",
        "invalid",
        "invalid",
        "ORA ABSOLUTE",
        "ASL ABSOLUTE",
        "invalid",
        "BPL RELATIVE",
        "ORA INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "ORA ZERO_PAGE_X",
        "ASL ZERO_PAGE_X",
        "invalid",
        "CLC IMPLICIT",
        "ORA ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "ORA ABSOLUTE_X",
        "ASL ABSOLUTE_X",
        "invalid",
        "JSR ABSOLUTE",
        "AND INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "BIT ZERO_PAGE",
        "AND ZERO_PAGE",
        "ROL ZERO_PAGE",
        "invalid",
        "PLP IMPLICIT",
        "AND IMMEDIATE",
        "ROL ACCUMULATOR",
        "invalid",
        "BIT ABSOLUTE",
        "AND ABSOLUTE",
        "ROL ABSOLUTE",
        "invalid",
        "BMI RELATIVE",
        "AND INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "AND ZERO_PAGE_X",
        "ROL ZERO_PAGE_X",
        "invalid",
        "SEC IMPLICIT",
        "AND ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "AND ABSOLUTE_X",
        "ROL ABSOLUTE_X",
        "invalid",
        "RTI IMPLICIT",
        "EOR INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "invalid",
        "EOR ZERO_PAGE",
        "LSR ZERO_PAGE",
        "invalid",
        "PHA IMPLICIT",
        "EOR IMMEDIATE",
        "LSR ACCUMULATOR",
        "invalid",
        "JMP ABSOLUTE",
        "EOR ABSOLUTE",
        "LSR ABSOLUTE",
        "invalid",
        "BVC RELATIVE",
        "EOR INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "EOR ZERO_PAGE_X",
        "LSR ZERO_PAGE_X",
        "invalid",
        "CLI IMPLICIT",
        "EOR ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "EOR ABSOLUTE_X",
        "LSR ABSOLUTE_X",
        "invalid",
        "RTS IMPLICIT",
        "ADC INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "invalid",
        "ADC ZERO_PAGE",
        "ROR ZERO_PAGE",
        "invalid",
        "PLA IMPLICIT",
        "ADC IMMEDIATE",
        "ROR ACCUMULATOR",
        "invalid",
        "JMP INDIRECT",
        "ADC ABSOLUTE",
        "ROR ABSOLUTE",
        "invalid",
        "BVS RELATIVE",
        "ADC INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "ADC ZERO_PAGE_X",
        "ROR ZERO_PAGE_X",
        "invalid",
        "SEI IMPLICIT",
        "ADC ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "ADC ABSOLUTE_X",
        "ROR ABSOLUTE_X",
        "invalid",
        "invalid",
        "STA INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "STY ZERO_PAGE",
        "STA ZERO_PAGE",
        "STX ZERO_PAGE",
        "invalid",
        "DEY IMPLICIT",
        "invalid",
        "TXA IMPLICIT",
        "invalid",
        "STY ABSOLUTE",
        "STA ABSOLUTE",
        "STX ABSOLUTE",
        "invalid",
        "BCC RELATIVE",
        "STA INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "STY ZERO_PAGE_X",
        "STA ZERO_PAGE_X",
        "STX ZERO_PAGE_Y",
        "invalid",
        "TYA IMPLICIT",
        "STA ABSOLUTE_Y",
        "TXS IMPLICIT",
        "invalid",
        "invalid",
        "STA ABSOLUTE_X",
        "invalid",
        "invalid",
        "LDY IMMEDIATE",
        "LDA INDEXED_INDIRECT",
        "LDX IMMEDIATE",
        "invalid",
        "LDY ZERO_PAGE",
        "LDA ZERO_PAGE",
        "LDX ZERO_PAGE",
        "invalid",
        "TAY IMPLICIT",
        "LDA IMMEDIATE",
        "TAX IMPLICIT",
        "invalid",
        "LDY ABSOLUTE",
        "LDA ABSOLUTE",
        "LDX ABSOLUTE",
        "invalid",
        "BCS RELATIVE",
        "LDA INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "LDY ZERO_PAGE_X",
        "LDA ZERO_PAGE_X",
        "LDX ZERO_PAGE_Y",
        "invalid",
        "CLV IMPLICIT",
        "LDA ABSOLUTE_Y",
        "TSX IMPLICIT",
        "invalid",
        "LDY ABSOLUTE_X",
        "LDA ABSOLUTE_X",
        "LDX ABSOLUTE_Y",
        "invalid",
        "CPY IMMEDIATE",
        "CMP INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "CPY ZERO_PAGE",
        "CMP ZERO_PAGE",
        "DEC ZERO_PAGE",
        "invalid",
        "INY IMPLICIT",
        "CMP IMMEDIATE",
        "DEX IMPLICIT",
        "invalid",
        "CPY ABSOLUTE",
        "CMP ABSOLUTE",
        "DEC ABSOLUTE",
        "invalid",
        "BNE RELATIVE",
        "CMP INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "CMP ZERO_PAGE_X",
        "DEC ZERO_PAGE_X",
        "invalid",
        "CLD IMPLICIT",
        "CMP ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "CMP ABSOLUTE_X",
        "DEC ABSOLUTE_X",
        "invalid",
        "CPX IMMEDIATE",
        "SBC INDEXED_INDIRECT",
        "invalid",
        "invalid",
        "CPX ZERO_PAGE",
        "SBC ZERO_PAGE",
        "INC ZERO_PAGE",
        "invalid",
        "INX IMPLICIT",
        "SBC IMMEDIATE",
        "NOP IMPLICIT",
        "invalid",
        "CPX ABSOLUTE",
        "SBC ABSOLUTE",
        "INC ABSOLUTE",
        "invalid",
        "BEQ RELATIVE",
        "SBC INDIRECT_INDEXED",
        "invalid",
        "invalid",
        "invalid",
        "SBC ZERO_PAGE_X",
        "INC ZERO_PAGE_X",
        "invalid",
        "SED IMPLICIT",
        "SBC ABSOLUTE_Y",
        "invalid",
        "invalid",
        "invalid",
        "SBC ABSOLUTE_X",
        "INC ABSOLUTE_X",
        "invalid"
    }};

//    void handleSimpleCommand(CPU& t_cpu, char t_command);
    void handleCommand(CPU& t_cpu, const std::string& t_command);

    void cpuDebug(CPU& t_cpu) {
        bool done = false;
        
        while (!done) {
            std::cout << "[0x" << std::hex << std::setw(4) << std::setfill('0') << t_cpu.m_pc << "]> ";

            std::string userInput;
            std::cin >> userInput;

            switch(userInput.length()) {
                case 0:
                    std::cout << "[0x" << std::hex << std::setw(4) << std::setfill('0') << t_cpu.m_pc << "]> ";
                    break;
                case 1:
                    handleSimpleCommand(t_cpu, userInput[0]);
                    break;
                default:
                    handleCommand(t_cpu, userInput);
            }
        }    
    }

    void handleSimpleCommand(CPU& t_cpu, char t_command) {
        switch (t_command) {
            case 's':
            case 'n':
                t_cpu.executeInstruction();
                break;
            case 'c':
                while (!t_cpu.executeInstruction());
                break;
            case 'i':
                std::cout << DECODED_OPCODES[t_cpu.m_memory->readWord(t_cpu.m_pc)] << '\n';
                break;
            case 'R':
                t_cpu.printRegisters();
            default:
                break;
        }
    }
    
    void handleCommand(CPU& t_cpu, const std::string& t_command) {
        // TODO
    }


}
