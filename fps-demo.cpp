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

        mFont = new WY_MonoFont(mFontImage->texture, 8, 4, {10, 10, 200, 50});
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

            case SDLK_RETURN:
                timer->reset();
            }
        }
    }

    void onRender()
    {
        std::string txt1 = "Time since start : ";
        std::string txt2 = std::to_string(timer->getStartTime());
        std::string txt3 = "\nFrames : ";
        std::string txt4 = std::to_string(timer->getFrames());
        std::string txt5 = "\ndTime : ";
        std::string txt6 = std::to_string(timer->getDeltaTime());
        std::string txt7 = "\nFPS : ";
        std::string txt8 = std::to_string(timer->getFPS());
        std::string txt9 = "\n\nPress enter to reset";
        mFont->print(mRenderer, txt1 + txt2 + txt3 + txt4 + txt5 + txt6 + txt7 + txt8 + txt9);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game("Wyngine keyboard demo", 256, 224, 3);

    game->run();

    return 0;
}