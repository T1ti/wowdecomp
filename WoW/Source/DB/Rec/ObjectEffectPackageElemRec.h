#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class ObjectEffectPackageElemRec {
    public:
    static const int32_t s_numColumns = 4;
    static const int32_t s_rowSize = 16;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_objectEffectPackageID;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_stateType; // ENUM OBJECT_EFFECT_STATE

    ObjectEffectPackageElemRec();
    ~ObjectEffectPackageElemRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
