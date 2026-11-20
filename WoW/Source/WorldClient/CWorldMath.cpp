#include "CWorldMath.h"
#include <cstring>
#include <cfloat>
#include <cmath>
#include <cmath>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"

// STATIC: 0x00D39650
static int32_t s_clipSide[32];

// STATIC: 0x00D396D0
static float s_clipDist[32];

// VERIFY: a0e6ed09 manual
// OFFSET: 0x007A61D0
int32_t CWorldMath::ComputeAaBoxOutcode(CAaBox* box, C3Vector* point) {
    float d0 = point->x - box->b.x + 0.019444443f;
    float d1 = point->y - box->b.y + 0.019444443f;
    float d2 = point->z - box->b.z + 0.019444443f;
    float d3 = box->t.x - point->x + 0.019444443f;
    float d4 = box->t.y - point->y + 0.019444443f;
    float d5 = box->t.z - point->z + 0.019444443f;

    uint32_t outcode = *reinterpret_cast<uint32_t*>(&d0) >> 31;
    outcode |= (*reinterpret_cast<uint32_t*>(&d1) >> 30) & 0x02;
    outcode |= (*reinterpret_cast<uint32_t*>(&d2) >> 29) & 0x04;
    outcode |= (*reinterpret_cast<uint32_t*>(&d3) >> 28) & 0x08;
    outcode |= (*reinterpret_cast<uint32_t*>(&d4) >> 27) & 0x10;
    outcode |= (*reinterpret_cast<uint32_t*>(&d5) >> 26) & 0x20;

    return outcode;
}

// VERIFY: fc48135f auto
// OFFSET: 0x007F9320
void CWorldMath::TransformAABoxInner(const float* row1, const float* row0, CAaBox& dst, CAaBox& src, const float* row2) {
    const float* rows[3] = { row0, row1, row2 };

    for (int32_t axis = 0; axis < 3; axis++) {
        for (int32_t component = 0; component < 3; component++) {
            float e = rows[component][axis];

            float a = e * (&src.b.x)[component];
            float b = e * (&src.t.x)[component];

            (&dst.b.x)[axis] += a < b ? a : b;
            (&dst.t.x)[axis] += a < b ? b : a;
        }
    }
}

// VERIFY: 51bf69b0 manual
// OFFSET: 0x007F93D0
void CWorldMath::TransformAABox(C33Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = { 0.0f, 0.0f, 0.0f };
    dst.t = { 0.0f, 0.0f, 0.0f };

    CWorldMath::TransformAABoxInner(&m.b0, &m.a0, dst, src, &m.c0);
}

// VERIFY: 07b42bca manual
// OFFSET: 0x007F9430
void CWorldMath::TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = C3Vector(m.d0, m.d1, m.d2);
    dst.t = C3Vector(m.d0, m.d1, m.d2);

    CWorldMath::TransformAABoxInner(&m.b0, &m.a0, dst, src, &m.c0);
}

// VERIFY: 011d2b0a manual
// OFFSET: 0x007F9480
bool CWorldMath::VectorIntersectAABox2(CAaBox& box, C3Vector& start, C3Vector& end) {
    const float* minB = &box.b.x;
    const float* maxB = &box.t.x;
    const float* origin = &start.x;
    const float* target = &end.x;

    float hit[3];
    float dir[3];
    float maxT[3];

    dir[0] = end.x - start.x;
    dir[1] = end.y - start.y;
    dir[2] = end.z - start.z;
    maxT[0] = -1.0f;
    maxT[1] = -1.0f;
    maxT[2] = -1.0f;

    int32_t inside = 1;

    for (uint32_t i = 0; i < 3; i++) {
        if (origin[i] < minB[i]) {
            if (target[i] < minB[i]) {
                return false;
            }

            hit[i] = minB[i];
            inside = 0;

            if (dir[i] != 0.0f) {
                maxT[i] = (minB[i] - origin[i]) / dir[i];
            }
        } else if (origin[i] > maxB[i]) {
            if (target[i] > maxB[i]) {
                return false;
            }

            hit[i] = maxB[i];
            inside = 0;

            if (dir[i] != 0.0f) {
                maxT[i] = (maxB[i] - origin[i]) / dir[i];
            }
        }
    }

    if (inside) {
        return true;
    }

    uint32_t plane = maxT[0] < maxT[1];

    if (maxT[plane] < maxT[2]) {
        plane = 2;
    }

    if (*reinterpret_cast<uint32_t*>(&maxT[plane]) & 0x80000000) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (i != plane) {
            hit[i] = origin[i] + maxT[plane] * dir[i];

            if (minB[i] - 0.00001f > hit[i] || maxB[i] + 0.00001f < hit[i]) {
                return false;
            }
        }
    }

    return true;
}

