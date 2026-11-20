#ifndef WOW_SOURCE_DB_REC_AREATABLEREC_H
#define WOW_SOURCE_DB_REC_AREATABLEREC_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

class SFileImpl;

// AreaTable.dbc flags tested by the client. Names follow EnumeratedString AreaTable::Flags
// except FLYABLE and NO_FLYING, whose bits mean something else in that later table.
enum AREA_FLAGS : int32_t {
    AREA_FLAG_NONE                      = 0x00000000,
    AREA_FLAG_EMIT_BREATH_PARTICLES     = 0x00000001,
    // Use this area's own breath particle bit instead of the parent area's.
    AREA_FLAG_BREATH_PARTICLES_OVERRIDE_PARENT = 0x00000002,
    // City-only chat channels are listed here.
    AREA_FLAG_ALLOW_TRADE_CHANNEL       = 0x00000008,
    // Reports zone PvP info even on normal realms.
    AREA_FLAG_ENEMIES_PVP_FLAGGED       = 0x00000010,
    // GetZonePVPInfo reports "arena".
    AREA_FLAG_FREE_FOR_ALL_PVP          = 0x00000080,
    // Linked chat channels use the AREA_FLAG_LINKED_CHAT_SPECIAL_AREA area's name here.
    AREA_FLAG_LINKED_CHAT_SET_IN_CITIES = 0x00000100,
    AREA_FLAG_LINKED_CHAT_SPECIAL_AREA  = 0x00000200,
    AREA_FLAG_FLYABLE                   = 0x00000400,
    // GetZonePVPInfo reports "sanctuary".
    AREA_FLAG_NO_PVP                    = 0x00000800,
    // Keeps its own ambient multiplier instead of the parent area's.
    AREA_FLAG_SUBZONE_AMBIENT_MULTIPLIER = 0x00002000,
    AREA_FLAG_PVP_POI                   = 0x00008000,
    // GetZonePVPInfo reports "contested" instead of the faction-relative type.
    AREA_FLAG_CONTESTED                 = 0x00040000,
    // GetZonePVPInfo reports "combat".
    AREA_FLAG_COMBAT_ZONE               = 0x01000000,
    AREA_FLAG_FORCE_INDOORS             = 0x02000000,
    // Tested before AREA_FLAG_FORCE_INDOORS.
    AREA_FLAG_FORCE_OUTDOORS            = 0x04000000,
    AREA_FLAG_HEARTH_AND_RESURRECT      = 0x08000000,
    AREA_FLAG_NO_FLYING                 = 0x20000000,
};

class AreaTableRec {
    public:
    static const int32_t s_numColumns = 36;
    static const int32_t s_rowSize = 144;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ int32_t m_parentAreaID;
    /* 000C */ int32_t m_areaBit;
    /* 0010 */ int32_t m_flags; // AREA_FLAGS
    /* 0014 */ int32_t m_soundProviderPref;
    /* 0018 */ int32_t m_soundProviderPrefUnderwater;
    /* 001C */ int32_t m_ambienceID;
    /* 0020 */ int32_t m_zoneMusic;
    /* 0024 */ int32_t m_introSound;
    /* 0028 */ int32_t m_explorationLevel;
    /* 002C */ const char* m_areaName;
    /* 0030 */ int32_t m_factionGroupMask;
    /* 0034 */ int32_t m_liquidTypeID [NUM_LIQUID_BASIC_TYPES]; // indexed by LIQUID_BASIC_TYPE
    /* 0044 */ float m_minElevation;
    /* 0048 */ float m_ambientMultiplier;
    /* 004C */ int32_t m_lightID;

    AreaTableRec();
    ~AreaTableRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
