#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: collision info display (0.5.3 CollisionInfo*, drawn by RenderCollisionInfo).
// The active mover's movement facets, its collision box and its move vector are collected while it moves and drawn
// over the world. 0.5.3 kept positions, colors and indices in separate arrays and drew them with GxPrimLockVertexPtrs;
// this keeps CGxVertexPC triangles and lines and draws them like CWorldScene::RenderCollisionDebug.

static WOWGUID s_currentWatchGUID;
static int32_t s_acceptingFacets;
static int32_t s_debugCollision;
static TSGrowableArray<CGxVertexPC> s_debugTriVerts;
static TSGrowableArray<uint16_t> s_debugTriIndices;
static TSGrowableArray<FACET_COLOR> s_debugFacetColors;
static TSGrowableArray<CGxVertexPC> s_debugLineVerts;
static TSGrowableArray<uint16_t> s_debugLineIndices;
static TSGrowableArray<uint16_t> s_debugDrawIndices;

// 0.5.3 s_facetColor (0x00842664) RGB values. 0.5.3 stored alpha 0 for every color but FACET_UNTESTED; here the
// non-blocking states are half transparent and blocking facets are opaque so they always read as green.
static const uint32_t s_facetColor[NUM_FACET_COLORS] = {
    0x80FF0080, // FACET_UNTESTED
    0x80FF0080, // FACET_TESTED_UNTOUCHED
    0x80FFFF80, // FACET_TESTED_TOUCHED
    0xFF00FF80, // FACET_BLOCKING
};

static const uint32_t s_boxColor = 0x80FF0000;    // 0.5.3 RenderCollisionInfo box diffuse.
static const uint32_t s_lineColor = 0xFF0000FF;   // 0.5.3 RenderCollisionInfo normal-line color (blue).
static const float s_normalLineScale = 0.8333333f; // 0.5.3 AddNormalLine scale.
static const float s_vectorLineScale = 0.5555556f; // 0.5.3 CollisionInfoAddVector scale.

static CImVector MakeColor(uint32_t argb) {
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = argb;
    return color;
}

static void AddTriangle(const C3Vector& a, const C3Vector& b, const C3Vector& c, uint32_t argb) {
    uint16_t base = static_cast<uint16_t>(s_debugTriVerts.Count());
    const C3Vector* corners[3] = { &a, &b, &c };

    for (uint32_t i = 0; i < 3; i++) {
        CGxVertexPC* vertex = s_debugTriVerts.New();
        vertex->p = *corners[i];
        vertex->c = MakeColor(argb);
    }

    *s_debugTriIndices.New() = base;
    *s_debugTriIndices.New() = base + 1;
    *s_debugTriIndices.New() = base + 2;
}

// 0.5.3 AddNormalLine_0 (0x004DD170): a line from start along direction * scale.
static void AddLine(const C3Vector& direction, const C3Vector& start, float scale) {
    uint16_t base = static_cast<uint16_t>(s_debugLineVerts.Count());

    CGxVertexPC* vertex = s_debugLineVerts.New();
    vertex->p = start;
    vertex->c = MakeColor(s_lineColor);

    vertex = s_debugLineVerts.New();
    vertex->p = C3Vector(start.x + direction.x * scale, start.y + direction.y * scale, start.z + direction.z * scale);
    vertex->c = MakeColor(s_lineColor);

    *s_debugLineIndices.New() = base;
    *s_debugLineIndices.New() = base + 1;
}

static void ClearBuffers() {
    s_debugTriVerts.SetCount(0);
    s_debugTriIndices.SetCount(0);
    s_debugFacetColors.SetCount(0);
    s_debugLineVerts.SetCount(0);
    s_debugLineIndices.SetCount(0);
}

// 0.5.3 reference: CollisionInfoSetWatchGUID (0x004DCC90).
void CollisionInfoSetWatchGUID(const WOWGUID& guid) {
    s_currentWatchGUID = guid;
}

// 0.5.3 reference: CollisionInfoReset (0x004DCCB0).
void CollisionInfoReset() {
    ClearBuffers();
}

