#ifndef WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H
#define WOW_SOURCE_DB_REC_TRANSPORTROTATIONREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class TransportRotationRec {
    public:
    static const int32_t s_numColumns = 7;
    static const int32_t s_rowSize = 28;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_gameObjectsID;
    /* 0008 */ int32_t m_TimeIndex;
    /* 000C */ float m_rot [4];

    TransportRotationRec();
    ~TransportRotationRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
