#ifndef WOW_SOURCE_DB_REC_SCREENEFFECTREC_H
#define WOW_SOURCE_DB_REC_SCREENEFFECTREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// ScreenEffect.dbc m_effect, named after the FFX effect CGWorldFrame selects for each.
enum SCREEN_EFFECT_TYPE : int32_t {
    SCREEN_EFFECT_GLOW = 0,
    SCREEN_EFFECT_DEATH = 1,
    SCREEN_EFFECT_NETHER_WORLD = 2,
    SCREEN_EFFECT_SWIRLING_FOG = 3,
};

// ScreenEffect.dbc record IDs the client selects directly, named from their m_name.
enum SCREEN_EFFECT_ID : int32_t {
    SCREEN_EFFECT_ID_NONE = 0, // No record: restores the default glow effect.
    SCREEN_EFFECT_ID_GHOST = 1, // "Ghost Screen Effect"
    SCREEN_EFFECT_ID_INVIS = 81, // "Invis Screen Effect"
};

class ScreenEffectRec {
    public:
    static const int32_t s_numColumns = 10;
    static const int32_t s_rowSize = 40;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_effect; // SCREEN_EFFECT_TYPE
    /* 000C */ int32_t m_param [4];
    /* 001C */ int32_t m_lightParamsID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_zoneMusicID;

    ScreenEffectRec();
    ~ScreenEffectRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
