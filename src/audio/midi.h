// MIDI specs
// http://www.somascape.org/midi/tech/mfile.html
// http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html

#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

#include "audio.h"

namespace wyaudio
{
    enum WY_MidiEventName : Uint8
    {
        VoiceNoteOff = 0x80,         // 0x80
        VoiceNoteOn = 0x90,          // 0x90
        VoiceAfterTouch = 0xA0,      // 0xA0
        VoiceControlChange = 0xB0,   // 0xB0
        VoiceProgramChange = 0xC0,   // 0xC0
        VoiceChannelPressure = 0xD0, // 0xD0
        VoicePitchBend = 0xE0,       // 0xE0
        SystemExclusive = 0xF0,      // 0xF0
    };

    enum WY_MidiEventMetaName : Uint8
    {
        MetaSequence = 0x00,          // 0x00
        MetaText = 0x01,              // 0x01
        MetaCopyright = 0x02,         // 0x02
        MetaTrackName = 0x03,         // 0x03
        MetaInstrumentName = 0x04,    // 0x04
        MetaLyric = 0x05,             // 0x05
        MetaMarker = 0x06,            // 0x06
        MetaCuePoint = 0x07,          // 0x07
        MetaProgramName = 0x08,       // 0x08
        MetaDeviceName = 0x09,        // 0x09
        MetaChannelPrefix = 0x20,     // 0x20
        MetaPort = 0x21,              // 0x21
        MetaEndOfTrack = 0x2F,        // 0x2F
        MetaTempo = 0x51,             // 0x51
        MetaSMPTEOffset = 0x54,       // 0x54
        MetaTimeSignature = 0x58,     // 0x58
        MetaKeySignature = 0x59,      // 0x59
        MetaSequencerSpecific = 0x7F, // 0x7F
    };

    struct WY_MidiEvent
    {
        enum class Type
        {
            NoteOff,
            NoteOn,
            Other
        } event;

        Uint8 nKey = 0;
        Uint8 nVelocity = 0;
        Uint32 nWallTick = 0;
        Uint32 nDeltaTick = 0;
    };

    struct WY_MidiNote
    {
        Uint8 nKey = 0;
        Uint8 nVelocity = 0;
        Uint32 nStartTime = 0;
        Uint32 nDuration = 0;
    };

    struct WY_MidiTrack
    {
        // Uint16 nSequenceNum;
        // std::string sText;
        // std::string sCopyright;
        std::string sName;
        std::string sInstrument;
        // std::string sLyric;
        // std::string sMarker; // e.g. First verse
        // std::string sCuePoint; // e.g. Car crashes, door opens
        // std::string sProgramName;
        // std::string sDeviceName; // hardware used to produce this track
        // Uint8 nChannelPrefix;
        // Uint8 nPort;
        // Uint32 nTempo; // 3-byte, defaults 120bpm
        // Uint64 nSMPTEOffset; // 5-byte hr mn se fr ff
        // Uint32 nTimeSignature; // 4-byte nn dd cc bb
        std::vector<WY_MidiEvent> vecEvents;
        std::vector<WY_MidiNote> vecNotes;
        Uint8 nMaxNote = 64;
        Uint8 nMinNote = 64;
    };

    class WY_MidiFile
    {
    public:
        std::vector<WY_MidiTrack> vecTracks;
        Uint32 nTempo = 0; // 24-bit (3-byte) of microseconds-per-quarternote (not miliseconds!)

        WY_MidiFile()
        {
        }

        WY_MidiFile(const char *file)
        {
            loadFile(file);
        }

        // Returns beats per minute, based on microseconds-per-quarternote
        int getBPM()
        {
            return nTempo == 0 ? 120 : (60 * 1000 * 1000) / nTempo;
        }

