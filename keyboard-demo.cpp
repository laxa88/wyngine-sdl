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
    Game() : Wyngine("Wyngine keyboard demo", 256, 224, 3)
    {
        loadMedia();

        mFont = new WY_MonoFont(mFontImage->texture, 8, 4, {8, 8, 240, 50});
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
        std::string txt1 = "char pressed: ";
        char txt2 = keyboard->getLastCharPressed();
        std::string txt3 = "\nkeycode     : ";
        std::string txt4 = std::to_string(keyboard->getLastCharPressed());

        std::string txt5 = "\n\nEntered text: ";
        std::string txt6 = io->getText();

        mFont->print(mRenderer, txt1 + txt2 + txt3 + txt4 + txt5 + txt6);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}