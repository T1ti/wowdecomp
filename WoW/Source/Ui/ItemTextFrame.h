#ifndef WOW_SOURCE_UI_ITEMTEXTFRAME_H
#define WOW_SOURCE_UI_ITEMTEXTFRAME_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"

struct lua_State;

class CGItemText {
    public:
    static void InitializeGame();
    static void ShutdownGame();
    static void LeaveWorld();
    static WOWGUID m_itemGUID;
    static uint32_t m_currentPage;
    static uint8_t m_hasItemText;
    static char m_text[8000];
    static TSGrowableArray<int32_t> m_pages;

    static void SetItem(const WOWGUID& guid, BOOL refresh);
    static void DisplayText(const WOWGUID& guid, int32_t translate);
    static void ItemTextCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
};

void ItemTextRegisterScriptFunctions();
void ItemTextUnregisterScriptFunctions();

#endif
