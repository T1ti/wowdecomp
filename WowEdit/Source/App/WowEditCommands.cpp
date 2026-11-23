#include "WowEdit/Source/App/WowEditCommands.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Event/Types.h"
#include "WowEdit/Source/App/WowEditModule.h"
#include "WowEdit/Source/App/WowEditUndo.h"

// See WowEditCommands.h.

static const int32_t MOD_CTRL = 2;

static const WowEditCommandData s_commandTable[] = {
    { WOWEDITCMD_OPEN_MAP, WOWEDITMENU_FILE, "&Open Map...", KEY_O, MOD_CTRL },
    { WOWEDITCMD_NONE, WOWEDITMENU_FILE, nullptr, KEY_NONE, 0 },
    { WOWEDITCMD_EXIT, WOWEDITMENU_FILE, "E&xit", KEY_NONE, 0 },

    { WOWEDITCMD_UNDO, WOWEDITMENU_EDIT, "&Undo", KEY_Z, MOD_CTRL },
    { WOWEDITCMD_REDO, WOWEDITMENU_EDIT, "&Redo", KEY_Y, MOD_CTRL },

    { WOWEDITCMD_VIEW_GAME, WOWEDITMENU_VIEW, "Match In-&Game View", KEY_NONE, 0 },
    { WOWEDITCMD_NONE, WOWEDITMENU_VIEW, nullptr, KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_GRID, WOWEDITMENU_VIEW, "&Grid", KEY_G, MOD_CTRL },
    { WOWEDITCMD_VIEW_AREAS, WOWEDITMENU_VIEW, "&Area IDs", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_SLOPES, WOWEDITMENU_VIEW, "&Impassable Slopes", KEY_NONE, 0 },
    { WOWEDITCMD_NONE, WOWEDITMENU_VIEW, nullptr, KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_TERRAIN, WOWEDITMENU_VIEW, "&Terrain", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_SHADOWS, WOWEDITMENU_VIEW, "Terrain S&hadows", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_DOODADS, WOWEDITMENU_VIEW, "&Doodads", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_DETAIL_DOODADS, WOWEDITMENU_VIEW, "D&etail Doodads", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_WMOS, WOWEDITMENU_VIEW, "&WMOs", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_WATER, WOWEDITMENU_VIEW, "Wat&er", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_SKY, WOWEDITMENU_VIEW, "S&ky", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_FOG, WOWEDITMENU_VIEW, "Distance &Fog", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_PARTICULATES, WOWEDITMENU_VIEW, "&Particulates", KEY_NONE, 0 },
    { WOWEDITCMD_NONE, WOWEDITMENU_VIEW, nullptr, KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_WIREFRAME, WOWEDITMENU_VIEW, "Wi&reframe", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_NORMALS, WOWEDITMENU_VIEW, "&Normals", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_COLLISION, WOWEDITMENU_VIEW, "&Collision", KEY_NONE, 0 },
    { WOWEDITCMD_VIEW_MARKERS, WOWEDITMENU_VIEW, "Sample &Markers", KEY_NONE, 0 },
    { WOWEDITCMD_NONE, WOWEDITMENU_VIEW, nullptr, KEY_NONE, 0 },
    { WOWEDITCMD_TIME_DAWN, WOWEDITMENU_VIEW, "Time: Dawn (6:00)", KEY_NONE, 0 },
    { WOWEDITCMD_TIME_NOON, WOWEDITMENU_VIEW, "Time: Noon (12:00)", KEY_NONE, 0 },
    { WOWEDITCMD_TIME_DUSK, WOWEDITMENU_VIEW, "Time: Dusk (19:00)", KEY_NONE, 0 },
    { WOWEDITCMD_TIME_MIDNIGHT, WOWEDITMENU_VIEW, "Time: Midnight (0:00)", KEY_NONE, 0 },

    { WOWEDITCMD_TOOL_BRUSH, WOWEDITMENU_TOOLS, "Terrain &Brush", KEY_B, MOD_CTRL },

    { WOWEDITCMD_WINDOW_TOOL_PALETTE, WOWEDITMENU_WINDOW, "&Tool Palette", KEY_NONE, 0 },
    { WOWEDITCMD_WINDOW_VIEW_PALETTE, WOWEDITMENU_WINDOW, "&View Palette", KEY_NONE, 0 },

    { WOWEDITCMD_ABOUT, WOWEDITMENU_HELP, "&About WowEdit", KEY_NONE, 0 },
};

static const char* const s_menuTitles[WOWEDITMENU_LAST] = {
    nullptr,
    "&File",
    "&Edit",
    "&View",
    "&Tools",
    "&Window",
    "&Help",
};

const WowEditCommandData* WowEditGetCommandTable(uint32_t* count) {
    *count = sizeof(s_commandTable) / sizeof(s_commandTable[0]);
    return s_commandTable;
}

const char* WowEditGetMenuTitle(uint8_t menu) {
    return menu > 0 && menu < WOWEDITMENU_LAST ? s_menuTitles[menu] : nullptr;
}

// ---- CWowEditCommander

CWowEditCommander::CWowEditCommander(CWowEditUndoer* undoer) {
    this->m_undoer = undoer;
}

CWowEditCommander::~CWowEditCommander() {
    for (uint32_t i = 0; i < this->m_modules.Count(); i++) {
        delete this->m_modules[i];
    }
}

void CWowEditCommander::AddModule(CWowEditModule* module) {
    *this->m_modules.New() = module;
}

uint32_t CWowEditCommander::GetNumModules() {
    return this->m_modules.Count();
}

CWowEditModule* CWowEditCommander::GetModule(uint32_t index) {
    return index < this->m_modules.Count() ? this->m_modules[index] : nullptr;
}

int32_t CWowEditCommander::FindMenuCommand(uint8_t menu, int32_t position) {
    int32_t current = 0;

    for (const WowEditCommandData& data : s_commandTable) {
        if (data.menu != menu) {
            continue;
        }

        if (current == position) {
            return data.command;
        }

        current++;
    }

    return WOWEDITCMD_NONE;
}

bool CWowEditCommander::DoCommand(int32_t command) {
    switch (command) {
        case WOWEDITCMD_NONE:
            return false;

        case WOWEDITCMD_UNDO:
            this->m_undoer->Undo();
            return true;

        case WOWEDITCMD_REDO:
            this->m_undoer->Redo();
            return true;

        default:
            break;
    }

    for (uint32_t i = 0; i < this->m_modules.Count(); i++) {
        CWowEditUndoable* undoable = nullptr;

        if (this->m_modules[i]->DoCommand(command, &undoable)) {
            this->m_undoer->Add(undoable);
            return true;
        }
    }

    return false;
}

void CWowEditCommander::GetCommandStatus(int32_t command, bool* enabled, bool* checked) {
    *enabled = true;
    *checked = false;

    switch (command) {
        case WOWEDITCMD_UNDO:
            *enabled = this->m_undoer->CanUndo();
            return;

        case WOWEDITCMD_REDO:
            *enabled = this->m_undoer->CanRedo();
            return;

        default:
            break;
    }

    for (uint32_t i = 0; i < this->m_modules.Count(); i++) {
        if (this->m_modules[i]->GetCommandStatus(command, enabled, checked)) {
            return;
        }
    }

    // A command no module answers for is not built yet.
    *enabled = false;
}

#endif
