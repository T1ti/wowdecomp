#ifndef WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H
#define WOW_SOURCE_DB_REC_SOUNDFILTERELEMREC_H

#include <cstddef>
#include <cstdint>

enum SI2_DSP_TYPE : int32_t;

class SFileImpl;

class SoundFilterElemRec {
    public:
    static const int32_t s_numColumns = 13;
    static const int32_t s_rowSize = 52;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundFilterID;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ SI2_DSP_TYPE m_filterType;
    /* 0010 */ float m_params [9];

    SoundFilterElemRec();
    ~SoundFilterElemRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
