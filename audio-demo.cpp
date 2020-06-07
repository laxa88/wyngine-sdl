#include "src/wyngine.h"

class Game : public Wyngine
{
    int frame;
    WY_Image *mFontImage;
    WY_MonoFont *mFont;

    void loadMedia()
    {
        mFontImage = loadPNG(mRenderer, "assets/ascii-bnw.png");
    }

public:
    Game() : Wyngine("Wyngine audio demo", 256, 224, 3)
    {
        loadMedia();

        mFont = new WY_MonoFont(mFontImage->texture, 8, 4, {8, 8, 240, 208});
        mFont->setDebug(true);
    }

    void onUpdate()
    {
        if (keyboard->isKeyPressed(SDLK_q))
        {
            audio->mSampleRate += 1000;
        }
        else if (keyboard->isKeyPressed(SDLK_a))
        {
            audio->mSampleRate -= 1000;
        }
        else if (keyboard->isKeyPressed(SDLK_w))
        {
            audio->mSampleSize *= 2;
        }
        else if (keyboard->isKeyPressed(SDLK_s))
        {
            audio->mSampleSize /= 2;
        }
        else if (keyboard->isKeyPressed(SDLK_e))
        {
            audio->mAmplitude += 100;
        }
        else if (keyboard->isKeyPressed(SDLK_d))
        {
            audio->mAmplitude -= 100;
        }
    }

    void onRender()
    {
        std::string t1 = "Time elapsed       : ";
        std::string t2 = std::to_string(timer->getTimeSinceStart());
        std::string t3 = "\nSample rate (freq) : ";
        std::string t4 = std::to_string(audio->mSampleRate);
        std::string t5 = "\nSample size        : ";
        std::string t6 = std::to_string(audio->mSampleSize);
        std::string t7 = "\nAmplitude (volume) : ";
        std::string t8 = std::to_string(audio->mAmplitude);
        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}