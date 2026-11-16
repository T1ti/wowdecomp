#ifndef WOW_SOURCE_DB_REC_LIQUIDTYPEREC_H
#define WOW_SOURCE_DB_REC_LIQUIDTYPEREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// LiquidType.dbc flags; names follow the EnumeratedString LiquidType flags.
enum LIQUID_TYPE_FLAGS : int32_t {
    LIQUID_TYPE_FLAG_NONE                    = 0x00000000,
    LIQUID_TYPE_FLAG_RIPPLE                  = 0x00000001,
    LIQUID_TYPE_FLAG_SPLASH                  = 0x00000002,
    // Walkable surface: kept by walk-on-liquid queries; entities ripple even above it.
    LIQUID_TYPE_FLAG_WATER_WALKING           = 0x00000004,
    LIQUID_TYPE_FLAG_PARTICLES               = 0x00000008,
    LIQUID_TYPE_FLAG_PARTICLE_FOG            = 0x00000010,
    // Underwater fog only applies when the WMO fog has SMOFog::F_ALLOW_UNDERWATER_FOG.
    LIQUID_TYPE_FLAG_WMO_FOG_CONTROL         = 0x00000020,
    // Underwater fog copies the group fog instead of blending.
    LIQUID_TYPE_FLAG_NO_INTERIOR_FOG_BLEND   = 0x00000040,
    // No water sound while the listener is inside.
    LIQUID_TYPE_FLAG_OUTDOOR_AMBIENCE_ONLY   = 0x00000080,
    // Fog is only applied when the WMO fog has SMOFog::F_ALLOW_SLIME_FOG.
    LIQUID_TYPE_FLAG_WMO_FOG_CONTROL_SLIME   = 0x00000100,
    LIQUID_TYPE_FLAG_FORCE_EXTERIOR_LIGHTING = 0x00000200,
};

// Basic liquid type, stored in the LiquidType.dbc column named m_soundBank.
// Basic IDs below NUM_BASIC_LIQUID_TYPES cycle through these, so (id - 1) & 3
// gives the type; AreaTable m_liquidTypeID is indexed by it. Names follow the
// DBC records (Water, Ocean, Magma, Slime).
enum LIQUID_BASIC_TYPE : int32_t {
    LIQUID_BASIC_TYPE_WATER = 0,
    LIQUID_BASIC_TYPE_OCEAN = 1,
    LIQUID_BASIC_TYPE_MAGMA = 2,
    LIQUID_BASIC_TYPE_SLIME = 3,
    NUM_LIQUID_BASIC_TYPES  = 4,
};

// LiquidType.dbc rows the client uses directly, named after m_name.
enum LIQUID_TYPE_ID : int32_t {
    LIQUID_TYPE_WATER              = 1,
    LIQUID_TYPE_OCEAN              = 2,
    LIQUID_TYPE_MAGMA              = 3,
    LIQUID_TYPE_SLIME              = 4,
    LIQUID_TYPE_WMO_WATER          = 13,
    LIQUID_TYPE_WMO_OCEAN          = 14,
    LIQUID_TYPE_GREEN_LAVA         = 15,
    LIQUID_TYPE_WMO_WATER_INTERIOR = 17,
    LIQUID_TYPE_WMO_MAGMA          = 19,
    LIQUID_TYPE_WMO_SLIME          = 20,
    NUM_BASIC_LIQUID_TYPES         = 21,
};

class LiquidTypeRec {
    public:
    static const int32_t s_numColumns = 45;
    static const int32_t s_rowSize = 180;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags; // LIQUID_TYPE_FLAGS
    /* 000C */ int32_t m_soundBank; // ENUM LIQUID_BASIC_TYPE
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
