#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Defs.hpp"
#include "Tetromino.hpp"

class Board {
private:
    // Grille : 0 = vide, 1-7 = ID de couleur de la pièce
    // On initialise avec des 0
    std::vector<std::vector<int>> grid;

public:
    Board(); // Constructeur

    // Initialise la grille à 0
    void init();

    // Dessine le fond du plateau et les blocs (plus tard)
    // On doit passer le "pinceau" (renderer) à la fonction
    void draw(SDL_Renderer* renderer);
    bool isCollision(const Tetromino& piece);

    // Ajoute la pièce dans la grille statique
    void lockPiece(const Tetromino& piece);

    // Vérifie et supprime les lignes pleines
    int clearLines(); 
};