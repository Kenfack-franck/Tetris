#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Defs.hpp"

class Tetromino {
private:
    int id; // 1 à 7 (le type de pièce)
    int gridX, gridY; // Position dans la grille (ex: x=4, y=0 au départ)
    
    // La forme actuelle de la pièce (ex: une matrice 3x3)
    // 0 = vide, 1 = bloc présent   
    std::vector<std::vector<int>> shape; 

public:
    // Constructeur : on lui donne un type (1-7)
    Tetromino(int typeId);

    // Méthodes de mouvement
    void move(int dx, int dy);
    void rotate(); // On fera la logique plus tard, préparons juste la méthode
    void rotateBack(); // Pour annuler une rotation si besoin

    // Dessin
    void draw(SDL_Renderer* renderer, int offsetX = 0, int offsetY = 0);

    // Accesseurs (Getters) pour plus tard (collisions)
    const std::vector<std::vector<int>>& getShape() const { return shape; }
    int getX() const { return gridX; }
    int getY() const { return gridY; }

    int getId() const { return id; }

    void setPosition(int x, int y) { gridX = x; gridY = y; }
};