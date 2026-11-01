#include "Unit.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC49D8
const uint32_t CGUnit::s_mirrorFieldIndices[CGUnit::NUM_MIRROR_FIELDS] = {
    CGUnitData::FIELD_ID::UNIT_FIELD_CHARM,
    CGUnitData::FIELD_ID::UNIT_FIELD_CHARM + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_SUMMON,
    CGUnitData::FIELD_ID::UNIT_FIELD_SUMMON + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_CRITTER,
    CGUnitData::FIELD_ID::UNIT_FIELD_CRITTER + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_CHARMEDBY,
    CGUnitData::FIELD_ID::UNIT_FIELD_CHARMEDBY + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_SUMMONEDBY,
    CGUnitData::FIELD_ID::UNIT_FIELD_SUMMONEDBY + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_TARGET,
    CGUnitData::FIELD_ID::UNIT_FIELD_TARGET + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_CHANNEL_OBJECT,
    CGUnitData::FIELD_ID::UNIT_FIELD_CHANNEL_OBJECT + 1,
    CGUnitData::FIELD_ID::UNIT_CHANNEL_SPELL,
    CGUnitData::FIELD_ID::UNIT_FIELD_BYTES_0,
    CGUnitData::FIELD_ID::UNIT_FIELD_HEALTH,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER1,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER2,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER3,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER4,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER5,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER6,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER7,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXHEALTH,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER1,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER2,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER3,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER4,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER5,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER6,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXPOWER7,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_LEVEL,
    CGUnitData::FIELD_ID::UNIT_FIELD_FACTIONTEMPLATE,
    CGUnitData::FIELD_ID::UNIT_VIRTUAL_ITEM_SLOT_ID,
    CGUnitData::FIELD_ID::UNIT_VIRTUAL_ITEM_SLOT_ID + 1,
    CGUnitData::FIELD_ID::UNIT_VIRTUAL_ITEM_SLOT_ID + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_FLAGS,
    CGUnitData::FIELD_ID::UNIT_FIELD_FLAGS_2,
    CGUnitData::FIELD_ID::UNIT_FIELD_AURASTATE,
    CGUnitData::FIELD_ID::UNIT_FIELD_BASEATTACKTIME,
    CGUnitData::FIELD_ID::UNIT_FIELD_BASEATTACKTIME + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_RANGEDATTACKTIME,
    CGUnitData::FIELD_ID::UNIT_FIELD_DISPLAYID,
    CGUnitData::FIELD_ID::UNIT_FIELD_MOUNTDISPLAYID,
    CGUnitData::FIELD_ID::UNIT_FIELD_MINDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_MINOFFHANDDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXOFFHANDDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_BYTES_1,
    CGUnitData::FIELD_ID::UNIT_FIELD_PETNUMBER,
    CGUnitData::FIELD_ID::UNIT_FIELD_PET_NAME_TIMESTAMP,
    CGUnitData::FIELD_ID::UNIT_FIELD_PETEXPERIENCE,
    CGUnitData::FIELD_ID::UNIT_FIELD_PETNEXTLEVELEXP,
    CGUnitData::FIELD_ID::UNIT_DYNAMIC_FLAGS,
    CGUnitData::FIELD_ID::UNIT_NPC_FLAGS,
    CGUnitData::FIELD_ID::UNIT_NPC_EMOTESTATE,
    CGUnitData::FIELD_ID::UNIT_FIELD_STAT0,
    CGUnitData::FIELD_ID::UNIT_FIELD_STAT1,
    CGUnitData::FIELD_ID::UNIT_FIELD_STAT2,
    CGUnitData::FIELD_ID::UNIT_FIELD_STAT3,
    CGUnitData::FIELD_ID::UNIT_FIELD_STAT4,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCES + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_BYTES_2,
    CGUnitData::FIELD_ID::UNIT_FIELD_ATTACK_POWER,
    CGUnitData::FIELD_ID::UNIT_FIELD_ATTACK_POWER_MODS,
    CGUnitData::FIELD_ID::UNIT_FIELD_ATTACK_POWER_MULTIPLIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_RANGED_ATTACK_POWER,
    CGUnitData::FIELD_ID::UNIT_FIELD_RANGED_ATTACK_POWER_MODS,
    CGUnitData::FIELD_ID::UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_MINRANGEDDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXRANGEDDAMAGE,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MODIFIER + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 1,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 2,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 3,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 4,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 5,
    CGUnitData::FIELD_ID::UNIT_FIELD_POWER_COST_MULTIPLIER + 6,
    CGUnitData::FIELD_ID::UNIT_FIELD_MAXHEALTHMODIFIER,
    CGUnitData::FIELD_ID::UNIT_FIELD_HOVERHEIGHT,
    CGUnitData::FIELD_ID::UNIT_END,
};

