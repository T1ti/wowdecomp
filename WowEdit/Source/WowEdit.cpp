#include "WowEdit/Source/WowEdit.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#include "WowEdit/Source/WowEditDraw.h"
#include "WowEdit/Source/WowEditShaders.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEdit.h.

// ---- Markers sample

static bool s_markers;

static C3Vector Along(const C3Vector& from, const C3Vector& forward, const C3Vector& right, float ahead, float side, float up) {
    return C3Vector(from.x + forward.x * ahead + right.x * side, from.y + forward.y * ahead + right.y * side, from.z + up);
}

// The wowEditMarkers overlay (WowEditDraw.h), registered while the command is on: the world editor's markers ahead and to the
// right of the player, the way screenshots of Blizzard's editor show them, placed on the ground under each position
// and tilted to its slope:
//   - spawn points: cyan directional spheres, spike along each spawn's facing, inside an orientation ring with a red
//     facing line (the editor also shows a green line, meaning unknown, left out);
//   - the early editor's look: green half spheres at spawns, and orange half spheres on a red path;
//   - a flying path: points in an arc a few yards up, joined by lines, with a red directional diamond at each point
//     pointing to the next one (in the air, so not placed on the ground).
static void MarkersOverlay(void* param) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayer() ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_PLAYER, __FILE__, __LINE__)) : nullptr;

    if (!player) {
        return;
    }

    C3Vector origin = player->GetPosition();
    float facing = player->GetFacing();
    C3Vector forward(cosf(facing), sinf(facing), 0.0f);
    C3Vector right(sinf(facing), -cosf(facing), 0.0f);

    // Spawn points, facing forward, left and back-right of the player's facing.
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

// ---- Console commands

// "1", "on" and "0", "off"; anything else (or nothing) toggles.
static bool ParseSwitch(const char* arguments, bool current) {
    if (!arguments || !*arguments) {
        return !current;
    }

    if (!SStrCmpI(arguments, "on", STORM_MAX_STR) || !SStrCmpI(arguments, "1", STORM_MAX_STR)) {
        return true;
    }

    if (!SStrCmpI(arguments, "off", STORM_MAX_STR) || !SStrCmpI(arguments, "0", STORM_MAX_STR)) {
        return false;
    }

    return !current;
}

// After an overlay is turned on: builds its shaders now, so a missing compiler, a compile error or a card without
// Shader Model 3 shows in the console at once rather than as an overlay that never appears.
static void PrepareTerrainOverlays() {
    const char* reason = nullptr;

    if (!WowEditShadersSupported(&reason)) {
        ConsoleWriteA("WowEdit: terrain overlays unavailable: %s", ERROR_COLOR, reason);
        return;
    }

    if (!WowEditTerrainPrepare()) {
        ConsoleWrite("WowEdit: terrain overlays unavailable: a shader could not be built (see above)", ERROR_COLOR);
    }
}

