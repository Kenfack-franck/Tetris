#include "../include/TetrisInstance.hpp"
#include "../include/Defs.hpp"
#include <string>

TetrisInstance::TetrisInstance(SDL_Renderer* ren, TTF_Font* f, int x, int y)
    : renderer(ren), font(f), offsetX(x), offsetY(y),
      currentPiece(rand() % 7 + 1), nextPiece(rand() % 7 + 1),
      isGameOver(false), score(0), level(0), speed(500), lastClearedCount(0)
{
    lastTime = SDL_GetTicks();
    
    // CORRECTION 1 : On force la NextPiece à être en (0,0) pour l'affichage UI
    // (Assure-toi d'avoir setPosition dans Tetromino.hpp, sinon ajoute-le !)
    nextPiece.setPosition(0, 0); 
}

void TetrisInstance::update() {
    if (isGameOver) return;
    
    unsigned int currentTime = SDL_GetTicks();
    if (currentTime - lastTime > speed) {
        currentPiece.move(0, 1);
        if (board.isCollision(currentPiece)) {
            currentPiece.move(0, -1);
            board.lockPiece(currentPiece);
            
            int lines = board.clearLines();
            if (lines > 0) {
                int points = 0;
                switch(lines) {
                    case 1: points = 40; break;
                    case 2: points = 100; break;
                    case 3: points = 300; break;
                    case 4: points = 1200; break;
                }
                score += points * (level + 1);
                lastClearedCount += lines;

                if (score > (level + 1) * 1000) {
                    level++;
                    if (speed > 100) speed -= 50;
                }
            }

            // Passage de témoin
            currentPiece = nextPiece;
            currentPiece.setPosition(4, 0); // On la place au milieu du plateau pour jouer
            
            // Génération nouvelle suivante
            nextPiece = Tetromino(rand() % 7 + 1);
            nextPiece.setPosition(0, 0); // CORRECTION : On la met à 0,0 pour l'affichage UI

            if (board.isCollision(currentPiece)) isGameOver = true;
        }
        lastTime = currentTime;
    }
}

void TetrisInstance::moveLeft() {
    if (isGameOver) return;
    currentPiece.move(-1, 0);
    if (board.isCollision(currentPiece)) currentPiece.move(1, 0);
}

void TetrisInstance::moveRight() {
    if (isGameOver) return;
    currentPiece.move(1, 0);
    if (board.isCollision(currentPiece)) currentPiece.move(-1, 0);
}

void TetrisInstance::rotate() {
    if (isGameOver) return;
    currentPiece.rotate();
    if (board.isCollision(currentPiece)) currentPiece.rotateBack();
}

void TetrisInstance::softDrop() {
    if (isGameOver) return;
    currentPiece.move(0, 1);
    if (board.isCollision(currentPiece)) currentPiece.move(0, -1);
}

int TetrisInstance::popLinesCleared() {
    int count = lastClearedCount;
    lastClearedCount = 0;
    return count;
}

void TetrisInstance::addGarbage(int lines) {
    if (lines > 0) board.addGarbage(lines);
}

void TetrisInstance::draw() {
    // 1. Dessiner le plateau
    board.draw(renderer, offsetX, offsetY);

    // 2. Dessiner le Ghost
    Tetromino ghost = currentPiece;
    while (!board.isCollision(ghost)) ghost.move(0, 1);
    ghost.move(0, -1);
    
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    const auto& s = ghost.getShape();
    for(int r=0; r<s.size(); r++) {
        for(int c=0; c<s[r].size(); c++) {
            if(s[r][c] != 0) {
                SDL_Rect rect = {
                    offsetX + (ghost.getX()+c)*BLOCK_SIZE, 
                    offsetY + (ghost.getY()+r)*BLOCK_SIZE, 
                    BLOCK_SIZE-1, BLOCK_SIZE-1
                };
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }

    // 3. Dessiner la pièce courante
    currentPiece.draw(renderer, offsetX, offsetY);

    // 4. Interface Texte (UI)
    auto drawText = [&](std::string txt, int x, int y, SDL_Color col) { // Ajout param couleur
        if(!font) return;
        SDL_Surface* surf = TTF_RenderText_Solid(font, txt.c_str(), col);
        if(surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {x, y, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    };

    // CORRECTION 2 : Meilleur positionnement du texte
    // On place le texte à droite du plateau (largeur 300px) + 20px de marge
    int textX = offsetX + (BOARD_WIDTH * BLOCK_SIZE) + 20;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {0, 255, 255, 255};
    
    drawText("SCORE", textX, offsetY + 10, white);
    drawText(std::to_string(score), textX, offsetY + 40, cyan);
    
    drawText("LEVEL", textX, offsetY + 90, white);
    drawText(std::to_string(level), textX, offsetY + 120, cyan);

    drawText("NEXT", textX, offsetY + 180, white);
    
    // CORRECTION 3 : Next Piece
    // On dessine la pièce juste en dessous du texte "NEXT"
    // Comme on a forcé nextPiece à (0,0), on doit juste donner la position absolue du cadre
    // On ajoute un petit +20 en X pour centrer visuellement
    nextPiece.draw(renderer, textX + 10, offsetY + 210);

    if (isGameOver) {
        drawText("PERDU !", offsetX + 50, offsetY + 200, {255, 0, 0, 255});
    }
}