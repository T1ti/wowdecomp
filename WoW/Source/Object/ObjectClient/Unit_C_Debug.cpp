#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
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
// 0.5.3 reference: CGUnit_C::RenderDebugPathing (0x005FD3E0), fed by CGUnit_C::OnMonsterMove (0x005F8EF0). No later
// client keeps it: 3.3.5 retail strips it, and the 4.1 symbol build has no pathing display either.
//
// 0.5.3 kept the path of every SMSG_MONSTER_MOVE in the unit: m_serverLoc, the start position the message carries, and
// m_debugPathPoints, every point of the spline it built (lead-in points included). For a flying spline (move flag
// 0x200, the Catmull-Rom splines taxi flights use) it also appended the unit position and the curve itself, sampled
// with C3Spline::Frame by arc length every 1 / (3 x length), about every third of a yard; m_numDebugPathNodes counts
// the points before the samples. RenderDebugPathing then drew, with DebugTargetPath set:
//   - flying spline in progress: a green solid sphere of radius 0.5 (ModelCreateSolidSphere, CImVector 0xFF00FF00)
//     at each control point, and a line strip (GxPrim_LineStrip, depth tested) through the unit and the samples;
//   - otherwise (ground moves, or the unit stopped): projected selection circles (SetCircleRenderStates +
//     ProjectTex2d, 1 x 1 yard, 2 yards deep, fade 0.5), green at m_serverLoc, then red at every stored point, so
//     they lie on the ground. No spheres and no line.
//
// 3.3.5 keeps no copy of the move, but the unit's movement spline (CMoveSpline, built from the same message by
// CGUnit_C::OnMonsterMove) holds the same points and the same Catmull-Rom curve (C3Spline_CatmullRom in
// MODE_CATMULLROM for flying splines, MODE_LINEAR for ground ones). A ground spline is
//   [lead-in extrapolated behind the start, start, path points..., end, end again],
// so the server location is point 1. This draws from it: spheres and the sampled curve through the Gxu debug-draw
// module for a Catmull-Rom spline, projected circles for a linear one (queued here, drawn by
// RenderDebugPathingCircles during the world render, where the client draws its selection circles). 3.3.5 does not
// keep the path of a stopped unit, so this keeps a copy of the last spline for the shown units (DebugPathLast) and
// draws its circles once the spline is done, as 0.5.3 drew the stored path of a stopped unit. Spline points of a unit
// on a transport are transport-local and are moved to world space with the transport matrix.
//
// Port changes to the ground display:
//   - The circles are centered on the ground below each point (World::Intersect against the collision geometry)
//     rather than on the point: 3.3.5 server paths can sit more than the circle's 1 yard half-depth under the ground,
//     and then the projection finds no terrain to draw on.
//   - A white line from the start to the end, draped on the ground (sampled every yard and dropped onto the collision
//     geometry), since 1 yard additive circles on a 3.3.5 spline's few points are hard to follow. 0.5.3 had the
//     server's dense debug path for that.
//
// Port addition: transports. A transport (boat, zeppelin) is a game object moving along its ShipPath, the TaxiPath
// curve CGGameObject_C_Type_MapObjTransport builds (ShipPath::BuildLegs), not a unit spline. While the active player
// rides one, the legs of its path on the current map are drawn like a flying spline: green spheres at the control
// points and a white line along the curve, sampled every 2 yards.

// A projected circle, for this frame's queue and for a kept path.
struct DebugPathCircle {
    C3Vector m_position;
    CImVector m_color;
};

static const uint32_t DEBUG_PATH_GREEN = 0xFF00FF00; // 0.5.3 m_serverLoc circle and spheres
static const uint32_t DEBUG_PATH_RED = 0xFFFF0000; // 0.5.3 path point circles

// The last path of a shown unit, still drawn after the spline is done (0.5.3 kept m_serverLoc and m_debugPathPoints
// in the unit until the next move message): its circles and, for a ground path, the line draped on the ground.
// Positions are world space, or transport-local for a path on a transport.
struct DebugPathLast {
    WOWGUID m_guid;
    uint32_t m_splineId;
    uint32_t m_pointCount;
    WOWGUID m_transportGuid;
    TSGrowableArray<DebugPathCircle> m_circles;
    TSGrowableArray<C3Vector> m_draped;
};

static const float DEBUG_PATH_DRAPE_STEP = 1.0f; // yards between ground samples
static const float DEBUG_PATH_GROUND_ABOVE = 3.0f; // ground search from this far above the point
static const float DEBUG_PATH_GROUND_BELOW = 10.0f; // to this far below it
static const float DEBUG_PATH_GROUND_FAR = 60.0f; // then, on a miss, from this far above to this far below
static const float DEBUG_PATH_DRAPE_LIFT = 0.1f; // the line sits this far above the ground

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

