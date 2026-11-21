#include "WoW/Source/Ui/WorldFrame_DebugDrawTest.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// Tool features: console command "debugDrawTest [0|1]" (port, not from any client). While on, every world frame
// queues one of each Gxu debug-draw primitive in front of the active player, so the module can be checked in game:
//   - a row of shapes 8 yards ahead, left to right: solid box (red), wireframe sphere (green), inside-faces cone
//     (blue), double-sided triangle (yellow), always-on-top sphere (half transparent magenta);
//   - three wireframe spheres 25 yards ahead, sized so they draw at sphere LOD 0, 1 and 2;
//   - a 3D label above each shape, and a line of screen text and an icon texture under the player frame;
//   - lines: a red/green/blue axis tripod at the player's feet (world X, Y, Z), a white wire box around the shape
//     row, and an always-on-top cyan line from the player to each LOD sphere;
//   - the 4.1 editor shapes as screenshots of Blizzard's world editor show them, ahead and to the right: spawn
//     points with orientation rings, the early editor's half spheres and path, and a flying path with diamonds.
// Without an argument the command toggles.

static bool s_debugDrawTest;
static CTexture* s_debugDrawTestTexture;

static int32_t CCommand_DebugDrawTest(const char* command, const char* arguments) {
    s_debugDrawTest = (arguments && *arguments) ? SStrToInt(arguments) != 0 : !s_debugDrawTest;
    ConsoleWrite(s_debugDrawTest ? "Debug draw test enabled" : "Debug draw test disabled", DEFAULT_COLOR);

    return 1;
}

void GxuDebugDrawTestRegister() {
    ConsoleCommandRegister("debugDrawTest", CCommand_DebugDrawTest, CATEGORY_DEBUG, "Draws one of each Gxu debug-draw primitive in front of the player");
}

// Unit shape -> world: scaled, turned to the player's facing, then placed at position.
static C44Matrix ShapeTransform(const C3Vector& position, float facing, float scale) {
    C44Matrix transform;
    transform.Translate(position);
    transform.RotateAroundZ(facing);
    transform.Scale(scale);

    return transform;
}

// Unit shape -> world from explicit axes: the shape's local x, y and z land on xAxis, yAxis and zAxis (scaled), then
// the result is placed at position.
static C44Matrix AxesTransform(const C3Vector& position, const C3Vector& xAxis, const C3Vector& yAxis, const C3Vector& zAxis, const C3Vector& scale) {
    C44Matrix transform;
    transform.a0 = xAxis.x * scale.x;
    transform.a1 = xAxis.y * scale.x;
    transform.a2 = xAxis.z * scale.x;
    transform.b0 = yAxis.x * scale.y;
    transform.b1 = yAxis.y * scale.y;
    transform.b2 = yAxis.z * scale.y;
    transform.c0 = zAxis.x * scale.z;
    transform.c1 = zAxis.y * scale.z;
    transform.c2 = zAxis.z * scale.z;
    transform.d0 = position.x;
    transform.d1 = position.y;
    transform.d2 = position.z;

    return transform;
}

static C3Vector Normalized(const C3Vector& v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return length > 0.0f ? C3Vector(v.x / length, v.y / length, v.z / length) : C3Vector(0.0f, 0.0f, 1.0f);
}

