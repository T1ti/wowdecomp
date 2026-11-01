#include "CGGameObject_C_Type_QuestGiver.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "Engine/Source/Model2/M2Model.h"

// VERIFY: 369810f6 auto
// OFFSET: 0x007125C0
CGGameObject_C_Type_QuestGiver::CGGameObject_C_Type_QuestGiver(CGGameObject_C* owner) : CGGameObject_C_TypeAnimated(owner) {
    this->m_interactDistance = MAX_SHOP_DISTANCE;
}

// VERIFY: 8178ce5d auto
// OFFSET: 0x0070FDE0
int32_t CGGameObject_C_Type_QuestGiver::CanChangeCursor() const {
    int32_t cursor = this->m_owner->GetQuestGiverCursor(!this->CanUseNow(nullptr, nullptr, nullptr));

    if (!cursor) {
        return CGGameObject_C_TypeBase::CanChangeCursor();
    }

    return cursor;
}

// VERIFY: 7cce95da auto
// OFFSET: 0x0070FF20
void CGGameObject_C_Type_QuestGiver::CloseInteraction() {
    uint32_t customAnimPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM);
    GameObjectStats* customAnimStats = this->m_owner->m_stats;
    uint32_t customAnim = (customAnimStats && customAnimPropertyIndex < GameObjectStats::NUM_PROP_VALUES) ? customAnimStats->m_propValue[customAnimPropertyIndex] : 0;

    if (customAnim) {
        return;
    }

    if (this->m_animState == 2 || this->m_animState == 3) {
        this->UpdateAnimState(4);
    }
}

// VERIFY: 86ad0112 manual
// OFFSET: 0x0070FE10
void CGGameObject_C_Type_QuestGiver::StartInteraction() {
    uint32_t customAnimPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM);
    GameObjectStats* customAnimStats = this->m_owner->m_stats;

    if (customAnimStats && customAnimPropertyIndex < GameObjectStats::NUM_PROP_VALUES && customAnimStats->m_propValue[customAnimPropertyIndex]) {
        uint32_t playAnimPropertyIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM);
        GameObjectStats* playAnimStats = this->m_owner->m_stats;
        this->m_owner->PlayCustomAnim(playAnimStats && playAnimPropertyIndex < GameObjectStats::NUM_PROP_VALUES ? playAnimStats->m_propValue[playAnimPropertyIndex] - 1 : 0xFFFFFFFF);
        return;
    }

    CM2Model* model = this->m_owner->m_worldModel;

    if (!model || !model->IsLoaded(0, 0)) {
        return;
    }

    uint32_t anim = CGGameObject_C_TypeAnimated::s_animSequenceIds[2];

    if (!model->HasSequence(anim)) {
        M2SequenceInfo info;
        info.Clear();
        model->GetSequenceInfo(anim, 0, info);
        anim = info.m_animId;
    }

    if (model->GetBoneSequenceId(0xFFFFFFFF) != anim) {
        this->UpdateAnimState(2);
    }
}
