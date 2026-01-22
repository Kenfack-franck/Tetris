#include "../include/AudioManager.hpp"

AudioManager::AudioManager() : bgMusic(nullptr), sfxDrop(nullptr), sfxLine(nullptr), sfxGameOver(nullptr) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Erreur SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }

    bgMusic = Mix_LoadMUS("assets/music1.mp3");
    sfxDrop = Mix_LoadWAV("assets/music2.mp3");
    sfxLine = Mix_LoadWAV("assets/music3.mp3");

    if (!bgMusic) std::cout << "Attention: Musique non trouvee !" << std::endl;
}

AudioManager::~AudioManager() {
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (sfxDrop) Mix_FreeChunk(sfxDrop);
    if (sfxLine) Mix_FreeChunk(sfxLine);
    if (sfxGameOver) Mix_FreeChunk(sfxGameOver);
    
    Mix_CloseAudio();
}

void AudioManager::playMusic() {
    if (bgMusic) Mix_PlayMusic(bgMusic, -1);
}

void AudioManager::playDrop() {
    if (sfxDrop) Mix_PlayChannel(-1, sfxDrop, 0);
}

void AudioManager::playLineClear() {
    if (sfxLine) Mix_PlayChannel(-1, sfxLine, 0);
}

void AudioManager::playGameOver() {
    Mix_HaltMusic();
}