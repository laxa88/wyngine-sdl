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
            dAttackTime = 0.01;
            dDecayTime = 0.1;
            dStartAmplitude = 1.0;
            dSustainAmplitude = 0.8;
            dReleaseTime = 0.2;
        }

        // Returns amplitude value between 0.0 to 1.0
        double getAmplitude(const double dTime, const double dTimeOn, const double dTimeOff)
        {
            double dAmplitude = 0.0;
            double dReleaseAmplitude = 0.0;

            if (dTimeOn > dTimeOff)
            {
                double dLifeTime = dTime - dTimeOn;

                if (dLifeTime <= dAttackTime)
                    dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;

                if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime + dDecayTime))
                    dAmplitude = ((dLifeTime - dAttackTime) / dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;

                if (dLifeTime > (dAttackTime + dDecayTime))
                    dAmplitude = dSustainAmplitude;
            }
            else
            {
                double dLifeTime = dTimeOff - dTimeOn;

                if (dLifeTime <= dAttackTime)
                    dReleaseAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;

                if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime + dDecayTime))
                    dReleaseAmplitude = ((dLifeTime - dAttackTime) / dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;

                if (dLifeTime > (dAttackTime + dDecayTime))
                    dReleaseAmplitude = dSustainAmplitude;

                dAmplitude = ((dTime - dTimeOff) / dReleaseTime) * (0.0 - dReleaseAmplitude) + dReleaseAmplitude;
            }

            if (dAmplitude <= ALMOST_SILENT)
            {
                dAmplitude = 0.0;
            }

            return dAmplitude;
        }
    };
} // namespace wyaudio