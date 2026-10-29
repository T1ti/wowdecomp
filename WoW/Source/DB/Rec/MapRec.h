#ifndef WOW_SOURCE_DB_REC_MAPREC_H
#define WOW_SOURCE_DB_REC_MAPREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Map.dbc instance types supported by the 3.3.5 client.
// Other raw values retain their value; UI token lookups fall back to "none".
enum MAP_INSTANCE_TYPE : int32_t {
    MAP_INSTANCE_TYPE_NOT_INSTANCED      = 0,
    MAP_INSTANCE_TYPE_PARTY_DUNGEON      = 1,
    MAP_INSTANCE_TYPE_RAID_DUNGEON       = 2,
    MAP_INSTANCE_TYPE_PVP_BATTLEFIELD    = 3,
    MAP_INSTANCE_TYPE_ARENA_BATTLEFIELD  = 4,
};

class MapRec {
    public:
    static const int32_t s_numColumns = 66;
    static const int32_t s_rowSize = 264;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_directory;
    /* 0008 */ MAP_INSTANCE_TYPE m_instanceType;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_pvp;
    /* 0014 */ const char* m_mapName;
    /* 0018 */ int32_t m_areaTableID;
    /* 001C */ const char* m_mapDescription0;
    /* 0020 */ const char* m_mapDescription1;
    /* 0024 */ int32_t m_loadingScreenID;
    /* 0028 */ float m_minimapIconScale;
    /* 002C */ int32_t m_corpseMapID;
    /* 0030 */ float m_corpse [2];
    /* 0038 */ int32_t m_timeOfDayOverride;
    /* 003C */ int32_t m_expansionID;
    /* 0040 */ int32_t m_raidOffset;
    /* 0044 */ int32_t m_maxPlayers;

    MapRec();
    ~MapRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
