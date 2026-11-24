#include "WowEdit/Source/App/WowEditSession.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Tempest/C3Vector.h"
#include "WowEdit/Source/App/WowEditModule.h"
#include "WowEdit/Source/App/WowEditRenderDistance.h"
#include "WowEdit/Source/App/WowEditSettings.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

CWowEditSession::CWowEditSession(void (*setViewportCursor)(bool looking)) : m_areaEdits(m_undoer), m_commander(&m_undoer) {
    this->m_commander.AddModule(WowEditCreateViewModule());
    this->m_commander.AddModule(WowEditCreateTerrainModule());

    const WowEditSettings& settings = WowEditGetSettings();
    WowEditWorldInitialize(this->m_areaEdits, setViewportCursor);
    WowEditWorldSetAzerty(settings.m_azerty);
    WowEditViewTogglesInitialize();
    WowEditRenderDistanceInitialize();

    C3Vector start(settings.m_start[0], settings.m_start[1], settings.m_start[2]);
    if (!this->OpenMap(settings.m_mapId, settings.m_hasStart ? &start : nullptr) && settings.m_mapId != 0) {
        this->OpenMap(0, nullptr);
    }
}

CWowEditSession::~CWowEditSession() {
    const int32_t mapId = WowEditWorldGetMapId();
    if (mapId >= 0) {
        float position[3];
        WowEditWorldGetCameraPosition(position);
        WowEditSaveStart(mapId, position);
    }

    WowEditRenderDistanceSave();
    WowEditWorldShutdown();
    this->m_undoer.Clear();
}

bool CWowEditSession::DoCommand(int32_t command) {
    if (command == WOWEDITCMD_AREA_PAINT) {
        if (WowEditWorldGetMapId() < 0) {
            return false;
        }

        this->m_areaEdits.SetPainting(!this->m_areaEdits.IsPainting());
        if (this->m_areaEdits.IsPainting()) {
            WowEditFindViewToggle(WOWEDITCMD_VIEW_AREAS)->set(true);
        }
        return true;
    }

    if (command == WOWEDITCMD_UNDO || command == WOWEDITCMD_REDO) {
        if (WowEditWorldGetMapId() < 0) {
            return false;
        }

        if (command == WOWEDITCMD_UNDO) {
            this->m_areaEdits.QueueUndo();
        } else {
            this->m_areaEdits.QueueRedo();
        }
        return true;
    }

    return this->m_commander.DoCommand(command);
}

void CWowEditSession::GetCommandStatus(int32_t command, bool* enabled, bool* checked) {
    if (command == WOWEDITCMD_AREA_PAINT) {
        *enabled = WowEditWorldGetMapId() >= 0;
        *checked = this->m_areaEdits.IsPainting();
        return;
    }

    this->m_commander.GetCommandStatus(command, enabled, checked);
}

bool CWowEditSession::OpenMap(int32_t mapId, const C3Vector* position) {
    return this->m_areaEdits.OpenMap(mapId, position, WowEditGetSettings().m_iniPath);
}

CWowEditAreaEdits& CWowEditSession::AreaEdits() {
    return this->m_areaEdits;
}

#endif
