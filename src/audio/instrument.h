#include <string>

#include "oscillator.h"
#include "envelope.h"

namespace wyaudio
{
    enum InstrumentType
    {
        INS_HARMONICA,
        INS_BELL
    };

    struct Instrument
    {
        InstrumentType type;
        std::string name;

        double dVolume;
        Envelope env;

        virtual double sound(double dTime, double dFreq) = 0;
    };

    struct harmonica : public Instrument
    {
        harmonica()
        {
            type = INS_HARMONICA;
            name = "Harmonica";

            env.dAttackTime = 100.0;
            env.dDecayTime = 100.0;
            env.dStartAmplitude = 1.0;
            env.dSustainAmplitude = 0.8;
            env.dReleaseTime = 200.0;
        }

        double sound(double dTime, double dFreq)
        {
            double dOutput = env.getAmplitude() * (1.0 * Oscillator::oscillate(dFreq, dTime, OSC_SQUARE, 5.0, 0.001) +
                                                   0.5 * Oscillator::oscillate(dFreq * 1.5, dTime, OSC_SQUARE) +
                                                   0.25 * Oscillator::oscillate(dFreq * 2.0, dTime, OSC_SQUARE) +
                                                   0.05 * Oscillator::oscillate(0, dTime, OSC_NOISE));

            return dOutput;
        }
    };

    struct bell : public Instrument
    {
        bell()
        {
            type = INS_BELL;
            name = "Bell";

            env.dAttackTime = 5.0;
            env.dDecayTime = 1000.0;
            env.dStartAmplitude = 1.0;
            env.dSustainAmplitude = 0.0;
            env.dReleaseTime = 1000.0;
        }

        double sound(double dTime, double dFreq)
        {
            double dOutput = env.getAmplitude() * (1.0 * Oscillator::oscillate(dFreq * 2.0, dTime, OSC_SINE, 5.0, 0.001) +
                                                   0.5 * Oscillator::oscillate(dFreq * 3.0, dTime, OSC_SINE) +
                                                   0.25 * Oscillator::oscillate(dFreq * 4.0, dTime, OSC_SINE));

            return dOutput;
        }
    };
} // namespace wyaudio