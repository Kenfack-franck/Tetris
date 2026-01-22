#include "../include/TetrisInstance.hpp"
#include "../include/Defs.hpp"
#include <string>
#include <stacktrace>

TetrisInstance::TetrisInstance(SDL_Renderer* ren, TTF_Font* f, AudioManager* audioMgr, int x, int y)
    : renderer(ren), font(f),audio(audioMgr), offsetX(x), offsetY(y),
      currentPiece(rand() % 7 + 1), 
      nextPiece(rand() % 7 + 1),
      holdPiece(1), // On initialise avec n'importe quoi, isHoldEmpty protège
      isHoldEmpty(true), 
      canHold(true),
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

            
            lockPieceLogic();

            // board.lockPiece(currentPiece);
            
            // int lines = board.clearLines();
            // if (lines > 0) {
            //     int points = 0;
            //     switch(lines) {
            //         case 1: points = 40; break;
            //         case 2: points = 100; break;
            //         case 3: points = 300; break;
            //         case 4: points = 1200; break;
            //     }
            //     score += points * (level + 1);
            //     lastClearedCount += lines;

            //     if (score > (level + 1) * 1000) {
            //         level++;
            //         if (speed > 100) speed -= 50;
            //     }
            // }

            // // Passage de témoin
            // currentPiece = nextPiece;
            // currentPiece.setPosition(4, 0); // On la place au milieu du plateau pour jouer
            
            // // Génération nouvelle suivante
            // nextPiece = Tetromino(rand() % 7 + 1);
            // nextPiece.setPosition(0, 0); // CORRECTION : On la met à 0,0 pour l'affichage UI

            // if (board.isCollision(currentPiece)) isGameOver = true;
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



    // // CORRECTION 2 : Meilleur positionnement du texte
    // // On place le texte à droite du plateau (largeur 300px) + 20px de marge
    // int textX = offsetX + (BOARD_WIDTH * BLOCK_SIZE) + 20;
    // SDL_Color white = {255, 255, 255, 255};
    // SDL_Color cyan = {0, 255, 255, 255};
    
    // drawText("SCORE", textX, offsetY + 10, white);
    // drawText(std::to_string(score), textX, offsetY + 40, cyan);
    
    // drawText("LEVEL", textX, offsetY + 90, white);
    // drawText(std::to_string(level), textX, offsetY + 120, cyan);

    // drawText("NEXT", textX, offsetY + 180, white);
    
    // // CORRECTION 3 : Next Piece
    // // On dessine la pièce juste en dessous du texte "NEXT"
    // // Comme on a forcé nextPiece à (0,0), on doit juste donner la position absolue du cadre
    // // On ajoute un petit +20 en X pour centrer visuellement
    // nextPiece.draw(renderer, textX + 10, offsetY + 210);

    int rightX = offsetX + (BOARD_WIDTH * BLOCK_SIZE) + 30;
    int leftX = offsetX - 110; // Zone à gauche pour le HOLD

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {0, 255, 255, 255};
    SDL_Color gray = {150, 150, 150, 255};

    // --- DROITE : SCORE & NEXT ---
    drawText("SCORE", rightX, offsetY + 10, white);
    drawText(std::to_string(score), rightX, offsetY + 40, cyan);
    drawText("NEXT", rightX, offsetY + 180, white);
    nextPiece.draw(renderer, rightX + 10, offsetY + 210);

    // --- GAUCHE : HOLD ---
    // On dessine le texte HOLD
    // Attention: Si leftX est < 0 (trop à gauche), ça peut être coupé.
    // Avec offsetX=50 pour J1, leftX = -50. C'est un problème.
    // On va afficher le HOLD en haut à gauche du board, ou juste au-dessus.
    
    // Nouvelle position HOLD : Juste à gauche du board (x - 90px)
    int holdX = offsetX - 110;
    // Si c'est J1 (x=50), on n'a pas la place à gauche. On va le mettre au dessus ?
    // Ou alors on décale tout le monde.
    
    // SOLUTION SIMPLE : Afficher HOLD à coté de NEXT pour l'instant (mais plus bas) ?
    // Non, standard Tetris c'est à gauche.
    // On va dessiner HOLD à gauche, mais il faudra décaler J1 dans Game.cpp plus tard.
    
    drawText("HOLD", offsetX , offsetY +10, white);
    if (!isHoldEmpty) {
        // Si canHold est faux, on dessine en gris pour dire "pas dispo"
        if (!canHold) {
            // Petite astuce couleur non gérée par draw simple...
            // On dessine normalement
        }
        holdPiece.draw(renderer, offsetX - 90, offsetY + 30);
    }


    if (isGameOver) {
        drawText("PERDU !", offsetX + 50, offsetY + 200, {255, 0, 0, 255});
    }
}



