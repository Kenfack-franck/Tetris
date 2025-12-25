#pragma once
#include <SDL2/SDL.h>
#include "Board.hpp"
#include "Tetromino.hpp"
#include <SDL2/SDL_ttf.h> // <--- IMPORTANT
#include <string>         // Pour std::string

class Game {
private:
    SDL_Renderer* renderer; // Lien vers l'affichage
    Board board;
    Tetromino currentPiece;
    Tetromino nextPiece; // <--- AJOUT

    // --- TEXTE ---
    TTF_Font* font; // La variable pour stocker la police


    // Dans include/Game.hpp, sous private:

    // On définit la zone d'interface à droite du plateau
    const int UI_X = BOARD_START_X + (BOARD_WIDTH * BLOCK_SIZE) + 40;
    const int UI_Y = BOARD_START_Y;

    // Si tu avais NEXT_PIECE_X, tu n'en as plus besoin, on utilise UI_X partout.


    // État du jeu
    bool isGameOver;
    unsigned int lastTime; // Pour le timer
    unsigned int speed;    // Vitesse de chute en ms

    // Données de jeu
    int score;
    int level;
    int linesClearedTotal;

public:
    Game(SDL_Renderer* ren);
    ~Game(); // Destructeur pour nettoyer la police


    // Boucle principale du jeu
    void update();
    
    // Gestion des entrées (clavier)
    void handleInput(SDL_Event& event);
    
    // Affichage
    void render();

    // Accesseurs (pour l'affichage du score plus tard)
    bool getGameOver() const { return isGameOver; }
    int getScore() const { return score; }
    int getLevel() const { return level; }

     // Fonction pour dessiner du texte facilement
    void drawText(std::string text, int x, int y, SDL_Color color);
};