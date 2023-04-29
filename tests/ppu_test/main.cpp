#include <iostream>
#include <fstream>
#include <vector>

#include "assert.hpp"
#include "test_chr_map.hpp"
#include "ppu/ppu.hpp"

std::vector<uint8_t> readFile(const char* t_path);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <ppu_dump> <oam_dump>\n";
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL\n";
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "sdl_test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        256,
        240,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Failed to create window\n";

        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer\n";

        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    std::vector<uint8_t> ppuDump = readFile(argv[1]);
    std::vector<uint8_t> oamDump = readFile(argv[2]);

    std::array<uint8_t, RNES::PPU::OAM_SIZE> oamDumpArr = {0};
    for (size_t i = 0; i < oamDumpArr.size(); i++) {
        oamDumpArr[i] = oamDump[i];
    }

    auto chrMap = std::make_unique<RNES::PPU::TestCHRMap>();
    auto ppuController = std::make_unique<RNES::PPU::PPUMemoryMap>(std::move(chrMap));

    for (size_t i = 0; i < 0x4000; i++) {
        ppuController->writeWord(i, ppuDump[i]);
    }

    RNES::PPU::PPU ppu(std::move(ppuController), oamDumpArr);

    for (size_t i = 0; i < 341 * 241; i++) {
        ppu.cycle();
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, ppu.getScreenOutput());
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);

    SDL_Event e;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}

std::vector<uint8_t> readFile(const char* t_path) {
    std::ifstream fileStream(t_path, std::ios::binary | std::ios::in | std::ios::ate);
    ASSERT(fileStream.is_open(), "Could not open file"); // TODO: change this

    std::vector<uint8_t> result(fileStream.tellg());
    fileStream.seekg(std::ios::beg);
    fileStream.read(reinterpret_cast<char*>(result.data()), result.size());

    return result;
}
