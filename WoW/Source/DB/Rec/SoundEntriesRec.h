#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// SoundEntries.dbc flags tested by the client (copied into VSOUNDKITDEF).
enum SOUND_ENTRY_FLAGS : uint32_t {
    SOUND_ENTRY_FLAG_NONE          = 0x00000000,
    // Playback is limited by the active suppressor list; SE2_SUPPRESSOR_MODE can force it on or off.
    SOUND_ENTRY_FLAG_SUPPRESSIBLE  = 0x00000020,
    SOUND_ENTRY_FLAG_LOOPING       = 0x00000200,
    // Pitch is randomized in [0.85, 1.15].
    SOUND_ENTRY_FLAG_RANDOM_PITCH  = 0x00000400,
    // Volume is offset randomly by up to 0.15.
    SOUND_ENTRY_FLAG_RANDOM_VOLUME = 0x00000800,
};

class SoundEntriesRec {
    public:
    static const int32_t s_numColumns = 30;
    static const int32_t s_rowSize = 120;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_file [10];
    /* 0034 */ int32_t m_freq [10];
    /* 005C */ const char* m_directoryBase;
    /* 0060 */ float m_volumeFloat;
    /* 0064 */ int32_t m_flags; // SOUND_ENTRY_FLAGS
    /* 0068 */ float m_minDistance;
    /* 006C */ float m_distanceCutoff;
    /* 0070 */ int32_t m_eaxdef;
    /* 0074 */ int32_t m_soundEntriesAdvancedID;

    SoundEntriesRec();
    ~SoundEntriesRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