static C3Vector Cross(const C3Vector& a, const C3Vector& b) {
    return C3Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static C3Vector Along(const C3Vector& from, const C3Vector& forward, const C3Vector& right, float ahead, float side, float up) {
    return C3Vector(from.x + forward.x * ahead + right.x * side, from.y + forward.y * ahead + right.y * side, from.z + up);
}

// The 4.1 editor shapes, and the plain spheres, the way screenshots of Blizzard's world editor show them, ahead and
// to the right of the player (heights are the player's ground height, so on sloped ground some sink or float):
//   - spawn points: translucent cyan directional spheres centered on the ground, the spike along each spawn's facing
//     (the ground hides the lower half), each with a rotation ring flat on the ground around it as its orientation
//     indicator and a red line out to the ring along the facing (the editor also shows a green line, meaning
//     unknown, left out);
//   - the early editor's look: translucent green half spheres at spawns, and orange half spheres on a red path;
//   - a flying path: points in an arc a few yards up, joined by lines, with a red directional diamond at each point
//     lying flat and pointing to the next one.
static void QueueEditorShapes(const C3Vector& origin, const C3Vector& forward, const C3Vector& right) {
    const C3Vector up(0.0f, 0.0f, 1.0f);
    const C3Vector left(-right.x, -right.y, 0.0f);

    // Spawn points, facing forward, left and back-right.
    const float spawnTurns[3] = { 0.0f, 1.5707964f, 3.6651914f };
    const CImVector cyan(0xFF, 0xFF, 0x00, 0x80);
    const CImVector ringColor(0xFF, 0x40, 0x80, 0xFF);
    const CImVector red(0x20, 0x20, 0xFF, 0xFF);

    for (uint32_t i = 0; i < 3; i++) {
        C3Vector position = Along(origin, forward, right, 12.0f, 4.0f + static_cast<float>(i) * 4.0f, 0.0f);
        float c = cosf(spawnTurns[i]);
        float s = sinf(spawnTurns[i]);
        // Facing turned left from forward by the angle; (side, up, facing) is right handed.
        C3Vector facing(forward.x * c + left.x * s, forward.y * c + left.y * s, 0.0f);
        C3Vector side(Cross(up, facing));
        GxuDebugDrawShape(GXUDEBUGSHAPE_DIRECTIONAL_SPHERE, AxesTransform(position, side, up, facing, C3Vector(0.6f, 0.6f, 0.6f)), cyan, GXUDEBUGSTYLE_SOLID);

        // Ring of radius 1.6 around the z axis, its 4 unit band flattened to 0.06.
        GxuDebugDrawShape(GXUDEBUGSHAPE_ROTATION_RING, AxesTransform(position, facing, side, up, C3Vector(1.6f, 1.6f, 0.015f)), ringColor, GXUDEBUGSTYLE_SOLID);
        GxuDebugDrawLine(position, C3Vector(position.x + facing.x * 1.6f, position.y + facing.y * 1.6f, position.z), red);
    }

    GxuDebugDrawText3D("spawn points", Along(origin, forward, right, 12.0f, 8.0f, 2.0f), cyan);

    // Early editor: green spawn half spheres, and an orange-pointed red path.
    const CImVector green(0x20, 0xE0, 0x40, 0xA0);
    const CImVector orange(0x00, 0xA0, 0xFF, 0xFF);

    for (uint32_t i = 0; i < 2; i++) {
        C3Vector position = Along(origin, forward, right, 20.0f, 4.0f + static_cast<float>(i) * 3.0f, 0.0f);
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, AxesTransform(position, forward, left, up, C3Vector(1.2f, 1.2f, 1.2f)), green, GXUDEBUGSTYLE_SOLID);
    }

    C3Vector previous;

    for (uint32_t i = 0; i < 3; i++) {
        C3Vector point = Along(origin, forward, right, 18.0f + static_cast<float>(i) * 4.0f, 10.0f + static_cast<float>(i & 1) * 2.0f, 0.0f);
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, AxesTransform(point, forward, left, up, C3Vector(0.4f, 0.4f, 0.4f)), orange, GXUDEBUGSTYLE_SOLID);

        if (i) {
            GxuDebugDrawLine(previous, point, red);
        }

        previous = point;
    }

    GxuDebugDrawText3D("early editor", Along(origin, forward, right, 20.0f, 7.0f, 2.0f), green);

    // Flying path.
    const uint32_t pathCount = 6;
    C3Vector path[pathCount];

    for (uint32_t i = 0; i < pathCount; i++) {
        float t = static_cast<float>(i) / static_cast<float>(pathCount - 1);
        path[i] = Along(origin, forward, right, 16.0f + sinf(t * 3.1415927f) * 4.0f, 3.0f + t * 9.0f, 5.0f + t * 3.0f);
    }

    for (uint32_t i = 0; i < pathCount; i++) {
        uint32_t from = i + 1 < pathCount ? i : i - 1;
        C3Vector direction = Normalized(C3Vector(path[from + 1].x - path[from].x, path[from + 1].y - path[from].y, path[from + 1].z - path[from].z));
        // The diamond is wide along x and thin along y: keep x horizontal so it lies flat.
        C3Vector xAxis = Normalized(Cross(direction, up));
        C3Vector yAxis = Cross(direction, xAxis);
        GxuDebugDrawShape(GXUDEBUGSHAPE_DIRECTIONAL_DIAMOND, AxesTransform(path[i], xAxis, yAxis, direction, C3Vector(0.8f, 0.8f, 0.8f)), red, GXUDEBUGSTYLE_SOLID);

        if (i) {
            GxuDebugDrawLine(path[i - 1], path[i], CImVector(0x20, 0x20, 0xFF, 0xA0));
        }
    }

    const C3Vector& middle = path[pathCount / 2];
    GxuDebugDrawText3D("flying path", C3Vector(middle.x, middle.y, middle.z + 1.5f), red);
}

