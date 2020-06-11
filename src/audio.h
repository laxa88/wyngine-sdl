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

#define BASE_FREQ_A0 27.50f

enum WY_OscillatorType
{
    OSC_SINE,
    OSC_SQUARE,
    OSC_TRIANGLE,
    OSC_SAW,
    OSC_NOISE,
    OSC_UNKNOWN
};

class WY_Oscillator
{
public:
    static std::string getOscillatorTypeName(WY_OscillatorType nType)
    {
        switch (nType)
        {
        case OSC_SINE:
            return "sine wave";
        case OSC_SQUARE:
            return "square wave";
        case OSC_TRIANGLE:
            return "triangle wave";
        case OSC_SAW:
            return "saw wave";
        case OSC_NOISE:
            return "noise";
        default:
            return "unknown";
        }
    }

    static double oscillate(double dFreq, double dTime, WY_OscillatorType nType)
    {
        switch (nType)
        {
        case OSC_SINE:
            return sin(2.0f * M_PI * dTime * dFreq);

        case OSC_SQUARE:
        {
            double output = sin(2.0f * M_PI * dTime * dFreq);
            if (output > 0.0)
            {
                return 1.0;
            }
            else
            {
                return -1.0;
            }
        }

        case OSC_TRIANGLE:
        {
            return asin(sin(2.0f * M_PI * dTime * dFreq)) * 2.0 / M_PI;
        }

        case OSC_SAW:
        {
            return (2.0 * M_PI) * (dFreq * M_PI * fmod(dTime, 1.0 / dFreq) - (M_PI / 2.0));
        }

        case OSC_NOISE:
            // TODO
            return 0;

        default:
            return 0;
        }
    }
};

enum WY_AudioNote
{
    NOTE_A,
    NOTE_AS,
    NOTE_B,
    NOTE_C,
    NOTE_CS,
    NOTE_D,
    NOTE_DS,
    NOTE_E,
    NOTE_F,
    NOTE_FS,
    NOTE_G,
    NOTE_GS,
};

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

    WY_AudioNote mNote = NOTE_A;
    int mOctave = 4;
    float mPlaying = 0.f;

    /**
     * Determines sampleSize format (range). Larger types = larger sample range.
     * If the format is too large, you may end up hearing nothing because the
     * buffer data's values are all too low to be audible. The inverse is also true.
     *
     * i.e.: Lower format gives off a DOS-like sound, higher format is smoother and modern.
    */
    SDL_AudioFormat audioFormat = AUDIO_S16;

    WY_Audio(unsigned int sampleRate = 44100, unsigned int sampleSize = 1024, unsigned int channels = 2, unsigned int amplitude = 500)
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
        mSampleSize = haveSpec.size / haveSpec.channels;
        mChannels = haveSpec.channels;
        mAmplitude = amplitude;

        play();
    }

    ~WY_Audio()
    {
        SDL_CloseAudioDevice(deviceId);

        delete &wantSpec;
        delete &haveSpec;
    }

    // Returns note in frequency
    double getNote()
    {
        double d12thRootOf2 = pow(2.0, 1.0 / 12);
        double octave = pow(2.0, mOctave);
        double res = BASE_FREQ_A0 * pow(d12thRootOf2, mNote) * (double)octave;

        return res;
    }

    void increaseOctave()
    {
        if (++mOctave > 7)
        {
            mOctave = 7;
        }
    }

    void decreaseOctave()
    {
        if (--mOctave < 1)
        {
            mOctave = 1;
        }
    }

    void speak(WY_AudioNote note)
    {
        mNote = note;
        mPlaying = 1.f;
    }

    void silence()
    {
        mPlaying = 0.f;
    }

    void play()
    {
        SDL_PauseAudioDevice(deviceId, 0);
    }

    void pause()
    {
        SDL_PauseAudioDevice(deviceId, 1);
    }

    // Overwrite this to create your own audio sample.
    // Do not printf here as it will be very slow;
    // It runs at a high frequency, e.g. ~44100 per frame
    // Expects a return value between -1 to 1.
    virtual double getAudioSample(double dTime)
    {
        return WY_Oscillator::oscillate(getNote(), dTime, OSC_SINE);

        // return std::sin(2.0f * M_PI * time * mNote);
    }

    virtual void updateAudio(Uint8 *stream, int streamLen)
    {
        // TODO: probably need to move this into update loops
        // to fix stutters in web build

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
            buffer[i] = mPlaying * mAmplitude * getAudioSample(samplePos);

            mSampleIndex++;
            // mSampleIndex %= mSampleRate;
        }
    }
};

void audioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    static_cast<WY_Audio *>(user_data)->updateAudio(raw_buffer, bytes);
}
