#include <SDL2/SDL.h>

#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio/midi.h"

class GameAudio : public wyaudio::WY_MidiPlayer
{
public:
    // TODO
    GameAudio()
    {
        wyaudio::WY_MidiFile *midi1 = new wyaudio::WY_MidiFile("assets/battle-theme-3.mid");
        wyaudio::WY_MidiFile *midi2 = new wyaudio::WY_MidiFile("assets/overworld-smb.mid");
        wyaudio::WY_MidiFile *midi3 = new wyaudio::WY_MidiFile("assets/pallet-town.mid");

        midiFiles.push_back(midi1);
        midiFiles.push_back(midi2);
        midiFiles.push_back(midi3);
    }
};

class Game : public Wyngine
{
    WY_Image *mFontImage;
    WY_MonoFont *mFont;
    GameAudio *audio;

    void loadMedia()
    {
        mFontImage = loadPNG(mRenderer, "assets/ascii-bnw.png");
    }

public:
    Game() : Wyngine("Wyngine midi demo", 256, 224, 2)
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
        // // key

        // for (int k = 0; k < 4; k++)
        // {
        //     short keyCode = (unsigned char)("1234"[k]);

        //     if (keyboard->isKeyPressed(keyCode))
        //     {
        //         // play different song
        //     }
        // }

        if (keyboard->isKeyPressed(SDLK_SPACE))
        {
            audio->play();
        }
    }

    void onRender()
    {
        std::string t1 = "Game dTime : ";
        std::string t2 = std::to_string(timer->getTimeSinceStart());
        std::string t3 = "\nAudio dTime : ";
        std::string t4 = std::to_string(audio->getDTime());
        std::string t5 = "\nAmplitude (volume) : ";
        std::string t6 = std::to_string(audio->getAmplitude());

        // std::string s1 = "\n\nSong                     : ";
        // std::string s2 = wyaudio::getInstrumentName(audio->instrument);
        // std::string s3 = "\nPlaying                  : ";
        // std::string s4 = std::to_string(audio->vecNotes.size());

        mFont->print(mRenderer, t1 + t2 + t3 + t4 + t5 + t6);
    }
};

int main(int argc, char *args[])
{
    Game *game = new Game();

    game->run();

    return 0;
}