// VERIFY: af22e65c auto
// OFFSET: 0x004F52D0
uint32_t CGUnit::GetSavedIndexFromDataIndex(uint32_t fieldIndex, uint32_t unused_count, uint32_t unused_local) {
    for (uint32_t i = 0; i < sizeof(s_mirrorFieldIndices) / sizeof(s_mirrorFieldIndices[0]); i++) {
        if (s_mirrorFieldIndices[i] == fieldIndex) {
            return i;
        }
    }

    return CGUnit::MIRROR_INDEX_NOT_FOUND;
}

// VERIFY: 2461627f manual
// OFFSET: 0x004F53D0
bool CGUnit::CanSwim() const {
    uint32_t flags = this->m_unit->m_unitFieldFlags;

    if (flags & UNIT_FLAGS::UNIT_FLAG_CANNOT_SWIM) {
        return false;
    }

    if ((flags & UNIT_FLAGS::UNIT_FLAG_PLAYER_CONTROLLED) || (flags & UNIT_FLAGS::UNIT_FLAG_PET_IN_COMBAT) || (flags & UNIT_FLAGS::UNIT_FLAG_EVADING_HOME)) {
        return true;
    }

    return flags & UNIT_FLAGS::UNIT_FLAG_CAN_SWIM;
}

// VERIFY: a616de1f manual
// OFFSET: 0x004F5410
int32_t CGUnit::CanPlayEmote(uint32_t emoteFlags, int32_t targeted, int32_t* standUp) const {
    *standUp = 0;

    if (targeted && (emoteFlags & 0x400)) {
        return 0;
    }

    if ((emoteFlags & 0x1) && (this->m_unit->m_standState != UNIT_STANDING)) {
        return 0;
    }

    if ((emoteFlags & 0x80) && (this->m_movement->m_flags & MOVEMENTFLAG_SWIMMING)) {
        return 0;
    }

    if (emoteFlags & 0x8000) {
        CMoveSpline* spline = this->m_movement->m_spline;

        if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FLYING)) {
            return 0;
        }

        if (this->m_movement->m_flags & MOVEMENTFLAG_FLYING) {
            return 0;
        }
    }

    if (!(emoteFlags & 0x200)) {
        uint8_t standState = this->m_unit->m_standState;

        if (standState == UNIT_SLEEPING || standState == UNIT_DEAD) {
            return 0;
        }
    }

    if ((emoteFlags & 0x4000) && (this->m_movement->m_flags & MOVEMENTFLAG_ACTIVE_MASK)) {
        *standUp = 1;
    }

    return 1;
}

// VERIFY: 4f508d75 auto
// OFFSET: 0x004F54D0
void CGUnit::GetResistanceAndBuffs(int32_t school, int32_t* baseValue, int32_t* total, int32_t* effective, int32_t* positive, int32_t* negative) const {
    int32_t resistance = this->m_unit->m_unitFieldResistances[school];
    *effective = resistance;
    *total = resistance;
    *positive = this->m_unit->m_unitFieldResistancebuffmodspositive[school];
    *negative = this->m_unit->m_unitFieldResistancebuffmodsnegative[school];
    *baseValue = *total - *positive - *negative;

    if (*total < 0) {
        *effective = 0;
        *total = 0;
    }
}

