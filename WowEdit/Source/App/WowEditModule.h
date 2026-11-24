#ifndef WOWEDIT_SOURCE_APP_WOWEDITMODULE_H
#define WOWEDIT_SOURCE_APP_WOWEDITMODULE_H

// Editor modules, after the Warcraft III World Editor's CWEModule. Each module handles editor commands and
// reports their state. Frontends own their windows and palettes.
//
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
    // Reports the command state and returns true if this module owns it.
    virtual bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) = 0;
};

CWowEditModule* WowEditCreateViewModule();
CWowEditModule* WowEditCreateTerrainModule();

#endif

#endif
