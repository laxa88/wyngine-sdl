#include <iostream>
#include <iomanip>

#include "../src/lib/midifile/MidiFile.h"
#include "../src/lib/midifile/Options.h"

using namespace std;
using namespace smf;

int main(int argc, char **argv)
{
    Options options;
    options.process(argc, argv);
    MidiFile midifile;
    if (options.getArgCount() == 0)
        midifile.read(cin);
    else
        midifile.read(options.getArg(1));
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    int tracks = midifile.getTrackCount();
    cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
    if (tracks > 1)
        cout << "TRACKS: " << tracks << endl;
    for (int track = 0; track < tracks; track++)
    {
        if (tracks > 1)
            cout << "\nTrack " << track << endl;
        cout << "Tick\t\t\tSeconds\t\t\tDur\t\t\tMessage" << endl;
        for (int event = 0; event < midifile[track].size(); event++)
        {
            cout << dec << midifile[track][event].tick;
            cout << '\t' << '\t' << '\t' << dec << midifile[track][event].seconds;
            cout << '\t' << '\t' << '\t';
            if (midifile[track][event].isNoteOn())
                cout << midifile[track][event].getDurationInSeconds();
            cout << '\t' << '\t' << '\t' << hex;
            for (int i = 0; i < midifile[track][event].size(); i++)
                cout << (int)midifile[track][event][i] << ' ';
            cout << endl;
        }
    }

    return 0;
}