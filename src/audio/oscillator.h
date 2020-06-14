#include "../math.h"

namespace wyaudio
{
    enum WaveType
    {
        OSC_SINE,
        OSC_SQUARE, // aka Pulse
        OSC_TRIANGLE,
        OSC_SAW_ANALOGUE,
        OSC_SAW_OPTIMIZED,
        OSC_NOISE,
        OSC_UFO,
        OSC_UNKNOWN
    };

    std::string getOscillatorName(WaveType nType)
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

    // Returns hertz in angular velocity; 2.0 * PI * hertz
    double w(const double dHertz)
    {
        return dHertz * TWO_PI;
    }

    // Return a value between -1 to 1
    double osc(double dTime, double dHertz, WaveType nType = OSC_SINE, double dLFOHertz = 0.0, double dLFOAmplitude = 0.0)
    {
        double dFreq = w(dHertz) * dTime + (dLFOAmplitude * dHertz * sin(w(dLFOHertz) * dTime));

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
            return sin(w(dHertz) * dTime + 0.01 * dHertz * sin(TWO_PI * 5.0 * dTime));

        default:
            return 0;
        }
    }
}; // namespace wyaudio