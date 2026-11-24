#include "WoW/Source/WorldClient/WorldEditHooks.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

// See WorldEditHooks.h. Everything here is a port addition.

static const WorldEditView* s_view;

void WorldEditSetView(const WorldEditView* view) {
    s_view = view;
}

const WorldEditView* WorldEditGetView() {
    return s_view;
}

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
    WorldEditClearTerrainChunks();
}

bool WorldEditTerrainPassActive() {
    return s_terrainPass != nullptr;
}

void WorldEditClearTerrainChunks() {
    s_terrainChunks.SetCount(0);
}

void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk) {
    if (s_terrainPass) {
        *s_terrainChunks.New() = chunk;
    }
}

// The chunks stay valid until the late call of the same frame. The terrain passes move them to
// CMap::s_mapRenderChunkUpdateList, which the next world update processes.
void WorldEditRenderTerrainPass() {
    if (s_terrainPass) {
        s_terrainPass(s_terrainChunks.m_data, s_terrainChunks.m_count, false);
    }
}

void WorldEditRenderTerrainPassLate() {
    if (s_terrainPass && s_terrainChunks.m_count) {
        s_terrainPass(s_terrainChunks.m_data, s_terrainChunks.m_count, true);
    }

    WorldEditClearTerrainChunks();
}

// ---- Cursor

static C3Vector s_cursorStart;
static C3Vector s_cursorEnd;
static bool s_cursorValid;

void WorldEditSetCursorSegment(const C3Vector* start, const C3Vector* end) {
    s_cursorValid = start && end && (start->x != end->x || start->y != end->y || start->z != end->z);

    if (s_cursorValid) {
        s_cursorStart = *start;
        s_cursorEnd = *end;
    }
}

bool WorldEditGetCursorSegment(C3Vector* start, C3Vector* end) {
    if (s_cursorValid) {
        *start = s_cursorStart;
        *end = s_cursorEnd;
    }

    return s_cursorValid;
}

#endif
