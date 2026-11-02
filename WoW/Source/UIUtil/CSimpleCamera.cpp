#include "CSimpleCamera.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cmath>

// VERIFY: fbf6cda8 auto
// OFFSET: 0x005FEE70
float CSimpleCamera::FOV() const {
    return this->m_fov;
}

// VERIFY: 98278b28 auto
// OFFSET: 0x005FEE80
C3Vector CSimpleCamera::Forward() const {
    return C3Vector(this->m_facing.a0, this->m_facing.a1, this->m_facing.a2);
}

// VERIFY: 8b1f5f49 auto
// OFFSET: 0x005FEEA0
C3Vector CSimpleCamera::Right() const {
    return C3Vector(this->m_facing.b0, this->m_facing.b1, this->m_facing.b2);
}

// VERIFY: 8ec84a54 auto
// OFFSET: 0x005FEEC0
C3Vector CSimpleCamera::Up() const {
    return C3Vector(this->m_facing.c0, this->m_facing.c1, this->m_facing.c2);
}

// VERIFY: 24a464c0 manual
// OFFSET: 0x00607C20
CSimpleCamera::CSimpleCamera(float nearZ, float farZ, float fov) {
    this->m_scene = nullptr;
    this->m_facing.a0 = 1.0f;
    this->m_facing.a1 = 0.0f;
    this->m_facing.a2 = 0.0f;
    this->m_facing.b0 = 0.0f;
    this->m_facing.b1 = 1.0f;
    this->m_facing.b2 = 0.0f;
    this->m_facing.c0 = 0.0f;
    this->m_facing.c1 = 0.0f;
    this->m_facing.c2 = 1.0f;
    this->m_nearZ = nearZ;
    this->m_farZ = farZ;
    this->m_fov = fov;
    this->m_aspect = 1.0f;
    this->m_facing.FromEulerAnglesZYX(0.0f, 0.0f, 0.0f);
}

// INLINED
CSimpleCamera::~CSimpleCamera() {
    if (this->m_scene) {
        this->m_scene->Release();
    }
}

// VERIFY: 3b9b73df auto
// OFFSET: 0x00607C00
CM2Scene* CSimpleCamera::GetScene() {
    if (!this->m_scene) {
        this->m_scene = M2CreateScene();
    }

    return this->m_scene;
}

// VERIFY: a99af1d8 auto
// OFFSET: 0x00607BD0
void CSimpleCamera::SetFacing(float yaw, float pitch, float roll) {
    this->m_facing.FromEulerAnglesZYX(yaw, pitch, roll);
}

// VERIFY: bc54e001 auto
// OFFSET: 0x00607CB0
void CSimpleCamera::SetGxProjectionAndView(const CRect& rect) {
    C44Matrix projection;
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
    GxuXformCreateProjection_Exact(this->FOV() * 0.6f, this->m_aspect, this->m_nearZ, this->m_farZ, projection);
    g_theGxDevicePtr->XformSetProjection(projection);

    C44Matrix view;
    C3Vector eye;
    GxuXformCreateLookAtSgCompat(eye, this->Forward(), this->Up(), view);
    g_theGxDevicePtr->XformSetView(view);
}

// VERIFY: 62c62fe7 auto
// OFFSET: 0x00607C90
void CSimpleCamera::SetScreenAspect(const CRect& rect) {
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
}

// VERIFY: 6c76bf00 auto
// OFFSET: 0x00607DF0
static void FaceDirection(const C3Vector& direction, C3Vector& right, C3Vector& forward, C3Vector& up) {
    if (fabs(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z) < 0.00000023841858f) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    forward = direction;

    if (fabs(forward.x * forward.x + forward.y * forward.y) < 0.00000023841858f) {
        right.x = 1.0f;
        right.y = 0.0f;
        right.z = 0.0f;
    } else {
        right.x = -forward.y;
        right.y = forward.x;
        right.z = 0.0f;

        float scale = 1.0f / sqrt(right.x * right.x + right.y * right.y);
        right.x *= scale;
        right.y *= scale;
    }

    up = C3Vector(-(right.y * forward.z), forward.z * right.x, right.y * forward.x - forward.y * right.x);
}

// VERIFY: b1593acc manual
// OFFSET: 0x00607ED0
static void BuildBillboardMatrix(C33Matrix& matrix, const C3Vector& direction) {
    C3Vector forward(0.0f, 0.0f, 0.0f);
    C3Vector right(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    FaceDirection(direction, right, forward, up);

    matrix.a0 = forward.x;
    matrix.a1 = forward.y;
    matrix.a2 = forward.z;
    matrix.b0 = right.x;
    matrix.b1 = right.y;
    matrix.b2 = right.z;
    matrix.c0 = up.x;
    matrix.c1 = up.y;
    matrix.c2 = up.z;
}

// VERIFY: 80ae91cc manual
// OFFSET: 0x00607F40
static void FaceDirectionWithRoll(const C3Vector& direction, const C3Vector& upHint, C3Vector& right, C3Vector& forward, C3Vector& up) {
    if (fabs(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z) < 0.00000023841858f) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    forward = direction;

    if (fabs(fabs(upHint.y * forward.y + upHint.x * forward.x + upHint.z * forward.z) - 1.0f) < 0.00000023841858f) {
        right.x = 1.0f;
        right.y = 0.0f;
        right.z = 0.0f;
    } else {
        right = C3Vector(upHint.y * forward.z - upHint.z * forward.y, upHint.z * forward.x - upHint.x * forward.z, upHint.x * forward.y - forward.x * upHint.y);

        float scale = 1.0f / sqrt(right.x * right.x + right.z * right.z + right.y * right.y);
        right.x *= scale;
        right.y *= scale;
        right.z *= scale;
    }

    up = C3Vector(right.z * forward.y - forward.z * right.y, forward.z * right.x - right.z * forward.x, forward.x * right.y - right.x * forward.y);
}

// VERIFY: 4383901d manual
// OFFSET: 0x00608080
static void BuildBillboardMatrixWithRoll(C33Matrix& matrix, const C3Vector& direction, const C3Vector& upHint) {
    C3Vector forward(0.0f, 0.0f, 0.0f);
    C3Vector right(0.0f, 0.0f, 0.0f);
    C3Vector up(0.0f, 0.0f, 0.0f);
    FaceDirectionWithRoll(direction, upHint, right, forward, up);

    matrix.a0 = forward.x;
    matrix.a1 = forward.y;
    matrix.a2 = forward.z;
    matrix.b0 = right.x;
    matrix.b1 = right.y;
    matrix.b2 = right.z;
    matrix.c0 = up.x;
    matrix.c1 = up.y;
    matrix.c2 = up.z;
}

// VERIFY: 2ba63539 auto
// OFFSET: 0x006080F0
void CSimpleCamera::SetFacing(const C3Vector& forward) {
    BuildBillboardMatrix(this->m_facing, forward);
}

// VERIFY: bc672379 auto
// OFFSET: 0x00608110
void CSimpleCamera::SetFacing(const C3Vector& forward, const C3Vector& up) {
    BuildBillboardMatrixWithRoll(this->m_facing, forward, up);
}
