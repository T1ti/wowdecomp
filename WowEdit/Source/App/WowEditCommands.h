#ifndef WOWEDIT_SOURCE_APP_WOWEDITCOMMANDS_H
#define WOWEDIT_SOURCE_APP_WOWEDITCOMMANDS_H

// Editor commands, after the Warcraft III World Editor's CCommander. Modules dispatch editor actions and return
// undoable changes to the commander. The OsGui shell's menu and shortcut metadata live in WowEditMenus.h.
//
#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CWowEditModule;
class CWowEditUndoer;

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
    WOWEDITCMD_SAVE_AREA_EDITS,
    WOWEDITCMD_AREA_PAINT,
    NUM_WOWEDITCMDS,
};

class CWowEditCommander {
    public:
    explicit CWowEditCommander(CWowEditUndoer* undoer);
    ~CWowEditCommander();

    // Takes ownership.
    void AddModule(CWowEditModule* module);

    // Runs the command; false when no module runs it.
    bool DoCommand(int32_t command);
    // Editor command state: enabled and checked.
    void GetCommandStatus(int32_t command, bool* enabled, bool* checked);

    private:
    CWowEditUndoer* m_undoer;
    TSGrowableArray<CWowEditModule*> m_modules;
};

#endif

#endif