// Circles queued for this frame's world render.
static TSGrowableArray<DebugPathCircle> s_debugPathCircles;

static void QueueDebugPathCircle(const C3Vector& position, const CImVector& color) {
    DebugPathCircle* circle = s_debugPathCircles.New();
    circle->m_position = position;
    circle->m_color = color;
}

static void AddDebugPathCircle(TSGrowableArray<DebugPathCircle>& circles, const C3Vector& position, uint32_t argb) {
    DebugPathCircle* circle = circles.New();
    circle->m_position = position;
    *reinterpret_cast<uint32_t*>(&circle->m_color) = argb;
}

// The ground below (or a little above) a point: first close to it, so a bridge or a roof overhead is not taken for the
// ground, then over a wider range for a server point well under (or over) the terrain.
static bool DebugPathGround(const C3Vector& point, C3Vector* ground) {
    float distance = 1.0f;

    if (World::Intersect(C3Vector(point.x, point.y, point.z + DEBUG_PATH_GROUND_ABOVE), C3Vector(point.x, point.y, point.z - DEBUG_PATH_GROUND_BELOW), ground, &distance, WORLD_QUERY_COLLISION_MASK, nullptr)) {
        return true;
    }

    distance = 1.0f;
    return World::Intersect(C3Vector(point.x, point.y, point.z + DEBUG_PATH_GROUND_FAR), C3Vector(point.x, point.y, point.z - DEBUG_PATH_GROUND_FAR), ground, &distance, WORLD_QUERY_COLLISION_MASK, nullptr) != 0;
}

// Drops the points of a line onto the ground. A point with no ground below takes the height of the nearest point that
// has some, so a miss never pulls the line down to the server height under the terrain; with no ground anywhere, the
// points keep their height. Returns the number of misses.
static uint32_t DrapeDebugPath(C3Vector* points, uint32_t count) {
    static TSGrowableArray<uint8_t> found;
    found.SetCount(count);
    uint32_t misses = 0;
    int32_t lastFound = -1;

    for (uint32_t i = 0; i < count; i++) {
        C3Vector hit;
        found.m_data[i] = DebugPathGround(points[i], &hit);

        if (found.m_data[i]) {
            points[i].z = hit.z + DEBUG_PATH_DRAPE_LIFT;
            lastFound = static_cast<int32_t>(i);
        } else {
            misses++;
        }
    }

    if (!misses || lastFound < 0) {
        return misses;
    }

    for (uint32_t i = 0; i < count; i++) {
        if (found.m_data[i]) {
            continue;
        }

        float best = 0.0f;
        uint32_t bestDistance = UINT32_MAX;

        for (uint32_t j = 0; j < count; j++) {
            uint32_t d = j > i ? j - i : i - j;

            if (found.m_data[j] && d < bestDistance) {
                bestDistance = d;
                best = points[j].z;
            }
        }

        points[i].z = best;
    }

    return misses;
}

static DebugPathLast* FindDebugPathLast(const WOWGUID& guid) {
    for (uint32_t i = 0; i < DEBUG_PATH_CURVES; i++) {
        if (s_debugPathLast[i].m_guid == guid) {
            return &s_debugPathLast[i];
        }
    }

    return nullptr;
}

