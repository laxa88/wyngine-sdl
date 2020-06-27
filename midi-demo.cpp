#include <SDL2/SDL.h>

#include "src/wyngine.h"
#include "src/font.h"
#include "src/audio/midi.h"

struct PlayNote
{
    wyaudio::WY_MidiNote note;
    int channel = 0;
    Uint32 playStartTime = 0; // start time (absolute)
};

class GameAudio : public wyaudio::WY_MidiPlayer
{
    Uint32 dStartTime;
    Uint32 dCurrTime;

    wyaudio::WY_MidiFile *midi;
    int trackLen;
    int *noteIndices;
    int *completedTracks;
    bool bLoop;
    std::vector<PlayNote> activeNotes;

protected:
    void onPlay()
    {
        dTime = 0.0;
        dStartTime = SDL_GetTicks();
        printf("\nplay %d", dStartTime);
    }

    void onPause()
    {
        printf("\npause %d", dCurrTime);
    }

public:
    GameAudio()
    {
        // wyaudio::WY_MidiFile *midi1 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip-0.mid");
        // wyaudio::WY_MidiFile *midi2 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip-1.mid");
        // wyaudio::WY_MidiFile *midi3 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip-01.mid");
        // wyaudio::WY_MidiFile *midi3 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip2.mid");
        // wyaudio::WY_MidiFile *midi4 = new wyaudio::WY_MidiFile("assets/overworld-smb--06.mid");
        // wyaudio::WY_MidiFile *midi4 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip-0123.mid");
        // wyaudio::WY_MidiFile *midi5 = new wyaudio::WY_MidiFile("assets/overworld-smb-snip-0123b.mid");
        // wyaudio::WY_MidiFile *midi1 = new wyaudio::WY_MidiFile("assets/battle-theme-3.mid");
        // wyaudio::WY_MidiFile *midi2 = new wyaudio::WY_MidiFile("assets/overworld-smb.mid");
        // wyaudio::WY_MidiFile *midi3 = new wyaudio::WY_MidiFile("assets/pallet-town.mid");

        // midiFiles.push_back(midi1);
        // midiFiles.push_back(midi1);
        // midiFiles.push_back(midi2);
        // midiFiles.push_back(midi3);

        midi = new wyaudio::WY_MidiFile("assets/overworld-smb.mid");
        // midi = new wyaudio::WY_MidiFile("assets/overworld-zelda.mid");
        // midi = new wyaudio::WY_MidiFile("assets/pallet-town.mid");
        trackLen = midi->vecTracks.size();
        noteIndices = new int[trackLen];
        completedTracks = new int[trackLen];
        for (int i = 0; i < trackLen; i++)
        {
            noteIndices[i] = 0;
            completedTracks[i] = 0;
        }
        bLoop = true;

        dStartTime = SDL_GetTicks();
    }

    ~GameAudio()
    {
        delete midi;
        delete noteIndices;

        SDL_DestroyMutex(muxNotes);
    }

    void playNote(wyaudio::WY_MidiNote k, int channel)
    {
        if (SDL_LockMutex(muxNotes) != 0)
            return;

        activeNotes.push_back({k, channel, SDL_GetTicks()});

        SDL_UnlockMutex(muxNotes);
    }

    double getAudioSample()
    {
        if (SDL_LockMutex(muxNotes) != 0)
            return 0.0;

        double dMixedOutput = 0.0;

        for (auto &n : activeNotes)
        {
            bool bNoteFinished = false;
            double dSound = 0.0;

            switch (n.channel)
            {
            case 1:
                dSound = chan0.speak2(getDTime(), n.note.nKey);
                break;
            case 2:
                dSound = chan1.speak2(getDTime(), n.note.nKey);
                break;
            case 3:
                dSound = chan2.speak2(getDTime(), n.note.nKey);
                break;
            // case 4:
            //     dSound = chan3.speak2(getDTime(), n.note.nKey);
            //     break;
            default:
                dSound = chan0.speak2(getDTime(), n.note.nKey);
                break;
            }

            dMixedOutput += dSound;
        }

        SDL_UnlockMutex(muxNotes);

        return dMixedOutput;
    }

    void reset(bool play = false)
    {
        printf("\n RESET \n");

        // reset start time to current time so we can loop
        dStartTime = SDL_GetTicks();

        // reset all tracks' note index to beginning
        for (int i = 0; i < trackLen; i++)
        {
            noteIndices[i] = 0;

            if (play)
            {
                completedTracks[i] = 0;
            }
        }
    }

    void update()
    {
        auto n = activeNotes.begin();
        while (n != activeNotes.end())
        {
            if (SDL_GetTicks() >= n->playStartTime + n->note.nDuration)
            {
                n = activeNotes.erase(n);
            }
            else
            {
                ++n;
            }
        }

        dCurrTime = SDL_GetTicks() - dStartTime;
        printf("\nelapsed: %d", dCurrTime);

        if (isPlaying())
        {
            for (int i = 0; i < trackLen; i++)
            {
                auto &track = midi->vecTracks.at(i);
                auto &notes = track.vecNotes;
                if (noteIndices[i] >= notes.size())
                {
                    // skip track if all notes are played
                    completedTracks[i] = 1;
                    continue;
                }

                auto &note = notes.at(noteIndices[i]);
                // printf(", play: %d/%d/%d/%d/%d", notes.size(), i, note.nStartTime, note.nDuration, note.nKey);

                if (dCurrTime >= note.nStartTime)
                {
                    // add this note to currently played notes
                    playNote(note, i);

                    // track next note
                    noteIndices[i] += 1;
                }
            }

            int completed = 0;
            for (int i = 0; i < trackLen; i++)
            {
                completed += completedTracks[i];
                if (completed == trackLen)
                {
                    reset(bLoop);
                }
            }

            // STOPPED HERE
            /*
                // init these outside update()
                init noteCounter to track.length
                init each element in noteCounter to 0
                init completedTracks = 0

                for event in track
                    if noteCounter[event] >= event.length
                        continue;

                    if event[noteCounter].startTime >= dCurrTime
                        playNote
                        increase noteCounter[event]
                        if noteCounter[event] >= event.length
                            completedTracks += 1

                if completedTracks == track.length
                    if LOOP
                        reset dStartTime
                        reset noteCounter
                        reset completedTracks
                    else
                        pause()
            */
        }
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
        audio->init();
        audio->play();
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
        audio->update();

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