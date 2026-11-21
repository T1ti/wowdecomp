#include "Engine/Source/Model2/M2Model.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Shared.h"

// Model bounds display.
//
// 0.5.3 built a separate debug model per shown bounds (CreateModelBoundingSphere / CreateModelBoundingBox: generated
// sphere or box geometry, a TextureCreateSolid texture of CImVector 0x7EFFFFFF, i.e. half transparent white, alpha
// blended), kept it in CModelBase::m_boundsModel with m_flags bit 1 set for a sphere, and rendered it with the model.
// Requests on a model still loading were queued (MODEL_SHOW_BOUNDING_SPHERE, MODEL_HIDE_BOUNDS).
//
// 3.3.5 M2 has no generated-model path, so the model only records which bounds it shows (m_debugBounds) and
// CM2Scene::Animate queues the matching Gxu debug shape for every model it puts on the frame's draw list, with the
// model's current world transform. No queueing is needed: the flag is read only once the model draws.

static const uint32_t BOUNDS_COLOR = 0x7EFFFFFF; // 0.5.3 bounds texture color (ARGB)

// Debug feature: model bounding sphere display.
// 0.5.3 reference: ModelShowBoundingSphere (0x004349B0).
void CM2Model::ShowBoundingSphere() {
    this->m_debugBounds = DEBUG_BOUNDS_SPHERE;
}

// Debug feature: model bounding box display.
// 0.5.3 reference: ModelShowBoundingBox (0x00434A90).
void CM2Model::ShowBoundingBox() {
    this->m_debugBounds = DEBUG_BOUNDS_BOX;
}

// Debug feature: model bounds display.
// 0.5.3 reference: ModelHideBounds (0x00434C10).
void CM2Model::HideBounds() {
    this->m_debugBounds = DEBUG_BOUNDS_NONE;
}

// 0.5.3 reference: ModelIsShowingBoundingBox (0x00440060): a bounds model without the sphere bit.
BOOL CM2Model::IsShowingBoundingBox() const {
    return this->m_debugBounds == DEBUG_BOUNDS_BOX;
}

// 0.5.3 reference: ModelIsShowingBoundingSphere (0x00440030): a bounds model with the sphere bit.
BOOL CM2Model::IsShowingBoundingSphere() const {
    return this->m_debugBounds == DEBUG_BOUNDS_SPHERE;
}

static CImVector MakeColor(uint32_t argb) {
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = argb;
    return color;
}

static void DrawBounds(const M2Data* data, CM2Model::DEBUG_BOUNDS mode, const C44Matrix& world) {
    const M2Bounds& bounds = data->m_bounds;
    CImVector color = MakeColor(BOUNDS_COLOR);

    if (mode == CM2Model::DEBUG_BOUNDS_SPHERE) {
        C3Vector center((bounds.m_extent.b.x + bounds.m_extent.t.x) * 0.5f, (bounds.m_extent.b.y + bounds.m_extent.t.y) * 0.5f, (bounds.m_extent.b.z + bounds.m_extent.t.z) * 0.5f);
        GxuDebugDrawBoundsSphere(center, bounds.m_radius, world, color, GXUDEBUGSTYLE_SOLID);
    } else {
        GxuDebugDrawBoundsBox(bounds.m_extent.b, bounds.m_extent.t, world, color, GXUDEBUGSTYLE_SOLID);
    }
}

// 0.5.3 CreateCollisionDisplayMesh + CreateCollisionDisplayNormals: the collision triangles, and per face a line from
// the face center along its normal, scaled by the collision box's smallest dimension when that exceeds 1 (opaque red,
// CImVector::Set(0xFF, 0xFF, 0, 0) = ARGB). 0.5.3 drew the triangles as a plain model; here they are half transparent
// white so the model stays visible through them.
static const uint32_t COLLISION_FACE_COLOR = 0x60FFFFFF;
static const uint32_t COLLISION_NORMAL_COLOR = 0xFFFF0000;

