#ifndef WOW_SOURCE_DB_REC_LIGHTREC_H
#define WOW_SOURCE_DB_REC_LIGHTREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CRange.h"

class SFileImpl;

class LightRec {
    public:
    static const int32_t s_numColumns = 15;
    static const int32_t s_rowSize = 60;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_gameCoords [3];
    /* 0014 */ CRange m_gameFalloff;
    /* 001C */ int32_t m_lightParamsID [8];

    LightRec();
    ~LightRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