void GxuDebugDrawTestQueue() {
    if (!s_debugDrawTest || !ClntObjMgrGetActivePlayer()) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    C3Vector origin = player->GetPosition();
    float facing = player->GetFacing();
    C3Vector forward(cosf(facing), sinf(facing), 0.0f);
    C3Vector right(sinf(facing), -cosf(facing), 0.0f);

    struct TestShape {
        GXUDEBUGSHAPE shape;
        GXUDEBUGSTYLE style;
        CImVector color; // b, g, r, a
        const char* label;
    };

    const TestShape shapes[] = {
        { GXUDEBUGSHAPE_BOX, GXUDEBUGSTYLE_SOLID, CImVector(0x20, 0x20, 0xFF, 0xFF), "box / solid" },
        { GXUDEBUGSHAPE_SPHERE, GXUDEBUGSTYLE_WIREFRAME, CImVector(0x20, 0xFF, 0x20, 0xFF), "sphere / wireframe" },
        { GXUDEBUGSHAPE_CONE, GXUDEBUGSTYLE_SOLID_BACKFACING, CImVector(0xFF, 0x40, 0x20, 0xFF), "cone / backfacing" },
        { GXUDEBUGSHAPE_TRIANGLE, GXUDEBUGSTYLE_SOLID, CImVector(0x20, 0xFF, 0xFF, 0xFF), "triangle / solid" },
        { GXUDEBUGSHAPE_SPHERE, GXUDEBUGSTYLE_SOLID_NODEPTH, CImVector(0xFF, 0x20, 0xFF, 0x80), "sphere / no depth" },
    };

    uint32_t count = sizeof(shapes) / sizeof(shapes[0]);

    for (uint32_t i = 0; i < count; i++) {
        float side = (static_cast<float>(i) - static_cast<float>(count - 1) * 0.5f) * 3.0f;
        C3Vector position(
            origin.x + forward.x * 8.0f + right.x * side,
            origin.y + forward.y * 8.0f + right.y * side,
            origin.z + 1.5f);

        // Spheres are unit radius, the others unit size: scale everything to about 2 yards across.
        float scale = shapes[i].shape == GXUDEBUGSHAPE_SPHERE ? 1.0f : 2.0f;
        GxuDebugDrawShape(shapes[i].shape, ShapeTransform(position, facing, scale), shapes[i].color, shapes[i].style);

        C3Vector labelPosition(position.x, position.y, position.z + 1.8f);
        GxuDebugDrawText3D(shapes[i].label, labelPosition, shapes[i].color);
    }

    // Sphere LODs: the drawer picks the LOD from distance / scale, so at the same distance (about 30 yards from the
    // camera) radius 1, 0.4 and 0.2 land on LOD 0, 1 and 2 (full, half and quarter grid).
    const float lodScales[GxuSphereShapeFactory::NUM_LODS] = { 1.0f, 0.4f, 0.2f };
    const char* lodLabels[GxuSphereShapeFactory::NUM_LODS] = { "LOD 0", "LOD 1", "LOD 2" };

    for (uint32_t lod = 0; lod < GxuSphereShapeFactory::NUM_LODS; lod++) {
        float side = (static_cast<float>(lod) - 1.0f) * 3.0f;
        C3Vector position(
            origin.x + forward.x * 25.0f + right.x * side,
            origin.y + forward.y * 25.0f + right.y * side,
            origin.z + 4.0f);
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, ShapeTransform(position, facing, lodScales[lod]), CImVector(0x20, 0xFF, 0x20, 0xFF), GXUDEBUGSTYLE_WIREFRAME);

        C3Vector labelPosition(position.x, position.y, position.z + 1.5f);
        GxuDebugDrawText3D(lodLabels[lod], labelPosition, CImVector(0x20, 0xFF, 0x20, 0xFF));

        C3Vector chest(origin.x, origin.y, origin.z + 1.0f);
        GxuDebugDrawLine(chest, position, CImVector(0xFF, 0xFF, 0x20, 0xFF), GXUDEBUGSTYLE_SOLID_NODEPTH);
    }

    // Axis tripod at the feet, 2 yards per axis.
    GxuDebugDrawLine(origin, C3Vector(origin.x + 2.0f, origin.y, origin.z), CImVector(0x20, 0x20, 0xFF, 0xFF));
    GxuDebugDrawLine(origin, C3Vector(origin.x, origin.y + 2.0f, origin.z), CImVector(0x20, 0xFF, 0x20, 0xFF));
    GxuDebugDrawLine(origin, C3Vector(origin.x, origin.y, origin.z + 2.0f), CImVector(0xFF, 0x20, 0x20, 0xFF));

    // Wire box enclosing the shape row, turned with the player: local x along forward, y along right, z up. (A
    // world-aligned box around the turning row would grow and shrink as the facing changes.)
    float halfRow = (static_cast<float>(count - 1) * 0.5f) * 3.0f + 1.5f;
    C44Matrix rowFrame;
    rowFrame.a0 = forward.x;
    rowFrame.a1 = forward.y;
    rowFrame.a2 = 0.0f;
    rowFrame.b0 = right.x;
    rowFrame.b1 = right.y;
    rowFrame.b2 = 0.0f;
    rowFrame.d0 = origin.x + forward.x * 8.0f;
    rowFrame.d1 = origin.y + forward.y * 8.0f;
    rowFrame.d2 = origin.z + 1.5f;
    GxuDebugDrawBoxLines(C3Vector(-1.5f, -halfRow, -1.5f), C3Vector(1.5f, halfRow, 1.5f), rowFrame, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    QueueEditorShapes(origin, forward, right);

    // Below the player frame, which covers the top left corner.
    GxuDebugDrawText("GxuDebugDraw test (debugDrawTest 0 to hide)", 0.02f, 0.16f, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    // TextureCacheGetTexture only finds textures something else already loaded; TextureCreate loads the file.
    if (!s_debugDrawTestTexture) {
        CStatus status;
        s_debugDrawTestTexture = TextureCreate("Interface\\Icons\\INV_Misc_QuestionMark", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1, 0, 0, 0), &status, 0);
    }

    // The texture loads asynchronously; nothing is queued until it is ready.
    CGxTex* gxTex = s_debugDrawTestTexture ? TextureGetGxTex(s_debugDrawTestTexture, 0, nullptr) : nullptr;

    if (gxTex) {
        GxuDebugDrawTexture(gxTex, 0.02f, 0.19f, 0.06f, 0.08f);
    }
}

#endif
