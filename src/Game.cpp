#include "../include/Game.hpp"
#include "../include/Defs.hpp"
#include <iostream> 

Game::Game(SDL_Renderer* ren) 
    : renderer(ren), 
      currentPiece(rand() % 7 + 1), 
      nextPiece(rand() % 7 + 1),
      isGameOver(false), score(0), level(0), linesClearedTotal(0), speed(500) 
{
    lastTime = SDL_GetTicks();

    // --- INIT TEXTE ---
    if (TTF_Init() == -1) {
        std::cout << "Erreur TTF_Init: " << TTF_GetError() << std::endl;
    }

    // Charge la police (taille 20). 
    // ATTENTION : Le fichier "assets/font.ttf" doit exister !
    font = TTF_OpenFont("assets/font.ttf", 20);
    if (!font) {
        std::cout << "Erreur chargement police (assets/font.ttf): " << TTF_GetError() << std::endl;
        // Fallback : essaie arial si font.ttf n'existe pas
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }
}


// 2. DESTRUCTEUR (Ajoute-le à la fin ou au début du fichier)
Game::~Game() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}


// 3. FONCTION D'AIDE DRAWTEXT
void Game::drawText(std::string text, int x, int y, SDL_Color color) {
    if (!font) return; // Si pas de police, on ne fait rien

    // Créer une surface (image CPU) avec le texte
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;

    // Convertir en texture (image GPU)
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Définir où ça s'affiche
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    
    // Copier à l'écran
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    // Nettoyer la mémoire (TRÈS IMPORTANT sinon fuite de mémoire)
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}



// 4. METHODE RENDER (Mise à jour complète)
void Game::render() {
    // Fond noir/bleu
    SDL_SetRenderDrawColor(renderer, 0, 0, 30, 255);
    SDL_RenderClear(renderer);

    board.draw(renderer);

    // --- GHOST PIECE (Bonus visuel) ---
    Tetromino ghost = currentPiece;
    while (!board.isCollision(ghost)) { ghost.move(0, 1); }
    ghost.move(0, -1);
    
    // Dessin du ghost (contour gris)
    const auto& shape = ghost.getShape();
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    for (int row = 0; row < shape.size(); row++) {
        for (int col = 0; col < shape[row].size(); col++) {
            if (shape[row][col] != 0) {
                SDL_Rect rect;
                rect.x = BOARD_START_X + (ghost.getX() + col) * BLOCK_SIZE;
                rect.y = BOARD_START_Y + (ghost.getY() + row) * BLOCK_SIZE;
                rect.w = BLOCK_SIZE - 1;
                rect.h = BLOCK_SIZE - 1;
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }

    // Dessin de la pièce courante
    currentPiece.draw(renderer);

    // --- INTERFACE UTILISATEUR (UI) ---
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {0, 255, 255, 255};

    // Cadre "Next Piece"
    drawText("SUIVANT", UI_X, UI_Y, white);
    SDL_Rect nextBox = {UI_X, UI_Y + 30, 120, 100};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &nextBox);
    nextPiece.draw(renderer, UI_X + 15, UI_Y + 45);

    // Score
    drawText("SCORE", UI_X, UI_Y + 150, white);
    drawText(std::to_string(score), UI_X, UI_Y + 180, cyan);

    // Niveau
    drawText("NIVEAU", UI_X, UI_Y + 220, white);
    drawText(std::to_string(level), UI_X, UI_Y + 250, cyan);

    // Lignes
    drawText("LIGNES", UI_X, UI_Y + 290, white);
    drawText(std::to_string(linesClearedTotal), UI_X, UI_Y + 320, cyan);

    // Game Over
    if (isGameOver) {
        drawText("GAME OVER", BOARD_START_X + 20, SCREEN_HEIGHT / 2 - 20, {255, 0, 0, 255});
        drawText("Appuyez sur ESC", BOARD_START_X + 10, SCREEN_HEIGHT / 2 + 20, white);
    }

    SDL_RenderPresent(renderer);
}



void Game::handleInput(SDL_Event& event) {
    if (isGameOver) return;

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            currentPiece.move(-1, 0);
            if (board.isCollision(currentPiece)) currentPiece.move(1, 0);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            currentPiece.move(1, 0);
            if (board.isCollision(currentPiece)) currentPiece.move(-1, 0);
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            currentPiece.rotate();
            if (board.isCollision(currentPiece)) currentPiece.rotateBack();
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            currentPiece.move(0, 1);
            if (board.isCollision(currentPiece)) currentPiece.move(0, -1);
        }
    }
}


void Game::update() {
    if (isGameOver) return;

    unsigned int currentTime = SDL_GetTicks();
    
    // Gestion de la gravité
    if (currentTime - lastTime > speed) {
        currentPiece.move(0, 1); // La pièce descend

        // Si elle touche quelque chose en bas
        if (board.isCollision(currentPiece)) {
            currentPiece.move(0, -1); // On remonte d'un cran (position valide)
            board.lockPiece(currentPiece); // On la verrouille dans la grille
            
            // --- GESTION DU SCORE ---
            int lines = board.clearLines(); 
            if (lines > 0) {
                int pointsBase = 0;
                switch (lines) {
                    case 1: pointsBase = 40; break;
                    case 2: pointsBase = 100; break;
                    case 3: pointsBase = 300; break;
                    case 4: pointsBase = 1200; break;
                }
                score += pointsBase * (level + 1);
                
                linesClearedTotal += lines;
                if (linesClearedTotal >= (level + 1) * 10) {
                    level++;
                    speed = (speed > 100) ? speed - 50 : 100; 
                }
                std::cout << "Score: " << score << " | Niveau: " << level << std::endl;
            }

            // --- PASSAGE DE TÉMOIN (C'est ICI qu'il faut le faire) ---
            currentPiece = nextPiece;            // La suivante devient l'actuelle
            currentPiece.setPosition(4, 0);      // On la remet en haut au milieu
            
            nextPiece = Tetromino(rand() % 7 + 1); // On génère une nouvelle "suivante"

            // Vérifier Game Over tout de suite (si la nouvelle pièce est déjà bloquée)
            if (board.isCollision(currentPiece)) {
                isGameOver = true;
                std::cout << "GAME OVER !" << std::endl;
            }
        }
        
        lastTime = currentTime;
    }
}

