#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: collision info display.
// 0.5.3 reference: CollisionInfoSetWatchGUID (0x004DCC90).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoSetWatchGUID(const WOWGUID& guid) {
}

// 0.5.3 reference: CollisionInfoReset (0x004DCCB0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoReset() {
}

// 0.5.3 reference: CollisionInfoSetFaces (0x004DCD60).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoSetFaces(const WOWGUID& guid, const TSGrowableArray<CFacet>& facets) {
}

// 0.5.3 reference: CollisionInfoColorFace (0x004DD370).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoColorFace(uint32_t face, FACET_COLOR color) {
}

// 0.5.3 reference: CollisionInfoSetFallBox (0x004DD520).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoSetFallBox(const C3Vector& position, float radius, float height) {
}

// 0.5.3 reference: CollisionInfoAddBox (0x004DDDC0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoAddBox(const C3Vector& minimum, const C3Vector& maximum) {
}

// 0.5.3 reference: CollisionInfoAddVector (0x004DE890).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CollisionInfoAddVector(const C3Vector& start, const C3Vector& end) {
}

// 0.5.3 reference: ToggleCollisionInfo (0x00613220).
// Backs the Lua ToggleCollisionDisplay.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t ToggleCollisionInfo() {
    return 0;
}

// 0.5.3 reference: RenderCollisionInfo (0x00613280).
// 0.5.3 called it from CGWorldFrame::OnWorldRender.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void RenderCollisionInfo() {
}

// Debug feature: movement collision box display.
// 0.5.3 reference: CMovement::ShowCollisionBox (0x004D2700).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CMovement_C::ShowCollisionBox(const C3Vector& position, uint32_t color) {
}

// Debug feature: movement logging.
// 0.5.3 reference: CMovement::ToggleLogging (0x004CAF70).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CMovement_C::ToggleLogging() {
    return 0;
}

// Debug feature: fall logging.
// 0.5.3 reference: CMovement::ToggleFallLogging (0x004CADC0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CMovement_C::ToggleFallLogging() {
    return 0;
}

#endif
