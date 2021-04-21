#ifndef EMULATOR_CPU_DEBUGGER
#define EMULATOR_CPU_DEBUGGER

#include <string>
#include <unordered_set>
#include <vector>

#include "cpu.hpp"

namespace Emulator {

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

        CommandReturnCode executeCommand(const std::string& t_command);

        CommandReturnCode commandStep(const std::vector<std::string>& t_args);
        CommandReturnCode commandContinue(const std::vector<std::string>& t_args);
        CommandReturnCode commandDecodeCurrentInstruction(const std::vector<std::string>& t_args);
        CommandReturnCode commandPrintRegisters(const std::vector<std::string>& t_args);
        CommandReturnCode commandQuit(const std::vector<std::string>& t_args);

        CommandReturnCode commandSetBreakpoint(const std::vector<std::string>& t_args);
        CommandReturnCode commandRemoveBreakpoint(const std::vector<std::string>& t_args);
        CommandReturnCode commandListBreakpoints(const std::vector<std::string>& t_args);

        using Command = CommandReturnCode (CPUDebugger::*)(const std::vector<std::string>&);

        CPU& m_cpu;
        std::unordered_set<Address> m_breakpoints;
        std::string m_lastCommand;
    };

}

#endif
