#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "../include/Game.hpp"
#include "../include/Defs.hpp"

int main(int argc, char* argv[]) {
    std::cout << "[MAIN] START" << std::endl;
    srand(time(0));

    std::cout << "[MAIN] Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "[ERROR] SDL_Init failed" << std::endl;
        return 1;
    }

    std::cout << "[MAIN] Creating window..." << std::endl;
    SDL_Window* window = SDL_CreateWindow("TETRIS POO - PROJET", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "[ERROR] Window creation failed" << std::endl;
        return 1;
    }
    
    std::cout << "[MAIN] Creating renderer..." << std::endl;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "[ERROR] Renderer creation failed" << std::endl;
        return 1;
    }

    // Création de l'objet Jeu
    std::cout << "[MAIN] Creating Game instance..." << std::endl;
    Game game(renderer);

    std::cout << "[MAIN] Game created. isRunning() = " << game.isRunning() << std::endl;

    bool running = true;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // ~16.67ms par frame
    Uint32 frameStart;
    int frameTime;

    std::cout << "[MAIN] Entering game loop..." << std::endl;
    int loopCount = 0;
    while (running && game.isRunning()) {
        loopCount++;
        if (loopCount % 120 == 0) {
            std::cout << "[MAIN] Loop iteration " << loopCount << std::endl;
        }
        
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout << "[MAIN] SDL_QUIT received" << std::endl;
                running = false;
            }

            
            // On délègue la gestion des touches au jeu
            game.handleInput(event);
        }

        // On délègue la logique au jeu
        game.update();

        // On délègue l'affichage au jeu
        game.render();

        // Limite FPS
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    std::cout << "[MAIN] Exiting loop. running=" << running << ", isRunning()=" << game.isRunning() << std::endl;
    std::cout << "[MAIN] Cleaning up..." << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "[MAIN] EXIT OK" << std::endl;
    return 0;
}