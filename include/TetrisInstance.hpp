#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Board.hpp"
#include "Tetromino.hpp"
#include "../include/AudioManager.hpp"


class TetrisInstance {
    AudioManager* audio;


private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int offsetX, offsetY;

    Board board;
    Tetromino currentPiece;
    Tetromino nextPiece;

    // --- NOUVEAU : SYSTEME HOLD ---
    Tetromino holdPiece; // La pièce en réserve
    bool isHoldEmpty;    // Est-ce qu'on a une pièce en réserve ?
    bool canHold;        // A-t-on le droit d'échanger ? (1 fois par tour max)


    bool isGameOver;
    unsigned int lastTime;
    unsigned int speed;
    int score;
    int level;
    
    // Pour l'attaque multijoueur
    int lastClearedCount; // Combien de lignes on vient de faire ?

    // --- NOUVEAU : FONCTION PRIVÉE ---
    // Cette fonction contient la logique "La pièce touche le sol -> Verrouiller -> Lignes -> Suivante"
    // On l'appelle depuis update() (gravité) ET depuis hardDrop()
    void lockPieceLogic();

public:
    TetrisInstance(SDL_Renderer* ren, TTF_Font* f, AudioManager* audioMgr, int x, int y);

    void update();
    void draw();

    // Commandes
    void moveLeft();
    void moveRight();
    void rotate();
    void softDrop();

     // --- NOUVELLES COMMANDES ---
    void hardDrop(); // Espace
    void hold();     // Touche C ou Maj

    // Multijoueur
    int popLinesCleared();       // Récupère et vide le compteur d'attaque
    void addGarbage(int lines);  // Reçoit l'attaque
    bool getGameOver () const { return isGameOver; } 
};