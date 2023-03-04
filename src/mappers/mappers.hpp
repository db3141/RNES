#ifndef RNES_MAPPERS_INCLUDED
#define RNES_MAPPERS_INCLUDED

#include <memory>

#include "cpu/cpu_controller.hpp"
#include "ppu/ppu_controller.hpp"

namespace RNES::Mappers {

    struct Mapper {
        std::unique_ptr<CPUController> cpuController;
        std::unique_ptr<PPUController> ppuController;
    };

    Mapper parseMapperFromINES(const char* t_filePath);

}

#endif
