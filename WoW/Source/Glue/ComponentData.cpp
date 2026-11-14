#include "ComponentData.h"

// VERIFY: 84ef6cc9 auto
// OFFSET: 0x004DFDA0
ComponentData::ComponentData() {
    this->m_flags &= ~ComponentData::DATA_FLAGS::INITIAL_CLEAR_MASK;
    this->m_preferences.m_raceID = 0;
    this->m_preferences.m_sexID = 0;
    this->m_preferences.m_classId = 0;
    this->m_preferences.m_hairColorId = 0;
    this->m_preferences.m_skinID = 0;
    this->m_preferences.m_faceId = 0;
    this->m_preferences.m_facialHairId = 0;
    this->m_preferences.m_hairStyleId = 0;
    this->m_npcSkinTexture[0] = 0;
    this->m_model = nullptr;
    this->m_geosets[CHARGEOSET_HAIR] = CHARACTER_GEOSET_ID::HAIR_DEFAULT;
    this->m_geosets[CHARGEOSET_BEARD] = CHARACTER_GEOSET_ID::BEARD_DEFAULT;
    this->m_geosets[CHARGEOSET_SIDEBURN] = CHARACTER_GEOSET_ID::SIDEBURN_DEFAULT;
    this->m_geosets[CHARGEOSET_MOUSTACHE] = CHARACTER_GEOSET_ID::MOUSTACHE_DEFAULT;
    this->m_geosets[CHARGEOSET_GLOVE] = CHARACTER_GEOSET_ID::GLOVE_DEFAULT;
    this->m_geosets[CHARGEOSET_BOOT] = CHARACTER_GEOSET_ID::BOOT_DEFAULT;
    this->m_geosets[CHARGEOSET_SHIRT] = CHARACTER_GEOSET_ID::SHIRT_DEFAULT;
    this->m_geosets[CHARGEOSET_EAR] = CHARACTER_GEOSET_ID::EAR_VARIANT_2;
    this->m_geosets[CHARGEOSET_SLEEVES] = CHARACTER_GEOSET_ID::SLEEVES_DEFAULT;
    this->m_geosets[CHARGEOSET_PANTS] = CHARACTER_GEOSET_ID::PANTS_DEFAULT;
    this->m_geosets[CHARGEOSET_DOUBLET] = CHARACTER_GEOSET_ID::DOUBLET_DEFAULT;
    this->m_geosets[CHARGEOSET_PANTDOUBLET] = CHARACTER_GEOSET_ID::PANTDOUBLET_DEFAULT;
    this->m_geosets[CHARGEOSET_TABARD] = CHARACTER_GEOSET_ID::TABARD_DEFAULT;
    this->m_geosets[CHARGEOSET_ROBE] = CHARACTER_GEOSET_ID::ROBE_DEFAULT;
    this->m_geosets[CHARGEOSET_LOINCLOTH] = CHARACTER_GEOSET_ID::LOINCLOTH_DEFAULT;
    this->m_geosets[CHARGEOSET_CAPE] = CHARACTER_GEOSET_ID::CAPE_DEFAULT;
    this->m_geosets[CHARGEOSET_FACIALJEWELRY] = CHARACTER_GEOSET_ID::FACIALJEWELRY_DEFAULT;
    this->m_geosets[CHARGEOSET_EYEEFFECTS] = CHARACTER_GEOSET_ID::EYEEFFECTS_DEFAULT;
    this->m_geosets[CHARGEOSET_BELT] = CHARACTER_GEOSET_ID::BELT_DEFAULT;
}

// VERIFY: 5d26ed7d auto
// OFFSET: 0x004DFE90
void ComponentData::SetPreferences(const CharacterPreferences* preferences) {
    if (!preferences) {
        return;
    }

    this->m_preferences.m_raceID = preferences->m_raceID;
    this->m_preferences.m_sexID = preferences->m_sexID;
    this->m_preferences.m_classId = preferences->m_classId;
    this->m_preferences.m_hairColorId = preferences->m_hairColorId;
    this->m_preferences.m_skinID = preferences->m_skinID;
    this->m_preferences.m_faceId = preferences->m_faceId;
    this->m_preferences.m_facialHairId = preferences->m_facialHairId;
    this->m_preferences.m_hairStyleId = preferences->m_hairStyleId;
}
