#ifndef WOWEDIT_SOURCE_APP_WOWEDITSESSION_H
#define WOWEDIT_SOURCE_APP_WOWEDITSESSION_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include "WowEdit/Source/App/WowEditAreaEdits.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditUndo.h"

class C3Vector;

// One session per initialized engine. Create and destroy it in the interactive context while Gx is alive.
class CWowEditSession {
    public:
    explicit CWowEditSession(void (*setViewportCursor)(bool looking));
    ~CWowEditSession();

    CWowEditSession(const CWowEditSession&) = delete;
    CWowEditSession& operator=(const CWowEditSession&) = delete;

    bool DoCommand(int32_t command);
    void GetCommandStatus(int32_t command, bool* enabled, bool* checked);
    bool OpenMap(int32_t mapId, const C3Vector* position);
    CWowEditAreaEdits& AreaEdits();

    private:
    CWowEditUndoer m_undoer;
    CWowEditAreaEdits m_areaEdits;
    CWowEditCommander m_commander;
};

#endif

#endif
