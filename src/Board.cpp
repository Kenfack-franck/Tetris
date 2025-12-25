#include "../include/Board.hpp"

Board::Board() {
    init();
}

void Board::init() {
    // Crée une grille de [HEIGHT][WIDTH] remplie de 0
    grid.resize(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
}

void Board::draw(SDL_Renderer* renderer) {
    // 1. Dessiner le contour du plateau (Cadre blanc)
    SDL_Rect border;
    border.x = BOARD_START_X - 2;
    border.y = BOARD_START_Y - 2;
    border.w = (BOARD_WIDTH * BLOCK_SIZE) + 4;
    border.h = (BOARD_HEIGHT * BLOCK_SIZE) + 4;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    SDL_RenderDrawRect(renderer, &border);

    // 2. Dessiner la grille intérieure (lignes grises légères)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Gris foncé


    // Dessiner les blocs verrouillés
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int colorId = grid[y][x];
            if (colorId > 0) {
                Color c = COLORS[colorId];
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
                
                SDL_Rect rect;
                rect.x = BOARD_START_X + x * BLOCK_SIZE;
                rect.y = BOARD_START_Y + y * BLOCK_SIZE;
                rect.w = BLOCK_SIZE - 1;
                rect.h = BLOCK_SIZE - 1;
                
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}


bool Board::isCollision(const Tetromino& piece) {
    const auto& shape = piece.getShape(); // On récupère la forme (getter à ajouter dans Tetromino.hpp si pas déjà fait)
    int px = piece.getX();
    int py = piece.getY();

    for (int row = 0; row < shape.size(); row++) {
        for (int col = 0; col < shape[row].size(); col++) {
            // Si c'est un bloc vide de la pièce (0), on s'en fiche
            if (shape[row][col] == 0) continue;

            // Position réelle sur le plateau
            int boardX = px + col;
            int boardY = py + row;

            // 1. Vérifier les murs (Gauche / Droite)
            if (boardX < 0 || boardX >= BOARD_WIDTH) return true;

            // 2. Vérifier le sol (Bas)
            if (boardY >= BOARD_HEIGHT) return true;

            // 3. Vérifier les blocs déjà existants dans la grille
            // (Note : on vérifie boardY >= 0 car la pièce commence un peu au-dessus de l'écran parfois)
            if (boardY >= 0 && grid[boardY][boardX] != 0) return true;
        }
    }
    // Si on arrive ici, tout va bien
    return false;
}


void Board::lockPiece(const Tetromino& piece) {
    const auto& shape = piece.getShape();
    int px = piece.getX();
    int py = piece.getY();

    for (int row = 0; row < shape.size(); row++) {
        for (int col = 0; col < shape[row].size(); col++) {
            if (shape[row][col] != 0) {
                // On enregistre l'ID de la couleur (shape[row][col] contient l'ID si tu as bien fait, sinon on prend l'ID de la pièce)
                // Pour simplifier, supposons que shape contient des 1 et que l'ID vient de la pièce.
                // NOTE : Il faut un getter pour l'ID dans Tetromino.hpp si ce n'est pas fait.
                // Solution simple ici : on stocke un code couleur.
                
                // Sécurité
                if (py + row >= 0 && py + row < BOARD_HEIGHT && px + col >= 0 && px + col < BOARD_WIDTH) {
                     // Astuce: On va tricher un peu et récupérer l'ID depuis la méthode draw, 
                     // ou alors ajoute "int getId() const { return id; }" dans Tetromino.hpp
                     // Supposons que tu as ajouté le getter :
                     grid[py + row][px + col] = piece.getId(); 
                }
            }
        }
    }
}


int Board::clearLines() { // Change void par int
    int linesCleared = 0; // Compteur

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool isFull = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (grid[y][x] == 0) {
                isFull = false;
                break;
            }
        }

        if (isFull) {
            linesCleared++; // On compte
            for (int upperY = y; upperY > 0; upperY--) {
                grid[upperY] = grid[upperY - 1];
            }
            grid[0] = std::vector<int>(BOARD_WIDTH, 0);
            y++; 
        }
    }
    return linesCleared; // On retourne le total
}