#include "src/game.h"

class Game : public Wyngine
{
public:
    Game()
    {
    }

    void onUpdate()
    {
        if (windowEvent.type == SDL_KEYDOWN)
        {
            switch (windowEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                mGameRunning = false;
                break;
            }
        }
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}