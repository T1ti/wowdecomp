#ifndef WOW_SOURCE_WORLDCLIENT_WORLDEDITDRAW_H
#define WOW_SOURCE_WORLDCLIENT_WORLDEDITDRAW_H

// Tool features: the world editor's overlay layer (port; no client keeps any of it).
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
//   - markers can be placed on the ground and tilted to its slope (WORLDEDIT_SNAP_TO_GROUND, WORLDEDIT_ALIGN_TO_GROUND)
//     with a vertical World::Intersect query, so they follow the terrain the way the editor's do;
//   - editor modules (spawns, paths, ...) do not draw from their own update; they register an overlay callback, and
//     CGWorldFrame::OnWorldRender runs every registered overlay once per frame (WorldEditRenderOverlays), after the
//     scene and before GxuDebugDrawRender, while the world view is set.
// All colors are defaults taken from the screenshots; every call takes the color to use.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

enum WORLDEDIT_PLACEMENT : uint32_t {
    WORLDEDIT_PLACE_AS_GIVEN = 0x0,
    // Move the marker down (or up, within 2 yards) onto the first collision surface below its position.
    WORLDEDIT_SNAP_TO_GROUND = 0x1,
    // Tilt the marker so its up axis is the ground normal there; its facing is kept, projected into the slope.
    WORLDEDIT_ALIGN_TO_GROUND = 0x2,
    WORLDEDIT_ON_GROUND = WORLDEDIT_SNAP_TO_GROUND | WORLDEDIT_ALIGN_TO_GROUND,
};

// Default colors (B, G, R, A), from the editor screenshots.
extern const CImVector WORLDEDIT_COLOR_SPAWN; // translucent cyan
extern const CImVector WORLDEDIT_COLOR_SPAWN_OLD; // translucent green (early editor)
extern const CImVector WORLDEDIT_COLOR_ORIENTATION; // blue-violet ring
extern const CImVector WORLDEDIT_COLOR_FACING; // red facing line
extern const CImVector WORLDEDIT_COLOR_PATH_POINT; // orange
extern const CImVector WORLDEDIT_COLOR_PATH_LINE; // red
extern const CImVector WORLDEDIT_COLOR_FLY_POINT; // red

// Ground under a position: the first collision surface (terrain, WMO and M2 collision) on a vertical ray from 2 yards
// above it down to 100 yards below, and its normal from two more rays half a yard away. Returns false when nothing is
// hit; position and normal (straight up) are then left as given.
bool WorldEditGetGround(const C3Vector& position, C3Vector* ground, C3Vector* normal);

// Spawn point: directional sphere of the given radius at position, spike along facing (radians, world z rotation).
void WorldEditDrawSpawn(const C3Vector& position, float facing, float radius, const CImVector& color, uint32_t placement);
// Orientation ring: flat ring of the given radius around position, with a facing line out to it.
void WorldEditDrawOrientation(const C3Vector& position, float facing, float radius, const CImVector& ringColor, const CImVector& lineColor, uint32_t placement);
// Path: a half sphere of the given radius at each point and a line between consecutive points.
void WorldEditDrawPath(const C3Vector* points, uint32_t count, float radius, const CImVector& pointColor, const CImVector& lineColor, uint32_t placement);
// Flying path point: a directional diamond lying flat (wide side horizontal), pointing along direction.
void WorldEditDrawPathPoint(const C3Vector& position, const C3Vector& direction, float size, const CImVector& color);

// Overlay callbacks, run once per frame by WorldEditRenderOverlays. Registering the same callback and parameter twice
// keeps one entry.
typedef void (*WORLDEDITOVERLAY)(void* param);
void WorldEditRegisterOverlay(WORLDEDITOVERLAY overlay, void* param);
void WorldEditUnregisterOverlay(WORLDEDITOVERLAY overlay, void* param);
// From CGWorldFrame::OnWorldRender, before GxuDebugDrawRender.
void WorldEditRenderOverlays();

#endif

#endif