static int32_t CCommand_WowEdit(const char* command, const char* arguments) {
    // "wowEdit test [0|1]": the flat magenta diagnostic pass.
    if (arguments && !SStrCmpI(arguments, "test", 4)) {
        const char* value = arguments + 4;

        while (*value == ' ') {
            value++;
        }

        bool show = ParseSwitch(value, WowEditTerrainGetTest());
        WowEditTerrainSetTest(show);
        ConsoleWrite(show ? "WowEdit: test overlay on (flat magenta on every terrain chunk)" : "WowEdit: test overlay off", DEFAULT_COLOR);

        if (show) {
            PrepareTerrainOverlays();
        }

        return 1;
    }

    const char* reason = nullptr;
    bool supported = WowEditShadersSupported(&reason);
    ConsoleWriteA("WowEdit: terrain shaders %s%s%s", DEFAULT_COLOR, supported ? "available" : "unavailable (", supported ? "" : reason, supported ? "" : ")");

    for (uint32_t i = 0; i < NUM_WOWEDITSHADERS; i++) {
        WOWEDITSHADER id = static_cast<WOWEDITSHADER>(i);
        ConsoleWriteA("  shader %s: %s", DEFAULT_COLOR, WowEditShaderName(id), WowEditShaderState(id));
    }

    uint32_t passes;
    uint32_t chunks;
    WowEditTerrainGetStats(&passes, &chunks);
    ConsoleWriteA("  terrain pass: %u frames drawn, %u chunks in the last", DEFAULT_COLOR, passes, chunks);

    int32_t grid = WowEditTerrainGetGrid();

    if (grid >= 0) {
        ConsoleWriteA("  grid: permutation %d", DEFAULT_COLOR, grid);
    } else {
        ConsoleWrite("  grid: off", DEFAULT_COLOR);
    }

    ConsoleWriteA("  slopes: %s", DEFAULT_COLOR, WowEditTerrainGetSlopes() ? "on" : "off");
    ConsoleWriteA("  test: %s", DEFAULT_COLOR, WowEditTerrainGetTest() ? "on" : "off");
    ConsoleWriteA("  markers: %s", DEFAULT_COLOR, s_markers ? "on" : "off");
    return 1;
}

static int32_t CCommand_WowEditGrid(const char* command, const char* arguments) {
    int32_t grid;

    if (!arguments || !*arguments) {
        grid = WowEditTerrainGetGrid() >= 0 ? WOWEDITGRID_OFF : 3;
    } else if (!SStrCmpI(arguments, "off", STORM_MAX_STR)) {
        grid = WOWEDITGRID_OFF;
    } else {
        grid = SStrToInt(arguments);

        if (grid < 0 || grid >= WOWEDITGRID_PERMUTATIONS) {
            ConsoleWrite("Usage: wowEditGrid [0-3|off]", ERROR_COLOR);
            return 1;
        }
    }

    WowEditTerrainSetGrid(grid);

    if (grid >= 0) {
        ConsoleWriteA("WowEdit: grid permutation %d", DEFAULT_COLOR, grid);
        PrepareTerrainOverlays();
    } else {
        ConsoleWrite("WowEdit: grid off", DEFAULT_COLOR);
    }

    return 1;
}

static int32_t CCommand_WowEditSlopes(const char* command, const char* arguments) {
    bool show = ParseSwitch(arguments, WowEditTerrainGetSlopes());
    WowEditTerrainSetSlopes(show);
    ConsoleWrite(show ? "WowEdit: impassable slopes on" : "WowEdit: impassable slopes off", DEFAULT_COLOR);

    if (show) {
        PrepareTerrainOverlays();
    }

    return 1;
}

static int32_t CCommand_WowEditMarkers(const char* command, const char* arguments) {
    s_markers = ParseSwitch(arguments, s_markers);

    if (s_markers) {
        WorldEditRegisterOverlay(MarkersOverlay, nullptr);
    } else {
        WorldEditUnregisterOverlay(MarkersOverlay, nullptr);
    }

    ConsoleWrite(s_markers ? "WowEdit: markers on" : "WowEdit: markers off", DEFAULT_COLOR);
    return 1;
}

void WowEditRegisterConsoleCommands() {
    ConsoleCommandRegister("wowEdit", CCommand_WowEdit, CATEGORY_DEBUG, "WowEdit: shader support and overlays in use");
    ConsoleCommandRegister("wowEditGrid", CCommand_WowEditGrid, CATEGORY_DEBUG, "WowEdit: terrain grid [0-3|off]");
    ConsoleCommandRegister("wowEditSlopes", CCommand_WowEditSlopes, CATEGORY_DEBUG, "WowEdit: impassable slopes [0|1]");
    ConsoleCommandRegister("wowEditMarkers", CCommand_WowEditMarkers, CATEGORY_DEBUG, "WowEdit: sample editor markers [0|1]");
}

#endif
