#ifndef RNES_MAPPER_INCLUDED
#define RNES_MAPPER_INCLUDED

#include <memory>

#include "error_or.hpp"
#include "cpu/cpu_memory_map.hpp"
#include "ppu/ppu_memory_map.hpp"

namespace RNES::Mapper {

    struct Mapper {
        std::unique_ptr<CPU::CPUMemoryMap> cpuController;
        std::unique_ptr<PPU::PPUMemoryMap> ppuController;
    };

    enum Error {
        ERROR_INDEX_OUT_OF_RANGE,
        ERROR_INVALID_FILE,
        ERROR_FAILED_TO_OPEN_FILE,
    };

    Mapper parseMapperFromINES(const char* t_filePath);

}

#endif
