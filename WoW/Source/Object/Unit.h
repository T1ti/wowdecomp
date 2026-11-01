#ifndef WOW_SOURCE_OBJECT_UNIT_H
#define WOW_SOURCE_OBJECT_UNIT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CGUnitData;
class CMovement_C;

class CGUnit {
    public:
    int32_t GetQuestGreenRange() const;
    static const int32_t NUM_MIRROR_FIELDS = 123;
    static const uint32_t MIRROR_INDEX_NOT_FOUND = 142;
    static const uint32_t s_mirrorFieldIndices[NUM_MIRROR_FIELDS];

    static uint32_t GetSavedIndexFromDataIndex(uint32_t fieldIndex, uint32_t unused_count, uint32_t unused_local);
    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, uint32_t local, uint32_t base);
    static int32_t TotalFieldsSaved();
    static float __stdcall GetPowerRegenBase(POWER_TYPE powerType, uint8_t inCombat);

    /* 0000 */ CGUnitData* m_unit;
    /* 0004 */ void* m_unitMirror;
    /* 0008 */ CMovement_C* m_movement;

    C3Vector GetVelocity() const;

    bool CanSwim() const;
    int32_t CanPlayEmote(uint32_t emoteFlags, int32_t targeted, int32_t* standUp) const;
    void GetResistanceAndBuffs(int32_t school, int32_t* baseValue, int32_t* total, int32_t* effective, int32_t* positive, int32_t* negative) const;
    const WOWGUID* GetCharmerOrCreatorGuid() const;
    bool HasCreator() const;
    bool HasSummoner() const;
    bool IsPossessedBy(const WOWGUID& guid) const;
    bool IsCharmed() const;
    bool IsSitting() const;
    bool IsGuildMaster() const;
    int32_t GetSpellCostModifier(int32_t schoolMask) const;
    float GetSpellCostMultiplier(int32_t schoolMask) const;
    float GetMeleeRange(const CGUnit* target) const;
    int32_t GetEffectiveStat(int32_t stat) const;
    int32_t HasAuraState(int32_t state) const;
    int32_t IsOnFlyingSpline() const;
    int32_t IsOnSpline() const;
    int32_t IsJumping() const;
    int32_t CanMove() const;
    float GetFacing(float facing) const;
    float GetPowerRegen(POWER_TYPE powerType, bool inCombat) const;
};

