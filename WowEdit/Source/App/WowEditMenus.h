#ifndef WOWEDIT_SOURCE_APP_WOWEDITMENUS_H
#define WOWEDIT_SOURCE_APP_WOWEDITMENUS_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

// Menu ids (COsMenu::m_id; an item's command id is (menu id << 8) | position).
enum WOWEDITMENU : uint8_t {
    WOWEDITMENU_FILE = 1,
    WOWEDITMENU_EDIT = 2,
    WOWEDITMENU_VIEW = 3,
    WOWEDITMENU_TOOLS = 4,
    WOWEDITMENU_WINDOW = 5,
    WOWEDITMENU_HELP = 6,
    WOWEDITMENU_LAST = 7,
};

struct WowEditCommandData {
    int32_t command; // WOWEDITCMD; WOWEDITCMD_NONE is a separator
    uint8_t menu; // WOWEDITMENU
    const char* text; // menu item text ('&' marks the access key)
    int32_t key; // KEY_* (Event/Types.h), KEY_NONE for no hotkey
    int32_t modifiers; // OsGuiMenuHotkey::modKeyID: 1 shift, 2 control, 4 alt
};

// The table, in menu order: each menu's items are the entries with its id, in table order.
const WowEditCommandData* WowEditGetCommandTable(uint32_t* count);
const char* WowEditGetMenuTitle(uint8_t menu);

int32_t WowEditFindMenuCommand(uint8_t menu, int32_t position);

#endif

#endif
