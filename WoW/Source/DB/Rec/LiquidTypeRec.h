#ifndef WOW_SOURCE_DB_REC_LIQUIDTYPEREC_H
#define WOW_SOURCE_DB_REC_LIQUIDTYPEREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// LiquidType.dbc flags tested by the client; names describe that behavior.
enum LIQUID_TYPE_FLAGS : int32_t {
    LIQUID_TYPE_FLAG_NONE            = 0x00000000,
    // Entities in this liquid get ripples.
    LIQUID_TYPE_FLAG_RIPPLES         = 0x00000001,
    // Entities in this liquid get splashes.
    LIQUID_TYPE_FLAG_SPLASHES        = 0x00000002,
    // Kept by fishable-only liquid queries; entities ripple even above its surface.
    LIQUID_TYPE_FLAG_FISHABLE        = 0x00000004,
    LIQUID_TYPE_FLAG_PARTICULATES    = 0x00000008,
    LIQUID_TYPE_FLAG_PARTICULATE_FOG = 0x00000010,
    // Underwater fog only applies when the WMO fog has its 0x100 flag.
    LIQUID_TYPE_FLAG_UNKNOWN_20      = 0x00000020,
    // Underwater fog copies the current group fog.
    LIQUID_TYPE_FLAG_USE_GROUP_FOG   = 0x00000040,
    // No water sound while the listener is inside.
    LIQUID_TYPE_FLAG_NO_SOUND_INSIDE = 0x00000080,
    // Fog is only applied when the WMO fog has its 0x10 flag.
    LIQUID_TYPE_FLAG_UNKNOWN_100     = 0x00000100,
    // Always counted as exterior.
    LIQUID_TYPE_FLAG_EXTERIOR        = 0x00000200,
};

class LiquidTypeRec {
    public:
    static const int32_t s_numColumns = 45;
    static const int32_t s_rowSize = 180;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags; // LIQUID_TYPE_FLAGS
    /* 000C */ int32_t m_soundBank;
    /* 0010 */ int32_t m_soundID;
    /* 0014 */ int32_t m_spellID;
    /* 0018 */ float m_maxDarkenDepth;
    /* 001C */ float m_fogDarkenIntensity;
    /* 0020 */ float m_ambDarkenIntensity;
    /* 0024 */ float m_dirDarkenIntensity;
    /* 0028 */ int32_t m_lightID;
    /* 002C */ float m_particleScale;
    /* 0030 */ int32_t m_particleMovement;
    /* 0034 */ int32_t m_particleTexSlots;
    /* 0038 */ int32_t m_materialID;
    /* 003C */ const char* m_texture [6];
    /* 0054 */ int32_t m_color [2];
    /* 005C */ float m_float [18];
    /* 00A4 */ int32_t m_int [4];

    LiquidTypeRec();
    ~LiquidTypeRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