static void DrawCollisionMesh(const M2Data* data, const C44Matrix& world) {
    uint32_t indexCount = data->m_collisionIndices.m_count;
    uint32_t positionCount = data->m_collisionPositions.m_count;

    if (indexCount < 3 || !positionCount) {
        return;
    }

    const uint16_t* indices = data->m_collisionIndices.Data();
    const C3Vector* positions = data->m_collisionPositions.Data();
    const C3Vector* normals = data->m_collisionFaceNormals.m_count ? data->m_collisionFaceNormals.Data() : nullptr;
    uint32_t faceCount = indexCount / 3;

    const CAaBox& box = data->m_collisionBounds.m_extent;
    float dimX = box.t.x - box.b.x;
    float dimY = box.t.y - box.b.y;
    float dimZ = box.t.z - box.b.z;
    float smallest = dimX < dimY ? (dimX < dimZ ? dimX : dimZ) : (dimY < dimZ ? dimY : dimZ);
    float normalScale = smallest > 1.0f ? smallest : 1.0f;

    CImVector faceColor = MakeColor(COLLISION_FACE_COLOR);
    CImVector normalColor = MakeColor(COLLISION_NORMAL_COLOR);

    for (uint32_t face = 0; face < faceCount; face++) {
        uint16_t i0 = indices[face * 3 + 0];
        uint16_t i1 = indices[face * 3 + 1];
        uint16_t i2 = indices[face * 3 + 2];

        if (i0 >= positionCount || i1 >= positionCount || i2 >= positionCount) {
            continue;
        }

        const C3Vector& a = positions[i0];
        const C3Vector& b = positions[i1];
        const C3Vector& c = positions[i2];
        GxuDebugDrawTriangle(a * world, b * world, c * world, faceColor, GXUDEBUGSTYLE_SOLID);

        if (normals && face < data->m_collisionFaceNormals.m_count) {
            C3Vector center((a.x + b.x + c.x) * (1.0f / 3.0f), (a.y + b.y + c.y) * (1.0f / 3.0f), (a.z + b.z + c.z) * (1.0f / 3.0f));
            const C3Vector& n = normals[face];
            C3Vector tip(center.x + n.x * normalScale, center.y + n.y * normalScale, center.z + n.z * normalScale);
            GxuDebugDrawLine(center * world, tip * world, normalColor, GXUDEBUGSTYLE_SOLID);
        }
    }
}

// 0.5.3 CollisionDataRenderAABox: the collision box, alpha blended, no depth write, material diffuse 0x8000FFFF (half
// transparent cyan).
static const uint32_t COLLISION_AABOX_COLOR = 0x8000FFFF;

static void DrawCollisionAaBox(const M2Data* data, const C44Matrix& world) {
    const CAaBox& box = data->m_collisionBounds.m_extent;

    if (!(box.b.x < box.t.x) && !(box.b.y < box.t.y) && !(box.b.z < box.t.z)) {
        return;
    }

    GxuDebugDrawBoundsBox(box.b, box.t, world, MakeColor(COLLISION_AABOX_COLOR), GXUDEBUGSTYLE_SOLID);
}

