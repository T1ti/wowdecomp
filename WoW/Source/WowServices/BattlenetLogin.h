#ifndef WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H
#define WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/Battlenet/Battlenet.h"
#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowServices/WowConnection.h"

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

namespace Battlenet {
class TumorManager;
}

class BattlenetApplication;

extern Battlenet::TumorManager* g_tumorManager;

class BattlenetAllocator : public Battlenet::Allocator {
    public:
    void* Alloc(uint32_t size) override;
    void Free(void* memory) override;
};

extern BattlenetAllocator s_battlenetAllocator;

class BattlenetLogin : public Login {
    public:
    // Descriptive client results, separate from SERVICE_STATE and LOGIN_* responses.
    // UI_* names describe unique display outcomes; coalesced server meanings remain UNKNOWN.
    // 138 is suppressed by HandleInvoke, without a proved original error identity.
    enum LOGON_RESULT : int32_t {
        LOGON_RESULT_SUCCESS = 0,
        LOGON_RESULT_LOCAL_CONNECTION_FAILURE = 2,
        LOGON_RESULT_UI_BAD_SERVER_PROOF = 103,
        LOGON_RESULT_UNKNOWN_104 = 104,
        LOGON_RESULT_UNKNOWN_105 = 105,
        LOGON_RESULT_UNKNOWN_106 = 106,
        LOGON_RESULT_UI_NO_GAME_ACCOUNT = 107,
        LOGON_RESULT_UNKNOWN_108 = 108,
        LOGON_RESULT_UNKNOWN_109 = 109,
        LOGON_RESULT_UNKNOWN_110 = 110,
        LOGON_RESULT_UNKNOWN_111 = 111,
        LOGON_RESULT_UNKNOWN_112 = 112,
        LOGON_RESULT_UNKNOWN_113 = 113,
        LOGON_RESULT_UNKNOWN_115 = 115,
        LOGON_RESULT_UNKNOWN_116 = 116,
        LOGON_RESULT_UI_TOO_FAST = 123,
        LOGON_RESULT_UI_USE_GRUNT = 124,
        LOGON_RESULT_UNKNOWN_138 = 138,
        LOGON_RESULT_UI_NO_GAME_ACCOUNTS_IN_REGION = 140,
        LOGON_RESULT_UI_ACCOUNT_LOCKED = 141,
        LOGON_RESULT_UNKNOWN_200 = 200,
        LOGON_RESULT_UNKNOWN_201 = 201,
        LOGON_RESULT_UI_BANNED = 202,
        LOGON_RESULT_UI_SUSPENDED = 203,
        LOGON_RESULT_UNKNOWN_204 = 204,
        LOGON_RESULT_UNKNOWN_205 = 205,
        LOGON_RESULT_UI_NO_TIME = 206,
        LOGON_RESULT_UNKNOWN_207 = 207,
        LOGON_RESULT_UNKNOWN_208 = 208,
        LOGON_RESULT_UI_PARENTAL_CONTROL = 209,
        LOGON_RESULT_UI_TRIAL_EXPIRED = 210,
        LOGON_RESULT_UI_ANTI_INDULGENCE = 211,
        LOGON_RESULT_UNKNOWN_212 = 212,
    };

    enum SERVICE_STATE : int32_t {
        SERVICE_STATE_ERROR = 0,
        SERVICE_STATE_WARNING = 1,
        SERVICE_STATE_LOST = 2,
        SERVICE_STATE_FLOODING_ERROR = 3,
        SERVICE_STATE_DISCONNECTED = 4,
        SERVICE_STATE_INTERRUPTED = 5,
        SERVICE_STATE_RECOVERED = 6,
        SERVICE_STATE_ESTABLISHED = 7,
    };

    struct BN_EVENT : public TSLinkedNode<BattlenetLogin::BN_EVENT> {
        /* 0008 */ BN_EVENT_ID m_id;
        /* 000C */ void* m_data;
        /* 0010 */ int32_t m_size;
    };

    struct BN_REALM : public TSLinkedNode<BattlenetLogin::BN_REALM> {
        /* 0008 */ REALM_INFO m_info;
    };

    struct GameAccount {
        /* 0000 */ uint32_t m_region;
        /* 0004 */ char m_name[256];
    };

    struct PatchInstruction {
        /* 0000 */ char m_patch[255];
    };

    struct RealmCharacterCount {
        /* 0000 */ uint32_t m_region;
        /* 0004 */ uint32_t m_battlegroup;
        /* 0008 */ uint32_t m_id;
        /* 000C */ uint32_t m_count;
    };

    static bool s_monolithicComponent;
    static char s_monolithicPlatform[5];

    static void LogConnectionStatus(const char* format, ...);

    /* 10F0 */ BattlenetApplication* m_application;
    /* 10F4 */ uint32_t m_moduleId;
    /* 10F8 */ int32_t m_logonResult;
    /* 10FC */ RealmCharacterCount* m_realmCharacterCounts;
    /* 1100 */ uint32_t m_realmCharacterCountCount;
    /* 1104 */ TSList<BattlenetLogin::BN_REALM> m_realms;
    /* 1110 */ bool m_realmListRequested;
    /* 1111 */ uint8_t unused_1111[3];
    /* 1114 */ uint32_t m_joinRealmId;
    /* 1118 */ SCritSect m_realmLock;
    /* 1140 */ TSGrowableArray<BattlenetLogin::GameAccount> m_gameAccounts;
    /* 1150 */ SCritSect m_gameAccountLock;
    /* 1178 */ uint32_t m_gameAccountSelected;
    /* 117C */ TSGrowableArray<BattlenetLogin::PatchInstruction> m_patchInstructions;
    /* 118C */ SCritSect m_patchInstructionLock;
    /* 11B4 */ uint32_t unused_11B4;
    /* 11B8 */ uint32_t m_accountFlags;
    /* 11BC */ uint32_t m_accountFlags2;
    /* 11C0 */ bool m_surveyPending;
    /* 11C1 */ uint8_t unused_11C1[3];
    /* 11C4 */ uint32_t m_surveyTime;
    /* 11C8 */ TSList<BattlenetLogin::BN_EVENT> m_events [2];
    /* 11E0 */ SCritSect m_eventLock;
    /* 1208 */ uint32_t m_eventIndex;
    /* 120C */ int32_t m_serviceState;

