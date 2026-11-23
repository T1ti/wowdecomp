#ifndef WOWEDIT_SOURCE_APP_WOWEDITCOMMANDS_H
#define WOWEDIT_SOURCE_APP_WOWEDITCOMMANDS_H

// WowEdit application: the command table and the commander (port; see App/WowEditApp.h).
//
// After the War3 World Editor's CCommander and its WECommandData table: every action has a command id; the table
// says which menu it sits in, its item text and its hotkey, and the menu bar is built from it. Picking the item or
// pressing the hotkey dispatches the id to the modules (CWowEditModule) until one runs it; a command that changes
// data hands back an undoable for the undoer. Undo and Redo are the commander's own.
//
// Hotkeys go through the menu bar's accelerator table (OsGuiProcessMessage), before the game sees the key, so they
// are kept to Ctrl combinations the game does not bind.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CWowEditModule;
class CWowEditUndoer;

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

enum WOWEDITCMD : int32_t {
    WOWEDITCMD_NONE = 0, // a separator in the table
    // File
    WOWEDITCMD_OPEN_MAP,
    WOWEDITCMD_EXIT,
    // Edit (the commander's own)
    WOWEDITCMD_UNDO,
    WOWEDITCMD_REDO,
    // View (CWowEditViewModule): WoWEdit's view toggles (WowEditViewToggles.h) and the time of day
    WOWEDITCMD_VIEW_GAME,
    WOWEDITCMD_VIEW_GRID,
    WOWEDITCMD_VIEW_AREAS,
    WOWEDITCMD_VIEW_SLOPES,
    WOWEDITCMD_VIEW_TERRAIN,
    WOWEDITCMD_VIEW_SHADOWS,
    WOWEDITCMD_VIEW_FOG,
    WOWEDITCMD_VIEW_DOODADS,
    WOWEDITCMD_VIEW_DETAIL_DOODADS,
    WOWEDITCMD_VIEW_WMOS,
    WOWEDITCMD_VIEW_WATER,
    WOWEDITCMD_VIEW_SKY,
    WOWEDITCMD_VIEW_PARTICULATES,
    WOWEDITCMD_VIEW_WIREFRAME,
    WOWEDITCMD_VIEW_NORMALS,
    WOWEDITCMD_VIEW_COLLISION,
    WOWEDITCMD_VIEW_MARKERS,
    WOWEDITCMD_TIME_DAWN,
    WOWEDITCMD_TIME_NOON,
    WOWEDITCMD_TIME_DUSK,
    WOWEDITCMD_TIME_MIDNIGHT,
    WOWEDITCMD_VIEW_AZERTY, // the camera's movement keys (WowEditWorldView.h)
    // Tools (CWowEditTerrainModule)
    WOWEDITCMD_TOOL_BRUSH,
    // Window
    WOWEDITCMD_WINDOW_TOOL_PALETTE,
    WOWEDITCMD_WINDOW_VIEW_PALETTE,
    WOWEDITCMD_WINDOW_DISTANCE_PALETTE,
    // Help
    WOWEDITCMD_ABOUT,
    NUM_WOWEDITCMDS,
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

class CWowEditCommander {
    public:
    explicit CWowEditCommander(CWowEditUndoer* undoer);
    ~CWowEditCommander();

    // Takes ownership.
    void AddModule(CWowEditModule* module);
    uint32_t GetNumModules();
    CWowEditModule* GetModule(uint32_t index);

    // The command at a menu position, or WOWEDITCMD_NONE.
    int32_t FindMenuCommand(uint8_t menu, int32_t position);

    // Runs the command; false when no module runs it.
    bool DoCommand(int32_t command);
    // Menu state: enabled and checked.
    void GetCommandStatus(int32_t command, bool* enabled, bool* checked);

    private:
    CWowEditUndoer* m_undoer;
    TSGrowableArray<CWowEditModule*> m_modules;
};

#endif

#endif
