#include <iostream>

#include "ppu/ppu.hpp"
#include "ppu_test_controller.hpp"

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

    auto ppuController = std::make_unique<RNES::PPUTestController>(argv[1]);
    RNES::PPU::PPU ppu(argv[2]);
    ppu.setController(std::move(ppuController));

    for (size_t i = 0; i < 341 * 241; i++) {
        ppu.cycle();
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, ppu.getScreenOutput());
    SDL_RenderCopy(renderer, texture, NULL, NULL);
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
