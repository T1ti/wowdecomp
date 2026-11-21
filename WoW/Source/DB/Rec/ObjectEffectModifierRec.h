#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class ObjectEffectModifierRec {
    public:
    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_inputType; // CObjectEffectModifier::INPUT_TYPE
    /* 0008 */ int32_t m_mapType; // CObjectEffectModifier::MAP_TYPE
    /* 000C */ int32_t m_outputType; // CObjectEffectModifier::OUTPUT_TYPE
    /* 0010 */ float m_param [4];

    ObjectEffectModifierRec();
    ~ObjectEffectModifierRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
