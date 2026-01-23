#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "TetrisInstance.hpp"
#include "AudioManager.hpp"
#include "NetworkManager.hpp" // <--- AJOUT



// 1. Définition des états possibles
enum class GameState {
    MENU,
    MODE_SELECTION, // Nouveau nom pour MULTI_CHOICE (plus clair)
    PLAY,
    PAUSE,
    GAME_OVER,
    MULTI_CHOICE, // Nouveau : Choisir "Créer" ou "Rejoindre"
    MULTI_HOST,   // Nouveau : Salle d'attente Hôte
    MULTI_JOIN   // Nouveau : Saisie IP Client
};

class Game {
    

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    AudioManager audio;
    NetworkManager net; // <--- AJOUT : Le gestionnaire réseau



    
    // NOS DEUX JOUEURS !
    TetrisInstance* player1;
    TetrisInstance* player2;


    // 2. La variable d'état
    GameState currentState;

    // --- Variables pour la saisie de texte (IP) ---
    std::string inputText; // L'IP que le joueur est en train de taper

    std::string connectionStatus; // Pour stocker les messages d'erreur/succès

    bool isNetworkGame; // Nouvelle variable pour savoir si on est en ligne

    bool isGameRunning; // <--- NOUVELLE VARIABLE


    // 3. Méthodes d'aide pour décharger le code principal
    void resetGame(); // Pour remettre les scores à 0 et recréer les plateaux

    // Méthodes d'affichage spécifiques (pour ne pas avoir un render géant)
    void renderMenu();
    void renderPause();
    void renderGameOver();
     // Helper pour centrer du texte
    void drawCenteredText(std::string text, int y, SDL_Color color);


    void renderMultiChoice(); // Nouveau
    void renderMultiHost();   // Nouveau
    void renderMultiJoin();   // Nouveau

    void renderModeSelection();

public:
    Game(SDL_Renderer* ren);
    ~Game(); // Important pour les delete

    bool isRunning() const { return isGameRunning; } 

    void update();
    void handleInput(SDL_Event& event);
    void render();
};