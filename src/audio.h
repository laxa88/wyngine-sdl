// https://codereview.stackexchange.com/questions/41086/play-some-sine-waves-with-sdl2

// Audio programming theory
// https://www.youtube.com/watch?v=GjmcXfgKq78

// Music note frequencies
// https://pages.mtu.edu/~suits/notefreqs.html

// NES audio channels
// https://www.youtube.com/watch?v=la3coK5pq5w

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

    // Frequency, a.k.a. number of samples per second. Higher value = higher accuracy
    int mSampleRate;
    // Current sample position (audio dTime)
    int mSampleIndex = 0;

    // Buffer size for samples. More samples = better accuracy, but slower performance.
    int mSampleSize;

    // Number of channels, e.g. mono = 1, stereo = 2, etc.
    int mChannels;

    // Volume. 0 = silence, 1000 = max
    int mAmplitude;

    /**
     * Determines sampleSize format (range). Larger types = larger sample range.
     * If the format is too large, you may end up hearing nothing because the
     * buffer data's values are all too low to be audible. The inverse is also true.
     *
     * i.e.: Lower format gives off a DOS-like sound, higher format is smoother and modern.
    */
    SDL_AudioFormat audioFormat = AUDIO_S16;

    WY_Audio(unsigned int sampleRate = 44100, unsigned int sampleSize = 1024, unsigned int channels = 1, unsigned int amplitude = 500)
    {
        SDL_Init(SDL_INIT_AUDIO);

        SDL_zero(wantSpec);
        SDL_zero(haveSpec);

        wantSpec.freq = sampleRate;
        wantSpec.format = audioFormat;
        wantSpec.channels = channels;
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

        mSampleRate = haveSpec.freq;
        mSampleSize = haveSpec.size / 4;
        mChannels = channels;
        mAmplitude = amplitude;

        play();
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

    virtual Sint16 getAudioSample(double time)
    {
        return mAmplitude * std::sin(2.0f * M_PI * time * 440.0f); // A4
    }

    virtual void updateAudio(Uint8 *stream, int streamLen)
    {
        // printf("\nsample: %d, %d", mSampleIndex, streamLen);

        Sint16 *buffer = (Sint16 *)stream;

        /**
         * stream length is (sampleSize * channels * byte format).
         *
         * e.g.
         * sampleSize = 1024
         * channels = 2
         * audioFormat = AUDIO_S16
         * streamLen = 1024 * 2 * 2 (S16 = 2 bytes) = 4096
         */
        int bufferLength = streamLen / 2; // 2 bytes per sample for AUDIO_S16SYS

        for (int i = 0; i < bufferLength; i++)
        {
            double samplePos = (double)mSampleIndex / (double)mSampleRate;
            buffer[i] = getAudioSample(samplePos);

            mSampleIndex++;
            mSampleIndex %= mSampleRate;
        }
    }
};

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;
int sample_nr = 0;

void audioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    static_cast<WY_Audio *>(user_data)->updateAudio(raw_buffer, bytes);
}
