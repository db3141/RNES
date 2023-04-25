#ifndef RNES_MAPPER_INCLUDED
#define RNES_MAPPER_INCLUDED

#include <memory>

#include "error_or.hpp"
#include "cpu/cpu_controller.hpp"
#include "ppu/ppu_controller.hpp"

namespace RNES::Mapper {

    struct Mapper {
        std::unique_ptr<CPU::CPUController> cpuController;
        std::unique_ptr<PPU::PPUController> ppuController;
    };

    enum Error {
        ERROR_INDEX_OUT_OF_RANGE,
        ERROR_INVALID_FILE,
        ERROR_FAILED_TO_OPEN_FILE,
    };

    ErrorOr<Mapper> parseMapperFromINES(const char* t_filePath);

}

#endif
