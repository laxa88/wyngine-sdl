#include "../math.h"

namespace wyaudio
{
    enum OscillatorType
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

    class Oscillator
    {
    public:
        static std::string getOscillatorName(OscillatorType nType)
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
        static double oscillate(double dHertz, double dTime, OscillatorType nType, double dLFOHertz = 0.0, double dLFOAmplitude = 0.0)
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
                // FIXME: use angular frequency
                return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));

            case OSC_NOISE:
                return 2.0 * ((double)wyrandom(RAND_MAX) / (double)RAND_MAX) - 1.0;

            case OSC_UFO:
                return sin(TWO_PI * dHertz * dTime + 0.01 * dHertz * sin(TWO_PI * 5.0 * dTime));

            default:
                return 0;
            }
        }
    };
}; // namespace wyaudio