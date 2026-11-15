#ifndef WOW_SOURCE_UI_GUILDREGISTRAR_H
#define WOW_SOURCE_UI_GUILDREGISTRAR_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

void GuildRegistrarRegisterScriptFunctions();
void GuildRegistrarUnregisterScriptFunctions();

struct GuildPetitionInfo {
    /* 0000 */ uint32_t m_index;
    /* 0004 */ uint32_t m_itemID;
    /* 0008 */ uint32_t m_displayID;
    /* 000C */ uint32_t m_cost;
    /* 0010 */ uint32_t m_charterType; // CGPetition::STATIC_TYPE
    /* 0014 */ uint32_t m_requiredSignatures;
};

class CGGuildRegistrar {
    public:
    static WOWGUID m_registrar;
    static void CloseRegistrar();
    static void EnterWorld();
    static GuildPetitionInfo m_petition;
    static void SetRegistrar(WOWGUID registrar, const GuildPetitionInfo* petition);
    static int32_t ValidateAndReportGuildName(const char* name);
    static void BuyGuildCharter(const char* name);
};

#endif
