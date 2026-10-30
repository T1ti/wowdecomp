#ifndef WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H
#define WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class AnimationDataRec {
    public:
    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id; // Global ANIMATION_ID.
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_weaponflags;
    /* 000C */ int32_t m_bodyflags;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_fallback; // Global ANIMATION_ID.
    /* 0018 */ int32_t m_behaviorID; // Base ANIMATION_ID shared by behavior tiers.
    /* 001C */ int32_t m_behaviorTier;

    AnimationDataRec();
    ~AnimationDataRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
