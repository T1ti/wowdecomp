#ifndef WOW_SOURCE_UI_LFGLIST_H
#define WOW_SOURCE_UI_LFGLIST_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Ui/LFGListParty.h"

class LFGLIST {
    public:
    /* 0000 */ uint32_t m_lfgID;
    /* 0004 */ uint32_t m_time;
    /* 0008 */ uint32_t m_totalPlayers;
    /* 000C */ uint32_t m_totalParties;
    /* 0010 */ uint8_t unused_0010;
    /* 0014 */ uint32_t m_pendingNames;
    /* 0018 */ TSHashTable<LFGListParty, CHashKeyGUID> m_parties;
    /* 0040 */ TSHashTable<LFGListPlayer, CHashKeyGUID> m_players;
    private:
    friend class CGLookingForGroup;

    /* 0068 */ TSGrowableArray<LFGListParty*> m_partyList;
    /* 0078 */ TSGrowableArray<LFGListPlayer*> m_playerList;

    void RemoveParty(LFGListParty* party);

    public:
    uint32_t GetPlayerCount() const { return this->m_playerList.Count(); } // (inferred)
    uint32_t GetPartyCount() const { return this->m_partyList.Count(); } // (inferred)
    LFGListPlayer* GetPlayer(uint32_t index) const { return this->m_playerList.m_data[index]; } // (inferred)
    LFGListParty* GetParty(uint32_t index) const { return this->m_partyList.m_data[index]; } // (inferred)

    void AddPlayer(LFGListPlayer* player) { // (inferred)
        const uint32_t index = this->m_playerList.Count();
        this->m_playerList.SetCount(index + 1);
        this->m_playerList.m_data[index] = player;
        player->indexPlayer = index;
    }

    void AddParty(LFGListParty* party) { // (inferred)
        const uint32_t index = this->m_partyList.Count();
        this->m_partyList.SetCount(index + 1);
        this->m_partyList.m_data[index] = party;
        party->indexParty = index;
    }

    ~LFGLIST();
    void Clear();
    bool Copy(const LFGLIST& otherList);
    void DeleteParty(LFGListParty* party);
    void Reindex();
    void RemovePlayer(LFGListPlayer* player);
};

void LFGNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
