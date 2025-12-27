#include "TetrisInstance.hpp"

class Game {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;

    // NOS DEUX JOUEURS !
    TetrisInstance* player1;
    TetrisInstance* player2;

public:
    Game(SDL_Renderer* ren);
    ~Game(); // Important pour les delete

    void update();
    void handleInput(SDL_Event& event);
    void render();
};