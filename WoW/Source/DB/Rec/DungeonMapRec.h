#ifndef WOW_SOURCE_DB_REC_DUNGEONMAPREC_H
#define WOW_SOURCE_DB_REC_DUNGEONMAPREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CRange.h"

class SFileImpl;

class DungeonMapRec {
    public:
    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_floorIndex;
    /* 000C */ CRange m_boundsX;
    /* 0014 */ CRange m_boundsY;
    /* 001C */ int32_t m_parentWorldMapID;

    DungeonMapRec();
    ~DungeonMapRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