// VERIFY: e47b9bbd manual
// OFFSET: 0x007F9650
void CWorldMath::ClipVertexListToPlane(const C3Vector* verts, uint32_t count, C3Vector* out, uint32_t& outCount, const C4Plane& plane, float epsilon, BOOL keepCoplanar) {
    uint32_t sideCount[3] = { 0, 0, 0 };
    uint32_t i;

    for (i = 0; i < count; i++) {
        float dist = verts[i].z * plane.n.z + verts[i].x * plane.n.x + verts[i].y * plane.n.y + plane.d;
        s_clipDist[i] = dist;

        if (dist > epsilon) {
            s_clipSide[i] = 1;
        } else if (-epsilon <= dist) {
            s_clipSide[i] = 0;
        } else {
            s_clipSide[i] = 2;
        }

        sideCount[s_clipSide[i]]++;
    }

    s_clipDist[i] = s_clipDist[0];
    s_clipSide[i] = s_clipSide[0];

    if (!sideCount[1]) {
        if (sideCount[2]) {
            outCount = 0;
        } else if (keepCoplanar) {
            memcpy(out, verts, sizeof(C3Vector) * count);
            outCount = count;
        } else {
            outCount = 0;
        }

        return;
    }

    if (!sideCount[2]) {
        memcpy(out, verts, sizeof(C3Vector) * count);
        outCount = count;
        return;
    }

    outCount = 0;

    for (i = 0; i < count; i++) {
        int32_t side = s_clipSide[i];

        if (!side) {
            out[outCount++] = verts[i];
            continue;
        }

        if (side == 1) {
            out[outCount++] = verts[i];
        }

        int32_t nextSide = s_clipSide[i + 1];

        if (!nextSide || nextSide == side) {
            continue;
        }

        float t = s_clipDist[i] / (s_clipDist[i] - s_clipDist[i + 1]);
        const C3Vector& a = verts[i];
        const C3Vector& b = verts[(i + 1) % count];
        C3Vector& point = out[outCount++];
        point.x = (b.x - a.x) * t + a.x;
        point.y = (b.y - a.y) * t + a.y;
        point.z = t * (b.z - a.z) + a.z;
    }
}

// VERIFY: 42b10466 manual
// OFFSET: 0x007F9BF0
float CWorldMath::DistanceSquaredFromEdge(const C2Vector& a, const C2Vector& b, const C2Vector& point) {
    float px = point.x - a.x;
    float py = point.y - a.y;
    float ex = b.x - a.x;
    float ey = b.y - a.y;
    float t = (ey * py + ex * px) / (ey * ey + ex * ex);

    if (t < 0.0f) {
        return px * px + py * py;
    }

    float dx;
    float dy;

    if (t <= 1.0f) {
        dy = point.y - (ey * t + a.y);
        dx = point.x - (ex * t + a.x);
    } else {
        dy = point.y - b.y;
        dx = point.x - b.x;
    }

    return dx * dx + dy * dy;
}

// VERIFY: bd915e8b manual
// OFFSET: 0x007F9C90
int32_t CWorldMath::PointInPolygon(int32_t count, const C2Vector* points, const C2Vector& point, float* distance) {
    float nearest = FLT_MAX;
    int32_t inside = 0;

    for (int32_t i = 0; i < count; i++) {
        const C2Vector& current = points[i];
        const C2Vector& previous = points[i ? i - 1 : count - 1];
        float edge = CWorldMath::DistanceSquaredFromEdge(current, previous, point);

        if (nearest > edge) {
            nearest = edge;
        }

        if (((point.y > current.y && point.y <= previous.y) || (point.y > previous.y && point.y <= current.y)) && (point.y - current.y) / (previous.y - current.y) * (previous.x - current.x) + current.x < point.x) {
            inside = !inside;
        }
    }

    *distance = sqrt(nearest);
    return inside;
}

// VERIFY: fd69952e auto
// OFFSET: 0x0079F6F0
void CWorldMath::MakeAaBox(CAaBox& box, const C3Vector& center, const float* size) {
    float extent = (size[1] >= size[0] ? size[1] : size[0]) * 0.66000003f;
    box.b = center;
    box.t = center;
    box.b.x -= extent;
    box.b.y -= extent;
    box.b.z -= extent;
    box.t.x += extent;
    box.t.y += extent;
    box.t.z += extent;
}
