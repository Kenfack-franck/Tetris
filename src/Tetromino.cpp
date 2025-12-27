#include "../include/Tetromino.hpp"

Tetromino::Tetromino(int typeId) : id(typeId), gridX(4), gridY(0) {
    // Définition des formes
    if (id == 1) shape = { {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} };
    else if (id == 2) shape = { {1,1}, {1,1} };
    else if (id == 3) shape = { {0,1,0}, {1,1,1}, {0,0,0} };
    else if (id == 4) shape = { {0,1,1}, {1,1,0}, {0,0,0} };
    else if (id == 5) shape = { {1,1,0}, {0,1,1}, {0,0,0} };
    else if (id == 6) shape = { {1,0,0}, {1,1,1}, {0,0,0} };
    else if (id == 7) shape = { {0,0,1}, {1,1,1}, {0,0,0} };
    // Pas de forme pour ID 8 (Garbage) car ce sont des blocs du plateau, pas une pièce
}

void Tetromino::move(int dx, int dy) {
    gridX += dx;
    gridY += dy;
}

void Tetromino::rotate() {
    int N = shape.size();
    std::vector<std::vector<int>> newShape(N, std::vector<int>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            newShape[j][N - 1 - i] = shape[i][j];
        }
    }
    shape = newShape;
}

void Tetromino::rotateBack() {
    rotate(); rotate(); rotate();
}

// DESSIN AVEC DÉCALAGE (OFFSET)
void Tetromino::draw(SDL_Renderer* renderer, int xOffset, int yOffset) {
    Color c = COLORS[id];
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);

    for (int row = 0; row < shape.size(); row++) {
        for (int col = 0; col < shape[row].size(); col++) {
            if (shape[row][col] != 0) {
                SDL_Rect rect;
                // Calcul de la position : Offset Global + Position Grille + Position Bloc
                rect.x = xOffset + (gridX + col) * BLOCK_SIZE;
                rect.y = yOffset + (gridY + row) * BLOCK_SIZE;
                rect.w = BLOCK_SIZE - 1;
                rect.h = BLOCK_SIZE - 1;

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

