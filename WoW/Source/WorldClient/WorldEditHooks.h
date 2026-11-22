#ifndef WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H
#define WOW_SOURCE_WORLDCLIENT_WORLDEDITHOOKS_H

// Tool features: the hook points the world editor attaches to (port; no client keeps any editor code).
//
// The editor itself lives in WowEdit/ at the repository root, outside the client sources; the client only offers
// these hooks and calls them when something is attached. Nothing here draws anything.
//   - overlays: callbacks run once per frame by CGWorldFrame::OnWorldRender, after the scene and before the Gxu
//     debug-draw queue is drawn, while the world view is set (markers: spawn points, paths, ...);
//   - the terrain pass: one callback run twice per frame with every terrain render chunk drawn this frame, so the
//     editor can draw the terrain mesh again with its own shaders (grid, slopes, ...): by CWorldScene::RenderChunks
//     after the terrain passes, and by CGWorldFrame::OnWorldRender with the overlays, after the rest of the scene. This is an editor pass of its own, apart from the debug-feature zone pass
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

// Terrain pass callback (nullptr detaches it). late is false for the call inside CWorldScene::RenderChunks' render-state
// push, true for the call from CGWorldFrame::OnWorldRender; the world view is set for both, and the pass sets and
// restores every state it uses. The chunks' vertices are local to CMapRenderChunk::m_vec1.
typedef void (*WORLDEDITTERRAINPASS)(CMapRenderChunk* const* chunks, uint32_t count, bool late);
void WorldEditSetTerrainPass(WORLDEDITTERRAINPASS pass);
bool WorldEditTerrainPassActive();
// From the terrain passes: records a chunk drawn this frame (only while a terrain pass is attached).
void WorldEditCollectTerrainChunk(CMapRenderChunk* chunk);
// From CWorldScene::RenderChunks after the terrain passes: runs the terrain pass. The chunks are kept for the late call
// and forgotten when the next frame collects its first chunk.
void WorldEditRenderTerrainPass();
// From CGWorldFrame::OnWorldRender, with the overlays: runs the terrain pass again, late.
void WorldEditRenderTerrainPassLate();

#endif

#endif
