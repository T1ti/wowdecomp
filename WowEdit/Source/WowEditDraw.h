#ifndef WOWEDIT_SOURCE_WOWEDITDRAW_H
#define WOWEDIT_SOURCE_WOWEDITDRAW_H

// WowEdit: the world editor's markers (port; no client keeps any editor code).
//
// What is known: Blizzard's world editor ("World Of Warcraft Editor", seen in screenshots running on 3.3.0 data) is
// built on this engine, and its overlays use the editor shapes that the 4.1 Gxu debug-draw module still builds
// (DirectionalSphere, RotationRing, DirectionalDiamond) next to plain half spheres and lines. The screenshots show
// the editor's conventions, which these functions follow:
//   - spawn point: a translucent cyan directional sphere centered on the ground, spike along the spawn's facing
//     (older editor builds: a plain green half sphere), with a rotation ring flat on the ground around it as its
//     orientation and a red line along the facing out to the ring;
//   - path: half spheres at the points, joined by lines (early editor: orange points, red lines, the same look as
//     the 0.5.3 client's DebugTargetPath);
//   - flying path point: a red directional diamond pointing along the path.
//
// What is not known: the editor's own code. The layer here is this port's guess at its shape, kept as thin as the
// evidence allows:
//   - each marker is one call that queues Gxu debug-draw objects for the current frame (the module is immediate mode:
//     GxuDebugDrawRender draws and clears the queue once per frame);
//   - markers can be placed on the ground and tilted to its slope (WOWEDIT_SNAP_TO_GROUND, WOWEDIT_ALIGN_TO_GROUND)
//     with a vertical World::Intersect query, so they follow the terrain the way the editor's do;
//   - editor modules (spawns, paths, ...) do not draw from their own update; they register an overlay callback with
//     the client (WoW/Source/WorldClient/WorldEditHooks.h), which runs every overlay once per frame after the scene
//     and before GxuDebugDrawRender, while the world view is set.
// All colors are defaults taken from the screenshots; every call takes the color to use.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

enum WOWEDIT_PLACEMENT : uint32_t {
    WOWEDIT_PLACE_AS_GIVEN = 0x0,
    // Move the marker down (or up, within 2 yards) onto the first collision surface below its position.
    WOWEDIT_SNAP_TO_GROUND = 0x1,
    // Tilt the marker so its up axis is the ground normal there; its facing is kept, projected into the slope.
    WOWEDIT_ALIGN_TO_GROUND = 0x2,
    WOWEDIT_ON_GROUND = WOWEDIT_SNAP_TO_GROUND | WOWEDIT_ALIGN_TO_GROUND,
};

// Default colors (B, G, R, A), from the editor screenshots.
extern const CImVector WOWEDIT_COLOR_SPAWN; // translucent cyan
extern const CImVector WOWEDIT_COLOR_SPAWN_OLD; // translucent green (early editor)
extern const CImVector WOWEDIT_COLOR_ORIENTATION; // blue-violet ring
extern const CImVector WOWEDIT_COLOR_FACING; // red facing line
extern const CImVector WOWEDIT_COLOR_PATH_POINT; // orange
extern const CImVector WOWEDIT_COLOR_PATH_LINE; // red
extern const CImVector WOWEDIT_COLOR_FLY_POINT; // red

// Ground under a position: the first collision surface (terrain, WMO and M2 collision) on a vertical ray from 2 yards
// above it down to 100 yards below, and its normal from two more rays half a yard away. Returns false when nothing is
// hit; position and normal (straight up) are then left as given.
bool WowEditGetGround(const C3Vector& position, C3Vector* ground, C3Vector* normal);

// Spawn point: directional sphere of the given radius at position, spike along facing (radians, world z rotation).
void WowEditDrawSpawn(const C3Vector& position, float facing, float radius, const CImVector& color, uint32_t placement);
// Orientation ring: flat ring of the given radius around position, with a facing line out to it.
void WowEditDrawOrientation(const C3Vector& position, float facing, float radius, const CImVector& ringColor, const CImVector& lineColor, uint32_t placement);
// Path: a half sphere of the given radius at each point and a line between consecutive points.
void WowEditDrawPath(const C3Vector* points, uint32_t count, float radius, const CImVector& pointColor, const CImVector& lineColor, uint32_t placement);
// Flying path point: a directional diamond lying flat (wide side horizontal), pointing along direction.
void WowEditDrawPathPoint(const C3Vector& position, const C3Vector& direction, float size, const CImVector& color);

#endif

#endif
