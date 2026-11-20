#ifndef WOW_SOURCE_UI_GMTICKETINFO_H
#define WOW_SOURCE_UI_GMTICKETINFO_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

// Result word of SMSG_GM_TICKET_CREATE, _UPDATE_TEXT, _GET_TICKET and
// _DELETE_TICKET. Error values follow the ERR_TICKET_* string each displays;
// the others are named after how Packet_Group_5 handles them (inferred).
enum GM_TICKET_RESPONSE : uint32_t {
    GM_TICKET_RESPONSE_DB_ERROR       = 0, // ERR_TICKET_DB_ERROR
    GM_TICKET_RESPONSE_ALREADY_EXISTS = 1, // ERR_TICKET_ALREADY_EXISTS
    GM_TICKET_RESPONSE_CREATED        = 2, // refreshes the ticket
    GM_TICKET_RESPONSE_CREATE_ERROR   = 3, // ERR_TICKET_CREATE_ERROR
    GM_TICKET_RESPONSE_UPDATED        = 4, // refreshes the ticket
    GM_TICKET_RESPONSE_UPDATE_ERROR   = 5, // ERR_TICKET_UPDATE_ERROR
    GM_TICKET_RESPONSE_HAS_TICKET     = 6, // ticket text follows
    GM_TICKET_RESPONSE_DELETED        = 9,
};

// SMSG_GM_TICKET_STATUS_UPDATE status word (inferred names).
enum GM_TICKET_STATUS : uint32_t {
    GM_TICKET_STATUS_UNKNOWN_1 = 1, // refreshes the ticket
    GM_TICKET_STATUS_CLOSED    = 2, // clears the ticket
    GM_TICKET_STATUS_SURVEY    = 3, // clears the ticket and shows GMSURVEY_DISPLAY
    GM_TICKET_STATUS_UNKNOWN_4 = 4, // refreshes the ticket
};

int32_t Packet_Group_5(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

struct StaticConstant {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_value;
};

class CGGMTicketInfo {
    public:
    static void LeaveWorld();
    static void EnterWorld();
    static void InitializeGame();
    static void ShutdownGame();
    static int32_t m_ticketID;
    static int32_t m_responseID;
    static void CreateTicket(const char* text, BOOL needResponse, uint32_t moreHelp);
};

class CGGMReportLag {
    public:
    static void Submit(int32_t type);
};

void GMTicketInfoRegisterScriptFunctions();
void GMTicketInfoUnregisterScriptFunctions();

void SendGetTicketOpcode();
void SendGMTicketUpdateText(const char* text);
void SendGMTicketDeleteTicket();
void SendGMResponseResolve();

#endif
