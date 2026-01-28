#include "../include/AudioManager.hpp"

AudioManager::AudioManager() 
    : bgMusic(nullptr), sfxDrop(nullptr), sfxLine(nullptr), sfxGameOver(nullptr), initialized(false) {
    init();
}

// Implémentation de la méthode virtuelle init() de Manager
bool AudioManager::init() {
    if (initialized) return true;  // Déjà initialisé
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Erreur SDL_mixer: " << Mix_GetError() << std::endl;
        initialized = false;
        return false;
    }

    bgMusic = Mix_LoadMUS("assets/music1.mp3");
    sfxDrop = Mix_LoadWAV("assets/music6.wav");
    sfxLine = Mix_LoadWAV("assets/music5.wav");

    if (!bgMusic) std::cout << "Attention: Musique non trouvee !" << std::endl;
    
    initialized = true;
    return true;
}

// Implémentation de la méthode virtuelle cleanup() de Manager
void AudioManager::cleanup() {
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (sfxDrop) Mix_FreeChunk(sfxDrop);
    if (sfxLine) Mix_FreeChunk(sfxLine);
    if (sfxGameOver) Mix_FreeChunk(sfxGameOver);
    
    Mix_CloseAudio();
    initialized = false;
}

// Implémentation de la méthode virtuelle isInitialized() de Manager
bool AudioManager::isInitialized() const {
    return initialized;
}

AudioManager::~AudioManager() {
    cleanup();
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