// --- LOGIQUE CENTRALE DE VERROUILLAGE ---
void TetrisInstance::lockPieceLogic() {

    // 1. Verrouiller
    board.lockPiece(currentPiece);

    // SON DE CHUTE (Quand la pièce se verrouille)
    audio->playDrop();
    
    // 2. Lignes
    int lines = board.clearLines();
    if (lines > 0) {
        // SON DE LIGNE
        audio->playLineClear();
        int points = 0;
        switch(lines) {
            case 1: points = 40; break;
            case 2: points = 100; break;
            case 3: points = 300; break;
            case 4: points = 1200; break;
        }
        score += points * (level + 1);
        lastClearedCount += lines; // Pour l'attaque

        if (score > (level + 1) * 1000) {
            level++;
            if (speed > 100) speed -= 50;
        }
    }

    // 3. Passer à la suivante
    currentPiece = nextPiece;
    currentPiece.setPosition(4, 0); // Remettre au départ
    
    nextPiece = Tetromino(rand() % 7 + 1);
    nextPiece.setPosition(0, 0);

    // 4. Réinitialiser le droit de Hold
    canHold = true; 

    // 5. Vérifier défaite
    if (board.isCollision(currentPiece)) {
        isGameOver = true;
        audio->playGameOver(); // Arrête la musique si un joueur perd
    }
}


// --- COMMANDES ---

void TetrisInstance::hardDrop() {
    if (isGameOver) return;

    // On fait descendre la pièce tant qu'elle ne touche pas
    while (!board.isCollision(currentPiece)) {
        currentPiece.move(0, 1);
    }
    currentPiece.move(0, -1); // Un cran en arrière (position valide au sol)
    
    // On verrouille immédiatement (plus besoin d'attendre le timer)
    lockPieceLogic();
    
    // Reset du timer pour ne pas que la prochaine pièce tombe tout de suite
    lastTime = SDL_GetTicks(); 
}

void TetrisInstance::hold() {
    if (isGameOver || !canHold) return;

    if (isHoldEmpty) {
        // Premier hold de la partie : On stocke l'actuelle et on prend la suivante
        holdPiece = currentPiece;
        
        // On prend la suivante comme nouvelle pièce courante
        currentPiece = nextPiece;
        currentPiece.setPosition(4, 0);

        // On génère une nouvelle suivante
        nextPiece = Tetromino(rand() % 7 + 1);
        nextPiece.setPosition(0, 0);
        
        isHoldEmpty = false;
    } else {
        // Échange standard : Courante <-> Réserve
        Tetromino temp = currentPiece;
        currentPiece = holdPiece;
        holdPiece = temp;
        
        // On remet la pièce qui sort du frigo au point de départ
        currentPiece.setPosition(4, 0);
        
        // On doit reset la rotation de la pièce qui sort (Optionnel, mais mieux)
        // (Pour l'instant on garde sa rotation, c'est acceptable)
    }

    // Préparer la pièce Hold pour l'affichage (à 0,0)
    holdPiece.setPosition(0, 0);
    
    // Interdire d'échanger à nouveau tant qu'on n'a pas posé la pièce
    canHold = false;
    
    // Reset timer gravité pour laisser le temps de réfléchir
    lastTime = SDL_GetTicks();
}