// 0.5.3 reference: CollisionInfoSetFaces (0x004DCD60), called by CMovement::GetMoveFacets.
// Replaces the facet set when guid is the watched unit; other units turn facet collection off until the next call.
void CollisionInfoSetFaces(const WOWGUID& guid, const TSGrowableArray<CFacet>& facets) {
    if (!s_debugCollision || guid != s_currentWatchGUID) {
        s_acceptingFacets = 0;
        return;
    }

    s_acceptingFacets = 1;
    ClearBuffers();

    for (uint32_t i = 0; i < facets.m_count; i++) {
        const CFacet& facet = facets.m_data[i];
        AddTriangle(facet.v[0], facet.v[1], facet.v[2], s_facetColor[FACET_UNTESTED]);
        *s_debugFacetColors.New() = FACET_UNTESTED;

        C3Vector center((facet.v[0].x + facet.v[1].x + facet.v[2].x) * 0.33333334f, (facet.v[0].y + facet.v[1].y + facet.v[2].y) * 0.33333334f, (facet.v[0].z + facet.v[1].z + facet.v[2].z) * 0.33333334f);
        AddLine(facet.plane.n, center, s_normalLineScale);
    }
}

// 0.5.3 reference: CollisionInfoColorFace (0x004DD370). A face only moves up the FACET_COLOR order.
// 0.5.3 called it from the CMovement obstacle and surface tests. 3.3.5 movement was rewritten around
// CMovement_C::DistanceToMove, so the calls sit there (CollisionInfoColorSweep) and in CMovement_C::TraceSurface
// (obstructing and too-steep facets as FACET_BLOCKING).
void CollisionInfoColorFace(uint32_t face, FACET_COLOR color) {
    if (!s_acceptingFacets || face >= s_debugFacetColors.Count() || color <= s_debugFacetColors.m_data[face]) {
        return;
    }

    s_debugFacetColors.m_data[face] = color;

    for (uint32_t i = 0; i < 3; i++) {
        s_debugTriVerts.m_data[face * 3 + i].c = MakeColor(s_facetColor[color]);
    }
}

// Debug feature: colors one CMovement_C::DistanceToMove sweep over the move facets. Every facet of the set was tested
// (0.5.3 FindObstacles colored facets it skipped FACET_TESTED_UNTOUCHED) and the facet the sweep stopped at was touched
// (0.5.3 TraceSurface colored the surface it followed FACET_TESTED_TOUCHED). No 0.5.3 function of its own.
void CollisionInfoColorSweep(uint32_t hitFace) {
    if (!s_acceptingFacets) {
        return;
    }

    for (uint32_t i = 0; i < s_debugFacetColors.m_count; i++) {
        CollisionInfoColorFace(i, FACET_TESTED_UNTOUCHED);
    }

    CollisionInfoColorFace(hitFace, FACET_TESTED_TOUCHED);
}

// 0.5.3 reference: CollisionInfoAddBox (0x004DDDC0). Adds the 12 triangles of an axis-aligned box.
void CollisionInfoAddBox(const C3Vector& minimum, const C3Vector& maximum) {
    if (!s_acceptingFacets) {
        return;
    }

    C3Vector v[8];

    for (uint32_t i = 0; i < 8; i++) {
        v[i] = C3Vector((i & 1) ? maximum.x : minimum.x, (i & 2) ? maximum.y : minimum.y, (i & 4) ? maximum.z : minimum.z);
    }

    static const uint8_t faces[6][4] = {
        { 0, 2, 3, 1 }, // bottom
        { 4, 5, 7, 6 }, // top
        { 0, 1, 5, 4 }, // -y
        { 2, 6, 7, 3 }, // +y
        { 0, 4, 6, 2 }, // -x
        { 1, 3, 7, 5 }, // +x
    };

    for (uint32_t i = 0; i < 6; i++) {
        AddTriangle(v[faces[i][0]], v[faces[i][1]], v[faces[i][2]], s_boxColor);
        AddTriangle(v[faces[i][0]], v[faces[i][2]], v[faces[i][3]], s_boxColor);
    }
}

// 0.5.3 reference: CollisionInfoSetFallBox (0x004DD520).
// TODO: 0.5.3 drew a 32-vertex fall shape (a box widened toward the bottom) with five outward normal lines; this draws
// the plain collision box instead.
void CollisionInfoSetFallBox(const C3Vector& position, float radius, float height) {
    CollisionInfoAddBox(C3Vector(position.x - radius, position.y - radius, position.z), C3Vector(position.x + radius, position.y + radius, position.z + height));
}

// 0.5.3 reference: CollisionInfoAddVector (0x004DE890).
void CollisionInfoAddVector(const C3Vector& start, const C3Vector& direction) {
    if (s_acceptingFacets) {
        AddLine(direction, start, s_vectorLineScale);
    }
}

// 0.5.3 reference: ToggleCollisionInfo (0x00613220). Backs the Lua ToggleCollisionDisplay.
// Watches the active mover; turning it on without an active mover leaves it off.
int32_t ToggleCollisionInfo() {
    WOWGUID watch = {};
    s_debugCollision = !s_debugCollision;

    if (s_debugCollision) {
        watch = CGUnit_C::s_activeMover;

        if (!watch) {
            s_debugCollision = 0;
        }
    }

    CollisionInfoReset();
    CollisionInfoSetWatchGUID(watch);
    s_acceptingFacets = 0;

    return s_debugCollision;
}