        // Loads and parses MIDI file into readable events for MidiPlayer
        bool loadFile(const char *file)
        {
            std::ifstream ifs;
            ifs.open(file, std::fstream::in | std::ios::binary);
            if (!ifs.is_open())
            {
                printf("\nFailed to open file: %s", file);
                return false;
            }

            printf("\nBegin parsing file: %s\n", file);

            auto swap32 = [](Uint32 n) {
                return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
            };

            auto swap16 = [](Uint16 n) {
                return ((n >> 8) | (n << 8));
            };

            auto readStr = [&ifs](Uint32 nLen) {
                std::string s;
                for (Uint32 i = 0; i < nLen; i++)
                {
                    s += ifs.get();
                }
                return s;
            };

            auto readVal = [&ifs]() {
                Uint32 nVal = 0;
                Uint8 nByte = 0;

                nVal = ifs.get();

                if (nVal & 0x80)
                {
                    nVal &= 0x7F;

                    do
                    {
                        nByte = ifs.get();

                        nVal = (nVal << 7) | (nByte & 0x7F);
                    } while (nByte & 0x80);
                }

                return nVal;
            };

            Uint32 n32 = 0;
            Uint16 n16 = 0;

            // ==================================================
            // Mthd / header chunk
            // ==================================================

            ifs.read((char *)&n32, sizeof(Uint32));
            Uint32 nHeaderId = swap32(n32); // 4-byte char: "Mthd"
            printf("\nnHeaderId: %x", nHeaderId);
            // printf("\nnHeaderId: %x %x %x %x\n", ((nHeaderId & 0xFF000000) >> 24), ((nHeaderId & 0xFF0000) >> 16), ((nHeaderId & 0xFF00) >> 8), ((nHeaderId & 0xFF))); // matches hex editor values
            // printf("\nnHeaderId: %c%c%c%c\n", ((nHeaderId & 0xFF000000) >> 24), ((nHeaderId & 0xFF0000) >> 16), ((nHeaderId & 0xFF00) >> 8), ((nHeaderId & 0xFF))); // outputs M T h d
            // printf("\nnHeaderId: %s\n", readStr(sizeof(Uint32)).c_str()); // outputs Mthd

            // Assume len is 6 bytes, but not necessarily in the future
            ifs.read((char *)&n32, sizeof(Uint32));
            Uint32 nHeaderLen = swap32(n32);
            printf("\nnHeaderLen: %d", nHeaderLen);

            ifs.read((char *)&n16, sizeof(Uint16));
            Uint16 nFormat = swap16(n16);
            printf("\nnFormat: %d", nFormat);

            ifs.read((char *)&n16, sizeof(Uint16));
            Uint16 nTrackChunks = swap16(n16);
            printf("\nnTrackChunks: %d", nTrackChunks);

            ifs.read((char *)&n16, sizeof(Uint16));
            Uint16 nTickDiv = swap16(n16);
            printf("\nnTickDiv: %d", nTickDiv); // usually 96

            // ==================================================
            // Mtrk / track chunk
            // ==================================================

            for (Uint16 nChunk = 0; nChunk < nTrackChunks; nChunk++)
            {
                printf("\n===== New track %d =====", nChunk);

                ifs.read((char *)&n32, sizeof(Uint32));
                Uint32 nTrackId = swap32(n32);
                // printf("\nnTrackId: %x", nTrackId); // 4-byte char: "Mtrk"

                ifs.read((char *)&n32, sizeof(Uint32));
                Uint32 nTrackLen = swap32(n32);
                // printf("\nnTrackLen: %d", nTrackLen);

                bool bEndOfTrack = false;
                Uint8 nPreviousStatus = 0;

                vecTracks.push_back(WY_MidiTrack());

                while (!ifs.eof() && !bEndOfTrack)
                {
                    Uint32 nStatusTimeDelta = 0;
                    Uint8 nStatus = 0;

                    nStatusTimeDelta = readVal();
                    nStatus = ifs.get();

                    // If running status, backtrack fstream by 1 byte
                    // so we can read full nStatus
                    if (nStatus < 0x80)
                    {
                        nStatus = nPreviousStatus;
                        ifs.seekg(-1, std::ios_base::cur);
                    }

                    if ((nStatus & 0xF0) == WY_MidiEventName::VoiceNoteOff)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nNoteId = ifs.get();
                        Uint8 nNoteVelocity = ifs.get();
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::NoteOff, nNoteId, nNoteVelocity, nStatusTimeDelta});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoiceNoteOn)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nNoteId = ifs.get();
                        Uint8 nNoteVelocity = ifs.get();
                        if (nNoteVelocity == 0)
                        {
                            vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::NoteOff, nNoteId, nNoteVelocity, nStatusTimeDelta});
                        }
                        else
                        {
                            vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::NoteOn, nNoteId, nNoteVelocity, nStatusTimeDelta});
                        }
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoiceAfterTouch)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nNoteId = ifs.get();
                        Uint8 nNotePressure = ifs.get();
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::Other});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoiceControlChange)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nNoteController = ifs.get(); // Channel Mode messages
                        Uint8 nNoteValue = ifs.get();
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::Other});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoiceProgramChange)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nProgram = ifs.get(); // voice, instrument, etc.
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::Other});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoiceChannelPressure)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nPressure = ifs.get();
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::Other});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::VoicePitchBend)
                    {
                        nPreviousStatus = nStatus;
                        Uint8 nChannel = nStatus & 0x0F;
                        Uint8 nLsb = ifs.get();
                        Uint8 nMsb = ifs.get();
                        vecTracks[nChunk].vecEvents.push_back({WY_MidiEvent::Type::Other});
                    }
                    else if ((nStatus & 0xF0) == WY_MidiEventName::SystemExclusive)
                    {
                        if (nStatus == 0xF0)
                        {
                            printf("\nSystem Exclusive Event start: %s\n", readStr(readVal()).c_str());
                        }
                        else if (nStatus == 0xF7)
                        {
                            printf("\nSystem Exclusive Event end: %s\n", readStr(readVal()).c_str());
                        }
                        else if (nStatus == 0xFF)
                        {
                            // Meta message
                            Uint8 nType = ifs.get();
                            Uint8 nLen = readVal();

                            switch (nType)
                            {
                            case MetaSequence:
                                printf("\nMetaSequence: %d %d", ifs.get(), ifs.get());
                                break;
                            case MetaText:
                                printf("\nMetaText: %s", readStr(nLen).c_str());
                                break;
                            case MetaCopyright:
                                printf("\nMetaCopyright: %s", readStr(nLen).c_str());
                                break;
                            case MetaTrackName:
                                vecTracks[nChunk].sName = readStr(nLen);
                                printf("\nMetaTrackName: %s", vecTracks[nChunk].sName.c_str());
                                break;
                            case MetaInstrumentName:
                                vecTracks[nChunk].sInstrument = readStr(nLen);
                                printf("\nMetaInstrumentName: %s", vecTracks[nChunk].sInstrument.c_str());
                                break;
                            case MetaLyric:
                                printf("\nMetaLyric: %s", readStr(nLen).c_str());
                                break;
                            case MetaMarker:
                                printf("\nMetaMarker: %s", readStr(nLen).c_str());
                                break;
                            case MetaCuePoint:
                                printf("\nMetaCuePoint: %s", readStr(nLen).c_str());
                                break;
                            case MetaProgramName:
                                printf("\nMetaProgramName: %s", readStr(nLen).c_str());
                                break;
                            case MetaDeviceName:
                                printf("\nMetaDeviceName: %s", readStr(nLen).c_str());
                                break;
                            case MetaChannelPrefix:
                                printf("\nMetaChannelPrefix: %d", ifs.get());
                                break;
                            case MetaPort:
                                printf("\nMetaPort: %d", ifs.get());
                                break;
                            case MetaEndOfTrack:
                                bEndOfTrack = true;
                                break;
                            case MetaTempo:
                                // Initialise tempo, otherwise store it for mid-song tempo change
                                if (nTempo == 0)
                                {
                                    nTempo |= ifs.get() << 16;
                                    nTempo |= ifs.get() << 8;
                                    nTempo |= ifs.get();
                                }
                                printf("\nMetaTempo: %d %d", nTempo, getBPM());
                                break;
                            case MetaSMPTEOffset:
                                printf("\nMetaSMPTEOffset: %d %d %d %d %d", ifs.get(), ifs.get(), ifs.get(), ifs.get(), ifs.get());
                                break;
                            case MetaTimeSignature:
                                printf("\nMetaTimeSignature: %d %d %d %d", ifs.get(), ifs.get(), ifs.get(), ifs.get());
                                break;
                            case MetaKeySignature:
                                printf("\nMetaKeySignature: %d %d", ifs.get(), ifs.get());
                                break;
                            case MetaSequencerSpecific:
                                printf("\nMetaSequencerSpecific: %s", readStr(nLen).c_str());
                                break;
                            default:
                                printf("\nUnrecognized meta: %d", nType);
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("\nUnrecognized status byte: %d\n", nStatus);
                    }
                }
            }

            for (auto &track : vecTracks)
            {
                Uint32 nWallTime = 0;
                std::list<WY_MidiNote> listNotesBeingProcessed;

                for (auto &event : track.vecEvents)
                {
                    nWallTime += event.nDeltaTick;

                    if (event.event == WY_MidiEvent::Type::NoteOn)
                    {
                        listNotesBeingProcessed.push_back({event.nKey, event.nVelocity, nWallTime, 0});
                    }

                    if (event.event == WY_MidiEvent::Type::NoteOff)
                    {
                        auto note = std::find_if(listNotesBeingProcessed.begin(), listNotesBeingProcessed.end(), [&](const WY_MidiNote &n) { return n.nKey == event.nKey; });

                        if (note != listNotesBeingProcessed.end())
                        {
                            note->nDuration = nWallTime - note->nStartTime;
                            track.vecNotes.push_back(*note);
                            listNotesBeingProcessed.erase(note);
                        }
                    }
                }
            }

            // End printf with \n, otherwise JS will not print last line
            printf("\nFile loaded: %s\n", file);

            return true;
        }

        // Converts MIDI events into playable audio samples
        void convertToWAV()
        {
            // TODO
        }
    };

    class WY_MidiPlayer : public WY_Audio
    {
    protected:
        std::vector<WY_MidiFile *> midiFiles;

    public:
        WY_MidiPlayer() {}

        ~WY_MidiPlayer()
        {
            for (int i = 0; i < midiFiles.size(); i++)
            {
                delete midiFiles[i];
            }
            midiFiles.clear();
        }

        double getAudioSample()
        {
            return 0.0;
        }
    };
} // namespace wyaudio