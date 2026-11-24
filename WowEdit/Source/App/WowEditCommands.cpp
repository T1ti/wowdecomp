#include "WowEdit/Source/App/WowEditCommands.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "WowEdit/Source/App/WowEditModule.h"
#include "WowEdit/Source/App/WowEditUndo.h"

// See WowEditCommands.h.

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
