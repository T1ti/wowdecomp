#ifndef WOW_SOURCE_UI_GOSSIPINFO_H
#define WOW_SOURCE_UI_GOSSIPINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct GossipOption {
    /* 0000 */ char* m_text;
    /* 0004 */ char* m_boxText;
    /* 0008 */ int32_t m_index;
    /* 000C */ uint8_t m_coded;
    /* 0010 */ uint32_t m_boxMoney;
    /* 0014 */ int32_t m_type;
};

struct GossipQuestInfo {
    /* 0000 */ uint32_t m_questID;
    /* 0004 */ int32_t m_level;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint32_t m_repeatable;
    /* 0010 */ uint32_t m_icon;
    /* 0014 */ char m_title[512];
};

class CGGossipInfo {
    public:
    static int32_t GetNumAvailableQuests();
    static GossipQuestInfo* GetAvailableQuest(int32_t index);
    static int32_t GetNumActiveQuests();
    static GossipQuestInfo* GetActiveQuest(int32_t index);
    static void SelectGossipOption(int32_t index, const char* code, int32_t confirmed);
    static void SelectGossipAvailableQuest(int32_t index);
    static void SelectGossipActiveQuest(int32_t index);
    static GossipQuestInfo s_quests[32];
    static int32_t s_textPending;
    static void ClearOptions();
    static void SetGossipText(int32_t textId);
    static uint32_t s_gossipID;
    static void SetGossip(const WOWGUID& guid);
    static GossipOption s_options[32];
    static char s_gossipText[2048];
    static void InitializeGame();
    static void ShutdownGame();
    static void LeaveWorld();
    static WOWGUID s_gossip;
    static uint32_t s_gossipOptionsCount;
};

void GossipInfoRegisterScriptFunctions();
void GossipInfoUnregisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_GOSSIP_MESSAGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GOSSIP_POI(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
