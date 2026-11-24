#include "WoW/Source/WorldClient/WorldEditHooks.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Storm/H/stpl.h"

// See WorldEditHooks.h. Everything here is a port addition.

static const WorldEditView* s_view;

void WorldEditSetView(const WorldEditView* view) {
    s_view = view;
}

const WorldEditView* WorldEditGetView() {
    return s_view;
}

// ---- Terrain pass

static WORLDEDITTERRAINPASS s_terrainPass;
static TSGrowableArray<CMapRenderChunk*> s_terrainChunks;

void WorldEditSetTerrainPass(WORLDEDITTERRAINPASS pass) {
    s_terrainPass = pass;
    WorldEditClearTerrainChunks();
}

void WorldEditClearTerrainChunks() {
    s_terrainChunks.SetCount(0);
}

void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk) {
    if (s_terrainPass) {
        *s_terrainChunks.New() = chunk;
    }
}

void WorldEditRenderTerrainPass() {
    if (s_terrainPass) {
        s_terrainPass(s_terrainChunks.m_data, s_terrainChunks.m_count);
        WorldEditClearTerrainChunks();
    }
}

#endif
