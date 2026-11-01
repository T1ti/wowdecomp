#include "CGGameObject_C_Type_CapturePoint.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Ui/GameUI.h"

// VERIFY: 09194bd4 auto
// OFFSET: 0x007127E0
CGGameObject_C_Type_CapturePoint::CGGameObject_C_Type_CapturePoint(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
}

// VERIFY: 19850204 auto
// OFFSET: 0x0070FF80
bool CGGameObject_C_Type_CapturePoint::CanHighlight() const {
    uint32_t highlightPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_HIGHLIGHT);
    GameObjectStats* highlightStats = this->m_owner->m_stats;
    uint32_t highlight = (highlightStats && highlightPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? highlightStats->m_propValue[highlightPropertyIndex] : 0;

    return highlight != 0;
}

// VERIFY: c0c21cab auto
// OFFSET: 0x00427A90
bool CGGameObject_C_Type_CapturePoint::CanUse() const {
    return false;
}

// VERIFY: 9740d5a5 auto
// OFFSET: 0x00710940
void CGGameObject_C_Type_CapturePoint::PostInitWithStats() {
    uint32_t radiusPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_RADIUS);
    GameObjectStats* radiusStats = this->m_owner->m_stats;
    uint32_t radius = (radiusStats && radiusPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? radiusStats->m_propValue[radiusPropertyIndex] : 0;
    uint32_t worldStatePropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE1);
    GameObjectStats* worldStateStats = this->m_owner->m_stats;
    uint32_t worldState = (worldStateStats && worldStatePropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? worldStateStats->m_propValue[worldStatePropertyIndex] : 0;

    g_capturePointUIManager.AddCapturePoint(this->m_owner, worldState, radius);
}

// VERIFY: f1c33a5b auto
// OFFSET: 0x00710940
void CGGameObject_C_Type_CapturePoint::Reenable() {
    uint32_t radiusPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_RADIUS);
    GameObjectStats* radiusStats = this->m_owner->m_stats;
    uint32_t radius = (radiusStats && radiusPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? radiusStats->m_propValue[radiusPropertyIndex] : 0;
    uint32_t worldStatePropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE1);
    GameObjectStats* worldStateStats = this->m_owner->m_stats;
    uint32_t worldState = (worldStateStats && worldStatePropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? worldStateStats->m_propValue[worldStatePropertyIndex] : 0;

    g_capturePointUIManager.AddCapturePoint(this->m_owner, worldState, radius);
}

// VERIFY: 9bb61b0e auto
// OFFSET: 0x0070CA30
void CGGameObject_C_Type_CapturePoint::Disable() {
    g_capturePointUIManager.RemoveCapturePoint(this->m_owner);
}
