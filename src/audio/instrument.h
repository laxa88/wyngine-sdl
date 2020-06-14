#include <string>

#include "oscillator.h"
#include "envelope.h"

// https://pages.mtu.edu/~suits/notefreqs.html
#define BASE_FREQ 16.35
#define MIN_OCT 0
#define MAX_OCT 8

namespace wyaudio
{
    // ==================================================
    // Note
    // ==================================================

    enum MusicNote
    {
        NOTE_C, // 0 starts with C
        NOTE_CS,
        NOTE_D,
        NOTE_DS,
        NOTE_E,
        NOTE_F,
        NOTE_FS,
        NOTE_G,
        NOTE_GS,
        NOTE_A,
        NOTE_AS,
        NOTE_B, // 11
    };

    struct Note
    {
        MusicNote id; // note id, to be converted to Hertz
        double on;    // time when note is activated
        double off;   // time when note is deactivated
        bool active;  // whether note is actively played
        int channel;  // instrument channel, determined by sequencer

        Note()
        {
            id = NOTE_C;
            on = 0.0;
            off = 0.0;
            active = false;
            channel = 0;
        }
    };

    // Returns frequency based on note and octave
    double scale(const MusicNote id, const int o = 4)
    {
        int nOctave = o;
        if (nOctave < MIN_OCT)
        {
            nOctave = MIN_OCT;
        }
        else if (nOctave > MAX_OCT)
        {
            nOctave = MAX_OCT;
        }

        double d12thRootOf2 = pow(2.0, 1.0 / 12);
        double octave = pow(2.0, nOctave);
        double res = BASE_FREQ * pow(d12thRootOf2, id) * (double)octave;

        return res;
    }

    // Returns frequency based on note (multiplied by octave of 12)
    double scale(const int id)
    {
        return BASE_FREQ * pow(1.0594630943592952645618252949463, id);
    }

    // ==================================================
    // Instrument
    // ==================================================

    enum InstrumentType
    {
        WAV_SQUARE,
        WAV_WAVE,
        INS_HARMONICA,
        INS_BELL
    };

    std::string getInstrumentName(InstrumentType t)
    {
        switch (t)
        {
        case WAV_SQUARE:
            return "Square";
        case WAV_WAVE:
            return "Wave";
        case INS_HARMONICA:
            return "Harmonica";
        case INS_BELL:
            return "Bell";
        default:
            return "----";
        }
    }

    struct Instrument
    {
        double dVolume;
        Envelope env;
        virtual double speak(const double dTime, Note n, bool &bNoteFinished) = 0;
    };

    struct square : public Instrument
    {
        square()
        {
            dVolume = 1.0;

            env.dAttackTime = 0.0;
            env.dDecayTime = 0.0;
            env.dStartAmplitude = 1.0;
            env.dSustainAmplitude = 1.0;
            env.dReleaseTime = 0.0;
        }

        double speak(const double dTime, Note n, bool &bNoteFinished)
        {
            double dAmplitude = env.getAmplitude(dTime, n.on, n.off);

            double dSound = osc(n.on - dTime, scale(n.id), OSC_SQUARE);

            double res = dAmplitude * dSound * dVolume;

            // printf("\nspeak: %f, %f, %f, %f", dAmplitude, dSound, dVolume, res);
            // printf("\nspeak: %f, %d, %f, %f", dTime, n.id, n.on, n.off);

            return res;
        }
    };

    struct harmonica : public Instrument
    {
        harmonica()
        {
            dVolume = 1.0;

            env.dAttackTime = 100.0;
            env.dDecayTime = 100.0;
            env.dStartAmplitude = 1.0;
            env.dSustainAmplitude = 0.8;
            env.dReleaseTime = 200.0;
        }

        double speak(const double dTime, Note n, bool &bNoteFinished)
        {
            double dAmplitude = env.getAmplitude(dTime, n.on, n.off);

            if (dAmplitude <= 0.0)
                bNoteFinished = true;

            double dSound = (1.00 * osc(n.on - dTime, scale(n.id), OSC_SQUARE, 5.0, 0.001) +
                             0.50 * osc(n.on - dTime, scale(n.id + 12), OSC_SQUARE) +
                             0.05 * osc(n.on - dTime, scale(n.id + 24), OSC_NOISE));

            return dAmplitude * dSound * dVolume;
        }
    };

    struct bell : public Instrument
    {
        bell()
        {
            dVolume = 1.0;

            env.dAttackTime = 5.0;
            env.dDecayTime = 1000.0;
            env.dStartAmplitude = 1.0;
            env.dSustainAmplitude = 0.0;
            env.dReleaseTime = 1000.0;
        }

        double speak(const double dTime, Note n, bool &bNoteFinished)
        {
            double dAmplitude = env.getAmplitude(dTime, n.on, n.off);

            if (dAmplitude <= 0.0)
                bNoteFinished = true;

            double dSound = (1.00 * osc(n.on - dTime, scale(n.id + 12), OSC_SINE, 5.0, 0.001) +
                             0.50 * osc(n.on - dTime, scale(n.id + 24)) +
                             0.25 * osc(n.on - dTime, scale(n.id + 36)));

            return dAmplitude * dSound * dVolume;
        }
    };
} // namespace wyaudio