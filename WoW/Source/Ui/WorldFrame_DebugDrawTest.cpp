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
//     row, and an always-on-top cyan line from the player to each LOD sphere.
// WowEdit's Markers view toggle draws the editor shapes (directional sphere, rotation ring, directional diamond).
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
