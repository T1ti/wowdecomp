#include "LFGLIST.h"
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"

// VERIFY: 6e9c76a9 auto
// OFFSET: 0x005534E0
void LFGLIST::Reindex() {
    for (uint32_t i = 0; i < this->m_playerList.Count(); i++) {
        STORM_ASSERT(m_playerList[i]->indexPlayer >= 0);
        this->m_playerList.m_data[i]->indexPlayer = i;
    }

    for (uint32_t i = 0; i < this->m_partyList.Count(); i++) {
        STORM_ASSERT(m_partyList[i]->indexParty >= 0);
        this->m_partyList.m_data[i]->indexParty = i;
    }
}

// VERIFY: f92b1a5a manual
// OFFSET: 0x00556F90
void LFGLIST::RemovePlayer(LFGListPlayer* player) {
    int32_t index = player->indexPlayer;

    if (index >= 0) {
        uint32_t count = this->m_playerList.Count();
        STORM_ASSERT(static_cast<uint32_t>(player->indexPlayer) < count); // 12213: "player->indexPlayer < count"
        STORM_ASSERT(player == m_playerList[player->indexPlayer]);

        for (; static_cast<uint32_t>(index) < count - 1; index++) {
            this->m_playerList.m_data[index] = this->m_playerList.m_data[index + 1];
            this->m_playerList.m_data[index]->indexPlayer = index;
        }

        this->m_playerList.SetCount(count - 1);
        player->indexPlayer = -1;
    }

    if (player->m_party) {
        player->m_party->RemovePlayer(player);
    }
}

// VERIFY: bd6a3fa4 manual
// OFFSET: 0x00557030
void LFGLIST::RemoveParty(LFGListParty* party) {
    int32_t index = party->indexParty;

    if (index < 0) {
        return;
    }

    uint32_t count = this->m_partyList.Count();
    STORM_ASSERT(static_cast<uint32_t>(party->indexParty) < count); // 12213: "party->indexParty < count"
    STORM_ASSERT(party == m_partyList[party->indexParty]);

    for (; static_cast<uint32_t>(index) < count - 1; index++) {
        this->m_partyList.m_data[index] = this->m_partyList.m_data[index + 1];
        this->m_partyList.m_data[index]->indexParty = index;
    }

    this->m_partyList.SetCount(count - 1);
    party->indexParty = -1;
}

// VERIFY: f402cb87 manual
// OFFSET: 0x005589D0
void LFGLIST::DeleteParty(LFGListParty* party) {
    for (int32_t i = party->playerList.Count() - 1; i >= 0; i--) {
        LFGListPlayer* player = party->playerList.m_data[i];
        this->RemovePlayer(player);
        delete player;
    }

    this->RemoveParty(party);
    delete party;
}

// VERIFY: b3ae5f7f manual
// OFFSET: 0x00558AF0
void LFGLIST::Clear() {
    LFGListPlayer* player = this->m_players.m_fulllist.Head();

    while (player) {
        LFGListPlayer* next = this->m_players.m_fulllist.Next(player);
        g_nameDBCache.CancelCallback(player->m_key.m_guid.m_guid, LFGNameCacheCallback, nullptr);
        delete player;
        player = next;
    }

    this->m_playerList.m_count = 0;
    this->m_players.Clear();

    LFGListParty* party = this->m_parties.m_fulllist.Head();

    while (party) {
        LFGListParty* next = this->m_parties.m_fulllist.Next(party);
        delete party;
        party = next;
    }

    this->m_partyList.m_count = 0;
    this->m_lfgID = 0;
    this->m_time = 0;
    this->m_totalPlayers = 0;
    this->m_totalParties = 0;
    this->unused_0010 = 0;
    this->m_pendingNames = 0;
}

// VERIFY: e64be9d0 manual
// OFFSET: 0x0055AA40
LFGLIST::~LFGLIST() {
    for (int32_t i = this->m_partyList.Count() - 1; i >= 0; i--) {
        this->DeleteParty(this->m_partyList.m_data[i]);
    }

    for (int32_t i = this->m_playerList.Count() - 1; i >= 0; i--) {
        LFGListPlayer* player = this->m_playerList.m_data[i];
        this->RemovePlayer(player);
        delete player;
    }
}

// VERIFY: dc50019e manual
// OFFSET: 0x0055B1A0
bool LFGLIST::Copy(const LFGLIST& otherList) {
    STORM_ASSERT(&otherList != this);

    this->Clear();
    this->m_lfgID = otherList.m_lfgID;
    this->m_time = otherList.m_time;
    this->m_totalPlayers = otherList.m_totalPlayers;
    this->m_totalParties = otherList.m_totalParties;
    this->unused_0010 = otherList.unused_0010;
    this->m_pendingNames = otherList.m_pendingNames;

    for (LFGListPlayer* player = otherList.m_players.m_fulllist.Head(); player; player = otherList.m_players.m_fulllist.Next(player)) {
        uint64_t guid = player->m_key.m_guid.m_guid;
        LFGListPlayer* copy = new (__FILE__, __LINE__) LFGListPlayer(*player);
        copy->indexPlayer = -1;
        copy->indexParty = -1;
        copy->m_party = nullptr;
        this->m_players.Insert(copy, static_cast<uint32_t>(guid), CHashKeyGUID(guid));
    }

    for (uint32_t i = 0; i < otherList.m_playerList.Count(); i++) {
        uint64_t guid = otherList.m_playerList.m_data[i]->m_key.m_guid.m_guid;
        LFGListPlayer* player = this->m_players.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (player->m_party) {
            player->m_party->AddPlayer(player);
        } else if (player->indexPlayer < 0) {
            this->AddPlayer(player);
        }
    }

    for (LFGListParty* party = otherList.m_parties.m_fulllist.Head(); party; party = otherList.m_parties.m_fulllist.Next(party)) {
        uint64_t guid = party->m_key.m_guid.m_guid;
        LFGListParty* copy = new (__FILE__, __LINE__) LFGListParty(*party);
        copy->indexParty = -1;

        for (uint32_t j = 0; j < party->playerList.Count(); j++) {
            uint64_t memberGuid = party->playerList.m_data[j]->m_key.m_guid.m_guid;
            LFGListPlayer* player = this->m_players.Ptr(static_cast<uint32_t>(memberGuid), CHashKeyGUID(memberGuid));
            player->m_party = copy;
            copy->AddPlayer(player);
        }

        this->m_parties.Insert(copy, static_cast<uint32_t>(guid), CHashKeyGUID(guid));
    }

    for (uint32_t i = 0; i < otherList.m_partyList.Count(); i++) {
        uint64_t guid = otherList.m_partyList.m_data[i]->m_key.m_guid.m_guid;
        LFGListParty* party = this->m_parties.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (party->indexParty < 0) {
            this->AddParty(party);
        }
    }

    this->Reindex();

    return true;
}
