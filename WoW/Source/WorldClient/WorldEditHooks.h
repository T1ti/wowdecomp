#ifndef WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H
#define WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H

// Tool features: the hook points the world editor attaches to (port; no client keeps any editor code).
//
// The editor itself lives in WowEdit/ at the repository root, outside the client sources; the client only offers
// these hooks and calls them when something is attached. Nothing here draws anything.
//   - overlays: callbacks run once per frame by CGWorldFrame::OnWorldRender, after the scene and before the Gxu
//     debug-draw queue is drawn, while the world view is set (markers: spawn points, paths, ...);
//   - the terrain pass: one callback run once per frame by CWorldScene::RenderChunks, after the terrain passes, with
//     every terrain render chunk drawn this frame, so the editor can draw the terrain mesh again with its own shaders
//     (grid, slopes, ...). This is an editor pass of its own, apart from the debug-feature zone pass
//     (CWorldScene::RenderChunksZoneDebug, render list 20).

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class CMapRenderChunk;

// Overlay callbacks. Registering the same callback and parameter twice keeps one entry.
typedef void (*WORLDEDITOVERLAY)(void* param);
void WorldEditRegisterOverlay(WORLDEDITOVERLAY overlay, void* param);
void WorldEditUnregisterOverlay(WORLDEDITOVERLAY overlay, void* param);
// From CGWorldFrame::OnWorldRender, before GxuDebugDrawRender.
void WorldEditRenderOverlays();

// Terrain pass callback (nullptr detaches it). It runs inside CWorldScene::RenderChunks' render-state push, with the
// world view set; it sets and restores every state it uses. The chunks' vertices are local to CMapRenderChunk::m_vec1.
typedef void (*WORLDEDITTERRAINPASS)(CMapRenderChunk* const* chunks, uint32_t count);
void WorldEditSetTerrainPass(WORLDEDITTERRAINPASS pass);
bool WorldEditTerrainPassActive();
// From the terrain passes: records a chunk drawn this frame (only while a terrain pass is attached).
void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk);
// From CWorldScene::RenderChunks after the terrain passes: runs the terrain pass and forgets the chunks.
void WorldEditRenderTerrainPass();

#endif

#endif
