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

// Port: queues the shown bounds. Called by CM2Scene::Animate for each model on the draw list, where m_matrixF4 (model
// to M2 view space) is current; viewInv is the scene's m_viewInv, so m_matrixF4 * viewInv is the model's world
// transform, as in CM2Model::GetAnimatedWorldTransform. The bounds are the model file's (M2Data m_bounds: the extent
// box, and a sphere of m_radius around the extent center, as CM2Model::GetBoundingSphere builds it).
void CM2Model::DebugDrawBounds(const C44Matrix& viewInv) const {
    if (this->m_debugBounds == DEBUG_BOUNDS_NONE || !this->m_shared || !this->m_shared->m_data) {
        return;
    }

    const M2Bounds& bounds = this->m_shared->m_data->m_bounds;
    C44Matrix world = this->m_matrixF4 * viewInv;
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = BOUNDS_COLOR;

    if (this->m_debugBounds == DEBUG_BOUNDS_SPHERE) {
        C3Vector center((bounds.m_extent.b.x + bounds.m_extent.t.x) * 0.5f, (bounds.m_extent.b.y + bounds.m_extent.t.y) * 0.5f, (bounds.m_extent.b.z + bounds.m_extent.t.z) * 0.5f);
        GxuDebugDrawBoundsSphere(center, bounds.m_radius, world, color, GXUDEBUGSTYLE_SOLID);
    } else {
        GxuDebugDrawBoundsBox(bounds.m_extent.b, bounds.m_extent.t, world, color, GXUDEBUGSTYLE_SOLID);
    }
}

// Debug feature: model collision mesh display.
// 0.5.3 reference: ModelShowCollision (0x004479B0).
// Built a debug model with CollisionDataCreateModel; queued MODEL_SHOW_COLLISION while loading.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowCollision(BOOL show) {
}

// Debug feature: model collision box display.
// 0.5.3 reference: ModelShowCollisionAaBox (0x00447A60).
// Drew the box with CollisionDataRenderAABox; queued MODEL_SHOW_COLLISION_AABOX while loading.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowCollisionAaBox(BOOL show) {
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
