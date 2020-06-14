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

        Envelope()
        {
            dAttackTime = 10.0;
            dDecayTime = 100.0;
            dStartAmplitude = 1.0;
            dSustainAmplitude = 0.8;
            dReleaseTime = 200.0;
        }

        double getAmplitude(const double dTime, const double dTimeOn, const double dTimeOff)
        {
            double dAmplitude = 0.0;
            double dReleaseAmplitude = 0.0;

            if (dTimeOn > dTimeOff)
            {
                double dLifeTime = dTime - dTimeOn;

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
                double dLifeTime = dTimeOff - dTimeOn;

                if (dLifeTime <= dAttackTime)
                {
                    dReleaseAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
                }
                else if (dLifeTime <= (dAttackTime + dDecayTime))
                {
                    dReleaseAmplitude = dStartAmplitude - ((dLifeTime - dAttackTime) / dDecayTime) * (dStartAmplitude - dSustainAmplitude);
                }
                else
                {
                    dReleaseAmplitude = dSustainAmplitude;
                }

                dAmplitude = dReleaseAmplitude - (((dTime - dTimeOff) / dReleaseTime) * dReleaseAmplitude);
            }

            if (dAmplitude < ALMOST_SILENT)
            {
                dAmplitude = 0.0;
            }

            return dAmplitude;
        }
    };
} // namespace wyaudio