class CGUnitData {
    public:
    // DWORD indices relative to this data block, not absolute network-field indices.
    enum FIELD_ID : uint32_t {
        UNIT_FIELD_CHARM = 0,
        UNIT_FIELD_SUMMON = 2,
        UNIT_FIELD_CRITTER = 4,
        UNIT_FIELD_CHARMEDBY = 6,
        UNIT_FIELD_SUMMONEDBY = 8,
        UNIT_FIELD_CREATEDBY = 10,
        UNIT_FIELD_TARGET = 12,
        UNIT_FIELD_CHANNEL_OBJECT = 14,
        UNIT_CHANNEL_SPELL = 16,
        UNIT_FIELD_BYTES_0 = 17,
        UNIT_FIELD_HEALTH = 18,
        UNIT_FIELD_POWER1 = 19,
        UNIT_FIELD_POWER2 = 20,
        UNIT_FIELD_POWER3 = 21,
        UNIT_FIELD_POWER4 = 22,
        UNIT_FIELD_POWER5 = 23,
        UNIT_FIELD_POWER6 = 24,
        UNIT_FIELD_POWER7 = 25,
        UNIT_FIELD_MAXHEALTH = 26,
        UNIT_FIELD_MAXPOWER1 = 27,
        UNIT_FIELD_MAXPOWER2 = 28,
        UNIT_FIELD_MAXPOWER3 = 29,
        UNIT_FIELD_MAXPOWER4 = 30,
        UNIT_FIELD_MAXPOWER5 = 31,
        UNIT_FIELD_MAXPOWER6 = 32,
        UNIT_FIELD_MAXPOWER7 = 33,
        UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER = 34,
        UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = 41,
        UNIT_FIELD_LEVEL = 48,
        UNIT_FIELD_FACTIONTEMPLATE = 49,
        UNIT_VIRTUAL_ITEM_SLOT_ID = 50,
        UNIT_FIELD_FLAGS = 53,
        UNIT_FIELD_FLAGS_2 = 54,
        UNIT_FIELD_AURASTATE = 55,
        UNIT_FIELD_BASEATTACKTIME = 56,
        UNIT_FIELD_RANGEDATTACKTIME = 58,
        UNIT_FIELD_BOUNDINGRADIUS = 59,
        UNIT_FIELD_COMBATREACH = 60,
        UNIT_FIELD_DISPLAYID = 61,
        UNIT_FIELD_NATIVEDISPLAYID = 62,
        UNIT_FIELD_MOUNTDISPLAYID = 63,
        UNIT_FIELD_MINDAMAGE = 64,
        UNIT_FIELD_MAXDAMAGE = 65,
        UNIT_FIELD_MINOFFHANDDAMAGE = 66,
        UNIT_FIELD_MAXOFFHANDDAMAGE = 67,
        UNIT_FIELD_BYTES_1 = 68,
        UNIT_FIELD_PETNUMBER = 69,
        UNIT_FIELD_PET_NAME_TIMESTAMP = 70,
        UNIT_FIELD_PETEXPERIENCE = 71,
        UNIT_FIELD_PETNEXTLEVELEXP = 72,
        UNIT_DYNAMIC_FLAGS = 73,
        UNIT_MOD_CAST_SPEED = 74,
        UNIT_CREATED_BY_SPELL = 75,
        UNIT_NPC_FLAGS = 76,
        UNIT_NPC_EMOTESTATE = 77,
        UNIT_FIELD_STAT0 = 78,
        UNIT_FIELD_STAT1 = 79,
        UNIT_FIELD_STAT2 = 80,
        UNIT_FIELD_STAT3 = 81,
        UNIT_FIELD_STAT4 = 82,
        UNIT_FIELD_POSSTAT0 = 83,
        UNIT_FIELD_POSSTAT1 = 84,
        UNIT_FIELD_POSSTAT2 = 85,
        UNIT_FIELD_POSSTAT3 = 86,
        UNIT_FIELD_POSSTAT4 = 87,
        UNIT_FIELD_NEGSTAT0 = 88,
        UNIT_FIELD_NEGSTAT1 = 89,
        UNIT_FIELD_NEGSTAT2 = 90,
        UNIT_FIELD_NEGSTAT3 = 91,
        UNIT_FIELD_NEGSTAT4 = 92,
        UNIT_FIELD_RESISTANCES = 93,
        UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE = 100,
        UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE = 107,
        UNIT_FIELD_BASE_MANA = 114,
        UNIT_FIELD_BASE_HEALTH = 115,
        UNIT_FIELD_BYTES_2 = 116,
        UNIT_FIELD_ATTACK_POWER = 117,
        UNIT_FIELD_ATTACK_POWER_MODS = 118,
        UNIT_FIELD_ATTACK_POWER_MULTIPLIER = 119,
        UNIT_FIELD_RANGED_ATTACK_POWER = 120,
        UNIT_FIELD_RANGED_ATTACK_POWER_MODS = 121,
        UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = 122,
        UNIT_FIELD_MINRANGEDDAMAGE = 123,
        UNIT_FIELD_MAXRANGEDDAMAGE = 124,
        UNIT_FIELD_POWER_COST_MODIFIER = 125,
        UNIT_FIELD_POWER_COST_MULTIPLIER = 132,
        UNIT_FIELD_MAXHEALTHMODIFIER = 139,
        UNIT_FIELD_HOVERHEIGHT = 140,
        UNIT_FIELD_PADDING = 141,
        UNIT_END = 142,
    };

