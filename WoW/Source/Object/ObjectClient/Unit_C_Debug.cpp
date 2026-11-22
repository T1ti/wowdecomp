#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/CAaBox.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

static CVar* s_cvDebugTargetPath;

// Debug feature: console command "CombatDebugForceActionOn".
// 0.5.3 reference: CCommand_ForceActionSet (0x005E17C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionSet(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOff".
// 0.5.3 reference: CCommand_ForceActionUnset (0x005E1920).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionUnset(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOtherOn".
// 0.5.3 reference: CCommand_ForceActionOnOtherSet (0x005E1970).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionOnOtherSet(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOtherOff".
// 0.5.3 reference: CCommand_ForceActionOnOtherUnset (0x005E19C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionOnOtherUnset(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugShowFlags".
// 0.5.3 reference: CCommand_ForceActionShowFlags (0x005E1A00).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionShowFlags(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "DumpDeathHoldLogs".
// 0.5.3 reference: CCommand_DumpDeathHoldLogs (0x005E1530).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_DumpDeathHoldLogs(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "playercombatlogdebug".
// 0.5.3 reference: DebugCombatLogHandler (0x0061D670).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t DebugCombatLogHandler(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: combat force-action flags display.
// 0.5.3 reference: ShowForceActionFlags (0x005DE6F0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static void ShowForceActionFlags() {
}

// Debug feature: combat debug log.
// 0.5.3 reference: UnitCombatDebugLogEnable (0x0061D730).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void UnitCombatDebugLogEnable(BOOL enable) {
}

// 0.5.3 reference: UnitDebugCombatLogOnEnable (0x0061CD50).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void UnitDebugCombatLogOnEnable(BOOL enable) {
}

// Debug feature: unit pathing display.
// 0.5.3 reference: CGUnit_C::RenderDebugPathing (0x005FD3E0), fed by CGUnit_C::OnMonsterMove (0x005F8EF0).
//
// 0.5.3 kept the path of every SMSG_MONSTER_MOVE in the unit (m_debugPathPoints, m_numDebugPathNodes, m_serverLoc):
// OnMonsterMove stored the spline's control points (with its lead-in points: one behind the unit, then the unit), and
// for a flying spline (move flag 0x200, the Catmull-Rom splines taxi flights use) appended the unit position and the
// curve itself, sampled with C3Spline::Frame by arc length every 1 / (3 x length), about every third of a yard.
// RenderDebugPathing then drew, with DebugTargetPath set:
//   - flying spline in progress: a green solid sphere of radius 0.5 (ModelCreateSolidSphere, CImVector 0xFF00FF00)
//     at each control point, and a white line strip (GxPrim_LineStrip, depth tested) through the sampled curve;
//   - otherwise (ground moves, or the unit stopped): projected selection circles (SetCircleRenderStates +
//     ProjectTex2d, 1 x 1 yard, 2 yards deep, fade 0.5), green at the server location and red at each path point,
//     so they lie on the ground.
//
// 3.3.5 keeps no copy of the move, but the unit's movement spline (CMoveSpline, built from the same message) holds the
// same control points and the same Catmull-Rom curve (C3Spline_CatmullRom in MODE_CATMULLROM for flying splines,
// MODE_LINEAR for ground ones), so this draws from it: spheres and the sampled curve through the Gxu debug-draw module
// for a Catmull-Rom spline, projected circles for a linear one (queued here, drawn by RenderDebugPathingCircles during
// the world render, where the client draws its selection circles). The server location is the spline's first point.
// 3.3.5 does not keep the path of a stopped unit, so this keeps a copy of the last spline's points for the shown
// units (DebugPathLast), and draws its circles once the spline is done, as 0.5.3 drew the stored path of a stopped
// unit. Spline points of a unit on a transport are transport-local and are moved to world space with the transport
// matrix.
//
// The circles are the 0.5.3 size, 1 yard across, so they are small from the usual camera distance.

