#ifndef WOW_SOURCE_UI_BATTLENETAPI_H
#define WOW_SOURCE_UI_BATTLENETAPI_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include "WoW/Source/Battlenet/GeneralFunction.h"

namespace BattlenetAPI {
uint16_t AcceptFriendInvite(uint32_t presenceId);
uint16_t AddCIDBlock(uint32_t presenceId);
uint16_t AddRIDBlock(uint32_t presenceId);
uint16_t CreateChatConversation(const Battlenet::Toon::FullName* first, const Battlenet::Toon::FullName* second, GeneralFunction::Callback* callback);
uint16_t DeclineFriendInvite(uint32_t presenceId);
uint32_t GetAccountPresenceId(uint32_t presenceId);
Battlenet::Presence::BlockList GetBlockList();
Battlenet::Chat::MemberList GetChatMemberList(uint8_t channel);
int32_t GetChatChannelType(uint8_t channel);
const char* GetErrorString(uint16_t error);
const char* GetExactNameForPresenceId(uint32_t presenceId, bool full);
uint32_t GetFocusedToon(uint32_t presenceId);
Battlenet::Friends::FriendInvite GetFriendInvite(uint32_t presenceId);
uint16_t GetFriendsOfFriend(uint32_t presenceId, GeneralFunction::Callback* callback);
const char* GetNameForPresenceId(uint32_t presenceId, bool full);
uint32_t GetNumChatMembers(uint8_t channel);
uint32_t GetNumToons(uint32_t presenceId);
uint32_t GetPresenceIDForCurrentAccount();
uint32_t GetPresenceIDForCurrentToon();
uint32_t GetPresenceIDForToon(const Battlenet::Toon::FullName* toon);
Battlenet::Presence::FieldVal GetPresenceValue(uint32_t presenceId, uint32_t field);
GeneralFunction::Arg GetSetting(const char* name);
uint32_t GetToon(uint32_t presenceId, uint32_t index);
const char* GetToonNameForPresenceId(uint32_t presenceId, bool full);
bool HasRIDFriendship(uint32_t presenceId);
bool IsCIDBlock(uint32_t presenceId);
bool IsCIDFriend(uint32_t presenceId);
bool IsRIDBlock(uint32_t presenceId);
bool IsRIDEnabled();
bool IsRIDFriend(uint32_t presenceId);
bool IsSelf(uint32_t presenceId);
uint16_t LeaveChatConversation(uint8_t channel);
uint32_t RegisterEvent(const char* name, GeneralFunction::Callback* callback);
uint32_t RegisterPresenceUpdateCallback(uint32_t presenceId, uint32_t field, GeneralFunction::Callback* callback);
uint16_t RemoveCIDBlock(uint32_t presenceId);
uint16_t RemoveFriend(uint32_t presenceId);
uint16_t RemoveRIDBlock(uint32_t presenceId);
uint16_t ReportFriendInvite(uint32_t presenceId);
// Names follow the BNReportPlayer reason strings.
enum REPORT_REASON : int32_t {
    REPORT_REASON_SPAM = 0,
    REPORT_REASON_ABUSE = 1,
    REPORT_REASON_THREAT = 2,
    REPORT_REASON_NAME = 3,
};

uint16_t ReportPlayer(uint32_t presenceId, int32_t reason, const char* note); // reason: REPORT_REASON
uint16_t SendChatConversationInvite(uint8_t channel, const Battlenet::Toon::FullName* toon, GeneralFunction::Callback* callback);
uint16_t SendChatMessage(uint8_t channel, const char* message);
uint16_t SendChatWhisper(uint32_t presenceId, const char* message);
uint16_t SendRIDFriendInviteByEmail(const char* email, const char* note, GeneralFunction::Callback* callback);
uint16_t SendRIDFriendInviteByPresenceId(uint32_t presenceId, const char* note, GeneralFunction::Callback* callback);
void SetAccountNameFormatString(const char* format);
uint16_t SetFriendNote(uint32_t presenceId, const char* note);
bool SetPresenceValue(uint32_t field, const Battlenet::Presence::FieldVal* value);
void SetSetting(const char* name, GeneralFunction::Arg value, bool notify);
bool UnregisterEvent(uint32_t handle);
bool UnregisterPresenceUpdateCallback(uint32_t handle);
}

#endif
