#include "WoW/Source/WorldClient/WorldEditHooks.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Storm/H/stpl.h"

// See WorldEditHooks.h. Everything here is a port addition.

// ---- Overlays

struct WorldEditOverlay {
    WORLDEDITOVERLAY m_func;
    void* m_param;
};

static TSGrowableArray<WorldEditOverlay> s_overlays;

void WorldEditRegisterOverlay(WORLDEDITOVERLAY overlay, void* param) {
    for (uint32_t i = 0; i < s_overlays.m_count; i++) {
        if (s_overlays.m_data[i].m_func == overlay && s_overlays.m_data[i].m_param == param) {
            return;
        }
    }

    WorldEditOverlay* entry = s_overlays.New();
    entry->m_func = overlay;
    entry->m_param = param;
}

void WorldEditUnregisterOverlay(WORLDEDITOVERLAY overlay, void* param) {
    for (uint32_t i = 0; i < s_overlays.m_count; i++) {
        if (s_overlays.m_data[i].m_func == overlay && s_overlays.m_data[i].m_param == param) {
            s_overlays.m_data[i] = s_overlays.m_data[s_overlays.m_count - 1];
            s_overlays.SetCount(s_overlays.m_count - 1);
            return;
        }
    }
}

void WorldEditRenderOverlays() {
    for (uint32_t i = 0; i < s_overlays.m_count; i++) {
        s_overlays.m_data[i].m_func(s_overlays.m_data[i].m_param);
    }
}

// ---- Terrain pass

static WORLDEDITTERRAINPASS s_terrainPass;
static TSGrowableArray<CMapRenderChunk*> s_terrainChunks;

void WorldEditSetTerrainPass(WORLDEDITTERRAINPASS pass) {
    s_terrainPass = pass;
    s_terrainChunks.SetCount(0);
}

bool WorldEditTerrainPassActive() {
    return s_terrainPass != nullptr;
}

void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk) {
    if (s_terrainPass) {
        *s_terrainChunks.New() = chunk;
    }
}

// The chunks stay valid here: the terrain passes only move them to CMap::s_mapRenderChunkUpdateList, and
// CMap::ProcessRenderChunkUpdateList frees a chunk's buffers only after it has gone 2 seconds without being drawn.
void WorldEditRenderTerrainPass() {
    if (s_terrainPass && s_terrainChunks.m_count) {
        s_terrainPass(s_terrainChunks.m_data, s_terrainChunks.m_count);
    }

    s_terrainChunks.SetCount(0);
}

#endif
