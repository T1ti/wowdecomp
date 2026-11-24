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
// Overlays: the 8.1 overlays shader (per-chunk and per-cell colors) showing 3.3.5 terrain data.
enum WOWEDITOVERLAY : int32_t {
    WOWEDITOVERLAY_OFF = 0,
    WOWEDITOVERLAY_AREAS = 1, // each chunk in a color of its area ID, and the area under the cursor named
    NUM_WOWEDITOVERLAYS = 2,
};

void WowEditTerrainSetOverlay(int32_t overlay);
int32_t WowEditTerrainGetOverlay();
// Terrain brush: the 8.1 editor's brush cursor, laid on the terrain under the mouse cursor: a dashed red-orange circle
// at radius and a dashed yellow one at innerRadius (the falloff radius; 0 or not below radius: none). Yards; a radius
// of 0 turns it off.
static const float WOWEDITBRUSH_MAX_RADIUS = 533.33333f; // one map tile
void WowEditTerrainSetBrush(float radius, float innerRadius);
float WowEditTerrainGetBrush();
float WowEditTerrainGetBrushInner();
// Builds the shaders the enabled overlays need, so a failure shows in the console when the overlay is turned on;
// false when one is missing.
bool WowEditTerrainPrepare();

#endif
