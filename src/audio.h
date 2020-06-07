// https://codereview.stackexchange.com/questions/41086/play-some-sine-waves-with-sdl2

// Audio programming theory
// https://www.youtube.com/watch?v=GjmcXfgKq78

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

void audioCallback(void *userData, Uint8 *stream, int streamLen);

class WY_Audio
{
    SDL_AudioDeviceID deviceId;

public:
    SDL_AudioSpec wantSpec;
    SDL_AudioSpec haveSpec;

    int sampleRate = 44100; // number of samples per second
    int sampleSize = 1024;  // accuracy of each sample
    int amplitude = 28000;  // volume
    int dTime = 0;          // deltaTime in milliseconds

    WY_Audio()
    {
        SDL_Init(SDL_INIT_AUDIO);

        SDL_zero(wantSpec);
        SDL_zero(haveSpec);

        wantSpec.freq = sampleRate;
        wantSpec.format = AUDIO_S16SYS;
        wantSpec.channels = 1;
        wantSpec.samples = sampleSize;
        wantSpec.callback = audioCallback;
        wantSpec.userdata = this;

        deviceId = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, 0);
        if (deviceId == 0)
        {
            printf("\nFailed to open audio: %s\n", SDL_GetError());
            return;
        }
        if (wantSpec.format != haveSpec.format)
            printf("\nFailed to get the desired AudioSpec");

        sampleRate = haveSpec.freq;
        sampleSize = haveSpec.size / 4;
    }

    ~WY_Audio()
    {
        SDL_CloseAudioDevice(deviceId);

        delete &wantSpec;
        delete &haveSpec;
    }

    void play()
    {
        SDL_PauseAudioDevice(deviceId, 0);
    }

    void pause()
    {
        SDL_PauseAudioDevice(deviceId, 1);
    }

    int sample_nr2 = 0;

    virtual void updateAudio(Uint8 *stream, int streamLen)
    {
        // TODO
        // - use dTime instead of sample_nr2
        // - move onUpdateAudio to inside the buffer loop, pass in dTime, allow custom waves
        // - add 4 channels ala NES

        // Sint16 *buffer = (Sint16 *)raw_buffer;
        // int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
        // printf("\nsample: %d", sample_nr);
        // printf("\nsample: %d, %d", sample_nr2, streamLen);

        // for (int i = 0; i < length; i++, sample_nr++)
        // {
        //     double time = (double)sample_nr / (double)SAMPLE_RATE;
        //     buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
        // }

        Sint16 *buffer = (Sint16 *)stream;
        int length = streamLen / 2; // 2 bytes per sample for AUDIO_S16SYS

        // int dTime = dTime / sampleRate;
        // double waveFunc = sin(2.0f * M_PI * 441.0f);

        for (int i = 0; i < length; i++, sample_nr2++)
        {
            double samplePos = (double)sample_nr2 / (double)sampleRate;
            buffer[i] = amplitude * std::sin(2.0f * M_PI * samplePos * 441.0f);
            // buffer[i] = (Sint16)(amplitude * sin(2.0f * M_PI * 441.0f * time));
        }
    }

    void update(int dt)
    {
        dTime += dt;
    }
};

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;
int sample_nr = 0;

void audioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    static_cast<WY_Audio *>(user_data)->updateAudio(raw_buffer, bytes);
}
