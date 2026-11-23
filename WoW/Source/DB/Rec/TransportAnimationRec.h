#ifndef WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTANIMATIONREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class TransportAnimationRec {
    public:
    static const int32_t s_numColumns = 7;
    static const int32_t s_rowSize = 28;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_transportID;
    /* 0008 */ int32_t m_TimeIndex;
    /* 000C */ float m_pos [3];
    /* 0018 */ int32_t m_sequenceID;

    TransportAnimationRec();
    ~TransportAnimationRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