// The last path of a shown unit: its spline's control points, still drawn after the spline is done (0.5.3 kept
// m_debugPathPoints in the unit until the next move message).
struct DebugPathLast {
    WOWGUID m_guid;
    uint32_t m_splineId;
    WOWGUID m_transportGuid;
    TSGrowableArray<C3Vector> m_points;
};

// The sampled curve of a flying spline, kept while the same spline (unit, spline id, point count) is shown: 0.5.3
// sampled once per move message, and a taxi flight's curve runs to thousands of samples.
struct DebugPathCurve {
    WOWGUID m_guid;
    uint32_t m_splineId;
    uint32_t m_pointCount;
    TSGrowableArray<C3Vector> m_samples;
};

static const uint32_t DEBUG_PATH_CURVES = 4; // locked target and player, with room to spare
static const uint32_t DEBUG_PATH_MAX_SAMPLES = 0x4000; // port cap (0.5.3 had none)
static DebugPathCurve s_debugPathCurves[DEBUG_PATH_CURVES];
static uint32_t s_debugPathCurveNext;
static DebugPathLast s_debugPathLast[DEBUG_PATH_CURVES];
static uint32_t s_debugPathLastNext;

static DebugPathLast* FindDebugPathLast(const WOWGUID& guid) {
    for (uint32_t i = 0; i < DEBUG_PATH_CURVES; i++) {
        if (s_debugPathLast[i].m_guid == guid) {
            return &s_debugPathLast[i];
        }
    }

    return nullptr;
}

// Copies the points of the unit's spline when it is a new one.
static void RememberDebugPath(const WOWGUID& guid, CMoveSpline* spline, const WOWGUID& transportGuid) {
    DebugPathLast* last = FindDebugPathLast(guid);

    if (!last) {
        last = &s_debugPathLast[s_debugPathLastNext];
        s_debugPathLastNext = (s_debugPathLastNext + 1) % DEBUG_PATH_CURVES;
        last->m_guid = guid;
    } else if (last->m_splineId == spline->m_id && last->m_points.m_count == spline->m_spline.m_pointCount) {
        return;
    }

    C3Spline_CatmullRom& path = spline->m_spline;
    last->m_splineId = spline->m_id;
    last->m_transportGuid = transportGuid;
    last->m_points.SetCount(path.m_pointCount);

    for (uint32_t i = 0; i < path.m_pointCount; i++) {
        last->m_points.m_data[i] = path.Point(i);
    }
}

static const TSGrowableArray<C3Vector>& SampleDebugPathCurve(const WOWGUID& guid, CMoveSpline* spline) {
    C3Spline_CatmullRom& path = spline->m_spline;

    for (uint32_t i = 0; i < DEBUG_PATH_CURVES; i++) {
        DebugPathCurve& curve = s_debugPathCurves[i];

        if (curve.m_guid == guid && curve.m_splineId == spline->m_id && curve.m_pointCount == path.m_pointCount) {
            return curve.m_samples;
        }
    }

    DebugPathCurve& curve = s_debugPathCurves[s_debugPathCurveNext];
    s_debugPathCurveNext = (s_debugPathCurveNext + 1) % DEBUG_PATH_CURVES;
    curve.m_guid = guid;
    curve.m_splineId = spline->m_id;
    curve.m_pointCount = path.m_pointCount;
    curve.m_samples.SetCount(0);

    // C3Spline::SetPoints computes the length only for more than 3 points; with fewer the curve is not evaluated.
    if (path.m_pointCount <= 3 || path.cachedLength <= 0.0f) {
        return curve.m_samples;
    }

    float step = 1.0f / (path.cachedLength * 3.0f);

    if (step < 1.0f / DEBUG_PATH_MAX_SAMPLES) {
        step = 1.0f / DEBUG_PATH_MAX_SAMPLES;
    }

    for (float t = 0.0f; t < 1.0f; t += step) {
        path.Pos(t, *curve.m_samples.New(), C3Spline::EVAL_ARCLENGTH);
    }

    return curve.m_samples;
}

