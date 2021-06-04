#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>

#include "cpu.hpp"
#include "cpu_debugger.hpp"

namespace Emulator {

    std::vector<std::string> splitString(const std::string& t_string, char t_delimiter); 
    bool stringToDWord(const std::string& t_string, Address& r_address);

    template<typename T>
    void printAsHex(T t_value) {
        std::cout << std::hex << std::setw(sizeof(T) * 2) << std::setfill('0') << static_cast<long long>(t_value);
    }

    CPUDebugger::CPUDebugger(CPU& t_cpu)
        : m_cpu(t_cpu) 
        , m_breakpoints()
        , m_lastCommand("")
    {
        ;
    }

    void CPUDebugger::start() {
        bool done = false;
        
        while (!done) {
            std::cout << "[0x"; printAsHex(m_cpu.m_pc); std::cout << "]> ";

            std::string userInput;
            std::getline(std::cin, userInput);

            switch (executeCommand(userInput)) {
                case CommandReturnCode::OKAY:
                    break;
                case CommandReturnCode::ERROR:
                    break;
                case CommandReturnCode::HALT:
                    done = true;
                    break;
                default:
                    assert(("shouldn't be here", false));
            }
        }    
    }

    CPUDebugger::CommandReturnCode CPUDebugger::executeCommand(const std::string& t_command) {
        static const std::unordered_map<std::string, Command> COMMAND_TABLE = {
            { "s"           , &CPUDebugger::commandStep },
            { "step"        , &CPUDebugger::commandStep },
            { "n"           , &CPUDebugger::commandStep },
            { "next"        , &CPUDebugger::commandStep },

            { "c"           , &CPUDebugger::commandContinue },
            { "continue"    , &CPUDebugger::commandContinue },

            { "i"           , &CPUDebugger::commandDecodeCurrentInstruction },

            { "r"           , &CPUDebugger::commandPrintRegisters },
            { "registers"   , &CPUDebugger::commandPrintRegisters },

            { "q"           , &CPUDebugger::commandQuit },
            { "quit"        , &CPUDebugger::commandQuit },

            { "b"           , &CPUDebugger::commandSetBreakpoint },
            { "break"       , &CPUDebugger::commandSetBreakpoint },

            { "db"          , &CPUDebugger::commandRemoveBreakpoint },
            { "deletebreak" , &CPUDebugger::commandRemoveBreakpoint },

            { "lb"          , &CPUDebugger::commandListBreakpoints },
            { "listbreaks"  , &CPUDebugger::commandListBreakpoints },

            { "d"           , &CPUDebugger::commandDisassembleInstructions },
            { "disassemble" , &CPUDebugger::commandDisassembleInstructions },

            { "x"           , &CPUDebugger::commandExamineAddress },
            { "examine"     , &CPUDebugger::commandExamineAddress },
        };

        //std::cout << "Command: " << t_command << " (" << t_command.length() << ")\n";
        if (t_command.length() == 0) {
            if (m_lastCommand.length() == 0) {
                return CommandReturnCode::OKAY;
            }
            else {
                return executeCommand(m_lastCommand);
            }
        }

        m_lastCommand = t_command;

        std::vector<std::string> arguments = splitString(t_command, ' ');
        assert(("arguments list should not be empty", arguments.size() != 0));

        const std::string command = arguments[0];
        arguments.erase(arguments.begin());
        
        auto commandIt = COMMAND_TABLE.find(command);
        if (commandIt != COMMAND_TABLE.end()) {
            const Command commandFunction = commandIt->second;
            return (this->*commandFunction)(arguments);
        }
        else {
            std::cerr << '\'' << command << "' is not a valid command\n";
            return CommandReturnCode::ERROR;
        }
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandStep(const std::vector<std::string>& t_args) {
        const bool result = m_cpu.executeInstruction();
        if (!result) {
            return CommandReturnCode::OKAY;
        }
        else {
            return CommandReturnCode::HALT;
        }
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandContinue(const std::vector<std::string>& t_args) {
        bool done = false;
        while (!done) {
            std::cout << "0x"; printAsHex(m_cpu.m_pc); std::cout << ": ";
            printDisassembledInstruction(m_cpu.m_pc);

            const Address previousPC = m_cpu.m_pc;
            if (m_cpu.executeInstruction()) {
                std::cout << "CPU halt\n";
                return CommandReturnCode::HALT;
            }
            if (m_cpu.m_pc == previousPC) {
                std::cerr << "Infinite loop detected\n";
                return CommandReturnCode::ERROR;
            }
            done = m_breakpoints.contains(m_cpu.m_pc);
        };
        std::cout << "Hit breakpoint\n";
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandDecodeCurrentInstruction(const std::vector<std::string>& t_args) {
        printDisassembledInstruction(m_cpu.m_pc);
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandPrintRegisters(const std::vector<std::string>& t_args) {
        // TODO: implement this here instead of in CPU
        m_cpu.printRegisters();
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandQuit(const std::vector<std::string>& t_args) {
        std::cout << "Do you really want to quit? (y/n)\n";
        std::string input;
        std::getline(std::cin, input);
        while (input != "y" && input != "n") {
            std::cout << "Please enter either 'y' or 'n'\n";
            std::getline(std::cin, input);
        }

        if (input == "y") {
            return CommandReturnCode::HALT;
        }
        else {
            return CommandReturnCode::OKAY;
        }
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandSetBreakpoint(const std::vector<std::string>& t_args) {
        if (t_args.size() != 1) {
            std::cerr << "This command takes 1 argument\n";
            return CommandReturnCode::ERROR;
        }

        Address breakpointAddress = 0;
        if (!stringToDWord(t_args[0], breakpointAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        m_breakpoints.insert(breakpointAddress);
        std::cout << "Added breakpoint at 0x"; printAsHex(breakpointAddress); std::cout << '\n';
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandRemoveBreakpoint(const std::vector<std::string>& t_args) {
        if (t_args.size() != 1) {
            std::cerr << "This command takes 1 argument\n";
            return CommandReturnCode::ERROR;
        }

        Address breakpointAddress = 0;
        if (!stringToDWord(t_args[0], breakpointAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        m_breakpoints.erase(breakpointAddress);
        std::cout << "Deleted breakpoint at 0x"; printAsHex(breakpointAddress); std::cout << '\n';
        return CommandReturnCode::ERROR;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandListBreakpoints(const std::vector<std::string>& t_args) {
        if (m_breakpoints.empty()) {
            std::cout << "No breakpoints are currently set\n";
            return CommandReturnCode::OKAY;   
        }

        std::cout << "\nBreakpoints\n-----------\n";
        for (Address breakpoint : m_breakpoints) {
            std::cout << "0x"; printAsHex(breakpoint); std::cout << '\n';
        }
        std::cout << '\n';
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandDisassembleInstructions(const std::vector<std::string>& t_args) {
        if (t_args.size() != 1 && t_args.size() != 2) {
            std::cerr << "This command takes 1 or 2 arguments\n";
            return CommandReturnCode::ERROR;
        }

        Address instructionAddress = 0;
        if (!stringToDWord(t_args[0], instructionAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        if (t_args.size() == 1) {
            std::cout << "0x"; printAsHex(instructionAddress); std::cout << ": ";
            printDisassembledInstruction(instructionAddress);
            return CommandReturnCode::OKAY;
        }
        else {
            DWord instructionCount = 0;
            if (!stringToDWord(t_args[1], instructionCount)) {
                std::cerr << "Argument is not a number\n";
                return CommandReturnCode::ERROR;
            }

            if (instructionCount == 0) {
                std::cerr << "Instruction count must be greater than 0\n";
                return CommandReturnCode::ERROR;
            }

            std::cout << '\n';
            Address currentAddress = instructionAddress;
            for (size_t i = 0; i < instructionCount; i++) {
                std::cout << "0x"; printAsHex(currentAddress); std::cout << ": ";
                currentAddress += printDisassembledInstruction(currentAddress);
            }
            std::cout << '\n';
            return CommandReturnCode::OKAY;
        }
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandExamineAddress(const std::vector<std::string>& t_args) {
        static const size_t BYTES_PER_LINE = 16;

        if (t_args.size() != 1 && t_args.size() != 2) {
            std::cerr << "This command takes 1 or 2 arguments\n";
            return CommandReturnCode::ERROR;
        }

        Address address = 0;
        if (!stringToDWord(t_args[0], address)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        DWord numberOfBytes = 0;
        if (t_args.size() == 1) {
            numberOfBytes = 1;
        }
        else {
            if (!stringToDWord(t_args[1], numberOfBytes)) {
                std::cerr << "Argument is not a number\n";
                return CommandReturnCode::ERROR;
            }

            if (numberOfBytes == 0) {
                std::cerr << "Number of bytes to read must be greater than 0\n";
                return CommandReturnCode::ERROR;
            }
        }
        for (size_t i = 0; i < numberOfBytes / BYTES_PER_LINE; i++) {
            std::cout << "0x"; printAsHex(static_cast<Address>(address + i * BYTES_PER_LINE)); std::cout << ": ";

            for (size_t j = 0; j < BYTES_PER_LINE; j++) {
                printAsHex(m_cpu.m_memory->readWord(static_cast<Address>(address + (i * BYTES_PER_LINE) + j))); std::cout << ' ';
            }
            std::cout << '\n';
        }

        const size_t numberOfLeftoverBytes = numberOfBytes % BYTES_PER_LINE;
        if (numberOfLeftoverBytes != 0) {
            const Address startAddress = address + (numberOfBytes - numberOfLeftoverBytes);

            std::cout << "0x"; printAsHex(startAddress); std::cout << ": ";
            for (size_t i = 0; i < numberOfLeftoverBytes; i++) {
                printAsHex(m_cpu.m_memory->readWord(static_cast<Address>(startAddress + i))); std::cout << ' ';
            }
        }
        std::cout << '\n';

        return CommandReturnCode::OKAY;
    }

    size_t CPUDebugger::printDisassembledInstruction(Address t_address) const {
        struct InstructionInfo {
            const char* name;
            CPU::AddressMode mode;
        };

        static const std::array<size_t, CPU::ADDRESS_MODE_COUNT> AMOUNT_TABLE {{
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

        static std::array<InstructionInfo, 256> DECODED_OPCODES = {{
            { "BRK", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::ZERO_PAGE },
            { "ASL", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "PHP", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::IMMEDIATE },
            { "ASL", CPU::AddressMode::ACCUMULATOR },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::ABSOLUTE },
            { "ASL", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BPL", CPU::AddressMode::RELATIVE },
            { "ORA", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::ZERO_PAGE_X },
            { "ASL", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CLC", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ORA", CPU::AddressMode::ABSOLUTE_X },
            { "ASL", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "JSR", CPU::AddressMode::ABSOLUTE },
            { "AND", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BIT", CPU::AddressMode::ZERO_PAGE },
            { "AND", CPU::AddressMode::ZERO_PAGE },
            { "ROL", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "PLP", CPU::AddressMode::IMPLICIT },
            { "AND", CPU::AddressMode::IMMEDIATE },
            { "ROL", CPU::AddressMode::ACCUMULATOR },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BIT", CPU::AddressMode::ABSOLUTE },
            { "AND", CPU::AddressMode::ABSOLUTE },
            { "ROL", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BMI", CPU::AddressMode::RELATIVE },
            { "AND", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "AND", CPU::AddressMode::ZERO_PAGE_X },
            { "ROL", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "SEC", CPU::AddressMode::IMPLICIT },
            { "AND", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "AND", CPU::AddressMode::ABSOLUTE_X },
            { "ROL", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "RTI", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::ZERO_PAGE },
            { "LSR", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "PHA", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::IMMEDIATE },
            { "LSR", CPU::AddressMode::ACCUMULATOR },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "JMP", CPU::AddressMode::ABSOLUTE },
            { "EOR", CPU::AddressMode::ABSOLUTE },
            { "LSR", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BVC", CPU::AddressMode::RELATIVE },
            { "EOR", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::ZERO_PAGE_X },
            { "LSR", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CLI", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "EOR", CPU::AddressMode::ABSOLUTE_X },
            { "LSR", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "RTS", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::ZERO_PAGE },
            { "ROR", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "PLA", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::IMMEDIATE },
            { "ROR", CPU::AddressMode::ACCUMULATOR },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "JMP", CPU::AddressMode::INDIRECT },
            { "ADC", CPU::AddressMode::ABSOLUTE },
            { "ROR", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BVS", CPU::AddressMode::RELATIVE },
            { "ADC", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::ZERO_PAGE_X },
            { "ROR", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "SEI", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "ADC", CPU::AddressMode::ABSOLUTE_X },
            { "ROR", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "STA", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "STY", CPU::AddressMode::ZERO_PAGE },
            { "STA", CPU::AddressMode::ZERO_PAGE },
            { "STX", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "DEY", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "TXA", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "STY", CPU::AddressMode::ABSOLUTE },
            { "STA", CPU::AddressMode::ABSOLUTE },
            { "STX", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BCC", CPU::AddressMode::RELATIVE },
            { "STA", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "STY", CPU::AddressMode::ZERO_PAGE_X },
            { "STA", CPU::AddressMode::ZERO_PAGE_X },
            { "STX", CPU::AddressMode::ZERO_PAGE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "TYA", CPU::AddressMode::IMPLICIT },
            { "STA", CPU::AddressMode::ABSOLUTE_Y },
            { "TXS", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "STA", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "LDY", CPU::AddressMode::IMMEDIATE },
            { "LDA", CPU::AddressMode::INDEXED_INDIRECT },
            { "LDX", CPU::AddressMode::IMMEDIATE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "LDY", CPU::AddressMode::ZERO_PAGE },
            { "LDA", CPU::AddressMode::ZERO_PAGE },
            { "LDX", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "TAY", CPU::AddressMode::IMPLICIT },
            { "LDA", CPU::AddressMode::IMMEDIATE },
            { "TAX", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "LDY", CPU::AddressMode::ABSOLUTE },
            { "LDA", CPU::AddressMode::ABSOLUTE },
            { "LDX", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BCS", CPU::AddressMode::RELATIVE },
            { "LDA", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "LDY", CPU::AddressMode::ZERO_PAGE_X },
            { "LDA", CPU::AddressMode::ZERO_PAGE_X },
            { "LDX", CPU::AddressMode::ZERO_PAGE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CLV", CPU::AddressMode::IMPLICIT },
            { "LDA", CPU::AddressMode::ABSOLUTE_Y },
            { "TSX", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "LDY", CPU::AddressMode::ABSOLUTE_X },
            { "LDA", CPU::AddressMode::ABSOLUTE_X },
            { "LDX", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPY", CPU::AddressMode::IMMEDIATE },
            { "CMP", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPY", CPU::AddressMode::ZERO_PAGE },
            { "CMP", CPU::AddressMode::ZERO_PAGE },
            { "DEC", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INY", CPU::AddressMode::IMPLICIT },
            { "CMP", CPU::AddressMode::IMMEDIATE },
            { "DEX", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPY", CPU::AddressMode::ABSOLUTE },
            { "CMP", CPU::AddressMode::ABSOLUTE },
            { "DEC", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BNE", CPU::AddressMode::RELATIVE },
            { "CMP", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CMP", CPU::AddressMode::ZERO_PAGE_X },
            { "DEC", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CLD", CPU::AddressMode::IMPLICIT },
            { "CMP", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CMP", CPU::AddressMode::ABSOLUTE_X },
            { "DEC", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPX", CPU::AddressMode::IMMEDIATE },
            { "SBC", CPU::AddressMode::INDEXED_INDIRECT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPX", CPU::AddressMode::ZERO_PAGE },
            { "SBC", CPU::AddressMode::ZERO_PAGE },
            { "INC", CPU::AddressMode::ZERO_PAGE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INX", CPU::AddressMode::IMPLICIT },
            { "SBC", CPU::AddressMode::IMMEDIATE },
            { "NOP", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "CPX", CPU::AddressMode::ABSOLUTE },
            { "SBC", CPU::AddressMode::ABSOLUTE },
            { "INC", CPU::AddressMode::ABSOLUTE },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "BEQ", CPU::AddressMode::RELATIVE },
            { "SBC", CPU::AddressMode::INDIRECT_INDEXED },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "SBC", CPU::AddressMode::ZERO_PAGE_X },
            { "INC", CPU::AddressMode::ZERO_PAGE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "SED", CPU::AddressMode::IMPLICIT },
            { "SBC", CPU::AddressMode::ABSOLUTE_Y },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "INVALID", CPU::AddressMode::IMPLICIT },
            { "SBC", CPU::AddressMode::ABSOLUTE_X },
            { "INC", CPU::AddressMode::ABSOLUTE_X },
            { "INVALID", CPU::AddressMode::IMPLICIT }
        }};

        const InstructionInfo instructionInfo = DECODED_OPCODES[m_cpu.m_memory->readWord(t_address)];
        std::cout << instructionInfo.name << ' ';
        switch (instructionInfo.mode) {
            case CPU::AddressMode::IMPLICIT:
                break;

            case CPU::AddressMode::ACCUMULATOR:
                std::cout << "A";
                break;

            case CPU::AddressMode::IMMEDIATE:
                std::cout << "#$"; printAsHex(m_cpu.m_memory->readWord(t_address + 1));
                break;

            case CPU::AddressMode::ZERO_PAGE:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readWord(t_address + 1));
                break;

            case CPU::AddressMode::ZERO_PAGE_X:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readWord(t_address + 1)); std::cout << ",X";
                break;

            case CPU::AddressMode::ZERO_PAGE_Y:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readWord(t_address + 1)); std::cout << ",Y";
                break;

            case CPU::AddressMode::RELATIVE:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readWord(t_address + 1));
                break;

            case CPU::AddressMode::ABSOLUTE:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readDWord(t_address + 1));
                break;

            case CPU::AddressMode::ABSOLUTE_X:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readDWord(t_address + 1)); std::cout << ",X";
                break;

            case CPU::AddressMode::ABSOLUTE_Y:
                std::cout << "$"; printAsHex(m_cpu.m_memory->readDWord(t_address + 1)); std::cout << ",Y";
                break;

            case CPU::AddressMode::INDIRECT:
                std::cout << "($"; printAsHex(m_cpu.m_memory->readDWord(t_address + 1)); std::cout << ")";
                break;

            case CPU::AddressMode::INDEXED_INDIRECT:
                std::cout << "($"; printAsHex(m_cpu.m_memory->readWord(t_address + 1)); std::cout << ",X)";
                break;

            case CPU::AddressMode::INDIRECT_INDEXED:
                std::cout << "($"; printAsHex(m_cpu.m_memory->readWord(t_address + 1)); std::cout << "),Y";
                break;

            default:
                assert(("Should not be here", false));
        }
        std::cout << '\n';
        return AMOUNT_TABLE[static_cast<size_t>(instructionInfo.mode)];
    }

    std::vector<std::string> splitString(const std::string& t_string, char t_delimiter) {
        std::vector<std::string> tokens;

        size_t current = 0;
        while (current < t_string.length()) {
            const size_t next = t_string.find(t_delimiter, current);
            if (next == std::string::npos) {
                tokens.push_back(t_string.substr(current, next));
                current = next;
            }
            else {
                tokens.push_back(t_string.substr(current, next - current));
                current = next + 1;
            }
        }

        return tokens;
    }

    bool stringToDWord(const std::string& t_string, Address& r_address) {
        try {
            if (t_string.starts_with("0x")) {
                r_address = std::stoul(t_string, nullptr, 16);
            }
            else {
                r_address = std::stoul(t_string, nullptr, 10);
            }
            return true;
        }
        catch (std::invalid_argument& e) {
            return false;
        }
    }

}
