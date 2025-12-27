#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Board.hpp"
#include "Tetromino.hpp"

class TetrisInstance {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int offsetX, offsetY;

    Board board;
    Tetromino currentPiece;
    Tetromino nextPiece;

    bool isGameOver;
    unsigned int lastTime;
    unsigned int speed;
    int score;
    int level;
    
    // Pour l'attaque multijoueur
    int lastClearedCount; // Combien de lignes on vient de faire ?

public:
    TetrisInstance(SDL_Renderer* ren, TTF_Font* f, int x, int y);

    void update();
    void draw();

    // Commandes
    void moveLeft();
    void moveRight();
    void rotate();
    void softDrop();

    // Multijoueur
    int popLinesCleared();       // Récupère et vide le compteur d'attaque
    void addGarbage(int lines);  // Reçoit l'attaque
};