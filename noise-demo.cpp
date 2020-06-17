#include <SDL2/SDL.h>
#include "src/wyngine.h"

class Game : public Wyngine
{
    SDL_Texture *pixelTexture = NULL;

public:
    Game() : Wyngine("Wyngine pixel noise demo", 160, 144, 4)
    {
        pixelTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, mGameW, mGameH);
    }

    ~Game()
    {
        SDL_DestroyTexture(pixelTexture);
        pixelTexture = NULL;
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

    void onRender()
    {
        unsigned char *bytes = nullptr;
        int pitch = 0; // row of pixels
        SDL_LockTexture(pixelTexture, NULL, reinterpret_cast<void **>(&bytes), &pitch);
        for (int y = 0; y < mGameH; y++)
        {
            for (int x = 0; x < mGameW; x++)
            {
                unsigned char rgba[4] = {wyrandom<unsigned char>(255), wyrandom<unsigned char>(255), wyrandom<unsigned char>(255), wyrandom<unsigned char>(255)};
                memcpy(&bytes[(y * mGameW + x) * sizeof(rgba)], rgba, sizeof(rgba));
            }
        }
        SDL_UnlockTexture(pixelTexture);

        SDL_RenderCopy(mRenderer, pixelTexture, NULL, NULL);
        SDL_RenderPresent(mRenderer);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}