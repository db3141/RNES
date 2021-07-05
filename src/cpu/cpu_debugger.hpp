#ifndef RNES_CPU_DEBUGGER
#define RNES_CPU_DEBUGGER

#include <string>
#include <unordered_set>
#include <vector>

#include "cpu.hpp"

namespace RNES {

    class CPUDebugger {
    public:
        CPUDebugger(CPU& t_cpu);

        void start();
    private:
        enum class CommandReturnCode {
            OKAY,
            ERROR,
            HALT
        };

        using CArgumentIterator = std::vector<std::string>::const_iterator;
        using Command = CommandReturnCode (CPUDebugger::*)(CArgumentIterator, CArgumentIterator);
        
        CommandReturnCode executeCommand(const std::string& t_command);

        CommandReturnCode commandStep(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);
        CommandReturnCode commandContinue(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        CommandReturnCode commandPrintRegisters(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        CommandReturnCode commandQuit(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        CommandReturnCode commandSetBreakpoint(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);
        CommandReturnCode commandRemoveBreakpoint(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);
        CommandReturnCode commandListBreakpoints(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        CommandReturnCode commandDecodeCurrentInstruction(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);
        CommandReturnCode commandDisassembleInstructions(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        CommandReturnCode commandExamineAddress(CArgumentIterator t_argsStart, CArgumentIterator t_argsEnd);

        size_t printDisassembledInstruction(Address t_address) const;

        CPU& m_cpu;
        std::unordered_set<Address> m_breakpoints;
        std::string m_lastCommand;
    };

}

#endif