    // Descriptor spans are measured in DWORDs.
    enum FIELD_SIZE : uint32_t {
        UNIT_FIELD_CHARM_SIZE = 2,
        UNIT_FIELD_SUMMON_SIZE = 2,
        UNIT_FIELD_CRITTER_SIZE = 2,
        UNIT_FIELD_CHARMEDBY_SIZE = 2,
        UNIT_FIELD_SUMMONEDBY_SIZE = 2,
        UNIT_FIELD_CREATEDBY_SIZE = 2,
        UNIT_FIELD_TARGET_SIZE = 2,
        UNIT_FIELD_CHANNEL_OBJECT_SIZE = 2,
        UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER_SIZE = 7,
        UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER_SIZE = 7,
        UNIT_VIRTUAL_ITEM_SLOT_ID_SIZE = 3,
        UNIT_FIELD_BASEATTACKTIME_SIZE = 2,
        UNIT_FIELD_RESISTANCES_SIZE = 7,
        UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE_SIZE = 7,
        UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE_SIZE = 7,
        UNIT_FIELD_POWER_COST_MODIFIER_SIZE = 7,
        UNIT_FIELD_POWER_COST_MULTIPLIER_SIZE = 7,
    };


    /* 0000 */ WOWGUID m_unitFieldCharm;
    /* 0008 */ WOWGUID m_unitFieldSummon;
    /* 0010 */ WOWGUID m_unitFieldCritter;
    /* 0018 */ WOWGUID m_unitFieldCharmedby;
    /* 0020 */ WOWGUID m_unitFieldSummonedby;
    /* 0028 */ WOWGUID m_unitFieldCreatedby;
    /* 0030 */ WOWGUID m_unitFieldTarget;
    /* 0038 */ WOWGUID m_unitFieldChannelObject;
    /* 0040 */ uint32_t m_unitChannelSpell;
    /* 0044 */ uint8_t m_raceID;       // UNIT_FIELD_BYTES_0 byte 0: CHARACTER_RACE / ChrRacesRec ID.
    /* 0045 */ uint8_t m_classID;      // UNIT_FIELD_BYTES_0 byte 1: CHARACTER_CLASS / ChrClassesRec ID.
    /* 0046 */ uint8_t m_sex;          // UNIT_FIELD_BYTES_0 byte 2: UNIT_SEX.
    /* 0047 */ uint8_t m_displayPower; // UNIT_FIELD_BYTES_0 byte 3: POWER_TYPE.
    /* 0048 */ int32_t m_unitFieldHealth;
    /* 004C */ uint32_t m_unitFieldPower [POWER_TYPE::NUM_POWER_TYPES];
    /* 0068 */ uint32_t m_unitFieldMaxhealth;
    /* 006C */ uint32_t m_unitFieldMaxpower [POWER_TYPE::NUM_POWER_TYPES];
    /* 0088 */ float m_unitFieldPowerRegenFlatModifier [POWER_TYPE::NUM_POWER_TYPES];
    /* 00A4 */ float m_unitFieldPowerRegenInterruptedFlatModifier [POWER_TYPE::NUM_POWER_TYPES];
    /* 00C0 */ uint32_t m_unitFieldLevel;
    /* 00C4 */ uint32_t m_unitFieldFactiontemplate;
    /* 00C8 */ uint32_t m_unitVirtualItemSlotId [NUM_VIRTUAL_MONSTER_SLOTS];
    /* 00D4 */ UNIT_FLAGS m_unitFieldFlags;
    /* 00D8 */ UNIT_FLAGS2 m_unitFieldFlags2;
    /* 00DC */ uint32_t m_unitFieldAurastate;
    /* 00E0 */ uint32_t m_unitFieldBaseattacktime [2];
    /* 00E8 */ uint32_t m_unitFieldRangedattacktime;
    /* 00EC */ float m_unitFieldBoundingradius;
    /* 00F0 */ float m_unitFieldCombatreach;
    /* 00F4 */ uint32_t m_unitFieldDisplayid;
    /* 00F8 */ uint32_t m_unitFieldNativedisplayid;
    /* 00FC */ uint32_t m_unitFieldMountdisplayid;
    /* 0100 */ float m_unitFieldMindamage;
    /* 0104 */ float m_unitFieldMaxdamage;
    /* 0108 */ float m_unitFieldMinoffhanddamage;
    /* 010C */ float m_unitFieldMaxoffhanddamage;
    /* 0110 */ uint8_t m_standState;        // UNIT_FIELD_BYTES_1 byte 0: UNITSTANDSTATE.
    /* 0111 */ uint8_t m_unitBytes1Unknown; // UNIT_FIELD_BYTES_1 byte 1: Trinity PET_TALENTS label; no target consumer established.
    /* 0112 */ uint8_t m_visFlags;          // UNIT_FIELD_BYTES_1 byte 2: UNIT_VIS_FLAGS.
    /* 0113 */ uint8_t m_animTier;          // UNIT_FIELD_BYTES_1 byte 3: ANIMATION_TIER.
    /* 0114 */ uint32_t m_unitFieldPetnumber;
    /* 0118 */ uint32_t m_unitFieldPetNameTimestamp;
    /* 011C */ uint32_t m_unitFieldPetexperience;
    /* 0120 */ uint32_t m_unitFieldPetnextlevelexp;
    /* 0124 */ uint32_t m_unitDynamicFlags; // UNIT_DYNAMIC_FLAGS; raw mirror word.
    /* 0128 */ float m_unitModCastSpeed;
    /* 012C */ uint32_t m_unitCreatedBySpell;
    /* 0130 */ uint32_t m_unitNpcFlags; // UNIT_NPC_FLAGS; raw mirror word.
    /* 0134 */ uint32_t m_unitNpcEmotestate;
    /* 0138 */ int32_t m_unitFieldStat [5];
    /* 014C */ int32_t m_unitFieldPosstat [5];
    /* 0160 */ int32_t m_unitFieldNegstat [5];
    /* 0174 */ int32_t m_unitFieldResistances [7];
    /* 0190 */ int32_t m_unitFieldResistancebuffmodspositive [7];
    /* 01AC */ int32_t m_unitFieldResistancebuffmodsnegative [7];
    /* 01C8 */ uint32_t m_unitFieldBaseMana;
    /* 01CC */ uint32_t m_unitFieldBaseHealth;
    /* 01D0 */ uint8_t m_sheatheState;   // UNIT_FIELD_BYTES_2 byte 0: WEAPONMODE.
    /* 01D1 */ uint8_t m_pvpFlags;       // UNIT_FIELD_BYTES_2 byte 1: UNIT_PVP_FLAGS.
    /* 01D2 */ uint8_t m_petFlags;       // UNIT_FIELD_BYTES_2 byte 2: UNIT_PET_FLAGS.
    /* 01D3 */ uint8_t m_shapeshiftForm; // UNIT_FIELD_BYTES_2 byte 3: SpellShapeshiftFormRec ID; zero means no form.
    /* 01D4 */ int32_t m_unitFieldAttackPower;
    /* 01D8 */ int16_t m_unitFieldAttackPowerMods [2];
    /* 01DC */ float m_unitFieldAttackPowerMultiplier;
    /* 01E0 */ int32_t m_unitFieldRangedAttackPower;
    /* 01E4 */ int16_t m_unitFieldRangedAttackPowerMods [2];
    /* 01E8 */ float m_unitFieldRangedAttackPowerMultiplier;
    /* 01EC */ float m_unitFieldMinrangeddamage;
    /* 01F0 */ float m_unitFieldMaxrangeddamage;
    /* 01F4 */ int32_t m_unitFieldPowerCostModifier [7];
    /* 0210 */ float m_unitFieldPowerCostMultiplier [7];
    /* 022C */ float m_unitFieldMaxhealthmodifier;
    /* 0230 */ float m_unitFieldHoverheight;
    /* 0234 */ uint32_t m_unitFieldPadding;
};

#endif
