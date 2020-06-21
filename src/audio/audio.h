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
#include "../math.h"

namespace wyaudio
{
    void audioCallback(void *userData, Uint8 *stream, int streamLen);

    class WY_Audio
    {
        SDL_AudioDeviceID deviceId;
        SDL_AudioSpec wantSpec;
        SDL_AudioSpec haveSpec;

        bool bInit = false;

        // Frequency, a.k.a. number of samples per second. Higher value = higher accuracy
        int mSampleRate; // e.g. 44100

        // Current sample (phase) position
        // Reference: https://en.wikipedia.org/wiki/Phase_(waves)
        double dTime = 0.0;

        // Buffer size for samples. More samples = better accuracy, but slower performance.
        int mSampleSize;

        // Number of channels, e.g. mono = 1, stereo = 2, etc.
        int mChannels;

        // Volume. 0 = silence, 1000 = normal volume
        int mAmplitude;

        /**
         * Determines sampleSize format (range). Larger types = larger sample range.
         * If the format is too large, you may end up hearing nothing because the
         * buffer data's values are all too low to be audible. The inverse is also true.
         *
         * i.e.: Lower format gives off a DOS-like sound, higher format is smoother and modern.
        */
        SDL_AudioFormat audioFormat = AUDIO_S16;

    protected:
        // Overwrite this to create your own audio sample.
        // Do not printf/log here as it will be very slow;
        // It runs at a high frequency, e.g. ~44100 per frame
        // - Runs in audio thread, use mutex when possible.
        // - Expects a return value between -1 to 1.
        virtual double getAudioSample() = 0;

    public:
        WY_Audio() {}

        ~WY_Audio()
        {
            if (deviceId > 0)
            {
                SDL_CloseAudioDevice(deviceId);
            }

            delete &wantSpec;
            delete &haveSpec;
        }

        // ==================================================
        // Getters
        // ==================================================

        bool isPlaying()
        {
            return SDL_GetAudioDeviceStatus(deviceId) == SDL_AUDIO_PLAYING;
        }

        int getAmplitude()
        {
            return mAmplitude;
        }

        int getSampleRate()
        {
            return mSampleRate;
        }

        int getSampleSize()
        {
            return mSampleSize;
        }

        int getChannelLen()
        {
            return mChannels;
        }

        double getDTime()
        {
            return dTime;
        }

        // ==================================================
        // Setters
        // ==================================================

        // Increase or decrease amplitude
        void changeAmplitude(int vol)
        {
            mAmplitude += vol;
        }

        // Increase or decrease frequency (pitch)
        void changeSampleRate(int rate)
        {
            mSampleRate += rate;
        }

        // ==================================================
        // Methods
        // ==================================================

        void init(unsigned int sampleRate = 44100, unsigned int sampleSize = 1024, unsigned int channels = 2, unsigned int amplitude = 500)
        {
            if (SDL_Init(SDL_INIT_AUDIO) < 0)
            {
                SDL_Log("SDL audio could not initialize! SDL_Error: %s\n", SDL_GetError());
                return;
            }

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
                SDL_Log("\nFailed to open audio: %s\n", SDL_GetError());
                return;
            }
            if (wantSpec.format != haveSpec.format)
            {
                SDL_Log("\nFailed to get the desired AudioSpec");
            }

            mSampleRate = haveSpec.freq;
            mSampleSize = haveSpec.size / haveSpec.channels;
            mChannels = haveSpec.channels;
            mAmplitude = amplitude;

            bInit = true;
        }

        // Plays audio. Will also init if called for the first time.
        void play()
        {
            if (!bInit)
                init();

            SDL_PauseAudioDevice(deviceId, 0);
        }

        void pause()
        {
            SDL_PauseAudioDevice(deviceId, 1);
        }

        // Used by audioCallback. Do not call or override this.
        void updateAudio(Uint8 *stream, int streamLen)
        {
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
                buffer[i] = mAmplitude * getAudioSample();

                dTime += dTimeDelta;

                // dTime doesn't have to wrap;
                // While basic (e.g. sine) waves theoretically can go on forever,
                // non-basic waves do not have a 2PI and won't cleanly wrap.
                // We assume in such cases we will turn them off manually,
                // and reset dTime so that the wave plays cleanly from beginning.
            }
        }
    };

    // SDL audio callback does not work in class methods; Only works via C method,
    // hence the need of intermediary audioCallback method.
    void audioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
    {
        static_cast<WY_Audio *>(user_data)->updateAudio(raw_buffer, bytes);
    }
}; // namespace wyaudio
