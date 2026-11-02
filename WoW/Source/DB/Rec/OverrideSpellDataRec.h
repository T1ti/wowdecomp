#ifndef WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Permissions that remain available while an OverrideSpellData record is active.
enum OVERRIDE_SPELL_DATA_FLAGS : int32_t {
    OVERRIDE_SPELL_DATA_FLAG_ALLOW_ITEMS = 0x1,
    // Native action-bar validation gates both attack and ranged-attack actions.
    OVERRIDE_SPELL_DATA_FLAG_ALLOW_ATTACKS = 0x2,
    OVERRIDE_SPELL_DATA_FLAG_ALLOW_NON_OVERRIDE_SPELLS = 0x4,
};

class OverrideSpellDataRec {
    public:
    static const int32_t s_numColumns = 12;
    static const int32_t s_rowSize = 48;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [10];
    /* 002C */ int32_t m_flags;

    OverrideSpellDataRec();
    ~OverrideSpellDataRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
