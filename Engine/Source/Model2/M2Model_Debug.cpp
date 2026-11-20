#include "Engine/Source/Model2/M2Model.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: model bounding sphere display.
// 0.5.3 reference: ModelShowBoundingSphere (0x004349B0).
// Built a debug model with CreateModelBoundingSphere ("Bounding Sphere") from IModelGetBoundingSphere; queued MODEL_SHOW_BOUNDING_SPHERE while loading.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowBoundingSphere() {
}

// Debug feature: model bounding box display.
// 0.5.3 reference: ModelShowBoundingBox (0x00434A90).
// Built a debug model with CreateModelBoundingBox ("BoundingBox").
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::ShowBoundingBox() {
}

// Debug feature: model bounds display.
// 0.5.3 reference: ModelHideBounds (0x00434C10).
// Queued MODEL_HIDE_BOUNDS while loading.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CM2Model::HideBounds() {
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
