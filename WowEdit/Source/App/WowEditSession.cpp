#include "WowEdit/Source/App/WowEditSession.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Tempest/C3Vector.h"
#include "WowEdit/Source/App/WowEditModule.h"
#include "WowEdit/Source/App/WowEditRenderDistance.h"
#include "WowEdit/Source/App/WowEditSettings.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

CWowEditSession::CWowEditSession(void (*setViewportCursor)(bool looking)) : m_commander(&m_undoer) {
    this->m_commander.AddModule(WowEditCreateViewModule());
    this->m_commander.AddModule(WowEditCreateTerrainModule());

    const WowEditSettings& settings = WowEditGetSettings();
    WowEditWorldInitialize(setViewportCursor);
    WowEditWorldSetAzerty(settings.m_azerty);
    WowEditViewTogglesInitialize();
    WowEditRenderDistanceInitialize();

    C3Vector start(settings.m_start[0], settings.m_start[1], settings.m_start[2]);
    if (!WowEditWorldLoadMap(settings.m_mapId, settings.m_hasStart ? &start : nullptr) && settings.m_mapId != 0) {
        WowEditWorldLoadMap(0, nullptr);
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
}

CWowEditCommander& CWowEditSession::Commands() {
    return this->m_commander;
}

#endif
