#ifndef WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H
#define WOW_SOURCE_DB_REC_VOCALUISOUNDSREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;
enum VOCALUISOUNDS : int32_t;

class VocalUISoundsRec {
    public:
    static const int32_t s_numColumns = 7;
    static const int32_t s_rowSize = 28;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ VOCALUISOUNDS m_vocalUienum;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_normalSoundID [2];
    /* 0014 */ int32_t m_pissedSoundID [2];

    VocalUISoundsRec();
    ~VocalUISoundsRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
