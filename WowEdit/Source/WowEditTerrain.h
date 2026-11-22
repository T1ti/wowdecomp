#ifndef WOWEDIT_SOURCE_WOWEDITTERRAIN_H
#define WOWEDIT_SOURCE_WOWEDITTERRAIN_H

// WowEdit: terrain overlays (port).
//
// The editor draws its terrain overlays the way the 8.1 editor shaders show: the terrain mesh again, after the
// terrain, with an editor pixel shader and alpha blending (WowEditShaders.h). The client offers the terrain mesh
// through the terrain pass hook (WoW/Source/WorldClient/WorldEditHooks.h): every render chunk drawn this frame, with
// its vertex and index buffers. Each enabled overlay is one more draw of each chunk, depth tested against the
// terrain (no depth write, a small depth bias so it wins against the terrain it repeats).
//
// The closest thing in the client is CMap::RenderFootprints, which also lays a blended layer on the ground with no
// depth write and a depth bias; it builds its own geometry rather than redrawing the terrain.

#include <cstdint>

// Grid overlay: an 8.1 woweditgrid permutation (0..3), or -1 for none.
static const int32_t WOWEDITGRID_OFF = -1;
static const int32_t WOWEDITGRID_PERMUTATIONS = 4;

void WowEditTerrainSetGrid(int32_t permutation);
int32_t WowEditTerrainGetGrid();
void WowEditTerrainSetSlopes(bool show);
bool WowEditTerrainGetSlopes();
// Port diagnostics, to find where an overlay that draws nothing fails. Each test mode draws every chunk the pass
// covers once more, before the overlays:
enum WOWEDITTEST : int32_t {
    WOWEDITTEST_OFF = 0,
    WOWEDITTEST_SHADER = 1, // the editor vertex shader and a flat magenta pixel shader (WOWEDITSHADER_TEST), depth tested
    WOWEDITTEST_NO_DEPTH = 2, // the same without the depth test: shows through anything in front of the terrain
    WOWEDITTEST_FIXED = 3, // no shaders: the fixed-function pipeline with the chunk transform of RenderSetup, opaque
    WOWEDITTEST_LATE = 4, // as 1, drawn late (CGWorldFrame::OnWorldRender, with the overlays) instead of after the terrain
    NUM_WOWEDITTESTS = 5,
};

void WowEditTerrainSetTest(int32_t mode);
int32_t WowEditTerrainGetTest();
// Port diagnostic: the next terrain pass prints how many pixels its first draw group wrote (an occlusion query), the
// Direct3D state that group ran with (bound shaders, vertex shader constants, vertex declaration, render states,
// viewport, targets), the first vertices it read, and where the first chunk lands on screen.
void WowEditTerrainProbe();
// Builds the shaders the enabled overlays need, so a failure shows in the console when the overlay is turned on;
// false when one is missing.
bool WowEditTerrainPrepare();
// For the status command: terrain passes drawn since an overlay was last switched, and the chunk count of the last.
void WowEditTerrainGetStats(uint32_t* passes, uint32_t* chunks);

#endif