// Circles queued for this frame's world render.
struct DebugPathCircle {
    C3Vector m_position;
    CImVector m_color;
};

static TSGrowableArray<DebugPathCircle> s_debugPathCircles;

static void QueueDebugPathCircle(const C3Vector& position, uint32_t argb) {
    DebugPathCircle* circle = s_debugPathCircles.New();
    circle->m_position = position;
    *reinterpret_cast<uint32_t*>(&circle->m_color) = argb;
}

void CGUnit_C::RenderDebugPathing() {
    const WOWGUID& guid = this->m_obj->m_guid;
    CMoveSpline* spline = this->m_movementData.m_spline;
    bool moving = spline && !(spline->m_flags & SPLINEFLAG_DONE) && spline->m_spline.m_pointCount;

    if (moving) {
        RememberDebugPath(guid, spline, this->m_movementData.m_transportGuid);
    }

    if (!moving || spline->m_spline.GetSplineMode() != MODE_CATMULLROM) {
        // Ground move, or the unit stopped: a green circle at the server location, red circles at the path points.
        DebugPathLast* last = FindDebugPathLast(guid);

        if (!last) {
            return;
        }

        C44Matrix lastTransport;
        bool lastOnTransport = last->m_transportGuid && MovementGetTransportMtxSafe(last->m_transportGuid, &lastTransport);

        for (uint32_t i = 0; i < last->m_points.m_count; i++) {
            C3Vector point = last->m_points.m_data[i];

            if (lastOnTransport) {
                point = point * lastTransport;
            }

            QueueDebugPathCircle(point, i ? 0xFFFF0000 : 0xFF00FF00);
        }

        return;
    }

    C3Spline_CatmullRom& path = spline->m_spline;
    uint32_t count = path.m_pointCount;
    C44Matrix transport;
    bool onTransport = this->m_movementData.m_transportGuid && MovementGetTransportMtxSafe(this->m_movementData.m_transportGuid, &transport);

    CImVector green(0x00, 0xFF, 0x00, 0xFF);
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);

    for (uint32_t i = 0; i < count; i++) {
        C3Vector point = path.Point(i);

        if (onTransport) {
            point = point * transport;
        }

        C44Matrix sphere;
        sphere.a0 = 0.5f;
        sphere.b1 = 0.5f;
        sphere.c2 = 0.5f;
        sphere.d0 = point.x;
        sphere.d1 = point.y;
        sphere.d2 = point.z;
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, sphere, green, GXUDEBUGSTYLE_SOLID);
    }

    // The line strip: the unit, then the curve (0.5.3 appended the unit position before the samples).
    const TSGrowableArray<C3Vector>& samples = SampleDebugPathCurve(this->m_obj->m_guid, spline);
    C3Vector previous = this->GetPosition();

    for (uint32_t i = 0; i < samples.m_count; i++) {
        C3Vector point = samples.m_data[i];

        if (onTransport) {
            point = point * transport;
        }

        GxuDebugDrawLine(previous, point, white, GXUDEBUGSTYLE_SOLID);
        previous = point;
    }
}

