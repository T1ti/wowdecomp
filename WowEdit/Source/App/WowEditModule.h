#ifndef WOWEDIT_SOURCE_APP_WOWEDITMODULE_H
#define WOWEDIT_SOURCE_APP_WOWEDITMODULE_H

// WowEdit application: editor modules (port; see App/WowEditApp.h).
//
// After the War3 World Editor's CWEModule: each editor is a module that runs its own commands, reports their menu
// state, and owns its palettes. War3 has one per editor window (terrain, triggers, sounds, objects, AI, campaign);
// here the modules share the one Gx window:
//   - CWowEditAppModule: File and Help (exit, about);
//   - CWowEditViewModule: the View menu, the terrain overlays and markers of the WowEdit library;
//   - CWowEditTerrainModule: the terrain tools, the brush and its palette (CWowEditToolPalette).
// A new editor (doodads, WMO placement, liquids, area painting...) is a new module plus its commands in the table.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class CWowEditUndoable;

class CWowEditModule {
    public:
    virtual ~CWowEditModule() {
    }

    virtual const char* GetName() = 0;

    // Runs the command when it is this module's, and returns true; a command that changed data sets *undoable.
    virtual bool DoCommand(int32_t command, CWowEditUndoable** undoable) = 0;
    // The command's menu state when it is this module's, and returns true.
    virtual bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) = 0;

    // The editor attached to (or left) the main window: create and destroy palettes here.
    virtual void OnAttach(void* mainWindow) {
    }

    virtual void OnDetach() {
    }

    // Before a menu opens: palettes resync with state changed elsewhere (the console commands).
    virtual void OnRefresh() {
    }
};

CWowEditModule* WowEditCreateAppModule();
CWowEditModule* WowEditCreateViewModule();
CWowEditModule* WowEditCreateTerrainModule();

#endif

#endif
