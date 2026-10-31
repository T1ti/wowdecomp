#ifndef WOW_SOURCE_UI_GUILDINFO_H
#define WOW_SOURCE_UI_GUILDINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

// SMSG_GUILD_COMMAND_RESULT result word, separate from guild command/event IDs.
// Names describe verified client actions; unknown results keep the silent default.
enum GUILD_COMMAND_RESULT : uint32_t {
    GUILD_RESULT_SUCCESS                                    = 0,
    GUILD_RESULT_INTERNAL                                   = 1,
    GUILD_RESULT_ALREADY_IN_GUILD                           = 2,
    GUILD_RESULT_PLAYER_ALREADY_IN_GUILD                    = 3,
    GUILD_RESULT_ALREADY_INVITED                            = 4,
    GUILD_RESULT_PLAYER_ALREADY_INVITED                     = 5,
    GUILD_RESULT_NAME_INVALID                               = 6,
    GUILD_RESULT_NAME_EXISTS                                = 7,
    // Quit command 3 reports leader-cannot-leave; others report permissions.
    GUILD_RESULT_LEADER_CANNOT_LEAVE_OR_NO_PERMISSION       = 8,
    GUILD_RESULT_NOT_IN_GUILD                               = 9,
    GUILD_RESULT_PLAYER_NOT_IN_GUILD                        = 10,
    GUILD_RESULT_PLAYER_NOT_FOUND                           = 11,
    GUILD_RESULT_NOT_ALLIED                                 = 12,
    GUILD_RESULT_RANK_TOO_HIGH                              = 13,
    GUILD_RESULT_RANK_TOO_LOW                               = 14,
    GUILD_RESULT_RANKS_LOCKED                               = 17,
    GUILD_RESULT_RANK_IN_USE                                = 18,
    GUILD_RESULT_IGNORING_YOU                               = 19,
    // No message; refreshes the roster only for command 5. Server cause unverified.
    GUILD_RESULT_REFRESH_ROSTER                             = 20,
    GUILD_RESULT_WITHDRAW_LIMIT                             = 25,
    GUILD_RESULT_NOT_ENOUGH_MONEY                           = 26,
    GUILD_RESULT_BANK_FULL                                  = 28,
    GUILD_RESULT_ITEM_NOT_FOUND                             = 29,
};

struct GuildRankInfo {
    /* 0000 */ uint32_t m_rights;
    /* 0004 */ int32_t m_bankMoneyPerDay;
    /* 0008 */ uint32_t m_bankTabRights[6];
    /* 0020 */ int32_t m_bankTabStacksPerDay[6];
};

struct GuildMemberInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name[48];
    /* 0038 */ int32_t m_level;
    /* 003C */ int32_t m_class;
    /* 0040 */ int32_t m_gender;
    /* 0044 */ int32_t m_zoneID;
    /* 0048 */ int32_t m_rank;
    /* 004C */ int32_t m_online;
    /* 0050 */ uint8_t m_status;
    /* 0051 */ uint8_t unk_0051;
    /* 0052 */ char m_note[128];
    /* 00D2 */ char m_officerNote[128];
    /* 0152 */ uint8_t unused_0152[2];
    /* 0154 */ float m_lastOnline;
};

struct GuildRosterSortCriteria {
    /* 0000 */ int32_t m_column;
    /* 0004 */ int32_t m_descending;
};

struct GuildEventLogEntry {
    /* 0000 */ uint32_t m_type;
    /* 0004 */ uint32_t unused_0004;
    /* 0008 */ WOWGUID m_player1;
    /* 0010 */ WOWGUID m_player2;
    /* 0018 */ uint32_t m_rank;
    /* 001C */ uint32_t m_time;
};

struct GuildRankFlag {
    /* 0000 */ uint32_t m_mask;
    /* 0004 */ int32_t m_enabled;
};

class CGGuildInfo {
    public:
    static uint32_t GetGuildBankTabPermissionsByRankID(uint8_t tab, uint32_t rank);
    static int32_t GetNumGuildBankTabItemWithdrawalsByRankID(uint8_t tab, uint32_t rank);
    static void SetMOTD(const char* motd);
    static void SaveToFile();
    static void QueryRosterAndRanks();
    static void InvalidatePlayer(const WOWGUID& guid);
    static char m_infoText[2004];
    static char m_motd[516];
    static GuildRosterSortCriteria m_sortCriteria[8];
    static uint32_t m_numMembers;
    static uint32_t m_numOnlineMembers;
    static int32_t m_showOffline;
    static WOWGUID m_selectedMember;
    static int32_t m_nextAllowedQuery;
    static int32_t m_pendingEventLogNames;
    static uint8_t m_saveToFile;
    static TSGrowableArray<GuildMemberInfo*> m_roster;
    static GuildRankInfo m_rankInfo[10];
    static int32_t m_numEventLogEntries;
    static uint32_t m_numRanks;
    static GuildEventLogEntry m_eventLog[100];
    static void SortRoster(int32_t column);
    static void EnterWorld();
    static void InitializeGame();
    static void ShutdownGame();
    static void QueryPermissions();
    static void SetShowOffline(int32_t show);
    static int32_t GetSelectedMemberIndex();
    static bool CanEditGuildEvent();
};

extern int32_t s_guildControlRank;

int32_t Packet_SMSG_GUILD_ROSTER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_PERMISSIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_EVENT_LOG_QUERY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t QSortRoster(const void* a, const void* b);
void GuildInfoCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void UpdateGuildRoster(int32_t a1);

int32_t Script_GuildRoster(lua_State* L);
void GuildInfoRegisterScriptFunctions();
void GuildInfoUnregisterScriptFunctions();

#endif
