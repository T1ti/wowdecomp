#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H

#include <cstdint>
#include <windows.h>

class C2Vector;
class C33Matrix;
class C3Vector;
class C44Matrix;
class CAaBox;

class C4Plane;

class CWorldMath {
    public:
    static void TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst);
    static void MakeAaBox(CAaBox& box, const C3Vector& center, const float* size);
    static void TransformAABoxInner(const float* row1, const float* row0, CAaBox& dst, CAaBox& src, const float* row2);
    static void TransformAABox(C33Matrix& m, CAaBox& src, CAaBox& dst);
    static bool VectorIntersectAABox2(CAaBox& box, C3Vector& start, C3Vector& end);
    static int32_t ComputeAaBoxOutcode(CAaBox* box, C3Vector* point);
    static float DistanceSquaredFromEdge(const C2Vector& a, const C2Vector& b, const C2Vector& point);
    static int32_t PointInPolygon(int32_t count, const C2Vector* points, const C2Vector& point, float* distance);
    static void ClipVertexListToPlane(const C3Vector* verts, uint32_t count, C3Vector* out, uint32_t& outCount, const C4Plane& plane, float epsilon, BOOL keepCoplanar);
};

#endif
