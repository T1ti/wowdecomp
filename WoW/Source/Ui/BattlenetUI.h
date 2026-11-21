#ifndef WOW_SOURCE_UI_BATTLENETUI_H
#define WOW_SOURCE_UI_BATTLENETUI_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include "WoW/Source/Battlenet/GeneralFunction.h"
#include "WoW/Source/Ui/BattlenetAPI.h"
#include "WoW/Source/Ui/Types.h"

struct lua_State;

struct BattlenetUIFriend {
    /* 0000 */ uint32_t m_presenceID;
    /* 0004 */ uint32_t m_program;
    /* 0008 */ BSN::String<72> m_name;
    /* 0134 */ BSN::String<40> m_givenName;
    /* 01E0 */ BSN::String<40> m_surname;
    /* 028C */ BSN::String<40> m_sortSurname;
};

// Battle.net friend invite results with a dedicated UI error (inferred names after that error).
enum BN_FRIEND_INVITE_RESULT : uint16_t {
    BN_FRIEND_INVITE_ALREADY = 604,
    BN_FRIEND_INVITE_SELF = 617,
    BN_FRIEND_INVITE_BLOCKED = 623,
};

// Battle.net error codes the client itself raises. Their text comes from the Battle.net module
// (BattlenetAPI_GetErrorString), so the meaning is not named in the client.
enum BN_ERROR : uint16_t {
    // Raised by BNSendWhisper when the target has no toon name; DisplayError keeps it out of the
    // UI error frame.
    BN_ERROR_UNKNOWN_318 = 318,
};

// BattlenetUI::SystemMessageCB message IDs, named after the BN_SYSTEM_MESSAGE type strings.
enum BN_SYSTEM_MESSAGE_ID : uint32_t {
    BN_SYSTEM_MESSAGE_SHUTDOWN_MINUTES = 3800,
    BN_SYSTEM_MESSAGE_SHUTDOWN_SECONDS = 3801,
    BN_SYSTEM_MESSAGE_SHUTDOWN_NOW = 3802,
};

class BattlenetUI {
    public:
    // Battle.net conversation channel slots (inferred name).
    static const int32_t MAX_CHAT_CHANNELS = 6;

    // BattlenetEvent_ChannelJoined channel type. CONVERSATION is named from the
    // YOU_JOINED_CONVERSATION notice; the other values are unknown.
    enum CHAT_CHANNEL_TYPE : int32_t {
        CHAT_CHANNEL_TYPE_UNKNOWN_0 = 0,
        CHAT_CHANNEL_TYPE_UNKNOWN_1 = 1,
        CHAT_CHANNEL_TYPE_CONVERSATION = 2,
    };

    struct EventCallback {
        /* 0000 */ const char* m_event;
        /* 0004 */ GeneralFunction::SimpleCallbackFunc m_func;
        /* 0008 */ uint32_t m_handle;
    };

    struct PresenceCallback {
        /* 0000 */ uint32_t m_field;
        /* 0004 */ GeneralFunction::SimpleCallbackFunc m_func;
        /* 0008 */ uint32_t m_handle;
    };

    class FriendInviteSentCB : public GeneralFunction::Callback {
        public:
        void Call(int32_t argc, const GeneralFunction::Arg* args) const override;
        void Destroy() const override;
    };

    static uint32_t s_selfRealm;
    static bool s_initialized;
    static bool m_friendListLoaded;
    static BSN::BSNEncodedObject<Battlenet::Friends::FriendInviteContainer, Protocol>* m_friendInvites;
    static BSN::BSNEncodedObject<Battlenet::Friends::FriendsOfFriendList, Protocol>* s_friendsOfFriend;
    static GeneralFunction::Callback* s_createConversationCB;
    static GeneralFunction::Callback* s_inviteToConversationCB;
    static GeneralFunction::Callback* s_friendsOfFriendCB;
    static uint32_t s_selectedFriend;
    static uint32_t s_selectedBlock;
    static uint32_t s_selectedToonBlock;
    static BSN::Array<BattlenetUIFriend, 100> m_friends;
    static EventCallback s_eventCallbacks[26];
    static PresenceCallback s_presenceCallbacks[NUM_BN_PRESENCE_FIELDS];
    static FriendInviteSentCB s_friendInviteSentCB;
    static int32_t s_broadcastCount;
    static char s_programString[16];
    static float s_broadcastTime;
    static char s_displayName[72];

    static bool AddToFriendList(uint32_t presenceId);
    static bool BNFeaturesEnabled();
    static int32_t BNGetFriendInfoHelper(lua_State* L, uint32_t index);
    static int32_t BNGetToonInfoHelper(lua_State* L, uint32_t presenceId);
    static void BlockAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void BlockListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void BlockRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelClosedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelCreateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelInviteCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberUpdatedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageBlockedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperSentCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void CustomMessageChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void DisambiguateFriendList();
    static void DisplayError(uint16_t error, uint32_t presenceId);
    static void FactionUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static BattlenetUIFriend* FindFriend(uint32_t presenceId);
    static int32_t FindFriendIndex(uint32_t presenceId);
    static void FriendAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInfoChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendsOfFriendCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static const char* GetNameForPresenceIdHelper(uint32_t presenceId);
    static uint32_t GetSelfRealmID();
    static void IgnoredFieldCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void Init();
    static void LastOnlineChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void NewPresenceCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void OnlinePresenceChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void OnlineTimeChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void PopulateAutoComplete();
    static int32_t QSortDisambiguateFriendListCB(const void* a, const void* b);
    static int32_t QSortFriendInviteListByTimeCB(const void* a, const void* b);
    static int32_t QSortFriendListCB(const void* a, const void* b);
    static void SelfToonOfflineCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SelfToonOnlineCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SendScriptEvent(uint32_t event, const char* format, ...);
    static void SetBattlenetConnectionRecovered();
    static void SetDisconnectedFromBattlenet(bool disconnected);
    static void SettingChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SortFriendInviteList();
    static void SystemMessageCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ToonNameUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void Uninit();
    static int32_t UpdateAutoComplete(uint32_t& presenceId, uint32_t flags, int32_t add, const char* name);
};

char* FilterChatLinks(const char* source, char* dest, uint32_t size);
bool BattlenetUI_IsAvailable();
void BattlenetUI_EnterRealm();
void BattlenetUI_LeaveRealm();
void BattlenetUI_RegisterScriptFunctions();
void BattlenetUI_UnregisterScriptFunctions();

#endif
