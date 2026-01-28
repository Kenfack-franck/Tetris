#include "../include/Board.hpp"


Board::Board() {
    init();
}

void Board::init() {
    // Crée une grille de [HEIGHT][WIDTH] remplie de 0
    grid.resize(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
}

void Board::draw(SDL_Renderer* renderer, int xOffset, int yOffset) {
    // Fond sombre attrayant derrière le plateau
    SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
    SDL_Rect bgBoard = {xOffset - 5, yOffset - 5, (BOARD_WIDTH * BLOCK_SIZE) + 10, (BOARD_HEIGHT * BLOCK_SIZE) + 10};
    SDL_RenderFillRect(renderer, &bgBoard);

    // Cadre extérieur blanc brillant (néon style)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect outerBorder = {xOffset - 4, yOffset - 4, (BOARD_WIDTH * BLOCK_SIZE) + 8, (BOARD_HEIGHT * BLOCK_SIZE) + 8};
    SDL_RenderDrawRect(renderer, &outerBorder);

    // Cadre intérieur sombre
    SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
    SDL_Rect innerBorder = {xOffset - 1, yOffset - 1, (BOARD_WIDTH * BLOCK_SIZE) + 2, (BOARD_HEIGHT * BLOCK_SIZE) + 2};
    SDL_RenderDrawRect(renderer, &innerBorder);

    // Grille subtile bleutée
    SDL_SetRenderDrawColor(renderer, 40, 60, 100, 180);

    // Lignes verticales
    for (int x = 0; x <= BOARD_WIDTH; x++) {
        int xPos = xOffset + (x * BLOCK_SIZE);
        SDL_RenderDrawLine(renderer, xPos, yOffset, xPos, yOffset + (BOARD_HEIGHT * BLOCK_SIZE));
    }

    // Lignes horizontales
    for (int y = 0; y <= BOARD_HEIGHT; y++) {
        int yPos = yOffset + (y * BLOCK_SIZE);
        SDL_RenderDrawLine(renderer, xOffset, yPos, xOffset + (BOARD_WIDTH * BLOCK_SIZE), yPos);
    }

    // Dessiner les blocs avec effet 3D biseauté luxueux
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int colorId = grid[y][x];
            if (colorId > 0) {
                const ColorWithShading& cs = SHADED_COLORS[colorId];
                
                SDL_Rect rect;
                rect.x = xOffset + x * BLOCK_SIZE;
                rect.y = yOffset + y * BLOCK_SIZE;
                rect.w = BLOCK_SIZE - 1;
                rect.h = BLOCK_SIZE - 1;

                // Bloc principal couleur vibrante
                SDL_SetRenderDrawColor(renderer, cs.base.r, cs.base.g, cs.base.b, 255);
                SDL_RenderFillRect(renderer, &rect);

                // Highlight haut-gauche (effet 3D)
                SDL_SetRenderDrawColor(renderer, cs.light.r, cs.light.g, cs.light.b, 255);
                SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y);
                SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h - 1);

                // Shadow bas-droite (effet 3D)
                SDL_SetRenderDrawColor(renderer, cs.dark.r, cs.dark.g, cs.dark.b, 255);
                SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y + 1, rect.x + rect.w - 1, rect.y + rect.h);
                SDL_RenderDrawLine(renderer, rect.x + 1, rect.y + rect.h - 1, rect.x + rect.w, rect.y + rect.h - 1);

                // Accent central pour plus de brillance
                SDL_SetRenderDrawColor(renderer, cs.light.r, cs.light.g, cs.light.b, 150);
                if (rect.w > 5) {
                    SDL_RenderDrawPoint(renderer, rect.x + 2, rect.y + 2);
                    SDL_RenderDrawPoint(renderer, rect.x + 3, rect.y + 2);
                }
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




void Board::addGarbage(int amount) {
    // 1. Décaler tout vers le haut
    // On perd les lignes du haut (Game Over potentiel non géré ici pour simplifier)
    for (int y = 0; y < BOARD_HEIGHT - amount; y++) {
        grid[y] = grid[y + amount];
    }

    // 2. Remplir le bas avec du gris et un trou
    for (int y = BOARD_HEIGHT - amount; y < BOARD_HEIGHT; y++) {
        // Choisir une colonne aléatoire pour le trou
        int hole = rand() % BOARD_WIDTH;
        
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (x == hole) {
                grid[y][x] = 0; // Vide
            } else {
                grid[y][x] = 8; // 8 = ID couleur gris (à définir dans Defs.hpp)
            }
        }
    }
}