// 0.5.3 reference: RenderCollisionInfo (0x00613280).
// Called from CGWorldFrame::OnWorldRender after the world, M2 models, liquids and particles, like 0.5.3. Depth testing
// is off (like CWorldScene::RenderCollisionDebug) so the facets, box and lines stay visible through the geometry they
// belong to.
void RenderCollisionInfo() {
    if (!s_debugCollision || (!s_debugTriIndices.Count() && !s_debugLineIndices.Count())) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_PolygonOffset, 1.0f);

    C44Matrix matrix;
    matrix.Translate(C3Vector(-CWorldScene::s_activeWorldView.x, -CWorldScene::s_activeWorldView.y, -CWorldScene::s_activeWorldView.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, matrix);

    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_Culling, 0);

    if (s_debugTriIndices.Count()) {
        // Facets draw in FACET_COLOR order so higher states blend over lower ones (blocking last), then the box.
        // 0.5.3 drew them in gather order.
        s_debugDrawIndices.SetCount(0);
        uint32_t facetCount = s_debugFacetColors.m_count;

        for (uint32_t color = 0; color < NUM_FACET_COLORS; color++) {
            for (uint32_t i = 0; i < facetCount; i++) {
                if (s_debugFacetColors.m_data[i] == color) {
                    for (uint32_t j = 0; j < 3; j++) {
                        *s_debugDrawIndices.New() = s_debugTriIndices.m_data[i * 3 + j];
                    }
                }
            }
        }

        for (uint32_t i = facetCount * 3; i < s_debugTriIndices.m_count; i++) {
            *s_debugDrawIndices.New() = s_debugTriIndices.m_data[i];
        }

        CGxVertexPC* vertices = s_debugTriVerts.m_data;
        GxPrimVertexPtr(s_debugTriVerts.Count(), &vertices->p, sizeof(CGxVertexPC), nullptr, 0, &vertices->c, sizeof(CGxVertexPC), nullptr, 0, nullptr, 0);
        GxPrimIndexPtr(s_debugDrawIndices.Count(), s_debugDrawIndices.m_data);

        CGxBatch batch;
        batch.m_count = s_debugDrawIndices.Count();
        batch.m_maxIndex = s_debugTriVerts.Count() - 1;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_minIndex = 0;
        g_theGxDevicePtr->Draw(&batch, 1);
    }

    if (s_debugLineIndices.Count()) {
        CGxVertexPC* vertices = s_debugLineVerts.m_data;
        GxPrimVertexPtr(s_debugLineVerts.Count(), &vertices->p, sizeof(CGxVertexPC), nullptr, 0, &vertices->c, sizeof(CGxVertexPC), nullptr, 0, nullptr, 0);
        GxPrimIndexPtr(s_debugLineIndices.Count(), s_debugLineIndices.m_data);

        CGxBatch batch;
        batch.m_count = s_debugLineIndices.Count();
        batch.m_maxIndex = s_debugLineVerts.Count() - 1;
        batch.m_primType = GxPrim_Lines;
        batch.m_start = 0;
        batch.m_minIndex = 0;
        g_theGxDevicePtr->Draw(&batch, 1);
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// 0.5.3 reference: CMovement::ShowCollisionBox (0x004D2700), called from CMovement::CollideRequestMove.
// position is the world position the move starts from and move the unit move vector. 0.5.3 drew the fall shape while
// falling, else the collision box and a move vector from its middle; 0.5.3 also used the redirected direction while
// redirected, which is not tracked here. 0.5.3 raised the box bottom by m_stepUpHeight, but the 3.3.5 step-up height is
// about as tall as the collision height (that left a thin slab at head height), so the box is drawn as
// BuildCollisionBox builds it, from the feet up to m_collisionHeight.
void CMovement_C::ShowCollisionBox(const C3Vector& position, const C3Vector& move) {
    if (this->m_flags & MOVEMENTFLAG_FALL_MASK) {
        CollisionInfoSetFallBox(position, this->m_collisionRadius, this->m_collisionHeight);
        return;
    }

    CAaBox box;
    this->BuildCollisionBox(position, box);
    CollisionInfoAddBox(box.b, box.t);

    C3Vector middle(position.x, position.y, position.z + this->m_collisionHeight * 0.5f);
    CollisionInfoAddVector(middle, move);
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
