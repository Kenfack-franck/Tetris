#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <ctime>
#include <cstdlib>
#include "../include/Game.hpp"
#include "../include/Defs.hpp"

int main(int argc, char* argv[]) {
    srand(time(0));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("TETRIS POO - PROJET", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Création de l'objet Jeu
    Game game(renderer);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            // On délègue la gestion des touches au jeu
            game.handleInput(event);
        }

        // On délègue la logique au jeu
        game.update();

        // On délègue l'affichage au jeu
        game.render();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}