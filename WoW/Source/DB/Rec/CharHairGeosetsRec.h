#ifndef WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H
#define WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class SFileImpl;

class CharHairGeosetsRec {
    public:
    static const int32_t s_numColumns = 6;
    static const int32_t s_rowSize = 24;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_variationID;
    /* 0010 */ int32_t m_geosetID;
    /* 0014 */ BOOL m_showscalp;

    CharHairGeosetsRec();
    ~CharHairGeosetsRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
