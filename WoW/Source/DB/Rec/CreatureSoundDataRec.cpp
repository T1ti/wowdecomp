#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

CreatureSoundDataRec::CreatureSoundDataRec() {
}

CreatureSoundDataRec::~CreatureSoundDataRec() {
}

// VERIFY: 762752b4 auto
// OFFSET: 0x008ADBB0
const char* CreatureSoundDataRec::GetFilename() {
    return "DBFilesClient\\CreatureSoundData.dbc";
}

// VERIFY: cce59143 auto
// OFFSET: 0x008ADBC0
bool CreatureSoundDataRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t soundExertionIDRead = SFileReadFile(file, &this->m_soundExertionID, sizeof(this->m_soundExertionID), nullptr, nullptr, 0);
    int32_t soundExertionCriticalIDRead = SFileReadFile(file, &this->m_soundExertionCriticalID, sizeof(this->m_soundExertionCriticalID), nullptr, nullptr, 0);
    int32_t soundInjuryIDRead = SFileReadFile(file, &this->m_soundInjuryID, sizeof(this->m_soundInjuryID), nullptr, nullptr, 0);
    int32_t soundInjuryCriticalIDRead = SFileReadFile(file, &this->m_soundInjuryCriticalID, sizeof(this->m_soundInjuryCriticalID), nullptr, nullptr, 0);
    int32_t soundInjuryCrushingBlowIDRead = SFileReadFile(file, &this->m_soundInjuryCrushingBlowID, sizeof(this->m_soundInjuryCrushingBlowID), nullptr, nullptr, 0);
    int32_t soundDeathIDRead = SFileReadFile(file, &this->m_soundDeathID, sizeof(this->m_soundDeathID), nullptr, nullptr, 0);
    int32_t soundStunIDRead = SFileReadFile(file, &this->m_soundStunID, sizeof(this->m_soundStunID), nullptr, nullptr, 0);
    int32_t soundStandIDRead = SFileReadFile(file, &this->m_soundStandID, sizeof(this->m_soundStandID), nullptr, nullptr, 0);
    int32_t soundFootstepIDRead = SFileReadFile(file, &this->m_soundFootstepID, sizeof(this->m_soundFootstepID), nullptr, nullptr, 0);
    int32_t soundAggroIDRead = SFileReadFile(file, &this->m_soundAggroID, sizeof(this->m_soundAggroID), nullptr, nullptr, 0);
    int32_t soundWingFlapIDRead = SFileReadFile(file, &this->m_soundWingFlapID, sizeof(this->m_soundWingFlapID), nullptr, nullptr, 0);
    int32_t soundWingGlideIDRead = SFileReadFile(file, &this->m_soundWingGlideID, sizeof(this->m_soundWingGlideID), nullptr, nullptr, 0);
    int32_t soundAlertIDRead = SFileReadFile(file, &this->m_soundAlertID, sizeof(this->m_soundAlertID), nullptr, nullptr, 0);
    int32_t soundFidgetRead = SFileReadFile(file, this->m_soundFidget, sizeof(this->m_soundFidget), nullptr, nullptr, 0);
    int32_t customAttackRead = SFileReadFile(file, this->m_customAttack, sizeof(this->m_customAttack), nullptr, nullptr, 0);
    int32_t npcsoundIDRead = SFileReadFile(file, &this->m_npcsoundID, sizeof(this->m_npcsoundID), nullptr, nullptr, 0);
    int32_t loopSoundIDRead = SFileReadFile(file, &this->m_loopSoundID, sizeof(this->m_loopSoundID), nullptr, nullptr, 0);
    int32_t creatureImpactTypeRead = SFileReadFile(file, &this->m_creatureImpactType, sizeof(this->m_creatureImpactType), nullptr, nullptr, 0);
    int32_t soundJumpStartIDRead = SFileReadFile(file, &this->m_soundJumpStartID, sizeof(this->m_soundJumpStartID), nullptr, nullptr, 0);
    int32_t soundJumpEndIDRead = SFileReadFile(file, &this->m_soundJumpEndID, sizeof(this->m_soundJumpEndID), nullptr, nullptr, 0);
    int32_t soundPetAttackIDRead = SFileReadFile(file, &this->m_soundPetAttackID, sizeof(this->m_soundPetAttackID), nullptr, nullptr, 0);
    int32_t soundPetOrderIDRead = SFileReadFile(file, &this->m_soundPetOrderID, sizeof(this->m_soundPetOrderID), nullptr, nullptr, 0);
    int32_t soundPetDismissIDRead = SFileReadFile(file, &this->m_soundPetDismissID, sizeof(this->m_soundPetDismissID), nullptr, nullptr, 0);
    int32_t fidgetDelaySecondsMinRead = SFileReadFile(file, &this->m_fidgetDelaySeconds.m_l, sizeof(this->m_fidgetDelaySeconds.m_l), nullptr, nullptr, 0);
    int32_t fidgetDelaySecondsMaxRead = SFileReadFile(file, &this->m_fidgetDelaySeconds.m_h, sizeof(this->m_fidgetDelaySeconds.m_h), nullptr, nullptr, 0);
    int32_t birthSoundIDRead = SFileReadFile(file, &this->m_birthSoundID, sizeof(this->m_birthSoundID), nullptr, nullptr, 0);
    int32_t spellCastDirectedSoundIDRead = SFileReadFile(file, &this->m_spellCastDirectedSoundID, sizeof(this->m_spellCastDirectedSoundID), nullptr, nullptr, 0);
    int32_t submergeSoundIDRead = SFileReadFile(file, &this->m_submergeSoundID, sizeof(this->m_submergeSoundID), nullptr, nullptr, 0);
    int32_t submergedSoundIDRead = SFileReadFile(file, &this->m_submergedSoundID, sizeof(this->m_submergedSoundID), nullptr, nullptr, 0);
    int32_t creatureSoundDataIdpetRead = SFileReadFile(file, &this->m_creatureSoundDataIdpet, sizeof(this->m_creatureSoundDataIdpet), nullptr, nullptr, 0);

    if (!idRead
        | !soundExertionIDRead
        | !soundExertionCriticalIDRead
        | !soundInjuryIDRead
        | !soundInjuryCriticalIDRead
        | !soundInjuryCrushingBlowIDRead
        | !soundDeathIDRead
        | !soundStunIDRead
        | !soundStandIDRead
        | !soundFootstepIDRead
        | !soundAggroIDRead
        | !soundWingFlapIDRead
        | !soundWingGlideIDRead
        | !soundAlertIDRead
        | !soundFidgetRead
        | !customAttackRead
        | !npcsoundIDRead
        | !loopSoundIDRead
        | !creatureImpactTypeRead
        | !soundJumpStartIDRead
        | !soundJumpEndIDRead
        | !soundPetAttackIDRead
        | !soundPetOrderIDRead
        | !soundPetDismissIDRead
        | !fidgetDelaySecondsMinRead
        | !fidgetDelaySecondsMaxRead
        | !birthSoundIDRead
        | !spellCastDirectedSoundIDRead
        | !submergeSoundIDRead
        | !submergedSoundIDRead
        | !creatureSoundDataIdpetRead) {
        return false;
    }

    return true;
}
