#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>

#include "assert.hpp"
#include "cpu.hpp"
#include "cpu_debugger.hpp"

namespace RNES::CPU {

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
                    ASSERT(false, "shouldn't be here");
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

        m_lastCommand = t_command;
        const std::vector<std::string> arguments = splitString(t_command, ' ');

        if (arguments.size() == 0) {
            std::cerr << '\'' << t_command << "' is not a valid command\n";
            return CommandReturnCode::ERROR;
        }

        const auto commandIt = COMMAND_TABLE.find(arguments[0]);
        if (commandIt == COMMAND_TABLE.end()) {
            std::cerr << '\'' << arguments[0] << "' is not a valid command\n";
            return CommandReturnCode::ERROR;
        }

        const Command commandFunction = commandIt->second;
        return (this->*commandFunction)(arguments.begin() + 1, arguments.end());
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandStep(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

        const bool result = m_cpu.executeInstruction();
        if (!result) {
            return CommandReturnCode::OKAY;
        }
        else {
            return CommandReturnCode::HALT;
        }
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandContinue(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

        bool done = false;
        while (!done) {
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

    CPUDebugger::CommandReturnCode CPUDebugger::commandDecodeCurrentInstruction(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

        printDisassembledInstruction(m_cpu.m_pc);
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandPrintRegisters(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

        // TODO: implement this here instead of in CPU
        m_cpu.printRegisters();
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandQuit(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

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

    CPUDebugger::CommandReturnCode CPUDebugger::commandSetBreakpoint(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 1) {
            std::cerr << "This command takes 1 argument\n";
            return CommandReturnCode::ERROR;
        }

        Address breakpointAddress = 0;
        if (!stringToDWord(*t_argsBegin, breakpointAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        m_breakpoints.insert(breakpointAddress);
        std::cout << "Added breakpoint at 0x"; printAsHex(breakpointAddress); std::cout << '\n';
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandRemoveBreakpoint(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 1) {
            std::cerr << "This command takes 1 argument\n";
            return CommandReturnCode::ERROR;
        }

        Address breakpointAddress = 0;
        if (!stringToDWord(*t_argsBegin, breakpointAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        m_breakpoints.erase(breakpointAddress);
        std::cout << "Deleted breakpoint at 0x"; printAsHex(breakpointAddress); std::cout << '\n';
        return CommandReturnCode::ERROR;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandListBreakpoints(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        if (std::distance(t_argsBegin, t_argsEnd) != 0) {
            std::cerr << "This command takes 0 arguments\n";
            return CommandReturnCode::ERROR;
        }

        if (m_breakpoints.empty()) {
            std::cout << "No breakpoints are currently set\n";
            return CommandReturnCode::OKAY;
        }

        std::cout << "\nBreakpoints\n-----------\n";
        for (const Address breakpoint : m_breakpoints) {
            std::cout << "0x"; printAsHex(breakpoint); std::cout << '\n';
        }
        std::cout << '\n';
        return CommandReturnCode::OKAY;
    }

    CPUDebugger::CommandReturnCode CPUDebugger::commandDisassembleInstructions(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        const size_t argsCount = std::distance(t_argsBegin, t_argsEnd);
        if (argsCount != 1 && argsCount != 2) {
            std::cerr << "This command takes 1 or 2 arguments\n";
            return CommandReturnCode::ERROR;
        }

        Address instructionAddress = 0;
        if (!stringToDWord(*t_argsBegin, instructionAddress)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        if (argsCount == 1) {
            std::cout << "0x"; printAsHex(instructionAddress); std::cout << ": ";
            printDisassembledInstruction(instructionAddress);
            return CommandReturnCode::OKAY;
        }
        else {
            DWord instructionCount = 0;
            if (!stringToDWord(*(t_argsBegin + 1), instructionCount)) {
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

    CPUDebugger::CommandReturnCode CPUDebugger::commandExamineAddress(CArgumentIterator t_argsBegin, CArgumentIterator t_argsEnd) {
        static const size_t BYTES_PER_LINE = 16;

        const size_t argsCount = std::distance(t_argsBegin, t_argsEnd);
        if (argsCount != 1 && argsCount != 2) {
            std::cerr << "This command takes 1 or 2 arguments\n";
            return CommandReturnCode::ERROR;
        }

        Address address = 0;
        if (!stringToDWord(*t_argsBegin, address)) {
            std::cerr << "Argument is not a valid address\n";
            return CommandReturnCode::ERROR;
        }

        DWord numberOfBytes = 0;
        if (argsCount == 1) {
            numberOfBytes = 1;
        }
        else {
            if (!stringToDWord(*(t_argsBegin + 1), numberOfBytes)) {
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
                printAsHex(m_cpu.m_controller->readWord(static_cast<Address>(address + (i * BYTES_PER_LINE) + j))); std::cout << ' ';
            }
            std::cout << '\n';
        }

        const size_t numberOfLeftoverBytes = numberOfBytes % BYTES_PER_LINE;
        if (numberOfLeftoverBytes != 0) {
            const Address startAddress = address + (numberOfBytes - numberOfLeftoverBytes);

            std::cout << "0x"; printAsHex(startAddress); std::cout << ": ";
            for (size_t i = 0; i < numberOfLeftoverBytes; i++) {
                printAsHex(m_cpu.m_controller->readWord(static_cast<Address>(startAddress + i))); std::cout << ' ';
            }
        }
        std::cout << '\n';

        return CommandReturnCode::OKAY;
    }

    size_t CPUDebugger::printDisassembledInstruction(Address t_address) const {
        static const std::array<const char*, INSTRUCTION_COUNT + 1> INSTRUCTION_STRINGS {
            "ADC",
            "AND",
            "ASL",
            "BCC",
            "BCS",
            "BEQ",
            "BIT",
            "BMI",
            "BNE",
            "BPL",
            "BRK",
            "BVC",
            "BVS",
            "CLC",
            "CLD",
            "CLI",
            "CLV",
            "CMP",
            "CPX",
            "CPY",
            "DEC",
            "DEX",
            "DEY",
            "EOR",
            "INC",
            "INX",
            "INY",
            "JMP",
            "JSR",
            "LDA",
            "LDX",
            "LDY",
            "LSR",
            "NOP",
            "ORA",
            "PHA",
            "PHP",
            "PLA",
            "PLP",
            "ROL",
            "ROR",
            "RTI",
            "RTS",
            "SBC",
            "SEC",
            "SED",
            "SEI",
            "STA",
            "STX",
            "STY",
            "TAX",
            "TAY",
            "TSX",
            "TXA",
            "TXS",
            "TYA",
            "INVALID"
        };

        const InstructionInfo instructionInfo = INSTRUCTION_TABLE[m_cpu.m_controller->readWord(t_address)];
        std::cout << INSTRUCTION_STRINGS[static_cast<size_t>(instructionInfo.id)] << ' ';
        switch (instructionInfo.addressMode) {
            case AddressMode::IMPLICIT:
                break;

            case AddressMode::ACCUMULATOR:
                std::cout << "A";
                break;

            case AddressMode::IMMEDIATE:
                std::cout << "#$"; printAsHex(m_cpu.m_controller->readWord(t_address + 1));
                break;

            case AddressMode::ZERO_PAGE:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readWord(t_address + 1));
                break;

            case AddressMode::ZERO_PAGE_X:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readWord(t_address + 1)); std::cout << ",X";
                break;

            case AddressMode::ZERO_PAGE_Y:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readWord(t_address + 1)); std::cout << ",Y";
                break;

            case AddressMode::RELATIVE:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readWord(t_address + 1));
                break;

            case AddressMode::ABSOLUTE:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readDWord(t_address + 1));
                break;

            case AddressMode::ABSOLUTE_X:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readDWord(t_address + 1)); std::cout << ",X";
                break;

            case AddressMode::ABSOLUTE_Y:
                std::cout << "$"; printAsHex(m_cpu.m_controller->readDWord(t_address + 1)); std::cout << ",Y";
                break;

            case AddressMode::INDIRECT:
                std::cout << "($"; printAsHex(m_cpu.m_controller->readDWord(t_address + 1)); std::cout << ")";
                break;

            case AddressMode::INDEXED_INDIRECT:
                std::cout << "($"; printAsHex(m_cpu.m_controller->readWord(t_address + 1)); std::cout << ",X)";
                break;

            case AddressMode::INDIRECT_INDEXED:
                std::cout << "($"; printAsHex(m_cpu.m_controller->readWord(t_address + 1)); std::cout << "),Y";
                break;

            default:
                ASSERT(false, "shouldn't be here");
        }
        std::cout << '\n';
        return instructionSize(instructionInfo.addressMode);
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
