#include <vector>
#include <algorithm>
#include <mutex>

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
    wyaudio::InstrumentType instType;
    wyaudio::bell instBell;
    wyaudio::harmonica instHarm;
    wyaudio::square instSquare;

    wyaudio::Note note;
    wyaudio::MusicNote mNote;
    double mPlaying;

    GameAudio() : wyaudio::WY_Audio(44100, 1024, 1, 1000)
    {
        // note = new wyaudio::Note();

        mPlaying = 0.0f;
    }

    void setInstrument(wyaudio::InstrumentType it)
    {
        instType = it;
    }

    void noteOn(wyaudio::MusicNote note)
    {
        mNote = note;
        mPlaying = 1.f;
    }

    virtual void noteOff()
    {
        mPlaying = 0.f;
    }

    void playNote(wyaudio::MusicNote k, bool bNoteOn)
    {

        // note.id = (wyaudio::MusicNote)(k);
        // note.on = dTime;
        // note.active = true;

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
                n.on = dTime;
                n.channel = 1;
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
            if (!bNoteOn)
            {
                // Key is still held, so do nothing
                if (noteFound->off > noteFound->on)
                {
                    // Key has been pressed again during release phase
                    noteFound->on = dTime;
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

        // printf("\nNotes: %d", vecNotes.size());
    }

    double getAudioSample()
    {
        double res = wyaudio::osc(dTime, wyaudio::scale(mNote, mOctave));

        // printf("\nsample: %f", res);

        return res;

        // printf("\ngetAudioSample: %f, %d", dTime, note);

        // double dMixedOutput = 0.0;

        // for (auto &n : vecNotes)
        // {
        //     bool bNoteFinished = false;
        //     double dSound = 0.0;

        //     if (n.channel == 2)
        //         dSound = instBell.speak(dTime, n, bNoteFinished);
        //     if (n.channel == 1)
        //         dSound = instHarm.speak(dTime, n, bNoteFinished) * 0.5;

        //     dMixedOutput += dSound;

        //     if (bNoteFinished && n.off > n.on)
        //         n.active = false;
        // }

        // safe_remove<std::vector<wyaudio::Note>>(vecNotes, [](wyaudio::Note const &item) { return item.active; });

        // return dMixedOutput;
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

        // instrument settings

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
            // assume index 0 == C
            short keyCode = (unsigned char)("zsxcfvgbnjmk,l./"[k]);

            if (keyboard->isKeyDown(keyCode))
            {
                // printf("\ndown: %d", keyCode);
                audio->noteOn((wyaudio::MusicNote)k);
            }
            else if (keyboard->isKeyUp(keyCode))
            {
                // audio->noteOff((wyaudio::MusicNote)k);
                audio->noteOff();
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
        std::string s2 = wyaudio::getInstrumentName(audio->instType);
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