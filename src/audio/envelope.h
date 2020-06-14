#include <SDL2/SDL.h>

#define ALMOST_SILENT 0.001

namespace wyaudio
{
    struct Envelope
    {
        double dAttackTime;
        double dDecayTime;
        double dStartAmplitude;
        double dSustainAmplitude;
        double dReleaseTime;

        double dTriggerOnTime;
        double dTriggerOffTime;
        bool bNoteOn;

        Envelope()
        {
            dAttackTime = 10.0;
            dDecayTime = 1000.0;
            dStartAmplitude = 1.0;
            dSustainAmplitude = 0.0;
            dReleaseTime = 1000.0;

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
} // namespace wyaudio