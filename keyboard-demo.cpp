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
        std::string t1 = "char pressed: ";
        char t2 = keyboard->getLastCharPressed();
        std::string t3 = "\nkeycode     : ";
        std::string t4 = std::to_string(keyboard->getLastCharPressed());

        std::string t5 = "\n\nEntered text: ";
        std::string t6 = io->getText();

        std::string t7 = "\n\n5 pressed?  : ";
        std::string t8 = std::to_string(keyboard->isKeyPressed(SDLK_5));
        std::string t9 = "\n5 release?  : ";
        std::string t10 = std::to_string(keyboard->isKeyReleased(SDLK_5));
        std::string t11 = "\n5 up?        : ";
        std::string t12 = std::to_string(keyboard->isKeyUp(SDLK_5));
        std::string t13 = "\n5 down?     : ";
        std::string t14 = std::to_string(keyboard->isKeyDown(SDLK_5));

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + t11 + t12 + t13 + t14);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}