#ifndef WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H
#define WOW_SOURCE_DB_REC_SPELLCATEGORYREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// 3.3.5 category flags. The any-category bit is used by the native cooldown scan.
// https://github.com/TrinityCore/TrinityCore/blob/3.3.5/src/server/shared/DataStores/DBCEnums.h
enum SPELL_CATEGORY_FLAGS : int32_t {
    SPELL_CATEGORY_FLAG_COOLDOWN_SCALES_WITH_WEAPON_SPEED = 0x1,
    SPELL_CATEGORY_FLAG_COOLDOWN_USES_ANY_CATEGORY = 0x2,
    SPELL_CATEGORY_FLAG_COOLDOWN_STARTS_ON_EVENT = 0x4,
};

class SpellCategoryRec {
    public:
    static const int32_t s_numColumns = 2;
    static const int32_t s_rowSize = 8;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;

    SpellCategoryRec();
    ~SpellCategoryRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
