#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio.h"

class GameAudio : public WY_Audio
{
public:
    WY_OscillatorType mInstrument = OSC_SINE;

    GameAudio() : WY_Audio(44100, 1024, 1, 1000) {}

    void setInstrument(WY_OscillatorType nType)
    {
        mInstrument = nType;
    }

    double getAudioSample(double dTime)
    {
        return WY_Oscillator::oscillate(getNote(), dTime, mInstrument);
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
    Game() : Wyngine("Wyngine audio demo", 256, 224, 2)
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

        if (keyboard->isKeyPressed(SDLK_q))
        {
            audio->mSampleRate += 1000;
        }
        else if (keyboard->isKeyPressed(SDLK_a))
        {
            audio->mSampleRate -= 1000;
        }
        // else if (keyboard->isKeyPressed(SDLK_w))
        // {
        //     audio->mSampleSize *= 2;
        // }
        // else if (keyboard->isKeyPressed(SDLK_s))
        // {
        //     audio->mSampleSize /= 2;
        // }
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
            audio->setInstrument(OSC_SINE);
        }
        else if (keyboard->isKeyPressed(SDLK_2))
        {
            audio->setInstrument(OSC_SQUARE);
        }
        else if (keyboard->isKeyPressed(SDLK_3))
        {
            audio->setInstrument(OSC_TRIANGLE);
        }
        else if (keyboard->isKeyPressed(SDLK_4))
        {
            audio->setInstrument(OSC_SAW);
        }
        else if (keyboard->isKeyPressed(SDLK_5))
        {
            audio->setInstrument(OSC_NOISE);
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
        std::string t1 = "Time elapsed        : ";
        std::string t2 = std::to_string(timer->getTimeSinceStart());
        std::string t3 = "\nSample rate (freq)  : ";
        std::string t4 = std::to_string(audio->mSampleRate);
        std::string t5 = "\nSample size         : ";
        std::string t6 = std::to_string(audio->mSampleSize);
        std::string t7 = "\nAmplitude (volume)  : ";
        std::string t8 = std::to_string(audio->mAmplitude);
        std::string t9 = "\nChannels (speakers) : ";
        std::string t10 = std::to_string(audio->mChannels);

        std::string s1 = "\n\nInstrument : ";
        std::string s2 = WY_Oscillator::getOscillatorTypeName(audio->mInstrument);
        std::string s3 = "\nOctave     : ";
        std::string s4 = std::to_string(audio->mOctave);

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + s1 + s2 + s3 + s4);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}