#ifndef WOW_SOURCE_UI_UILCD_LCD_H
#define WOW_SOURCE_UI_UILCD_LCD_H

#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"

class CEzLcd;
class CGPlayer_C;
class CStatus;
class XMLNode;
struct BattlefieldStatus;

struct LCDHANDLE : public TSHashObject<LCDHANDLE, HASHKEY_CONSTSTRI> {
    /* 0018 */ void* m_handle;
    /* 001C */ C2iVector m_position;
    /* 0024 */ RCString m_text;

    void SetText(const char* text);
};

struct MAPDATA : public TSLinkedNode<MAPDATA> {
    /* 0008 */ int32_t m_screen;
    /* 000C */ C2iVector m_size;
    /* 0014 */ C2iVector m_offset;
};

struct POIIcon : public TSLinkedNode<POIIcon> {
    /* 0008 */ void* m_handle;
    /* 000C */ int32_t m_id;
    /* 0010 */ char m_name[128];
};

// CHATMESSAGE::m_type; named from the CGLCD notifier that pushes each value
// and the text SetAlertText shows for it.
enum LCD_ALERT_TYPE : int32_t {
    LCD_ALERT_NONE = 0,
    LCD_ALERT_WHISPER = 1, // OnWhisper, WHISPER_FROM
    LCD_ALERT_IDLE_LOGOUT = 2, // IdleDisconnectWarning, IDLE_LOGOUT_WARNING
    LCD_ALERT_AUCTION_OUTBID = 3, // AuctionOutbid, OUTBID_ON_SHORT
    LCD_ALERT_AUCTION_EXPIRED = 4, // AuctionExpired, AUCTION_EXPIRED_MAIL_SUBJECT
    LCD_ALERT_AUCTION_WON = 5, // AuctionWon, AUCTION_WON_MAIL_SUBJECT
    LCD_ALERT_AUCTION_SOLD = 6, // AuctionSold, AUCTION_ITEM_SOLD
};

// Player info subscreens; named from the CGLCD::Display*Stats function each
// one calls. The ranged stats reuse the melee subscreen layout.
enum LCD_SUBSCREEN : int32_t {
    LCD_SUBSCREEN_MELEE = 0,
    LCD_SUBSCREEN_DEFENSE = 1,
    LCD_SUBSCREEN_SPELL = 2,
    LCD_SUBSCREEN_RANGED = 3,
    NUM_LCD_SUBSCREENS = 4,
};

// LCD Bitmap source; named from the XML "type" and "icon" attributes that
// select each value. Value 1 is never set by the XML loader.
enum LCD_BITMAP_TYPE : int32_t {
    LCD_BITMAP_NONE = 0,
    LCD_BITMAP_UNKNOWN_1 = 1,
    LCD_BITMAP_TEXTURE = 2,
    LCD_BITMAP_PREV_TEXTURE = 3,
    LCD_BITMAP_ICON = 4,
};

struct CHATMESSAGE : public TSLinkedNode<CHATMESSAGE> {
    /* 0008 */ char m_text[64];
    /* 0048 */ void* m_handle;
    /* 004C */ int32_t m_type; // LCD_ALERT_TYPE
};

// LCD screens; names follow the screen names CGLCD::GetScreenFromName accepts.
enum LCD_SCREEN : int32_t {
    LCD_SCREEN_INVALID = -1,
    LCD_SCREEN_WOW_TITLE = 0, // WoWTitleScreen
    LCD_SCREEN_PLAYER_INFO = 1, // PlayerInfoScreen
    LCD_SCREEN_BATTLEFIELD_ALERT = 2, // BattlefieldAlertScreen
    LCD_SCREEN_BATTLEFIELD_STATUS = 3, // BattlefieldStatusScreen
    LCD_SCREEN_NOT_IN_BATTLEFIELD = 4, // NotInBattlefieldScreen
    LCD_SCREEN_WAIT_QUEUE = 5, // WaitQueueScreen
    LCD_SCREEN_CHAT_ALERT = 6, // ChatAlertScreen
    LCD_SCREEN_ARATHI_BASIN_MAP = 7, // ArathiBasinMapScreen
    LCD_SCREEN_WARSONG_MAP = 8, // WarsongMapScreen
    LCD_SCREEN_EYE_OF_THE_STORM_MAP = 9, // EyeOfTheStormMapScreen
    LCD_SCREEN_STRAND_OF_THE_ANCIENTS_MAP = 10, // StrandOfTheAncientsMapScreen
    LCD_SCREEN_ALTERAC_VALLEY_MAP = 11, // AlteracValleyMapScreen
    LCD_SCREEN_ISLE_OF_CONQUEST_MAP = 12, // IsleOfConquestMapScreen
    LCD_SCREEN_FIRST_BATTLEFIELD_MAP = LCD_SCREEN_ARATHI_BASIN_MAP,
    LCD_SCREEN_LAST_BATTLEFIELD_MAP = LCD_SCREEN_ISLE_OF_CONQUEST_MAP,
};

class CGLCD {
    public:
    static void Logout();
    static CEzLcd* m_lcd;
    static int32_t m_chatAlertLines;
    static uint8_t initComplete;
    static CGPlayer_C* m_player;
    static uint8_t m_inWorld;
    static TSHashTable<LCDHANDLE, HASHKEY_CONSTSTRI> m_handles;
    static TSList<MAPDATA, TSGetLink<MAPDATA> > m_mapData;
    static TSList<POIIcon, TSGetLink<POIIcon> > m_POIIcons;

    static void ClearWaitQueueScreen();
    static void IdleDisconnectWarning();
    static void Initialize();
    static void SetScreen(int32_t screen);
    static void ShowBattlefieldScreen();
    static void HideBattlefieldAlert();
    static void OnBattlefieldReady(const BattlefieldStatus* status);
    static void OnBattlefieldActive();
    static void UpdateBattlefieldStats();
    static void Shutdown();
    static int32_t Update(const void* data, void* param);
    static void OnWhisper(const char* name);
    static void AuctionOutbid(const char* name);
    static void AuctionExpired(const char* name);
    static void AuctionSold(const char* name);
    static void AuctionWon(const char* name);
    static void PushAlert(const char* text, int32_t type);
    static void Login();
    static void* GetHandle(const char* name);
    static int32_t GetScreenFromName(const char* name);
    static void CheckInput();
    static void UpdateMapPositions();
    static void SetStat(const char* name, int32_t value);
    static void SetStat(const char* name, float value, bool percent);
    static void SetStat(const char* name, int32_t minValue, int32_t maxValue);
    static void SetStat(const char* name, const char* value);
    static void SetHandleText(const char* name, const char* text);
    static void DisplayMeleeStats();
    static void DisplayRangedStats();
    static void DisplayDefenseStats();
    static void DisplaySpellStats();
    static void SetAlertText();
    static void PostInit();
    static bool LoadLCDMap(int32_t screen, XMLNode* node, CStatus* status, int32_t subscreen);
    static bool LoadLCDText(int32_t screen, XMLNode* node, CStatus* status, int32_t subscreen);
    static bool LoadLCDBitmap(int32_t screen, XMLNode* node, CStatus* status);
    static bool LoadLCDElement(int32_t screen, XMLNode* node, CStatus* status, int32_t subscreen);
    static bool LoadGlobals(XMLNode* node, CStatus* status);
    static bool GeneratePOIs(XMLNode* node);
    static bool LoadScreen(XMLNode* node, CStatus* status);
    static bool LoadLayout();
    static void EnterWorld();
    static void LeaveWorld();
};

#endif
