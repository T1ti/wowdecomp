#ifndef WOW_SOURCE_DB_REC_AREAPOIREC_H
#define WOW_SOURCE_DB_REC_AREAPOIREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// AreaPOI.dbc flags tested by the client; names follow EnumeratedString AreaPOI::Flags.
enum AREA_POI_FLAGS : int32_t {
    AREA_POI_FLAG_NONE                  = 0x00000000,
    // Loaded into its continent's minimap POI list.
    AREA_POI_FLAG_MINIMAP_LIST          = 0x00000001,
    // Drawn on the minimap while in range.
    AREA_POI_FLAG_MINIMAP_ICON          = 0x00000002,
    AREA_POI_FLAG_ZONE_MAP              = 0x00000004,
    AREA_POI_FLAG_CONTINENT_MAP         = 0x00000008,
    AREA_POI_FLAG_WORLD_MAP             = 0x00000010,
    // The map landmark uses the POI icon.
    AREA_POI_FLAG_SHOW_ICON             = 0x00000080,
    // The minimap direction arrow is kept while indoors.
    AREA_POI_FLAG_SHOW_WHEN_INDOORS     = 0x00000100,
    // GetMapLandmarkInfo showInBattleMap.
    AREA_POI_FLAG_SHOW_IN_BATTLEMAP     = 0x00000200,
    // Only shown on its own area's zone map.
    AREA_POI_FLAG_CURRENT_AREA_ONLY     = 0x00000400,
};

class AreaPOIRec {
    public:
    static const int32_t s_numColumns = 54;
    static const int32_t s_rowSize = 216;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_importance;
    /* 0008 */ int32_t m_icon [9];
    /* 002C */ int32_t m_factionID;
    /* 0030 */ float m_pos [3];
    /* 003C */ int32_t m_continentID;
    /* 0040 */ int32_t m_flags; // AREA_POI_FLAGS
    /* 0044 */ int32_t m_areaID;
    /* 0048 */ const char* m_name;
    /* 004C */ const char* m_description;
    /* 0050 */ int32_t m_worldStateID;
    /* 0054 */ int32_t m_worldMapLink;

    AreaPOIRec();
    ~AreaPOIRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