// Port: queues the shown bounds and collision displays. Called by CM2Scene::Animate for each model on the draw list,
// where m_matrixF4 (model to M2 view space) is current; viewInv is the scene's m_viewInv, so m_matrixF4 * viewInv is
// the model's world transform, as in CM2Model::GetAnimatedWorldTransform. The bounds are the model file's (M2Data
// m_bounds: the extent box, and a sphere of m_radius around the extent center, as CM2Model::GetBoundingSphere builds
// it); the collision displays use the file's collision mesh and collision bounds. Models without collision data show
// no collision display, as in 0.5.3 (which checked the shared collision data first).
void CM2Model::DebugDraw(const C44Matrix& viewInv) const {
    uint32_t collision = this->m_debugCollision | CM2Model::s_debugWorldCollision;

    if ((this->m_debugBounds == DEBUG_BOUNDS_NONE && !collision) || !this->m_shared || !this->m_shared->m_data) {
        return;
    }

    const M2Data* data = this->m_shared->m_data;
    C44Matrix world = this->m_matrixF4 * viewInv;

    if (this->m_debugBounds != DEBUG_BOUNDS_NONE) {
        DrawBounds(data, this->m_debugBounds, world);
    }

    if (collision & DEBUG_COLLISION_MESH) {
        DrawCollisionMesh(data, world);
    }

    if ((collision & DEBUG_COLLISION_AABOX) && data->m_collisionIndices.m_count) {
        DrawCollisionAaBox(data, world);
    }
}

// Debug feature: model collision mesh display.
// 0.5.3 reference: ModelShowCollision (0x004479B0).
// 0.5.3 built a debug model with CollisionDataCreateModel and kept it in m_collideModel (queued MODEL_SHOW_COLLISION
// while loading). Here the flag is read when the model draws (CM2Model::DebugDraw).
void CM2Model::ShowCollision(BOOL show) {
    if (show) {
        this->m_debugCollision |= DEBUG_COLLISION_MESH;
    } else {
        this->m_debugCollision &= ~DEBUG_COLLISION_MESH;
    }
}

// Debug feature: model collision box display.
// 0.5.3 reference: ModelShowCollisionAaBox (0x00447A60).
// 0.5.3 added a custom geoset whose callback drew CollisionDataRenderAABox (queued MODEL_SHOW_COLLISION_AABOX while
// loading). Here the flag is read when the model draws (CM2Model::DebugDraw).
void CM2Model::ShowCollisionAaBox(BOOL show) {
    if (show) {
        this->m_debugCollision |= DEBUG_COLLISION_AABOX;
    } else {
        this->m_debugCollision &= ~DEBUG_COLLISION_AABOX;
    }
}

// Port: the world-wide switches. 0.5.3 CWorldScene::RenderDoodads called ModelShowCollision(enables & Enable_Collision)
// and ModelShowCollisionAaBox(enables & Enable_AABoxes) on every visible doodad each frame, and
// CGGameObject_C::ObjectPostAnimate the former on game objects. 3.3.5 has no such per-doodad hook, so CGWorldFrame
// syncs the two bits here every frame and DebugDraw applies them to every drawn M2 that has collision data (doodads
// and game objects; characters and creatures normally have none).
uint32_t CM2Model::s_debugWorldCollision;

void CM2Model::SetDebugWorldCollision(BOOL mesh, BOOL aaBox) {
    CM2Model::s_debugWorldCollision = (mesh ? DEBUG_COLLISION_MESH : 0) | (aaBox ? DEBUG_COLLISION_AABOX : 0);
}

// Debug feature: model hit-test geometry display.
// 0.5.3 reference: ModelShowHitTestGeometry (0x00434C80).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowHitTestGeometry() {
}

// Debug feature: unselectable geometry display.
// 0.5.3 reference: ModelShowUnselectable (0x00443200).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowUnselectable(uint8_t red, uint8_t green, uint8_t blue) {
}

// Debug feature: material layer toggle.
// 0.5.3 reference: ModelMaterialShowLayer (0x0043E880).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowMaterialLayer(uint32_t material, uint32_t layer, BOOL show) {
}

// Debug feature: scene render log.
// 0.5.3 reference: ModelRenderSceneLogToggle (0x0042DBA0).
// Wrote RenderLog.txt.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CM2Model::RenderSceneLogToggle(const char* fileName) {
    return 0;
}

// Debug feature: animation log.
// 0.5.3 reference: ModelAnimateLogToggle (0x00438DC0).
// Wrote AnimLog.txt.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CM2Model::AnimateLogToggle(const char* fileName) {
    return 0;
}

#endif
