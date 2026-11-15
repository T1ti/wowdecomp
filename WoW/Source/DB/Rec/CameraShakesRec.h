#ifndef WOW_SOURCE_DB_REC_CAMERASHAKESREC_H
#define WOW_SOURCE_DB_REC_CAMERASHAKESREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;
enum CGCameraShakeType : int32_t;
enum CGCameraDir : int32_t;

class CameraShakesRec {
    public:
    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ CGCameraShakeType m_shakeType;
    /* 0008 */ CGCameraDir m_direction;
    /* 000C */ float m_amplitude;
    /* 0010 */ float m_frequency;
    /* 0014 */ float m_duration;
    /* 0018 */ float m_phase;
    /* 001C */ float m_coefficient;

    CameraShakesRec();
    ~CameraShakesRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
