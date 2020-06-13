#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio.h"

enum WY_InstrumentType
{
    INS_HARMONICA,
    INS_BELL
};

class GameAudio : public WY_Audio
{
public:
    WY_InstrumentType instrumentType = INS_HARMONICA;
    harmonica *ih;
    bell *ib;

    GameAudio() : WY_Audio(44100, 1024, 1, 1000)
    {
        ih = new harmonica();
        ib = new bell();
    }

    void setInstrument(WY_InstrumentType it)
    {
        instrumentType = it;
    }

    void speak(WY_AudioNote note)
    {
        WY_Audio::speak(note);

        if (!ih->env.bNoteOn)
            ih->env.noteOn();

        if (!ib->env.bNoteOn)
            ib->env.noteOn();
    }

    void silence()
    {
        WY_Audio::silence();

        if (ih->env.bNoteOn)
            ih->env.noteOff();

        if (ib->env.bNoteOn)
            ib->env.noteOff();
    }

    std::string getInstrumentName()
    {
        switch (instrumentType)
        {
        case INS_HARMONICA:
            return "Harmonica";

        case INS_BELL:
            return "Bell";

        default:
            return "Unknown";
        }
    }

    double getAudioSample()
    {
        switch (instrumentType)
        {
        case INS_HARMONICA:
            return ih->sound(dTime, getNote());

        case INS_BELL:
            return ib->sound(dTime, getNote());

        default:
            return 0.0;
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
            audio->setInstrument(INS_HARMONICA);
        }
        else if (keyboard->isKeyPressed(SDLK_2))
        {
            audio->setInstrument(INS_BELL);
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
        std::string t3 = "\n\nfrequency / current sample :\n";
        std::string t4 = std::to_string(audio->mSampleRate);
        std::string t4a = " / ";
        std::string t4b = std::to_string(audio->dTime);
        std::string t5 = "\n\nSample size         : ";
        std::string t6 = std::to_string(audio->mSampleSize);
        std::string t7 = "\nAmplitude (volume)  : ";
        std::string t8 = std::to_string(audio->mAmplitude);
        std::string t9 = "\nChannels (speakers) : ";
        std::string t10 = std::to_string(audio->mChannels);

        std::string s1 = "\n\nInstrument : ";
        std::string s2 = audio->getInstrumentName();
        std::string s3 = "\nOctave     : ";
        std::string s4 = std::to_string(audio->mOctave);

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t4a + t4b + t5 + t6 + t7 + t8 + t9 + t10 + s1 + s2 + s3 + s4);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}