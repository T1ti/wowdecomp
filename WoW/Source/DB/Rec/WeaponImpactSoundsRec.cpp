#include "WoW/Source/DB/Rec/WeaponImpactSoundsRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

WeaponImpactSoundsRec::WeaponImpactSoundsRec() {
}

WeaponImpactSoundsRec::~WeaponImpactSoundsRec() {
}

// VERIFY: c3998070 auto
// OFFSET: 0x008BB620
const char* WeaponImpactSoundsRec::GetFilename() {
    return "DBFilesClient\\WeaponImpactSounds.dbc";
}

// VERIFY: c4a4da4c manual
// OFFSET: 0x008BB630
bool WeaponImpactSoundsRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t weaponSubClassIDRead = SFileReadFile(file, &this->m_WeaponSubClassID, sizeof(this->m_WeaponSubClassID), nullptr, nullptr, 0);
    int32_t parrySoundTypeRead = SFileReadFile(file, &this->m_ParrySoundType, sizeof(this->m_ParrySoundType), nullptr, nullptr, 0);
    int32_t impactSoundIDRead = SFileReadFile(file, this->m_impactSoundID, sizeof(this->m_impactSoundID), nullptr, nullptr, 0);
    int32_t critImpactSoundIDRead = SFileReadFile(file, this->m_critImpactSoundID, sizeof(this->m_critImpactSoundID), nullptr, nullptr, 0);

    if (!idRead
        | !weaponSubClassIDRead
        | !parrySoundTypeRead
        | !impactSoundIDRead
        | !critImpactSoundIDRead) {
        return false;
    }

    return true;
}
