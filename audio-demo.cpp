#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>

#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio/audio.h"
#include "src/audio/instrument.h"

typedef bool (*lambda)(wyaudio::Note const &item);
template <class T>
void safe_remove(T &v, lambda f)
{
    auto n = v.begin();
    while (n != v.end())
        if (!f(*n))
            n = v.erase(n);
        else
            ++n;
}

class GameAudio : public wyaudio::WY_Audio
{
public:
    std::vector<wyaudio::Note> vecNotes;
    SDL_mutex *muxNotes;

    wyaudio::InstrumentType instrument;
    wyaudio::bell instBell;
    wyaudio::harmonica instHarm;
    wyaudio::square instSquare;
    wyaudio::wave instWave;

    GameAudio() : wyaudio::WY_Audio()
    {
        muxNotes = SDL_CreateMutex();
        instrument = wyaudio::INS_HARMONICA;
    }

    ~GameAudio()
    {
        vecNotes.clear();
        delete &vecNotes;

        delete &instBell;
        delete &instHarm;
        delete &instSquare;
        delete &instWave;

        SDL_DestroyMutex(muxNotes);
    }

    void setInstrument(wyaudio::InstrumentType ins)
    {
        instrument = ins;
    }

    void playNote(wyaudio::MusicNote k, bool bNoteOn)
    {
        if (SDL_LockMutex(muxNotes) != 0)
            return;

        auto noteFound = std::find_if(vecNotes.begin(), vecNotes.end(), [&k](wyaudio::Note const &item) { return item.id == k; });

        if (noteFound == vecNotes.end())
        {
            // Note not found in vector

            if (bNoteOn)
            {
                // Key has been pressed so create a new note
                wyaudio::Note n;
                n.id = k;
                n.octave = mOctave;
                n.on = dTime;
                n.channel = instrument;
                n.active = true;

                // Add note to vector
                vecNotes.emplace_back(n);
            }
            else
            {
                // Note not in vector, but key has been released...
                // ...nothing to do
            }
        }
        else
        {
            // Note exists in vector
            if (bNoteOn)
            {
                // Key is still held, so do nothing
                if (noteFound->off > noteFound->on)
                {
                    // Key has been pressed again during release phase
                    noteFound->on = dTime;
                    noteFound->octave = mOctave;
                    noteFound->active = true;
                }
            }
            else
            {
                // Key has been released, so switch off
                if (noteFound->off < noteFound->on)
                {
                    noteFound->off = dTime;
                }
            }
        }

        SDL_UnlockMutex(muxNotes);
    }

    double getAudioSample()
    {
        if (SDL_LockMutex(muxNotes) != 0)
            return 0.0;

        double dMixedOutput = 0.0;

        for (auto &n : vecNotes)
        {
            bool bNoteFinished = false;
            double dSound = 0.0;

            // dSound = instHarm.speak(dTime, n, bNoteFinished);
            switch (n.channel)
            {
            case wyaudio::INS_HARMONICA:
                dSound = instHarm.speak(dTime, n, bNoteFinished);
                break;
            case wyaudio::INS_BELL:
                dSound = instBell.speak(dTime, n, bNoteFinished);
                break;
            case wyaudio::INS_SQUARE:
                dSound = instSquare.speak(dTime, n, bNoteFinished);
                break;
            case wyaudio::INS_WAVE:
                dSound = instWave.speak(dTime, n, bNoteFinished);
                break;
            default:
                dSound = 0.0;
                break;
            }

            dMixedOutput += dSound;

            if (bNoteFinished && n.off >= n.on)
            {
                n.active = false;
            }
        }

        safe_remove<std::vector<wyaudio::Note>>(vecNotes, [](wyaudio::Note const &item) { return item.active; });

        SDL_UnlockMutex(muxNotes);

        return dMixedOutput;
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

        if (keyboard->isKeyPressed('q'))
        {
            audio->mSampleRate += 1000;
        }
        else if (keyboard->isKeyPressed('a'))
        {
            audio->mSampleRate -= 1000;
        }
        else if (keyboard->isKeyPressed('e'))
        {
            audio->mAmplitude += 100;
        }
        else if (keyboard->isKeyPressed('d'))
        {
            audio->mAmplitude -= 100;
        }

        // instruments

        for (int k = 0; k < 4; k++)
        {
            short keyCode = (unsigned char)("1234"[k]);

            if (keyboard->isKeyPressed(keyCode))
            {
                audio->setInstrument((wyaudio::InstrumentType)k);
            }
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

        for (int k = 0; k < 16; k++)
        {
            short keyCode = (unsigned char)("zsxcfvgbnjmk,l./"[k]);

            if (keyboard->isKeyDown(keyCode))
            {
                audio->playNote((wyaudio::MusicNote)k, true);
            }
            else if (keyboard->isKeyUp(keyCode))
            {
                audio->playNote((wyaudio::MusicNote)k, false);
            }
        }

        if (keyboard->isKeyPressed(SDLK_SPACE))
        {
            if (audio->isPlaying())
            {
                audio->pause();
            }
            else
            {
                audio->play();
            }
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
        std::string s2 = wyaudio::getInstrumentName(audio->instrument);
        std::string s3 = "\nNotes      : ";
        std::string s4 = std::to_string(audio->vecNotes.size());
        std::string s5 = "\nOctave     : ";
        std::string s6 = std::to_string(audio->mOctave);

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t4a + t4b + t5 + t6 + t7 + t8 + t9 + t10 + s1 + s2 + s3 + s4 + s5 + s6);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}