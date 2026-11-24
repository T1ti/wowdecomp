#include "WowEdit/Source/WowEdit.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "WowEdit/Source/WowEditDraw.h"
#include "WowEdit/Source/WowEditShaders.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEdit.h.

// ---- Markers sample

static bool s_markers;

static C3Vector Along(const C3Vector& from, const C3Vector& forward, const C3Vector& right, float ahead, float side, float up) {
    return C3Vector(from.x + forward.x * ahead + right.x * side, from.y + forward.y * ahead + right.y * side, from.z + up);
}

// Sample markers near the editor camera, based on screenshots of Blizzard's editor, placed on the ground
// and tilted to its slope:
//   - spawn points: cyan directional spheres, spike along each spawn's facing, inside an orientation ring with a red
//     facing line (the editor also shows a green line, meaning unknown, left out);
//   - the early editor's look: green half spheres at spawns, and orange half spheres on a red path;
//   - a flying path: points in an arc a few yards up, joined by lines, with a red directional diamond at each point
//     pointing to the next one (in the air, so not placed on the ground).
void WowEditDrawSampleMarkers(const C3Vector& origin, float facing) {
    C3Vector forward(cosf(facing), sinf(facing), 0.0f);
    C3Vector right(sinf(facing), -cosf(facing), 0.0f);

    // Spawn points, facing forward, left and back-right of the camera heading.
    const float spawnTurns[3] = { 0.0f, 1.5707964f, 3.6651914f };

    for (uint32_t i = 0; i < 3; i++) {
        C3Vector position = Along(origin, forward, right, 12.0f, 4.0f + static_cast<float>(i) * 4.0f, 0.0f);
        WowEditDrawSpawn(position, facing + spawnTurns[i], 0.6f, WOWEDIT_COLOR_SPAWN, WOWEDIT_ON_GROUND);
        WowEditDrawOrientation(position, facing + spawnTurns[i], 1.6f, WOWEDIT_COLOR_ORIENTATION, WOWEDIT_COLOR_FACING, WOWEDIT_ON_GROUND);
    }

    GxuDebugDrawText3D("spawn points", Along(origin, forward, right, 12.0f, 8.0f, 2.0f), WOWEDIT_COLOR_SPAWN);

    // Early editor.
    for (uint32_t i = 0; i < 2; i++) {
        C3Vector position = Along(origin, forward, right, 20.0f, 4.0f + static_cast<float>(i) * 3.0f, 0.0f);
        WowEditDrawPath(&position, 1, 1.2f, WOWEDIT_COLOR_SPAWN_OLD, WOWEDIT_COLOR_SPAWN_OLD, WOWEDIT_ON_GROUND);
    }

    C3Vector groundPath[3];

    for (uint32_t i = 0; i < 3; i++) {
        groundPath[i] = Along(origin, forward, right, 18.0f + static_cast<float>(i) * 4.0f, 10.0f + static_cast<float>(i & 1) * 2.0f, 0.0f);
    }

    WowEditDrawPath(groundPath, 3, 0.4f, WOWEDIT_COLOR_PATH_POINT, WOWEDIT_COLOR_PATH_LINE, WOWEDIT_ON_GROUND);
    GxuDebugDrawText3D("early editor", Along(origin, forward, right, 20.0f, 7.0f, 2.0f), WOWEDIT_COLOR_SPAWN_OLD);

    // Flying path.
    const uint32_t pathCount = 6;
    C3Vector path[pathCount];

    for (uint32_t i = 0; i < pathCount; i++) {
        float t = static_cast<float>(i) / static_cast<float>(pathCount - 1);
        path[i] = Along(origin, forward, right, 16.0f + sinf(t * 3.1415927f) * 4.0f, 3.0f + t * 9.0f, 5.0f + t * 3.0f);
    }

    for (uint32_t i = 0; i < pathCount; i++) {
        uint32_t from = i + 1 < pathCount ? i : i - 1;
        C3Vector direction(path[from + 1].x - path[from].x, path[from + 1].y - path[from].y, path[from + 1].z - path[from].z);
        WowEditDrawPathPoint(path[i], direction, 0.8f, WOWEDIT_COLOR_FLY_POINT);

        if (i) {
            CImVector line = WOWEDIT_COLOR_PATH_LINE;
            line.a = 0xA0;
            GxuDebugDrawLine(path[i - 1], path[i], line);
        }
    }

    const C3Vector& middle = path[pathCount / 2];
    GxuDebugDrawText3D("flying path", C3Vector(middle.x, middle.y, middle.z + 1.5f), WOWEDIT_COLOR_FLY_POINT);
}

// See WowEdit.h.
void WowEditPrepareTerrainOverlays() {
    const char* reason = nullptr;

    if (!WowEditShadersSupported(&reason)) {
        ConsoleWriteA("WowEdit: terrain overlays unavailable: %s", ERROR_COLOR, reason);
        return;
    }

    if (!WowEditTerrainPrepare()) {
        ConsoleWrite("WowEdit: terrain overlays unavailable: a shader could not be built (see above)", ERROR_COLOR);
    }
}

void WowEditSetMarkers(bool show) {
    s_markers = show;
}

bool WowEditGetMarkers() {
    return s_markers;
}

#endif
