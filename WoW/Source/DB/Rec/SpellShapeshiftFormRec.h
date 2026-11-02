#ifndef WOW_SOURCE_DB_REC_SPELLSHAPESHIFTFORMREC_H
#define WOW_SOURCE_DB_REC_SPELLSHAPESHIFTFORMREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Descriptive 3.3.5 client meanings. UNKNOWN does not mean unused in data.
enum SPELL_SHAPESHIFT_FORM_FLAGS : int32_t {
    SPELL_SHAPESHIFT_FORM_FLAG_STANCE = 0x1,
    SPELL_SHAPESHIFT_FORM_FLAG_NOT_TOGGLEABLE = 0x2,
    SPELL_SHAPESHIFT_FORM_FLAG_UNKNOWN_2 = 0x4, // Later reference: PersistOnDeath; unverified in this client.
    SPELL_SHAPESHIFT_FORM_FLAG_CAN_INTERACT = 0x8, // Later CanInteractNPC; native also gates game objects.
    SPELL_SHAPESHIFT_FORM_FLAG_UNKNOWN_4 = 0x10, // Later reference: DontUseWeapon; unverified in this client.
    SPELL_SHAPESHIFT_FORM_FLAG_AGILITY_TO_ATTACK_POWER = 0x20,
    SPELL_SHAPESHIFT_FORM_FLAG_CAN_USE_EQUIPPED_ITEMS = 0x40,
    SPELL_SHAPESHIFT_FORM_FLAG_CAN_USE_ITEMS = 0x80,
    // Also prevents looting while shifted (CGPlayer_C::CanLoot).
    SPELL_SHAPESHIFT_FORM_FLAG_NO_AUTO_UNSHIFT = 0x100,
    SPELL_SHAPESHIFT_FORM_FLAG_UNKNOWN_9 = 0x200, // Later reference: ConsideredDead; unverified in this client.
    SPELL_SHAPESHIFT_FORM_FLAG_RESTRICT_SPELLS_TO_FORM = 0x400,
    SPELL_SHAPESHIFT_FORM_FLAG_CANCEL_STANCE_AT_FLIGHT_MASTER = 0x800,
    SPELL_SHAPESHIFT_FORM_FLAG_SUPPRESS_TEXT_EMOTE_SOUND = 0x1000,
};

class SpellShapeshiftFormRec {
    public:
    static const int32_t s_numColumns = 35;
    static const int32_t s_rowSize = 140;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_bonusActionBar;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_flags; // SPELL_SHAPESHIFT_FORM_FLAGS
    /* 0010 */ int32_t m_creatureType;
    /* 0014 */ int32_t m_attackIconID;
    /* 0018 */ int32_t m_combatRoundTime;
    /* 001C */ int32_t m_creatureDisplayID [4];
    /* 002C */ int32_t m_presetSpellID [8];

    SpellShapeshiftFormRec();
    ~SpellShapeshiftFormRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
