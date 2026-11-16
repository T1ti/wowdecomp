#ifndef WOW_SOURCE_DB_REC_DANCEMOVESREC_H
#define WOW_SOURCE_DB_REC_DANCEMOVESREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// DanceMoves.dbc m_type, by what CGPlayer_C::PlayDanceMove does with m_param;
// HOLD and REPEAT follow the records' "Hold - N Sec" / "Repeat - N Sec" names.
enum DANCE_MOVE_TYPE : int32_t {
    DANCE_MOVE_ANIMATION        = 0, // m_param is an animation ID
    DANCE_MOVE_SPELL_VISUAL_KIT = 1, // m_param is a SpellVisualKit ID
    DANCE_MOVE_SOUND_KIT        = 2, // m_param is a SoundEntries ID
    DANCE_MOVE_HOLD             = 3, // m_param is a wait in seconds
    DANCE_MOVE_REPEAT           = 4, // waits, then replays the previous move
};

class DanceMovesRec {
    public:
    static const int32_t s_numColumns = 24;
    static const int32_t s_rowSize = 96;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type; // ENUM DANCE_MOVE_TYPE
    /* 0008 */ int32_t m_param;
    /* 000C */ int32_t m_fallback;
    /* 0010 */ int32_t m_racemask;
    /* 0014 */ const char* m_internalName;
    /* 0018 */ const char* m_name;
    /* 001C */ int32_t m_lockID;

    DanceMovesRec();
    ~DanceMovesRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
