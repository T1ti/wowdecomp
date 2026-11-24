#ifndef WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H
#define WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CRect.h"

// World renderer inputs and terrain geometry callback for WowEdit (port).
class CGCamera;
class CMapRenderChunk;

// The camera and loading range used when preparing an editor view of the map.
struct WorldStreamingView {
    CGCamera* m_camera;
    float m_farClip;
    float m_objectMinZ;
};

// Persistent view inputs for shared map streaming and terrain consumers.
struct WorldEditView {
    CGCamera* m_camera;
    float m_minFarClip;
    float m_maxFarClip;
    float m_horizonNearClip;
    float m_horizonFarClip;
    CRect m_terrainBounds;
    uint32_t m_terrainBufferCount;
    bool m_useTerrainOcclusion;
    bool m_allowPendingMapAssets;

    bool IncludesTerrain(const CAaBox& chunk) const {
        return chunk.t.x >= m_terrainBounds.minX && chunk.b.x <= m_terrainBounds.maxX &&
               chunk.t.y >= m_terrainBounds.minY && chunk.b.y <= m_terrainBounds.maxY;
    }
};

// The editor updates this data before loading or drawing a map and keeps it alive until shutdown.
void WorldEditSetView(const WorldEditView* view);
const WorldEditView* WorldEditGetView();

// After the terrain draws, the callback can redraw its chunks with the same view and projection.
typedef void (*WORLDEDITTERRAINPASS)(CMapRenderChunk* const* chunks, uint32_t count);
void WorldEditSetTerrainPass(WORLDEDITTERRAINPASS pass);
// Start of terrain rendering, or before releasing the current map.
void WorldEditClearTerrainChunks();
// From the terrain passes: records a chunk drawn this frame (only while a terrain pass is attached).
void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk);
// From CWorldScene::RenderChunks after the terrain passes, including frames with no visible chunks.
void WorldEditRenderTerrainPass();

#endif

#endif
