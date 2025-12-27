#include "../include/Game.hpp"
#include <iostream> // Pour afficher les erreurs dans la console

Game::Game(SDL_Renderer* ren) : renderer(ren) {
    // 1. D'ABORD : Initialiser la librairie de texte
    if (TTF_Init() == -1) {
        std::cout << "ERREUR CRITIQUE : TTF_Init n'a pas pu demarrer ! " << TTF_GetError() << std::endl;
    }

    // 2. ENSUITE : Charger la police
    // Assure-toi que le fichier s'appelle bien "font.ttf" et est dans le dossier "assets"
    font = TTF_OpenFont("assets/font.ttf", 20);

    if (!font) {
        std::cout << "ERREUR : Impossible de charger assets/font.ttf ! " << TTF_GetError() << std::endl;
        std::cout << "Essai de chargement de Arial par defaut..." << std::endl;
        // Essai de secours (Windows)
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 20);
    }

    if (font) {
        std::cout << "Succes : Police chargee !" << std::endl;
    }

    // INSTANCIATION DES DEUX JOUEURS
    // Joueur 1 à Gauche (x=50)
    player1 = new TetrisInstance(renderer, font, 50, 50);

    // Joueur 2 à Droite (x=550) -> J'ai décalé un peu plus à droite pour éviter le chevauchement
    // Largeur plateau = 300px (10 blocs * 30px). 50 + 300 + Marge = 450 c'était un peu juste.
    player2 = new TetrisInstance(renderer, font, 550, 50); 
}

void Game::handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // --- CONTRÔLES JOUEUR 1 (ZQSD / WASD) ---
        switch (event.key.keysym.sym) {
            case SDLK_q: player1->moveLeft(); break; // Azerty A
            case SDLK_a: player1->moveLeft(); break; // Qwerty A
            case SDLK_d: player1->moveRight(); break;
            case SDLK_z: player1->rotate(); break;   // Azerty Z
            case SDLK_w: player1->rotate(); break;   // Qwerty W
            case SDLK_s: player1->softDrop(); break;
            
        // --- CONTRÔLES JOUEUR 2 (Flèches) ---
            case SDLK_LEFT: player2->moveLeft(); break;
            case SDLK_RIGHT: player2->moveRight(); break;
            case SDLK_UP: player2->rotate(); break;
            case SDLK_DOWN: player2->softDrop(); break;
        }
    }
}

void Game::update() {
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
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 30, 255);
    SDL_RenderClear(renderer);
    
    // Ligne de séparation
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

    player1->draw();
    player2->draw();

    SDL_RenderPresent(renderer);
}

Game::~Game() {
    delete player1;
    delete player2;
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

