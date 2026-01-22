#include "../include/Game.hpp"
#include "../include/Defs.hpp"
#include <iostream> // Pour afficher les erreurs dans la console

Game::Game(SDL_Renderer* ren) 
    : renderer(ren), 
      currentState(GameState::MENU), // On commence au MENU
      player1(nullptr),              // IMPORTANT
      player2(nullptr)               // IMPORTANT
{
    // 1. D'ABORD : Initialiser la librairie de texte
    if (TTF_Init() == -1) {
        std::cout << "ERREUR CRITIQUE : TTF_Init n'a pas pu demarrer ! " << TTF_GetError() << std::endl;
    }

    // Lancer la musique au démarrage
    audio.playMusic();

    // 2. ENSUITE : Charger la police
    // Assure-toi que le fichier s'appelle bien "font.ttf" et est dans le dossier "assets"
    font = TTF_OpenFont("assets/font.ttf", 24);

    if (!font) {
        std::cout << "ERREUR : Impossible de charger assets/font.ttf ! " << TTF_GetError() << std::endl;
        std::cout << "Essai de chargement de Arial par defaut..." << std::endl;
        // Essai de secours (Windows)
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }

    if (font) {
        std::cout << "Succes : Police chargee !" << std::endl;
    }

    resetGame();

    // // INSTANCIATION DES DEUX JOUEURS
    // // Joueur 1 à Gauche (x=50)
    // player1 = new TetrisInstance(renderer, font, &audio, 50, 50);

    // // Joueur 2 à Droite (x=550) -> J'ai décalé un peu plus à droite pour éviter le chevauchement
    // // Largeur plateau = 300px (10 blocs * 30px). 50 + 300 + Marge = 450 c'était un peu juste.
    // player2 = new TetrisInstance(renderer, font,&audio, 600, 50); 
}

void Game::handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {

         // --- ÉTAT MENU ---
        if (currentState == GameState::MENU) {
            if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                resetGame(); // Nouvelle partie propre
                currentState = GameState::PLAY;
            }
        }

        // --- ÉTAT PLAY ---
        else if (currentState == GameState::PLAY) {
            // Touche Pause
            if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::PAUSE;
                return; // On arrête là pour ne pas traiter d'autres touches
            }
        // --- CONTRÔLES JOUEUR 1 (ZQSD / WASD) ---
        switch (event.key.keysym.sym) {
            case SDLK_q: player1->moveLeft(); break; // Azerty A
            case SDLK_a: player1->moveLeft(); break; // Qwerty A
            case SDLK_d: player1->moveRight(); break;
            case SDLK_z: player1->rotate(); break;   // Azerty Z
            case SDLK_w: player1->rotate(); break;   // Qwerty W
            case SDLK_s: player1->softDrop(); break;
            case SDLK_SPACE: player1->hardDrop(); break; // Hard Drop
            case SDLK_c:     player1->hold(); break;     // Hold

        // --- CONTRÔLES JOUEUR 2 (Flèches) ---
            case SDLK_LEFT: player2->moveLeft(); break;
            case SDLK_RIGHT: player2->moveRight(); break;
            case SDLK_UP: player2->rotate(); break;
            case SDLK_DOWN: player2->softDrop(); break;
            case SDLK_RETURN:   player2->hardDrop(); break; 
            case SDLK_KP_ENTER: player2->hardDrop(); break;
            case SDLK_m:        player2->hold(); break;      // Hold (M ou L ou Shift Droit)
            case SDLK_RSHIFT:   player2->hold(); break;
        }
    }
    // --- ÉTAT PAUSE ---
        else if (currentState == GameState::PAUSE) {
            if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::PLAY; // Reprendre
            }
            if (event.key.keysym.sym == SDLK_q) {
                currentState = GameState::MENU; // Quitter vers menu
            }
        }

        // --- ÉTAT GAME OVER ---
        else if (currentState == GameState::GAME_OVER) {
            if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                resetGame();
                currentState = GameState::PLAY;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::MENU;
            }
        }
}
}

void Game::update() {
    if (currentState != GameState::PLAY) return;

    player1->update();
    player2->update();

    // --- GESTION DE L'ATTAQUE ---
    int p1Attack = player1->popLinesCleared();
    if (p1Attack > 1) { 
        player2->addGarbage(p1Attack - 1); 
    }

    int p2Attack = player2->popLinesCleared();
    if (p2Attack > 1) {
        player1->addGarbage(p2Attack - 1);
    }

     // Détection Game Over
    // Si l'un des deux perd, c'est fini.
    // (Note: on pourrait attendre que les deux perdent, mais en Versus le premier qui perd a perdu)
    if (player1->getGameOver() || player2->getGameOver()) {
        currentState = GameState::GAME_OVER;
        //audio.playGameOver(); // Si tu as implémenté cette méthode
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 30, 255);
    SDL_RenderClear(renderer);
    
    // Si on n'est pas au menu, on affiche le jeu en fond
    if (currentState != GameState::MENU) {
        // Ligne de séparation
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

        player1->draw();
        player2->draw();
    }

    // Affichage des interfaces (Overlays)
    switch (currentState) {
        case GameState::MENU:      renderMenu(); break;
        case GameState::PAUSE:     renderPause(); break;
        case GameState::GAME_OVER: renderGameOver(); break;
        default: break;
    }

    SDL_RenderPresent(renderer);
}

Game::~Game() {
    delete player1;
    delete player2;
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void Game::resetGame() {
    // Nettoyage sécurisé
    if (player1) { delete player1; player1 = nullptr; }
    if (player2) { delete player2; player2 = nullptr; }

    // Création de nouveaux joueurs
    player1 = new TetrisInstance(renderer, font, &audio, 140, 50);
    player2 = new TetrisInstance(renderer, font, &audio, 700, 50);
}


// --- RENDERING HELPERS ---

void Game::drawCenteredText(std::string text, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = { (SCREEN_WIDTH - surf->w) / 2, y, surf->w, surf->h }; // Centré X
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void Game::renderMenu() {
    // Fond simple ou image si tu veux
    drawCenteredText("TETRIS VERSUS", 200, {0, 255, 255, 255});
    drawCenteredText("Appuyez sur ENTREE pour jouer", 400, {255, 255, 255, 255});
    drawCenteredText("J1: ZQSD + Espace | J2: Fleches + Entree", 500, {150, 150, 150, 255});
}

void Game::renderPause() {
    // Fond semi-transparent noir
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawCenteredText("PAUSE", 300, {255, 255, 0, 255});
    drawCenteredText("P: Reprendre  |  Q: Menu", 400, {255, 255, 255, 255});
}

void Game::renderGameOver() {
    // Fond semi-transparent rouge
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 200);
    SDL_Rect screen = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &screen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    std::string winner;
    if (player1->getGameOver() && !player2->getGameOver()) winner = "JOUEUR 2 GAGNE !";
    else if (!player1->getGameOver() && player2->getGameOver()) winner = "JOUEUR 1 GAGNE !";
    else winner = "EGALITE !";

    drawCenteredText("GAME OVER", 250, {255, 0, 0, 255});
    drawCenteredText(winner, 320, {0, 255, 0, 255});
    drawCenteredText("ESPACE: Rejouer  |  ECHAP: Menu", 500, {255, 255, 255, 255});
}