// The 0.5.3 circles: SetCircleRenderStates and a projected 1 x 1 yard box, 2 yards deep, centered on the point (0.5.3
// built it from the point minus (0.5, 0.5, 1) to the point plus (0.5, 0.5, 1)), fade 0.5. ProjectTex::Render2d is the
// 3.3.5 form of ProjectTex2d; its query and polygon offset are the ones the selection circles use. Called by
// CGWorldFrame::OnWorldRender right after the selection circles.
void RenderDebugPathingCircles() {
    if (!s_debugPathCircles.m_count) {
        return;
    }

    g_theGxDevicePtr->RsPush();

    if (CGObject_C::SetCircleRenderStates()) {
        for (uint32_t i = 0; i < s_debugPathCircles.m_count; i++) {
            const DebugPathCircle& circle = s_debugPathCircles.m_data[i];
            CAaBox box;
            box.b = C3Vector(circle.m_position.x - 0.5f, circle.m_position.y - 0.5f, circle.m_position.z - 1.0f);
            box.t = C3Vector(circle.m_position.x + 0.5f, circle.m_position.y + 0.5f, circle.m_position.z + 1.0f);
            ProjectTex::Render2d(box, circle.m_color, nullptr, 0.5f, WORLD_QUERY_RENDER_MASK, PROJECT_TEX_NONE, 0.4f);
        }
    }

    g_theGxDevicePtr->RsPop();
    s_debugPathCircles.SetCount(0);
}

// Port: the 0.5.3 CGObject_C::PreAnimate condition, called from CGUnit_C::PreAnimate. With DebugTargetPath set, 0.5.3
// drew the path of the locked target, and of the active player (when not on a vehicle-like state it also checked).
void CGUnit_C::DebugPathingPreAnimate() {
    if (!s_cvDebugTargetPath || !s_cvDebugTargetPath->m_intValue) {
        return;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (guid == CGGameUI::m_lockedTarget || guid == ClntObjMgrGetActivePlayer()) {
        this->RenderDebugPathing();
    }
}

// Debug feature: combat hit-roll debug.
// 0.5.3 reference: CGUnit_C::SetDebugHitRolls (0x00622A40).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CGUnit_C::SetDebugHitRolls(const ATTACKROUNDINFO& info) {
}

// 0.5.3 reference: CGUnit_C::GetDebugStateInfo (0x005FC400).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CGUnit_C::GetDebugStateInfo(ATTACKROUNDINFO* info) {
    return 0;
}

// Debug feature: game object debug status.
// 0.5.3 reference: CGGameObject_C::GetDebugStatus (0x005EF930).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
const char* CGGameObject_C::GetDebugStatus() const {
    return nullptr;
}

// Debug feature: player bounding sphere display.
// 0.5.3 reference: CGPlayer_C::TogglePlayerBounds (0x005DCE60).
// Flips boundsPresent and shows or hides the bounding sphere on the active player's model (CM2Model::ShowBoundingSphere
// / HideBounds, drawn through the Gxu debug-draw module).
static bool s_boundsPresent; // 0.5.3 boundsPresent

void CGPlayer_C::TogglePlayerBounds() {
    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_PLAYER, __FILE__, __LINE__);
    CM2Model* model = player ? player->GetObjectModel() : nullptr;

    if (!model) {
        return;
    }

    bool wasPresent = s_boundsPresent;
    s_boundsPresent = !s_boundsPresent;

    if (wasPresent) {
        model->HideBounds();
    } else {
        model->ShowBoundingSphere();
    }
}

static CVar* s_cvShowEffectsStandalone;

// Debug feature: registers the 0.5.3 PlayerClientInitialize / UnitCombatLogInitialize debug commands and the
// CGObject_C / UnitEffects debug CVars.
void UnitDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("CombatDebugForceActionOn", CCommand_ForceActionSet, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOff", CCommand_ForceActionUnset, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOtherOn", CCommand_ForceActionOnOtherSet, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOtherOff", CCommand_ForceActionOnOtherUnset, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugShowFlags", CCommand_ForceActionShowFlags, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("DumpDeathHoldLogs", CCommand_DumpDeathHoldLogs, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("playercombatlogdebug", DebugCombatLogHandler, CATEGORY_DEBUG, nullptr);
    // 0.5.3 registered DebugTargetPath from CGObject_C::Initialize and showEffectsStandalone from UnitEffectsInitialize.
    s_cvDebugTargetPath = CVar::Register("DebugTargetPath", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
    s_cvShowEffectsStandalone = CVar::Register("showEffectsStandalone", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
}

#endif
