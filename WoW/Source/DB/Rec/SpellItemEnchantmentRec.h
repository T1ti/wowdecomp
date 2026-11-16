#ifndef WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTREC_H
#define WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Inferred name: effect slots of a SpellItemEnchantment.dbc record.
constexpr int32_t NUM_SPELL_ITEM_ENCHANTMENT_EFFECTS = 3;

// Effect kinds in the 3.3.5 SpellItemEnchantment.dbc three-slot effect array.
// Names describe behavior; raw file values and unknown-kind fallbacks stay intact.
enum ITEM_ENCHANTMENT_EFFECT : int32_t {
    ENCHANTMENT_EFFECT_NONE                  = 0,
    ENCHANTMENT_EFFECT_COMBAT_SPELL          = 1,
    ENCHANTMENT_EFFECT_WEAPON_DAMAGE         = 2,
    ENCHANTMENT_EFFECT_EQUIP_SPELL           = 3,
    ENCHANTMENT_EFFECT_RESISTANCE            = 4,
    ENCHANTMENT_EFFECT_STAT                  = 5,
    ENCHANTMENT_EFFECT_WEAPON_DPS            = 6,
    ENCHANTMENT_EFFECT_USE_SPELL             = 7,
    ENCHANTMENT_EFFECT_PRISMATIC_SOCKET      = 8,
};

// SpellItemEnchantment.dbc flags tested by the client.
enum ITEM_ENCHANTMENT_FLAGS : int32_t {
    ENCHANTMENT_FLAG_NONE      = 0x00000000,
    // Makes the enchanted item soulbound.
    ENCHANTMENT_FLAG_SOULBOUND = 0x00000001,
    // 3.3.5 Packet_SMSG_ENCHANTMENTLOG skips apply/remove entries when set.
    ENCHANTMENT_FLAG_SUPPRESS_COMBAT_LOG = 0x00000002,
};

class SpellItemEnchantmentRec {
    public:
    static const int32_t s_numColumns = 38;
    static const int32_t s_rowSize = 152;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_charges;
    /* 0008 */ ITEM_ENCHANTMENT_EFFECT m_effect [NUM_SPELL_ITEM_ENCHANTMENT_EFFECTS];
    /* 0014 */ int32_t m_effectPointsMin [NUM_SPELL_ITEM_ENCHANTMENT_EFFECTS];
    /* 0020 */ int32_t m_effectPointsMax [NUM_SPELL_ITEM_ENCHANTMENT_EFFECTS];
    /* 002C */ int32_t m_effectArg [NUM_SPELL_ITEM_ENCHANTMENT_EFFECTS];
    /* 0038 */ const char* m_name;
    /* 003C */ int32_t m_itemVisual;
    /* 0040 */ ITEM_ENCHANTMENT_FLAGS m_flags;
    /* 0044 */ int32_t m_srcItemID;
    /* 0048 */ int32_t m_conditionID;
    /* 004C */ int32_t m_requiredSkillID;
    /* 0050 */ int32_t m_requiredSkillRank;
    /* 0054 */ int32_t m_minLevel;

    SpellItemEnchantmentRec();
    ~SpellItemEnchantmentRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