// VERIFY: fca45f02 auto
// OFFSET: 0x004F5F20
const WOWGUID* CGUnit::GetCharmerOrCreatorGuid() const {
    CGUnitData* unit = this->m_unit;

    if (unit->m_unitFieldCharmedby) {
        return &unit->m_unitFieldCharmedby;
    }

    return &unit->m_unitFieldCreatedby;
}

// VERIFY: 5d850478 auto
// OFFSET: 0x004F5F40
float CGUnit::GetMeleeRange(const CGUnit* target) const {
    float range = target->m_unit->m_unitFieldCombatreach + this->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        return 5.0f;
    }

    return range;
}

// VERIFY: 803010fd manual
// OFFSET: 0x005774B0
int32_t CGUnit::GetEffectiveStat(int32_t stat) const {
    return this->m_unit->m_unitFieldStat[stat] < 0 ? 0 : this->m_unit->m_unitFieldStat[stat];
}

// VERIFY: 4fc14c7a manual
// OFFSET: 0x00716160
int32_t CGUnit::HasAuraState(int32_t state) const {
    return (this->m_unit->m_unitFieldAurastate & (1 << (state - 1))) != 0;
}

// VERIFY: a8bc343d manual
// OFFSET: 0x004F5290
int32_t CGUnit::IsOnFlyingSpline() const {
    CMoveSpline* spline = this->m_movement->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FLYING)) {
        return 1;
    }

    return 0;
}

// VERIFY: dd6976d0 auto
// OFFSET: 0x005F9810
int32_t CGUnit::IsOnSpline() const {
    CMoveSpline* spline = this->m_movement->m_spline;
    return spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE);
}

// VERIFY: ea2cbe01 auto
// OFFSET: 0x005FEE10
int32_t CGUnit::IsJumping() const {
    return (this->m_movement->m_flags & MOVEMENTFLAG_FALLING) && 0.0f != this->m_movement->m_fallVelocity;
}

// VERIFY: 4e64bda0 auto
// OFFSET: 0x005FA9E0
int32_t CGUnit::CanMove() const {
    return this->m_unit->m_unitFieldHealth > 0 && !(this->m_movement->m_flags & (MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_ROOT | MOVEMENTFLAG_PENDING_ROOT)) && this->m_unit->m_standState != UNIT_DEAD;
}

