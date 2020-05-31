#include "src/wyngine.h"

class Game : public Wyngine
{
    WY_Image *mFontImage;
    WY_MonoFont *mFont;

    void loadMedia()
    {
        mFontImage = loadPNG(mRenderer, "assets/ascii-bnw.png");
    }

public:
    Game(const char *title, int w, int h, int ps) : Wyngine(title, w, h, ps)
    {
        loadMedia();

        mFont = new WY_MonoFont(mFontImage->texture, 8, 4, {10, 10, 80, 50});
        mFont->setDebug(true);
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
        mFont->print(mRenderer, "Hello wrapped world!\n\nNew line!");
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game("Wyngine audio demo", 256, 224, 3);

    game->run();

    return 0;
}