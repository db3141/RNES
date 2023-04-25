#include <iostream>

#include "mapper/mapper.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: <rom_path>" << std::endl;
        return 1;
    }

    ErrorOr<RNES::Mapper::Mapper> mapper = RNES::Mapper::parseMapperFromINES(argv[1]);

    return 0;
}
