#include <cstddef>
#include <cstdint>
#include "WoW/Source/SoundInterface/SoundInterface2Combat.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: 0x00B4AE7C
static TSFixedArray<IMPACTSOUNDARRAY> s_impactSounds;

// STATIC: 0x00B4AE88
static int32_t s_swingSounds[NUM_WEAPONSWINGSOUNDTYPES][NUM_SWINGTYPES];

// VERIFY: 647f0380 auto
// OFFSET: 0x007DA1F0
IMPACTSOUND::IMPACTSOUND() {
    this->m_soundId = 0;
    this->m_critSoundId = 0;
}

// VERIFY: 2250fb03 manual
// OFFSET: 0x004D0380
static void InitWeaponImpactSound(const WeaponImpactSoundsRec* rec) {
    if (!s_impactSounds.m_count) {
        return;
    }

    STORM_ASSERT(rec->m_WeaponSubClassID < (int)ClientDBGetNumWeaponSubclasses());
    IMPACTSOUNDARRAY* sounds = &s_impactSounds[rec->m_WeaponSubClassID];

    for (int32_t i = 0; i < NUM_WEAPON_IMPACT_SOUNDS; i++) {
        sounds->m_sounds[i][rec->m_ParrySoundType].m_soundId = rec->m_impactSoundID[i];
        sounds->m_sounds[i][rec->m_ParrySoundType].m_critSoundId = rec->m_critImpactSoundID[i];
    }
}

// VERIFY: d720637a auto
// OFFSET: 0x004D04E0
void SI2::InitWeaponSwingCombatSounds() {
    for (int32_t i = g_weaponSwingSounds2DB.GetNumRecords(); i;) {
        i--;

        WeaponSwingSounds2Rec* rec = (i < 0 || i >= g_weaponSwingSounds2DB.GetNumRecords()) ? nullptr : &g_weaponSwingSounds2DB.m_records[i];

        if (rec && rec->m_swingType < NUM_WEAPONSWINGSOUNDTYPES && rec->m_crit < NUM_SWINGTYPES) {
            s_swingSounds[rec->m_swingType][rec->m_crit != SWING_NORMAL] = rec->m_soundID;
        }
    }
}

static inline void GetWeaponTypeAndMaterial(const CGUnitVirtualItem* item, uint32_t* weaponType, PARRYMATERIALS* material) { // (inferred)
    STORM_ASSERT(weaponType);
    STORM_ASSERT(material);

    if (item) {
        STORM_ASSERT(item->m_classID == ITEM_CLASS::ITEM_CLASS_WEAPON); // 12213: "item->m_classID == ITEM_WEAPON"
        *weaponType = item->m_soundOverrideSubclassID == 0xFF ? item->m_subclassID : item->m_soundOverrideSubclassID;
        *material = static_cast<PARRYMATERIALS>(CGItem_C::IsMetal(item->m_material));
    } else {
        *material = PARRYMATERIAL_WOOD;
        *weaponType = ClientDBGetUnarmedWeapon();
    }
}