    bool Connected(const NETADDR& address) override;
    bool Disconnected() override;
    bool CantConnect() override;
    void GetLogonMethod() override;
    void GetVersionProof(const uint8_t* versionChallenge) override;
    void SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt) override;
    void GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash) override;
    void SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength) override;
    void GetMatrixProof(bool* enabled, const uint8_t** proof) const override;
    void SetTokenInfo(bool enabled, uint8_t required) override;
    void GetTokenInfo(bool* enabled, char* token) override;
    void LogonResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) override;
    void ReconnectResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) override;
    LOGIN_STATE NextSecurityState(LOGIN_STATE state) override;
    int32_t GetServerId() override;
    const uint8_t* GetVersionChallenge() override;
    void GetRealmList() override;
    void Reconnect() override;
    void Logon(const char* loginServer, const char* portal) override;
    int32_t Wait(const char* message, LoginData& loginData) override;
    void ProveVersion(const uint8_t* versionChecksum) override;
    void FileAccept(bool accept, uint64_t offset) override;
    void FileComplete(bool success) override;
    int32_t GetSurveyId() override;
    void SendSurveyData(uint16_t size, uint8_t* data) override;
    void SendSurveyFailed() override;
    void Logoff() override;
    const uint8_t* GetKeyMap() override;
    void EnterPIN(int32_t count, uint8_t* digits) override;
    void EnterMatrix(uint8_t value) override;
    void RevertMatrix() override;
    void CommitMatrix() override;
    void FinalizeMatrix() override;
    bool GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount) override;
    bool GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y) override;
    void EnterToken(const char* token) override;
    bool IsTrialAccount() override;
    bool IsGMAccount() override;
    bool IsProPassAccount() override;
    void Update() override;
    void Init(LoginResponse* loginResponse) override;
    ~BattlenetLogin() override;
    void JoinRealm(uint32_t region, uint32_t battlegroup, uint32_t id) override;
    int32_t GetLoginServerType() override;

    BattlenetLogin();
    void BuildRealmListResult();
    Battlenet::TumorManager* CreateTumorManager();
    void DestroyTumorManager();
    void EventQueueUpdate();
    uint8_t GetCharacterCount(uint32_t region, uint32_t battlegroup, uint32_t id);
    const char* GetGameAccountName(uint32_t index);
    uint32_t GetGameAccountRegion(uint32_t index);
    uint32_t GetNumGameAccounts();
    const char* GetPatchInstruction(uint32_t index);
    void HandleDisconnectEvents(uint32_t index);
    void HandleInvoke(const Battlenet::Event::Larva::Invoke* invoke);
    void HandleUpdateInvoke(const Battlenet::Event::Update::Invoke* invoke);
    bool IsConnected();
    void LogonResultBN(int32_t result);
    void QueueEvent(BN_EVENT_ID id, void* data, int32_t size);
    void SendSurveyInfoToModule();
    void SetAuthModuleMatrixInfo(const char* info, const char* challenges);
    void SetAuthModulePinInfo(const char* keyMap);
    bool SetGameAccount(uint32_t index);
    void UpdateRealmListData(uint32_t region, uint32_t battlegroup, uint32_t id, const Battlenet::Realm::Info* info, uint8_t flags, bool remove);
};

class BattlenetApplication : public Battlenet::Application, public WowConnectionResponse {
    public:
    /* 0010 */ uint32_t m_connection;
    /* 0014 */ Battlenet::IP4::AddressPort m_localAddress;
    /* 001A */ Battlenet::IP4::AddressPort m_remoteAddress;
    /* 0020 */ BattlenetLogin* m_login;
    /* 0024 */ WowConnection* m_wowConnection;

    void RecvEvent(const Battlenet::Event::Header& header, const BSN::EncodedObject* data) override;
    ~BattlenetApplication() override;
    uint32_t Resolve(uint32_t token, const char* hostname) override;
    uint32_t Connect(const Battlenet::IP4::AddressPort& address, const uint32_t& tendrilId) override;
    void Disconnect(uint32_t connection) override;
    bool Send(uint32_t connection, const void* data, uint32_t size) override;
    uint32_t OpenDatagram(int32_t port, const uint32_t& tendrilId) override;
    void CloseDatagram(uint32_t connection) override;
    bool SendDatagram(uint32_t connection, const void* data, uint32_t size, const Battlenet::IP4::AddressPort& address) override;
    uint64_t GetCurrentTime() const override;
    uint64_t GetCurrentTick() const override;
    void LogLevel(const char* message, int32_t level) override;
    Battlenet::ApplicationFile* OpenApplicationFile(const char* name) override;
    void CloseApplicationFile(Battlenet::ApplicationFile* file) override;
    void WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) override;
    void WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) override;

    BattlenetApplication();
};

#endif
