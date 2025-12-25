#include "../include/Tetromino.hpp"

Tetromino::Tetromino(int typeId) {
    id = typeId;
    gridX = BOARD_WIDTH / 2 - 1; // Centrer au départ (environ colonne 4)
    gridY = 0; // Tout en haut

    // Définition des formes (Matrices)
    // On utilise resize pour créer la grille
    if (id == 1) { // I (Barre)
        shape = { {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} };
    }
    else if (id == 2) { // O (Carré)
        shape = { {1,1}, {1,1} };
    }
    else if (id == 3) { // T
        shape = { {0,1,0}, {1,1,1}, {0,0,0} };
    }
    else if (id == 4) { // S
        shape = { {0,1,1}, {1,1,0}, {0,0,0} };
    }
    else if (id == 5) { // Z
        shape = { {1,1,0}, {0,1,1}, {0,0,0} };
    }
    else if (id == 6) { // J
        shape = { {1,0,0}, {1,1,1}, {0,0,0} };
    }
    else if (id == 7) { // L
        shape = { {0,0,1}, {1,1,1}, {0,0,0} };
    }
}

void Tetromino::move(int dx, int dy) {
    gridX += dx;
    gridY += dy;
}

void Tetromino::rotate() {
    // 1. Créer une copie temporaire de la forme actuelle
    int N = shape.size();
    std::vector<std::vector<int>> newShape(N, std::vector<int>(N));

    // 2. Appliquer la rotation mathématique (90° horaire)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            newShape[j][N - 1 - i] = shape[i][j];
        }
    }

    // 3. Appliquer la nouvelle forme
    shape = newShape;
}

// Ajoute aussi cette méthode utilitaire qu'on va utiliser pour annuler une rotation si elle est invalide
void Tetromino::rotateBack() {
    // Tourner 3 fois à droite = 1 fois à gauche
    rotate();
    rotate();
    rotate();
}

void Tetromino::draw(SDL_Renderer* renderer, int offsetX, int offsetY) {
    Color c = COLORS[id];
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);

    for (int row = 0; row < shape.size(); row++) {
        for (int col = 0; col < shape[row].size(); col++) {
            if (shape[row][col] != 0) {
                SDL_Rect rect;
                
                if (offsetX == 0 && offsetY == 0) {
                    // Mode Normal (Jeu)
                    rect.x = BOARD_START_X + (gridX + col) * BLOCK_SIZE;
                    rect.y = BOARD_START_Y + (gridY + row) * BLOCK_SIZE;
                } else {
                    // Mode UI (Next Piece)
                    rect.x = offsetX + (col * BLOCK_SIZE);
                    rect.y = offsetY + (row * BLOCK_SIZE);
                }

                rect.w = BLOCK_SIZE - 1;
                rect.h = BLOCK_SIZE - 1;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}