// Builds the kept path of the unit's spline when it is a new one.
static void RememberDebugPath(const WOWGUID& guid, CMoveSpline* spline, const WOWGUID& transportGuid) {
    C3Spline_CatmullRom& path = spline->m_spline;
    DebugPathLast* last = FindDebugPathLast(guid);

    if (!last) {
        last = &s_debugPathLast[s_debugPathLastNext];
        s_debugPathLastNext = (s_debugPathLastNext + 1) % DEBUG_PATH_CURVES;
        last->m_guid = guid;
    } else if (last->m_splineId == spline->m_id && last->m_pointCount == path.m_pointCount) {
        return;
    }

    uint32_t count = path.m_pointCount;
    last->m_splineId = spline->m_id;
    last->m_pointCount = count;
    last->m_transportGuid = transportGuid;
    last->m_circles.SetCount(0);
    last->m_draped.SetCount(0);

    if (!count) {
        return;
    }

    // Ground positions are looked up for a ground path in world space only: a flying path ends in the air (0.5.3 drew
    // its circles there too, where they find nothing to project on), and a transport deck is not below a
    // transport-local point until the point is moved.
    bool ground = path.GetSplineMode() != MODE_CATMULLROM && !transportGuid;

    // The circle centers: every point, dropped onto the ground for a ground path (DrapeDebugPath lifts them by
    // DEBUG_PATH_DRAPE_LIFT, taken off again: the projection box is centered on the ground).
    static TSGrowableArray<C3Vector> centers;
    centers.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        centers.m_data[i] = path.Point(i);
    }

    uint32_t misses = 0;

    if (ground) {
        misses += DrapeDebugPath(centers.m_data, count);

        for (uint32_t i = 0; i < count; i++) {
            centers.m_data[i].z -= DEBUG_PATH_DRAPE_LIFT;
        }
    }

    // 0.5.3: green at the server location, then red at every point.
    AddDebugPathCircle(last->m_circles, centers.m_data[count > 1 ? 1 : 0], DEBUG_PATH_GREEN);

    for (uint32_t i = 0; i < count; i++) {
        AddDebugPathCircle(last->m_circles, centers.m_data[i], DEBUG_PATH_RED);
    }

    if (path.GetSplineMode() == MODE_CATMULLROM || count < 3) {
        return;
    }

    // The line, from the start (point 1) to the end (point count - 2), leaving out the extrapolated lead-in and the
    // repeated end, sampled every yard. Draped on the ground in world space; straight on a transport.
    for (uint32_t i = 1; i + 2 < count; i++) {
        const C3Vector& a = path.Point(i);
        const C3Vector& b = path.Point(i + 1);
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        uint32_t steps = static_cast<uint32_t>(sqrtf(dx * dx + dy * dy) / DEBUG_PATH_DRAPE_STEP) + 1;

        if (!ground || last->m_draped.m_count + steps > DEBUG_PATH_MAX_SAMPLES) {
            steps = 1;
        }

        for (uint32_t s = 0; s < steps; s++) {
            float t = static_cast<float>(s) / steps;
            *last->m_draped.New() = C3Vector(a.x + dx * t, a.y + dy * t, a.z + (b.z - a.z) * t);
        }
    }

    *last->m_draped.New() = path.Point(count - 2);

    if (ground) {
        misses += DrapeDebugPath(last->m_draped.m_data, last->m_draped.m_count);
    }

    // Port diagnostic: a server path the ground lookups could not follow (out of the loaded map, or far from the
    // terrain), with the end's server height and the height drawn.
    if (misses) {
        const C3Vector& end = path.Point(count - 2);
        const C3Vector& drawn = last->m_draped.m_data[last->m_draped.m_count - 1];
        ConsoleWriteA("DebugTargetPath: no ground under %u of %u path samples; end at server z %.2f, drawn at z %.2f", WARNING_COLOR, misses, count + last->m_draped.m_count, end.z, drawn.z);
    }
}

// Draws a white line strip through points, moved to world space by an optional transport matrix.
static void DrawDebugPathStrip(const C3Vector* points, uint32_t count, const C44Matrix* transport) {
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);

    for (uint32_t i = 1; i < count; i++) {
        C3Vector start = points[i - 1];
        C3Vector end = points[i];

        if (transport) {
            start = start * *transport;
            end = end * *transport;
        }

        GxuDebugDrawLine(start, end, white, GXUDEBUGSTYLE_SOLID);
    }
}

static void DrawDebugPathSphere(const C3Vector& point) {
    CImVector green(0x00, 0xFF, 0x00, 0xFF);
    C44Matrix sphere;
    sphere.a0 = 0.5f;
    sphere.b1 = 0.5f;
    sphere.c2 = 0.5f;
    sphere.d0 = point.x;
    sphere.d1 = point.y;
    sphere.d2 = point.z;
    GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, sphere, green, GXUDEBUGSTYLE_SOLID);
}

// The sampled curve of the active player's transport path, kept while the player rides the same transport on the
// same map.
static const float DEBUG_SHIP_PATH_STEP = 2.0f; // yards between samples
static WOWGUID s_debugShipGuid;
static int32_t s_debugShipMapId = -1;
static TSGrowableArray<C3Vector> s_debugShipControl;
static TSGrowableArray<uint32_t> s_debugShipLegEnds; // end of each leg's samples in s_debugShipSamples
static TSGrowableArray<C3Vector> s_debugShipSamples;

