#include "LFGListParty.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: d16f7f09 manual
// OFFSET: 0x00553490
LFGListPlayer* LFGListParty::GetMember(uint32_t index) {
    if (index + 1 >= this->m_members.Count()) {
        return nullptr;
    }

    return this->m_members.m_data[index + 1];
}

// VERIFY: 56468a50 manual
// OFFSET: 0x005534C0
uint32_t LFGListParty::GetNumMembers() {
    uint32_t count = this->m_members.Count();
    return count <= 1 ? 0 : count - 1;
}

// VERIFY: b8ecd60a manual
// OFFSET: 0x00556E60
void LFGListParty::AddPlayer(LFGListPlayer* player) {
    if (player->m_partyIndex >= 0) {
        return;
    }

    uint32_t count = this->m_members.Count();
    this->m_members.SetCount(count + 1);

    if (player->m_leader) {
        for (int32_t i = count; i > 0; i--) {
            this->m_members.m_data[i] = this->m_members.m_data[i - 1];
            this->m_members.m_data[i]->m_partyIndex = i;
        }

        this->m_members.m_data[0] = player;
        player->m_partyIndex = 0;
    } else {
        this->m_members.m_data[count] = player;
        player->m_partyIndex = count;
    }
}

// VERIFY: 493e9107 manual
// OFFSET: 0x00556EF0
void LFGListParty::RemovePlayer(LFGListPlayer* player) {
    int32_t index = player->m_partyIndex;

    if (index < 0) {
        return;
    }

    uint32_t count = this->m_members.Count();

    for (; static_cast<uint32_t>(index) < count - 1; index++) {
        this->m_members.m_data[index] = this->m_members.m_data[index + 1];
        this->m_members.m_data[index]->m_partyIndex = index;
    }

    this->m_members.SetCount(count - 1);
    player->m_partyIndex = -1;
}

// VERIFY: 2ee43711 auto
// OFFSET: 0x00558160
LFGListParty::LFGListParty(const LFGListParty& source) : m_index(-1) {
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->m_roles[0] = source.m_roles[0];
    this->m_roles[1] = source.m_roles[1];
    this->m_roles[2] = source.m_roles[2];
    this->unk_0128 = source.unk_0128;
    this->m_encounterMask = source.m_encounterMask;
}

// VERIFY: b695eb19 manual
// OFFSET: 0x00558200
LFGListPlayer::LFGListPlayer(const LFGListPlayer& source) : m_index(source.m_index), m_partyIndex(source.m_partyIndex), m_party(source.m_party), m_level(source.m_level), m_class(source.m_class), m_race(source.m_race), m_armor(source.m_armor), m_spellDamage(source.m_spellDamage), m_spellHeal(source.m_spellHeal), m_critMelee(source.m_critMelee), m_critRanged(source.m_critRanged), m_critSpell(source.m_critSpell), m_mp5(source.m_mp5), m_mp5Combat(source.m_mp5Combat), m_attackPower(source.m_attackPower), m_agility(source.m_agility), m_maxHealth(source.m_maxHealth), m_maxMana(source.m_maxMana), unk_0064(source.unk_0064), m_avgItemLevel(source.m_avgItemLevel), m_defenseSkill(source.m_defenseSkill), m_dodge(source.m_dodge), m_block(source.m_block), m_parry(source.m_parry), m_haste(source.m_haste), m_expertise(source.m_expertise), m_areaID(source.m_areaID), m_status(source.m_status), m_leader(source.m_leader), m_roles(source.m_roles), unk_0198(source.unk_0198), m_encounterMask(source.m_encounterMask) {
    this->m_talents[0] = source.m_talents[0];
    this->m_talents[1] = source.m_talents[1];
    this->m_talents[2] = source.m_talents[2];
    memcpy(this->m_comment, source.m_comment, sizeof(this->m_comment));
}