// VERIFY: 68641d42 auto
// OFFSET: 0x004D4240
uint32_t CGUnit::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, uint32_t local, uint32_t base) {
    int32_t objectFields = CGObject::TotalFieldsSaved();
    return (offset & 3) + 4 * (objectFields + GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: 93ca7f49 auto
// OFFSET: 0x004F52C0
int32_t CGUnit::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + CGUnit::NUM_MIRROR_FIELDS;
}

// VERIFY: 65e96d9b auto
// OFFSET: 0x005F97F0
float CGUnit::GetFacing(float facing) const {
    return this->m_movement->GetFacing(facing);
}

// VERIFY: 5712b6fb manual
// OFFSET: 0x004F5300
float __stdcall CGUnit::GetPowerRegenBase(POWER_TYPE powerType, uint8_t inCombat) {
    static float s_regen[7][2] = {
        { 0.0f, 0.0f },
        { -12.5f, 0.0f },
        { 5.0f, 5.0f },
        { 10.0f, 10.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { -12.5f, 0.0f }
    };

    return s_regen[powerType][inCombat];
}

// VERIFY: 5dbc6590 auto
// OFFSET: 0x004F5390
float CGUnit::GetPowerRegen(POWER_TYPE powerType, bool inCombat) const {
    if (inCombat) {
        return CGUnit::GetPowerRegenBase(powerType, 1) + this->m_unit->m_unitFieldPowerRegenInterruptedFlatModifier[powerType];
    }

    return CGUnit::GetPowerRegenBase(powerType, 0) + this->m_unit->m_unitFieldPowerRegenFlatModifier[powerType];
}

// VERIFY: 90149eb1 manual
// OFFSET: 0x004F7250
bool CGUnit::IsPossessedBy(const WOWGUID& guid) const {
    return (this->m_unit->m_unitFieldFlags & UNIT_FLAGS::UNIT_FLAG_POSSESSED) && *this->GetCharmerOrCreatorGuid() == guid;
}

// VERIFY: 3c755101 auto
// OFFSET: 0x00716140
bool CGUnit::HasCreator() const {
    return this->m_unit->m_unitFieldCreatedby.m_guid != 0;
}

// VERIFY: 94ea7916 auto
// OFFSET: 0x0053B8C0
bool CGUnit::HasSummoner() const {
    return this->m_unit->m_unitFieldSummonedby.m_guid != 0;
}

// VERIFY: 00a60867 manual
// OFFSET: 0x004F5F80
bool CGUnit::IsGuildMaster() const {
    uint32_t npcFlags = this->m_unit->m_unitNpcFlags;
    return (npcFlags & UNIT_NPC_FLAGS::UNIT_NPC_FLAG_PETITIONER) && (npcFlags & UNIT_NPC_FLAGS::UNIT_NPC_FLAG_TABARD_DESIGNER);
}

// VERIFY: 124e37fd auto
// OFFSET: 0x004F5F00
bool CGUnit::IsCharmed() const {
    return this->m_unit->m_unitFieldCharmedby != WOWGUID();
}

// VERIFY: 2ed8af1f manual
// OFFSET: 0x005EE050
int32_t CGUnit::GetSpellCostModifier(int32_t schoolMask) const {
    int32_t result = 0;
    bool found = false;

    for (uint32_t i = 0; i < sizeof(this->m_unit->m_unitFieldPowerCostModifier) / sizeof(this->m_unit->m_unitFieldPowerCostModifier[0]); i++) {
        if (((1 << i) & schoolMask) && (!found || this->m_unit->m_unitFieldPowerCostModifier[i] < result)) {
            result = this->m_unit->m_unitFieldPowerCostModifier[i];
            found = true;
        }
    }

    return result;
}

// VERIFY: 335247cb manual
// OFFSET: 0x005EE0B0
float CGUnit::GetSpellCostMultiplier(int32_t schoolMask) const {
    float result = 0.0f;
    bool found = false;

    for (uint32_t i = 0; i < sizeof(this->m_unit->m_unitFieldPowerCostMultiplier) / sizeof(this->m_unit->m_unitFieldPowerCostMultiplier[0]); i++) {
        if (((1 << i) & schoolMask) && (!found || result > this->m_unit->m_unitFieldPowerCostMultiplier[i])) {
            result = this->m_unit->m_unitFieldPowerCostMultiplier[i];
            found = true;
        }
    }

    return result + 1.0f;
}

// VERIFY: 43c5b8ef auto
// OFFSET: 0x0058AAB0
int32_t CGUnit::GetQuestGreenRange() const {
    static const int32_t s_questGreenRange[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
    uint32_t band = static_cast<int32_t>(this->m_unit->m_unitFieldLevel) / 5;
    return band < 20 ? s_questGreenRange[band] : 8;
}

// VERIFY: 467ea880 manual
// OFFSET: 0x00716190
bool CGUnit::IsSitting() const {
    uint8_t standState = this->m_unit->m_standState;
    return standState == UNIT_SITTING || (standState >= UNIT_FIRSTCHAIRSIT && standState <= UNIT_FIRSTCHAIRSIT + UNIT_NUMCHAIRSTATES - 1);
}

// VERIFY: 01cf0dc8 auto
// OFFSET: 0x006FED70
C3Vector CGUnit::GetVelocity() const {
    float speed = this->m_movement->m_currentSpeed;
    return C3Vector(this->m_movement->m_moveDir.x * speed, this->m_movement->m_moveDir.y * speed, speed * this->m_movement->m_moveDir.z);
}
