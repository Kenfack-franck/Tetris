#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <SDL2/SDL_mixer.h>
#include <iostream>
#include "Manager.hpp"

// Dérive de Manager - Démontre l'héritage (Séance 2 & 8)
class AudioManager : public Manager {
private:
    Mix_Music* bgMusic;
    Mix_Chunk* sfxDrop;
    Mix_Chunk* sfxLine;
    Mix_Chunk* sfxGameOver;
    bool initialized;

public:
    AudioManager();
    ~AudioManager();

    // Implémentation des méthodes virtuelles de Manager
    bool init() override;
    void cleanup() override;
    bool isInitialized() const override;

    void playMusic();
    void playDrop();
    void playLineClear();
    void playGameOver();
};

#endif