// VERIFY: df290633 manual
// OFFSET: 0x004D0540
void SI2::PlayParrySound(const CGUnitVirtualItem* weapon, const CGUnitVirtualItem* defender, int32_t crit, const C3Vector* position, int32_t self) {
    if (!s_impactSounds.m_count) {
        return;
    }

    uint32_t weaponType;
    PARRYMATERIALS material;
    GetWeaponTypeAndMaterial(weapon, &weaponType, &material);

    int32_t type = 6;

    if (defender) {
        if (defender->m_classID == ITEM_CLASS::ITEM_CLASS_WEAPON) {
            type = 6 - CGItem_C::IsMetal(defender->m_material);
        } else if (defender->m_classID == ITEM_CLASS::ITEM_CLASS_ARMOR) {
            type = 4 - CGItem_C::IsMetal(defender->m_material);
        } else {
            STORM_ASSERT(!"parrying object is not a weapon or armor..something is wrong here!");
        }
    }

    STORM_ASSERT(weaponType < ClientDBGetNumWeaponSubclasses());
    const IMPACTSOUND* sound = &s_impactSounds[weaponType].m_sounds[type][material];

    C3Vector soundPosition;
    soundPosition.x = position->x;
    soundPosition.y = position->y;
    soundPosition.z = position->z + 2.0f;

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();
    properties.m_group = SI2_SOUND_GROUP_IMPACT;

    if (self) {
        properties.m_priority = SI2_PRIORITY_OBJECT;
    }

    SI2::PlaySoundKit(crit != 0 ? sound->m_critSoundId : sound->m_soundId, &soundPosition, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: 4ec94dcc manual
// OFFSET: 0x004D0660
void SI2::PlayHitSound(const CGUnitVirtualItem* weapon, int32_t impactType, int32_t crit, const C3Vector* position, int32_t self) {
    if (!s_impactSounds.m_count) {
        return;
    }

    uint32_t weaponType;
    PARRYMATERIALS material;
    GetWeaponTypeAndMaterial(weapon, &weaponType, &material);

    STORM_ASSERT(weaponType < ClientDBGetNumWeaponSubclasses());
    const IMPACTSOUND* sound = &s_impactSounds[weaponType].m_sounds[impactType][material];

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();
    properties.m_group = SI2_SOUND_GROUP_IMPACT;

    if (self) {
        properties.m_priority = SI2_PRIORITY_OBJECT;
    }

    SI2::PlaySoundKit(crit != 0 ? sound->m_critSoundId : sound->m_soundId, position, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: 1275dcb8 manual
// OFFSET: 0x004D0720
SI2_RESULT SI2::PlayWeaponSwooshSound(WEAPONSWING_SOUNDTYPES type, int32_t crit, const C3Vector* position, int32_t quiet, int32_t self) {
    if (type >= NUM_WEAPONSWINGSOUNDTYPES) {
        return SI2_OK;
    }

    float volume = quiet ? 0.5f : 1.0f;

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();
    properties.m_volume = volume;
    properties.m_group = SI2_SOUND_GROUP_WEAPON_SWING;

    if (self) {
        properties.m_priority = SI2_PRIORITY_OBJECT;
    }

    return SI2::PlaySoundKit(s_swingSounds[type][crit != SWING_NORMAL], position, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: 4281e82d manual
// OFFSET: 0x004D07B0
SI2_RESULT SI2::PlaySheatheSound(const CGUnitVirtualItem* item, int32_t sheathe, const C3Vector* position, int32_t self) {
    if (!item) {
        return SI2_OK;
    }

    MaterialRec* material = g_materialDB.GetRecord(item->m_material);

    if (!material) {
        return SI2_OK;
    }

    C3Vector soundPosition;
    soundPosition.x = position->x;
    soundPosition.y = position->y;
    soundPosition.z = position->z + 2.0f;

    int32_t soundKitId = sheathe ? material->m_sheatheSoundID : material->m_unsheatheSoundID;

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();

    if (self) {
        properties.m_priority = SI2_PRIORITY_OBJECT;
    }

    return SI2::PlaySoundKit(soundKitId, &soundPosition, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: c557bfee auto
// OFFSET: 0x004D0850
SI2_RESULT SI2::PlayMissedSound(bool oneHand, const C3Vector* position, int32_t self) {
    SE2SoundKitProperties properties;
    properties.ResetToDefaults();

    if (self) {
        properties.m_priority = SI2_PRIORITY_OBJECT;
    }

    return SI2::PlaySoundKit(oneHand ? "(DONOTRENAME)Combat Miss 1H" : "(DONOTRENAME)Combat Miss 2H", position, nullptr, &properties);
}

// VERIFY: 3aee2ee4 manual
// OFFSET: 0x004D0B70
void SI2::InitWeaponImpactCombatSounds() {
    s_impactSounds.SetCount(ClientDBGetNumWeaponSubclasses());

    for (int32_t i = g_weaponImpactSoundsDB.GetNumRecords(); i;) {
        i--;

        const WeaponImpactSoundsRec* rec = (i < 0 || i >= g_weaponImpactSoundsDB.GetNumRecords()) ? nullptr : &g_weaponImpactSoundsDB.m_records[i];
        STORM_ASSERT(rec);
        STORM_ASSERT(rec->m_WeaponSubClassID < (int)ClientDBGetNumWeaponSubclasses());
        STORM_ASSERT(rec->m_ParrySoundType < NUM_WEAPONMATERIALS);
        InitWeaponImpactSound(rec);
    }
}
