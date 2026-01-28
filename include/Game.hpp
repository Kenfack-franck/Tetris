#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>  // Pointeurs intelligents - Séance 11
#include "TetrisInstance.hpp"
#include "AudioManager.hpp"
#include "NetworkManager.hpp"



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
    
    // Images
    SDL_Texture* bgMenuTexture;     // Image fond du menu
    SDL_Texture* avatar1Texture;    // Avatar joueur 1
    SDL_Texture* avatar2Texture;    // Avatar joueur 2

    
    // NOS DEUX JOUEURS - Pointeurs intelligents (unique_ptr) - Séance 11
    // Game possède formellement les joueurs - destruction automatique
    std::unique_ptr<TetrisInstance> player1;
    std::unique_ptr<TetrisInstance> player2;


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
    // Helpers pour rendu attrayant
    void drawCenteredText(std::string text, int y, SDL_Color color);
    void drawGradientBG();
    void drawAvatar(int x, int y, int size, SDL_Color color);
    void drawAvatarImage(SDL_Texture* avatarTex, int x, int y, int size);
    SDL_Texture* loadImage(const std::string& path); // Charger une image PNG/JPG
    void drawImageBackground(SDL_Texture* bgTex); // Afficher image en fond


    void renderMultiChoice(); // Nouveau
    void renderMultiHost();   // Nouveau
    void renderMultiJoin();   // Nouveau

    void renderModeSelection();

     void renderControlsHelp(); // <--- AJOUTER CECI

public:
    Game(SDL_Renderer* ren);
    ~Game(); // Important pour les delete

    bool isRunning() const { return isGameRunning; } 

    void update();
    void handleInput(SDL_Event& event);
    void render();
};