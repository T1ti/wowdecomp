#include "WowEdit/Source/WowEditDraw.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/WorldClient/World.h"

// See WowEditDraw.h. Everything here is a port addition.

const CImVector WOWEDIT_COLOR_SPAWN(0xC0, 0xC8, 0x30, 0x90);
const CImVector WOWEDIT_COLOR_SPAWN_OLD(0x40, 0xE0, 0x50, 0xA0);
const CImVector WOWEDIT_COLOR_ORIENTATION(0xFF, 0x30, 0x80, 0xFF);
const CImVector WOWEDIT_COLOR_FACING(0x20, 0x20, 0xFF, 0xFF);
const CImVector WOWEDIT_COLOR_PATH_POINT(0x00, 0xA0, 0xFF, 0xFF);
const CImVector WOWEDIT_COLOR_PATH_LINE(0x20, 0x20, 0xFF, 0xFF);
const CImVector WOWEDIT_COLOR_FLY_POINT(0x20, 0x20, 0xFF, 0xFF);

// ---- Vector helpers

static C3Vector Cross(const C3Vector& a, const C3Vector& b) {
    return C3Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static float Dot(const C3Vector& a, const C3Vector& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static bool Normalize(C3Vector& v) {
    float length = sqrtf(Dot(v, v));

    if (length <= 1.0e-6f) {
        return false;
    }

    v.x /= length;
    v.y /= length;
    v.z /= length;
    return true;
}

// Unit shape to world: the shape's local x, y and z land on the given axes, each scaled, and the origin on position.
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

// ---- Ground

static const float GROUND_RAY_ABOVE = 2.0f;
static const float GROUND_RAY_BELOW = 100.0f;
static const float GROUND_NORMAL_STEP = 0.5f;

static bool GroundHeight(float x, float y, float z, C3Vector* hit) {
    C3Vector start(x, y, z + GROUND_RAY_ABOVE);
    C3Vector end(x, y, z - GROUND_RAY_BELOW);
    float distance = 1.0f;

    return World::Intersect(start, end, hit, &distance, WORLD_QUERY_COLLISION_MASK, nullptr) != 0;
}

bool WowEditGetGround(const C3Vector& position, C3Vector* ground, C3Vector* normal) {
    C3Vector hit;

    if (!GroundHeight(position.x, position.y, position.z, &hit)) {
        *ground = position;
        *normal = C3Vector(0.0f, 0.0f, 1.0f);
        return false;
    }

    *ground = hit;

    // The slope from two more samples, one step along x and one along y. A miss (an edge of the world, a hole) falls
    // back to straight up.
    C3Vector hitX;
    C3Vector hitY;

    if (GroundHeight(position.x + GROUND_NORMAL_STEP, position.y, position.z, &hitX) && GroundHeight(position.x, position.y + GROUND_NORMAL_STEP, position.z, &hitY)) {
        C3Vector alongX(hitX.x - hit.x, hitX.y - hit.y, hitX.z - hit.z);
        C3Vector alongY(hitY.x - hit.x, hitY.y - hit.y, hitY.z - hit.z);
        C3Vector n = Cross(alongX, alongY);

        if (n.z > 0.0f && Normalize(n)) {
            *normal = n;
            return true;
        }
    }

    *normal = C3Vector(0.0f, 0.0f, 1.0f);
    return true;
}

// Where a marker goes and how it is turned: its position, its up axis and its facing in the plane of that up axis.
struct MarkerFrame {
    C3Vector position;
    C3Vector up;
    C3Vector facing;
    C3Vector side; // up x facing, so (side, up, facing) and (facing, side, up) are right handed
};

static MarkerFrame PlaceMarker(const C3Vector& position, float facingAngle, uint32_t placement) {
    MarkerFrame frame;
    frame.position = position;
    frame.up = C3Vector(0.0f, 0.0f, 1.0f);

    if (placement & (WOWEDIT_SNAP_TO_GROUND | WOWEDIT_ALIGN_TO_GROUND)) {
        C3Vector ground;
        C3Vector normal;

        if (WowEditGetGround(position, &ground, &normal)) {
            if (placement & WOWEDIT_SNAP_TO_GROUND) {
                frame.position = ground;
            }

            if (placement & WOWEDIT_ALIGN_TO_GROUND) {
                frame.up = normal;
            }
        }
    }

    // The facing projected into the plane of the up axis.
    C3Vector facing(cosf(facingAngle), sinf(facingAngle), 0.0f);
    float along = Dot(facing, frame.up);
    frame.facing = C3Vector(facing.x - frame.up.x * along, facing.y - frame.up.y * along, facing.z - frame.up.z * along);

    if (!Normalize(frame.facing)) {
        frame.facing = facing;
    }

    frame.side = Cross(frame.up, frame.facing);
    return frame;
}

// ---- Markers

// The directional sphere's spike runs along its local +z: put local z on the facing and local y on the up axis, so
// the spike lies along the ground (half in it, like the body) and turns with the slope when aligned.
void WowEditDrawSpawn(const C3Vector& position, float facing, float radius, const CImVector& color, uint32_t placement) {
    MarkerFrame frame = PlaceMarker(position, facing, placement);
    GxuDebugDrawShape(GXUDEBUGSHAPE_DIRECTIONAL_SPHERE, AxesTransform(frame.position, frame.side, frame.up, frame.facing, C3Vector(radius, radius, radius)), color, GXUDEBUGSTYLE_SOLID);
}

// The ring is a unit-radius band around its local z, 4 units tall: local z on the up axis, flattened to a thin band.
static const float ORIENTATION_RING_HEIGHT = 0.06f;

void WowEditDrawOrientation(const C3Vector& position, float facing, float radius, const CImVector& ringColor, const CImVector& lineColor, uint32_t placement) {
    MarkerFrame frame = PlaceMarker(position, facing, placement);
    GxuDebugDrawShape(GXUDEBUGSHAPE_ROTATION_RING, AxesTransform(frame.position, frame.facing, frame.side, frame.up, C3Vector(radius, radius, ORIENTATION_RING_HEIGHT * 0.25f)), ringColor, GXUDEBUGSTYLE_SOLID);

    C3Vector tip(frame.position.x + frame.facing.x * radius, frame.position.y + frame.facing.y * radius, frame.position.z + frame.facing.z * radius);
    GxuDebugDrawLine(frame.position, tip, lineColor);
}

void WowEditDrawPath(const C3Vector* points, uint32_t count, float radius, const CImVector& pointColor, const CImVector& lineColor, uint32_t placement) {
    C3Vector previous;

    for (uint32_t i = 0; i < count; i++) {
        MarkerFrame frame = PlaceMarker(points[i], 0.0f, placement);
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, AxesTransform(frame.position, frame.facing, frame.side, frame.up, C3Vector(radius, radius, radius)), pointColor, GXUDEBUGSTYLE_SOLID);

        if (i) {
            GxuDebugDrawLine(previous, frame.position, lineColor);
        }

        previous = frame.position;
    }
}

// The diamond points along its local +z, is 1 wide along x and 0.5 along y: keep x horizontal so it lies flat.
void WowEditDrawPathPoint(const C3Vector& position, const C3Vector& direction, float size, const CImVector& color) {
    C3Vector up(0.0f, 0.0f, 1.0f);
    C3Vector zAxis = direction;

    if (!Normalize(zAxis)) {
        zAxis = C3Vector(1.0f, 0.0f, 0.0f);
    }

    C3Vector xAxis = Cross(zAxis, up);

    if (!Normalize(xAxis)) {
        xAxis = C3Vector(1.0f, 0.0f, 0.0f); // pointing straight up or down
    }

    C3Vector yAxis = Cross(zAxis, xAxis);
    GxuDebugDrawShape(GXUDEBUGSHAPE_DIRECTIONAL_DIAMOND, AxesTransform(position, xAxis, yAxis, zAxis, C3Vector(size, size, size)), color, GXUDEBUGSTYLE_SOLID);
}


#endif