static void RenderDebugShipPath(const WOWGUID& transportGuid) {
    CGObject_C* object = ClntObjMgrObjectPtr(transportGuid, TYPE_GAMEOBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    ShipPath* shipPath = static_cast<CGGameObject_C*>(object)->GetShipPath();

    if (!shipPath || !shipPath->m_legs.m_count) {
        return;
    }

    int32_t mapId = World::QueryMapId();

    if (s_debugShipGuid != transportGuid || s_debugShipMapId != mapId) {
        s_debugShipGuid = transportGuid;
        s_debugShipMapId = mapId;
        s_debugShipControl.SetCount(0);
        s_debugShipLegEnds.SetCount(0);
        s_debugShipSamples.SetCount(0);

        for (uint32_t l = 0; l < shipPath->m_legs.m_count; l++) {
            ShipPathLeg& leg = shipPath->m_legs.m_data[l];

            if (leg.m_mapId != mapId) {
                continue;
            }

            C3Spline_CatmullRom& path = leg.m_spline;

            for (uint32_t i = 0; i < path.m_pointCount; i++) {
                *s_debugShipControl.New() = path.Point(i);
            }

            if (path.m_pointCount > 3 && path.cachedLength > 0.0f) {
                float step = DEBUG_SHIP_PATH_STEP / path.cachedLength;

                if (step < 1.0f / DEBUG_PATH_MAX_SAMPLES) {
                    step = 1.0f / DEBUG_PATH_MAX_SAMPLES;
                }

                for (float t = 0.0f; t < 1.0f; t += step) {
                    path.Pos(t, *s_debugShipSamples.New(), C3Spline::EVAL_ARCLENGTH);
                }

                path.Pos(1.0f, *s_debugShipSamples.New(), C3Spline::EVAL_ARCLENGTH);
            }

            *s_debugShipLegEnds.New() = s_debugShipSamples.m_count;
        }
    }

    for (uint32_t i = 0; i < s_debugShipControl.m_count; i++) {
        DrawDebugPathSphere(s_debugShipControl.m_data[i]);
    }

    uint32_t begin = 0;

    for (uint32_t l = 0; l < s_debugShipLegEnds.m_count; l++) {
        uint32_t end = s_debugShipLegEnds.m_data[l];
        DrawDebugPathStrip(s_debugShipSamples.m_data + begin, end - begin, nullptr);
        begin = end;
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

void CGUnit_C::RenderDebugPathing() {
    const WOWGUID& guid = this->m_obj->m_guid;
    CMoveSpline* spline = this->m_movementData.m_spline;
    bool moving = spline && !(spline->m_flags & SPLINEFLAG_DONE) && spline->m_spline.m_pointCount;

    if (moving) {
        RememberDebugPath(guid, spline, this->m_movementData.m_transportGuid);
    }

    if (!moving || spline->m_spline.GetSplineMode() != MODE_CATMULLROM) {
        // Ground move, or the unit stopped: the kept path's circles and line.
        DebugPathLast* last = FindDebugPathLast(guid);

        if (!last) {
            return;
        }

        C44Matrix lastTransport;
        bool lastOnTransport = last->m_transportGuid && MovementGetTransportMtxSafe(last->m_transportGuid, &lastTransport);

        if (last->m_transportGuid && !lastOnTransport) {
            return;
        }

        for (uint32_t i = 0; i < last->m_circles.m_count; i++) {
            const DebugPathCircle& circle = last->m_circles.m_data[i];
            C3Vector point = circle.m_position;

            if (lastOnTransport) {
                point = point * lastTransport;
            }

            QueueDebugPathCircle(point, circle.m_color);
        }

        DrawDebugPathStrip(last->m_draped.m_data, last->m_draped.m_count, lastOnTransport ? &lastTransport : nullptr);
        return;
    }

    C3Spline_CatmullRom& path = spline->m_spline;
    uint32_t count = path.m_pointCount;
    C44Matrix transport;
    bool onTransport = this->m_movementData.m_transportGuid && MovementGetTransportMtxSafe(this->m_movementData.m_transportGuid, &transport);

    for (uint32_t i = 0; i < count; i++) {
        C3Vector point = path.Point(i);

        if (onTransport) {
            point = point * transport;
        }

        DrawDebugPathSphere(point);
    }

    // The line strip through the samples. 0.5.3 also stored the unit position at the time of the message after the
    // control points, as one more sphere node (m_numDebugPathNodes is the point count plus one, and the strip starts
    // after it); in 3.3.5 that position is the spline's point 1, which already has its sphere.
    const TSGrowableArray<C3Vector>& samples = SampleDebugPathCurve(this->m_obj->m_guid, spline);
    DrawDebugPathStrip(samples.m_data, samples.m_count, onTransport ? &transport : nullptr);
}

// The 0.5.3 circles: SetCircleRenderStates and a projected 1 x 1 yard box, 2 yards deep, centered on the circle's
// position (0.5.3 built it from the point minus (0.5, 0.5, 1) to the point plus (0.5, 0.5, 1)), fade 0.5. ProjectTex::Render2d is the
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

    // Port addition: the path of the transport the active player rides.
    if (guid == ClntObjMgrGetActivePlayer() && this->m_movementData.m_transportGuid) {
        RenderDebugShipPath(this->m_movementData.m_transportGuid);
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
