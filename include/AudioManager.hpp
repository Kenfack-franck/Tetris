#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <SDL2/SDL_mixer.h>
#include <iostream>

class AudioManager {
private:
    Mix_Music* bgMusic;
    Mix_Chunk* sfxDrop;
    Mix_Chunk* sfxLine;
    Mix_Chunk* sfxGameOver;

public:
    AudioManager();
    ~AudioManager();

    void playMusic();
    void playDrop();
    void playLineClear();
    void playGameOver();
};

#endif