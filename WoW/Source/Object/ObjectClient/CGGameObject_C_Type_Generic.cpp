#include "CGGameObject_C_Type_Generic.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"

// VERIFY: 9d36ee11 auto
// OFFSET: 0x007125F0
CGGameObject_C_Type_Generic::CGGameObject_C_Type_Generic(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// VERIFY: b7241514 auto
// OFFSET: 0x0070FF80
bool CGGameObject_C_Type_Generic::CanHighlight() const {
    uint32_t highlightPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_HIGHLIGHT);
    GameObjectStats* highlightStats = this->m_owner->m_stats;
    uint32_t highlight = (highlightStats && highlightPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? highlightStats->m_propValue[highlightPropertyIndex] : 0;

    return highlight != 0;
}

// VERIFY: ebc1408d auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_Generic::CanUse() const {
    return false;
}
