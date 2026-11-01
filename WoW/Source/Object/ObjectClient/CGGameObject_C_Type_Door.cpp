#include "WoW/Source/Ui/Types.h"
#include "CGGameObject_C_Type_Door.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"

// VERIFY: c0423e81 auto
// OFFSET: 0x00712520
CGGameObject_C_Type_Door::CGGameObject_C_Type_Door(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
    this->m_useErrorType = ERR_DOOR_LOCKED;
}

// VERIFY: cfff901d auto
// OFFSET: 0x00712550
bool CGGameObject_C_Type_Door::CanBeSolid() const {
    return this->m_animState == 1;
}

// VERIFY: e34ee88e auto
// OFFSET: 0x0070D8D0
void CGGameObject_C_Type_Door::UpdateAnimState(uint32_t state) {
    CGGameObject_C_TypeAnimated::UpdateAnimState(state);
    this->m_owner->m_hasWorldBounds = this->m_animState == 1;
}

// VERIFY: c1cac2f1 auto
// OFFSET: 0x0070FD50
bool CGGameObject_C_Type_Door::CheckPropState() const {
    uint32_t autoClosePropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE);
    GameObjectStats* autoCloseStats = this->m_owner->m_stats;
    uint32_t autoClose = (autoCloseStats && autoClosePropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? autoCloseStats->m_propValue[autoClosePropertyIndex] : 0;

    if (!autoClose) {
        return true;
    }

    uint32_t startOpenPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN);
    GameObjectStats* startOpenStats = this->m_owner->m_stats;
    uint32_t startOpen = (startOpenStats && startOpenPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? startOpenStats->m_propValue[startOpenPropertyIndex] : 0;

    if (startOpen) {
        return this->m_animState != 1;
    }

    return this->m_animState != 3;
}

// VERIFY: 4dfa3075 manual
// OFFSET: 0x007119D0
bool CGGameObject_C_Type_Door::CanUseNow(GAME_ERROR_TYPE* error, float* distance) const {
    if (this->CheckPropState()) {
        return CGGameObject_C_TypeBase::CanUseNow(error, distance, nullptr);
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(ERR_USE_OBJECT_MOVING);
    }

    return false;
}
