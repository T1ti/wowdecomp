#ifndef WOW_SOURCE_DB_REC_SPELLREC_H
#define WOW_SOURCE_DB_REC_SPELLREC_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Magic/MagicClient/SpellEnums.h"

class SFileImpl;

class SpellRec {
    public:
    static const int32_t s_numColumns = 234;
    static const int32_t s_rowSize = 936;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_category;
    /* 0008 */ int32_t m_dispelType;
    /* 000C */ int32_t m_mechanic;
    /* 0010 */ SPELL_ATTR0 m_attributes;
    /* 0014 */ SPELL_ATTR1 m_attributesEx;
    /* 0018 */ SPELL_ATTR2 m_attributesExB;
    /* 001C */ SPELL_ATTR3 m_attributesExC;
    /* 0020 */ SPELL_ATTR4 m_attributesExD;
    /* 0024 */ SPELL_ATTR5 m_attributesExE;
    /* 0028 */ SPELL_ATTR6 m_attributesExF;
    /* 002C */ SPELL_ATTR7 m_attributesExG;
    /* 0030 */ int32_t m_shapeshiftMask [2];
    /* 0038 */ int32_t m_shapeshiftExclude [2];
    /* 0040 */ int32_t m_targets;
    /* 0044 */ int32_t m_targetCreatureType;
    /* 0048 */ int32_t m_requiresSpellFocus;
    /* 004C */ int32_t m_facingCasterFlags;
    /* 0050 */ int32_t m_casterAuraState;
    /* 0054 */ int32_t m_targetAuraState;
    /* 0058 */ int32_t m_excludeCasterAuraState;
    /* 005C */ int32_t m_excludeTargetAuraState;
    /* 0060 */ int32_t m_casterAuraSpell;
    /* 0064 */ int32_t m_targetAuraSpell;
    /* 0068 */ int32_t m_excludeCasterAuraSpell;
    /* 006C */ int32_t m_excludeTargetAuraSpell;
    /* 0070 */ int32_t m_castingTimeIndex;
    /* 0074 */ int32_t m_recoveryTime;
    /* 0078 */ int32_t m_categoryRecoveryTime;
    /* 007C */ int32_t m_interruptFlags;
    /* 0080 */ int32_t m_auraInterruptFlags;
    /* 0084 */ int32_t m_channelInterruptFlags;
    /* 0088 */ int32_t m_procTypeMask;
    /* 008C */ int32_t m_procChance;
    /* 0090 */ int32_t m_procCharges;
    /* 0094 */ int32_t m_maxLevel;
    /* 0098 */ int32_t m_baseLevel;
    /* 009C */ int32_t m_spellLevel;
    /* 00A0 */ int32_t m_durationIndex;
    /* 00A4 */ int32_t m_powerType;
    /* 00A8 */ int32_t m_manaCost;
    /* 00AC */ int32_t m_manaCostPerLevel;
    /* 00B0 */ int32_t m_manaPerSecond;
    /* 00B4 */ int32_t m_manaPerSecondPerLevel;
    /* 00B8 */ int32_t m_rangeIndex;
    /* 00BC */ float m_speed;
    /* 00C0 */ int32_t m_modalNextSpell;
    /* 00C4 */ int32_t m_cumulativeAura;
    /* 00C8 */ int32_t m_totem [2];
    /* 00D0 */ int32_t m_reagent [8];
    /* 00F0 */ int32_t m_reagentCount [8];
    /* 0110 */ int32_t m_equippedItemClass;
    /* 0114 */ int32_t m_equippedItemSubclass;
    /* 0118 */ int32_t m_equippedItemInvTypes;
    /* 011C */ int32_t m_effect [3];
    /* 0128 */ int32_t m_effectDieSides [3];
    /* 0134 */ float m_effectRealPointsPerLevel [3];
    /* 0140 */ int32_t m_effectBasePoints [3];
    /* 014C */ int32_t m_effectMechanic [3];
    /* 0158 */ int32_t m_implicitTargetA [3];
    /* 0164 */ int32_t m_implicitTargetB [3];
    /* 0170 */ int32_t m_effectRadiusIndex [3];
    /* 017C */ int32_t m_effectAura [3];
    /* 0188 */ int32_t m_effectAuraPeriod [3];
    /* 0194 */ float m_effectAmplitude [3];
    /* 01A0 */ int32_t m_effectChainTargets [3];
    /* 01AC */ int32_t m_effectItemType [3];
    /* 01B8 */ int32_t m_effectMiscValue [3];
    /* 01C4 */ int32_t m_effectMiscValueB [3];
    /* 01D0 */ int32_t m_effectTriggerSpell [3];
    /* 01DC */ float m_effectPointsPerCombo [3];
    /* 01E8 */ int32_t m_effectSpellClassMaskA [3];
    /* 01F4 */ int32_t m_effectSpellClassMaskB [3];
    /* 0200 */ int32_t m_effectSpellClassMaskC [3];
    /* 020C */ int32_t m_spellVisualID [2];
    /* 0214 */ int32_t m_spellIconID;
    /* 0218 */ int32_t m_activeIconID;
    /* 021C */ int32_t m_spellPriority;
    /* 0220 */ const char* m_name;
    /* 0224 */ const char* m_nameSubtext;
    /* 0228 */ const char* m_description;
    /* 022C */ const char* m_auraDescription;
    /* 0230 */ int32_t m_manaCostPct;
    /* 0234 */ int32_t m_startRecoveryCategory;
    /* 0238 */ int32_t m_startRecoveryTime;
    /* 023C */ int32_t m_maxTargetLevel;
    /* 0240 */ int32_t m_spellClassSet;
    /* 0244 */ int32_t m_spellClassMask [3];
    /* 0250 */ int32_t m_maxTargets;
    /* 0254 */ int32_t m_defenseType;
    /* 0258 */ int32_t m_preventionType;
    /* 025C */ int32_t m_stanceBarOrder;
    /* 0260 */ float m_effectChainAmplitude [3];
    /* 026C */ int32_t m_minFactionID;
    /* 0270 */ int32_t m_minReputation;
    /* 0274 */ int32_t m_requiredAuraVision;
    /* 0278 */ int32_t m_requiredTotemCategoryID [2];
    /* 0280 */ int32_t m_requiredAreasID;
    /* 0284 */ int32_t m_schoolMask;
    /* 0288 */ int32_t m_runeCostID;
    /* 028C */ int32_t m_spellMissileID;
    /* 0290 */ int32_t m_powerDisplayID;
    /* 0294 */ float m_effectBonusCoefficient [3];
    /* 02A0 */ int32_t m_descriptionVariablesID;
    /* 02A4 */ int32_t m_difficulty;

    SpellRec();
    ~SpellRec();
    static const char* GetFilename();
    static bool HasAura(const SpellRec* spell, int32_t aura);
    static int32_t IsHelpfulOrHarmful(const SpellRec* spell);
    static bool WillSummonCritter(const SpellRec* spell);
    static const int32_t* GetClassMask(const SpellRec* spell, int32_t effect);
    static bool IsAffectedBy(const SpellRec* spell, const SpellRec* modifier, int32_t effect);
    static bool UsableInShapeshift(const SpellRec* spell, int32_t form);
    static bool NotUsableInShapeshiftForm(const SpellRec* spell, int32_t form);
    static int32_t GetCastTime(const SpellRec* spell, int32_t isPet, int32_t isUnit, int32_t allowNegative);
    static bool IsMeleeRange(const SpellRec* spell);
    static bool IsMinimumRange(const SpellRec* spell);
    static void GetMinRange(const SpellRec* spell, float* range);
    static bool HasModifier(const SpellRec* spell, int32_t modifier);
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
