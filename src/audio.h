// https://codereview.stackexchange.com/questions/41086/play-some-sine-waves-with-sdl2

// Oscillators and Envelopes
// https://www.youtube.com/watch?v=OSCzKOqtgcA

// Wrapping sample phase
// https://codelabs.developers.google.com/codelabs/making-waves-1-synth/#3

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

#define PI M_PI
#define TWO_PI (2.0 * M_PI)
#define BASE_FREQ_A0 27.50f
#define ALMOST_SILENT 0.001

struct WY_Envelope
{
    double dAttackTime;
    double dDecayTime;
    double dStartAmplitude;
    double dSustainAmplitude;
    double dReleaseTime;

    double dTriggerOnTime;
    double dTriggerOffTime;
    bool bNoteOn;

    WY_Envelope()
    {
        dAttackTime = 100.0;
        dDecayTime = 100.0;
        dStartAmplitude = 1.0;
        dSustainAmplitude = 0.8;
        dReleaseTime = 200.0;

        dTriggerOnTime = 0.0;
        dTriggerOffTime = 0.0;
        bNoteOn = false;
    }

    double getAmplitude()
    {
        double dTime = SDL_GetTicks();
        double dAmplitude = 0.0;

        if (bNoteOn)
        {
            double dLifeTime = dTime - dTriggerOnTime;

            if (dLifeTime <= dAttackTime)
            {
                // Attack
                dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
            }
            else if (dLifeTime <= (dAttackTime + dDecayTime))
            {
                // Decay
                dAmplitude = dStartAmplitude - ((dLifeTime - dAttackTime) / dDecayTime) * (dStartAmplitude - dSustainAmplitude);
            }
            else
            {
                // Sustain
                dAmplitude = dSustainAmplitude;
            }
        }
        else
        {
            // Release
            dAmplitude = dSustainAmplitude - (((dTime - dTriggerOffTime) / dReleaseTime) * dSustainAmplitude);
        }

        if (dAmplitude < ALMOST_SILENT)
        {
            dAmplitude = 0.0;
        }

        return dAmplitude;
    }

    void noteOn()
    {
        dTriggerOnTime = SDL_GetTicks();
        bNoteOn = true;
    }

    void noteOff()
    {
        dTriggerOffTime = SDL_GetTicks();
        bNoteOn = false;
    }
};

enum WY_OscillatorType
{
    OSC_SINE,
    OSC_SQUARE,
    OSC_TRIANGLE,
    OSC_SAW_ANALOGUE,
    OSC_SAW_OPTIMIZED,
    OSC_NOISE,
    OSC_UFO,
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
        case OSC_SAW_ANALOGUE:
            return "saw wave (analogue, warm)";
        case OSC_SAW_OPTIMIZED:
            return "saw wave (optimized, harsh)";
        case OSC_NOISE:
            return "noise";
        case OSC_UFO:
            return "woo wee woo sound";
        default:
            return "unknown";
        }
    }

    // Return a value between -1 to 1
    static double oscillate(double dHertz, double dTime, WY_OscillatorType nType, double dLFOHertz = 0.0, double dLFOAmplitude = 0.0)
    {
        double dFreq = TWO_PI * dHertz * dTime + (dLFOAmplitude * dHertz * sin(TWO_PI * dLFOHertz * dTime));

        switch (nType)
        {
        case OSC_SINE:
            return sin(dFreq);

        case OSC_SQUARE:
            return sin(dFreq) > 0.0 ? 1.0 : -1.0;

        case OSC_TRIANGLE:
            return asin(sin(dFreq));

        case OSC_SAW_ANALOGUE:
        {
            double dOutput = 0.0;

            for (double n = 1.0; n < 10.0; n++)
            {
                dOutput += (sin(n * dFreq)) / n;
            }

            return dOutput;
        }

        case OSC_SAW_OPTIMIZED:
            return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));

        case OSC_NOISE:
            return 2.0 * ((double)random(RAND_MAX) / (double)RAND_MAX) - 1.0;

        case OSC_UFO:
            return sin(TWO_PI * dHertz * dTime + 0.01 * dHertz * sin(TWO_PI * 5.0 * dTime));

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

    // Current sample (phase) position
    // Reference: https://en.wikipedia.org/wiki/Phase_(waves)
    double dTime = 0.0;

    // Buffer size for samples. More samples = better accuracy, but slower performance.
    int mSampleSize;

    // Number of channels, e.g. mono = 1, stereo = 2, etc.
    int mChannels;

    // Volume. 0 = silence, 1000 = normal volume
    int mAmplitude;

    WY_AudioNote mNote = NOTE_A;
    int mOctave = 4;
    float mPlaying = 0.f;

    WY_Envelope envelope;

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

        if (!envelope.bNoteOn)
        {
            mPlaying = 1.f;
            envelope.noteOn();
        }
    }

    void silence()
    {
        if (envelope.bNoteOn)
        {
            mPlaying = 0.f;
            envelope.noteOff();
        }
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
    virtual double getAudioSample()
    {
        return WY_Oscillator::oscillate(getNote(), dTime, OSC_SINE);
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

        double dTimeDelta = 1.0 / (double)mSampleRate;

        for (int i = 0; i < bufferLength; i++)
        {
            buffer[i] = envelope.getAmplitude() * mAmplitude * getAudioSample();

            dTime += dTimeDelta;

            // dTime doesn't have to wrap;
            // While basic (e.g. sine) waves theoretically can go on forever,
            // non-basic waves do not have a 2PI and won't cleanly wrap.
            // We assume in such cases we will turn them off manually,
            // and reset dTime so that the wave plays cleanly from beginning.
        }
    }
};

void audioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    static_cast<WY_Audio *>(user_data)->updateAudio(raw_buffer, bytes);
}
