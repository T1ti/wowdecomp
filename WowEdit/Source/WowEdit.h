#ifndef WOWEDIT_SOURCE_WOWEDIT_H
#define WOWEDIT_SOURCE_WOWEDIT_H

// WowEdit: the world editor's side of the engine (port), built only with WOWDECOMP_TOOL_FEATURES.
//
// Blizzard's world editor ("World Of Warcraft Editor", seen in screenshots running on 3.3.0 data) was its own
// program on the same engine. This port draws on those screenshots, the editor shapes the 4.1 Gxu debug-draw
// module still builds, and the pixel shaders of the 8.1 editor:
//   - WowEditDraw: markers (spawn points, orientation rings, paths, flying path points) on Gxu debug draw.
//   - WowEditShaders / WowEditTerrain: the 8.1 terrain overlay shaders rewritten for Shader Model 3.
//   - App/: the standalone editor, with menus and palettes built with OsGui (App/WowEditApp.h).
// The editor reuses terrain geometry from WoW/Source/WorldClient/WorldEditHooks.h.

#ifdef WOWDECOMP_TOOL_FEATURES

class C3Vector;

// After a terrain overlay is turned on: builds its shaders now, so a missing compiler, a compile error or a card
// without Shader Model 3 shows in the console at once rather than as an overlay that never appears.
void WowEditPrepareTerrainOverlays();

// The editor viewport draws the sample when the Markers view toggle is enabled.
void WowEditSetMarkers(bool show);
bool WowEditGetMarkers();
void WowEditDrawSampleMarkers(const C3Vector& origin, float facing);

#endif

#endif
