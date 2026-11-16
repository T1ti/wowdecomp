#ifndef WOW_SOURCE_DB_REC_WORLDSTATEUIREC_H
#define WOW_SOURCE_DB_REC_WORLDSTATEUIREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Inferred name: extended UI state variables of a WorldStateUI.dbc record.
constexpr int32_t NUM_WORLD_STATE_UI_EXTENDED_VARIABLES = 3;

// WorldStateUI.dbc m_type, as the client sorts the rows.
enum WORLD_STATE_UI_TYPE : int32_t {
    WORLD_STATE_UI_TYPE_DEFAULT = 0, // Always-up world state UI.
    WORLD_STATE_UI_TYPE_UNKNOWN_1 = 1, // Listed like WORLD_STATE_UI_TYPE_DEFAULT.
    WORLD_STATE_UI_TYPE_BATTLEFIELD_STAT = 2, // Battleground score column for the active map.
    WORLD_STATE_UI_TYPE_TIMER = 3, // Raises WORLD_STATE_UI_TIMER_UPDATE every second.
};

class WorldStateUIRec {
    public:
    static const int32_t s_numColumns = 63;
    static const int32_t s_rowSize = 252;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_phaseShift;
    /* 0010 */ const char* m_icon;
    /* 0014 */ const char* m_string;
    /* 0018 */ const char* m_tooltip;
    /* 001C */ int32_t m_stateVariable;
    /* 0020 */ int32_t m_type; // WORLD_STATE_UI_TYPE
    /* 0024 */ const char* m_dynamicIcon;
    /* 0028 */ const char* m_dynamicTooltip;
    /* 002C */ const char* m_extendedUi;
    /* 0030 */ int32_t m_extendedUistateVariable [NUM_WORLD_STATE_UI_EXTENDED_VARIABLES];

    WorldStateUIRec();
    ~WorldStateUIRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
