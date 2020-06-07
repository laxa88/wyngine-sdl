#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio.h"

class GameAudio : public WY_Audio
{
    int mInstrument = 1;

public:
    GameAudio() : WY_Audio(44100, 1024, 1, 500) {}

    void setInstrument(int instrument)
    {
        mInstrument = instrument;
    }

    std::string getInstrumentName()
    {
        switch (mInstrument)
        {
        case 1:
            return "sine wave";
        case 2:
            return "square wave";
        default:
            return "unknown";
        }
    }

    double getAudioSample(double time)
    {
        switch (mInstrument)
        {
        case 1:
            return std::sin(2.0f * M_PI * time * getNote());

        case 2:
        {
            double output = std::sin(2.0f * M_PI * time * getNote());
            if (output > 0.0)
            {
                return 1.0;
            }
            else
            {
                return -1.0;
            }
        }

        default:
            return 0;
        }
    }
};

class Game : public Wyngine
{
    int frame;
    WY_Image *mFontImage;
    WY_MonoFont *mFont;
    GameAudio *audio;

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

        audio = new GameAudio();
    }

    ~Game()
    {
        SDL_DestroyTexture(mFontImage->texture);
        delete mFontImage;

        delete mFont;

        delete audio;
    }

    void onUpdate()
    {
        // audio settings

        if (keyboard->isKeyDown(SDLK_q))
        {
            audio->mSampleRate += 10;
        }
        else if (keyboard->isKeyDown(SDLK_a))
        {
            audio->mSampleRate -= 10;
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

        // instrument settings

        if (keyboard->isKeyPressed(SDLK_1))
        {
            audio->setInstrument(1);
        }
        else if (keyboard->isKeyPressed(SDLK_2))
        {
            audio->setInstrument(2);
        }

        // music octave

        if (keyboard->isKeyPressed(SDLK_UP))
        {
            audio->increaseOctave();
        }
        else if (keyboard->isKeyPressed(SDLK_DOWN))
        {
            audio->decreaseOctave();
        }

        // music notes

        if (keyboard->isKeyDown(SDLK_z))
        {
            audio->speak(NOTE_A);
        }
        else if (keyboard->isKeyDown(SDLK_s))
        {
            audio->speak(NOTE_AS);
        }
        else if (keyboard->isKeyDown(SDLK_x))
        {
            audio->speak(NOTE_B);
        }
        else if (keyboard->isKeyDown(SDLK_c))
        {
            audio->speak(NOTE_C);
        }
        else if (keyboard->isKeyDown(SDLK_f))
        {
            audio->speak(NOTE_CS);
        }
        else if (keyboard->isKeyDown(SDLK_v))
        {
            audio->speak(NOTE_D);
        }
        else if (keyboard->isKeyDown(SDLK_g))
        {
            audio->speak(NOTE_DS);
        }
        else if (keyboard->isKeyDown(SDLK_b))
        {
            audio->speak(NOTE_E);
        }
        else if (keyboard->isKeyDown(SDLK_n))
        {
            audio->speak(NOTE_F);
        }
        else if (keyboard->isKeyDown(SDLK_j))
        {
            audio->speak(NOTE_FS);
        }
        else if (keyboard->isKeyDown(SDLK_m))
        {
            audio->speak(NOTE_G);
        }
        else if (keyboard->isKeyDown(SDLK_k))
        {
            audio->speak(NOTE_GS);
        }
        else
        {
            audio->silence();
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

        std::string t9 = "\n\nInstrument : ";
        std::string t10 = audio->getInstrumentName();
        std::string t11 = "\nOctave     : ";
        std::string t12 = std::to_string(audio->mOctave);

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + t11 + t12);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}