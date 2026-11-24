#ifndef WOWEDIT_SOURCE_APP_WOWEDITSESSION_H
#define WOWEDIT_SOURCE_APP_WOWEDITSESSION_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditUndo.h"

// One session per initialized engine. Create and destroy it in the interactive context while Gx is alive.
class CWowEditSession {
    public:
    explicit CWowEditSession(void (*setViewportCursor)(bool looking));
    ~CWowEditSession();

    CWowEditSession(const CWowEditSession&) = delete;
    CWowEditSession& operator=(const CWowEditSession&) = delete;

    CWowEditCommander& Commands();

    private:
    CWowEditUndoer m_undoer;
    CWowEditCommander m_commander;
};

#endif

#endif
