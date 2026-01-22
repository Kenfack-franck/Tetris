#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "TetrisInstance.hpp"
#include "AudioManager.hpp"


// 1. Définition des états possibles
enum class GameState {
    MENU,
    PLAY,
    PAUSE,
    GAME_OVER
};

class Game {
    

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    AudioManager audio;


    
    // NOS DEUX JOUEURS !
    TetrisInstance* player1;
    TetrisInstance* player2;


    // 2. La variable d'état
    GameState currentState;

    // 3. Méthodes d'aide pour décharger le code principal
    void resetGame(); // Pour remettre les scores à 0 et recréer les plateaux

    // Méthodes d'affichage spécifiques (pour ne pas avoir un render géant)
    void renderMenu();
    void renderPause();
    void renderGameOver();
     // Helper pour centrer du texte
    void drawCenteredText(std::string text, int y, SDL_Color color);

public:
    Game(SDL_Renderer* ren);
    ~Game(); // Important pour les delete

    void update();
    void handleInput(SDL_Event& event);
    void render();
};