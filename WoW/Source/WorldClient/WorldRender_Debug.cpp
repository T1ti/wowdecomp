#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: late drawing of the world debug overlays.
// The 3.3.5 release client draws the portal planes (CMapObj::RenderPortals, per WMO group) and the collision query
// facets (CWorldScene::RenderCollisionDebug, end of CWorldScene::Render) in the middle of the frame without writing
// depth, so the WMO groups, M2 models, liquids and particles drawn afterwards paint over them. With debug features
// both are held back and drawn by WorldDebugRenderLate at the end of CGWorldFrame::OnWorldRender, next to the
// collision info display. The release build keeps the original draw points.

struct DeferredPortalDraw {
    CMapObj* m_mapObj;
    CMapObjGroup* m_group;
    C44Matrix m_world;
};

static TSGrowableArray<DeferredPortalDraw> s_deferredPortals;
static TSGrowableArray<uint16_t> s_queryDrawIndices;

// Query facet colors set by CMapChunk / CMapObjGroup query code (Enable_CollisionDebug): tested faces red, hit faces
// green. Hit faces draw last and opaque so overlapping tested faces cannot tint them.
static const uint32_t s_queryHitColor = 0x00FF00;
static const uint32_t s_colorMask = 0x00FFFFFF;

// Records one WMO group's portal planes with the world transform RenderGroup set for it.
void CMapObj::DebugDeferPortals(CMapObjGroup* group) {
    const CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    DeferredPortalDraw* draw = s_deferredPortals.New();
    draw->m_mapObj = this;
    draw->m_group = group;
    draw->m_world = world.m_mtx[world.m_level];
}

static void RenderDeferredPortals() {
    for (uint32_t i = 0; i < s_deferredPortals.m_count; i++) {
        DeferredPortalDraw& draw = s_deferredPortals.m_data[i];
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, draw.m_world);
        draw.m_mapObj->RenderPortals(draw.m_group);
        GxXformPop(GxXform_World);
    }

    s_deferredPortals.SetCount(0);
}

static void RenderQueryFacets() {
    uint32_t vertexCount = CMap::debugVertexArray.m_count;
    uint32_t indexCount = CMap::debugIndexArray.m_count;

    if (!vertexCount) {
        return;
    }

    // CMap::TestQueryAdd writes three vertices and three indices per facet, so facet i owns vertices 3i..3i+2.
    // Reorder the index list so tested faces draw first and hit faces last, and make hit faces opaque.
    s_queryDrawIndices.SetCount(0);

    for (uint32_t pass = 0; pass < 2; pass++) {
        for (uint32_t i = 0; i + 2 < indexCount; i += 3) {
            CGxVertexPC& first = CMap::debugVertexArray.m_data[CMap::debugIndexArray.m_data[i]];
            bool hit = (*reinterpret_cast<uint32_t*>(&first.c) & s_colorMask) == s_queryHitColor;

            if (hit != (pass == 1)) {
                continue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                uint16_t index = CMap::debugIndexArray.m_data[i + j];

                if (hit) {
                    *reinterpret_cast<uint32_t*>(&CMap::debugVertexArray.m_data[index].c) |= 0xFF000000;
                }

                *s_queryDrawIndices.New() = index;
            }
        }
    }

    for (uint32_t i = 0; i < s_queryDrawIndices.m_count; i++) {
        CMap::debugIndexArray.m_data[i] = s_queryDrawIndices.m_data[i];
    }

    CWorldScene::RenderCollisionDebug();
    CMap::debugVertexArray.SetCount(0);
    CMap::debugIndexArray.m_count = 0;
}

void WorldDebugRenderLate() {
    RenderDeferredPortals();

    if (CWorld::enables & Enables::Enable_CollisionDebug) {
        RenderQueryFacets();
    }
}

#endif
