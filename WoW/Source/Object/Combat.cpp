#include "Combat.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"

// VERIFY: 596a8ec9 auto
// OFFSET: 0x0098E520
void CCombat::SetAttacking(WOWGUID victim) {
    STORM_ASSERT(victim);

    this->m_attacking = victim;
}

// VERIFY: 1fe6be8f auto
// OFFSET: 0x0098E540
void CCombatClient::SetAttackSent(WOWGUID guid) {
    this->m_attacking = guid;
    this->m_attackSent = 1;
}

// VERIFY: f8fdc1f7 auto
// OFFSET: 0x0098E560
void CCombat::SetClientInitData(const CClientObjCreate& create) {
    this->m_attacking = create.m_attackTarget;
}
