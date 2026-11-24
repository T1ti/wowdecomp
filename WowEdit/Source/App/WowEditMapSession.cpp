#include "WowEdit/Source/App/WowEditMapSession.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdio>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"

uint32_t CWowEditMapSession::GetMapCount() {
    return g_mapDB.GetNumRecords();
}

WowEditMapInfo CWowEditMapSession::GetMapInfo(uint32_t index) {
    if (index >= GetMapCount()) {
        return { -1, "", "" };
    }

    const MapRec& map = g_mapDB.m_records[index];
    return { map.m_id, map.m_mapName ? map.m_mapName : "", map.m_directory ? map.m_directory : "" };
}

const MapRec* CWowEditMapSession::FindAvailableMap(int32_t mapId) {
    const MapRec* map = g_mapDB.GetRecord(mapId);
    if (!map || !map->m_directory || !map->m_directory[0]) {
        ConsolePrintf("WowEdit: no map directory for map %d in Map.dbc", mapId);
        return nullptr;
    }

    char path[256];
    int length = snprintf(path, sizeof(path), "World\\Maps\\%s\\%s.wdt", map->m_directory, map->m_directory);
    if (length < 0 || static_cast<size_t>(length) >= sizeof(path)) {
        ConsolePrintf("WowEdit: WDT path for map %d is too long", mapId);
        return nullptr;
    }

    SFileImpl* file = nullptr;
    if (!SFileOpenFile(path, &file)) {
        ConsolePrintf("WowEdit: could not open map %d: %s", mapId, path);
        return nullptr;
    }

    SFileCloseFile(file);
    return map;
}

void CWowEditMapSession::Open(const MapRec& map, C3Vector& start, const WorldStreamingView& streaming) {
    CWorld::LoadMap(map.m_directory, &start, map.m_id, 1, &streaming);
    this->m_map = &map;
}

void CWowEditMapSession::Close() {
    if (!this->m_map) {
        return;
    }

    this->m_map = nullptr;
    WorldEditClearTerrainChunks();
    World::UnloadMap();
    if (World::s_weather) {
        World::s_weather->Reset();
    }
}

#endif
