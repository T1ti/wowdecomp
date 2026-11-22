#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxBuf.h"
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

// Debug feature: the WMO BSP display (showMapObjBSP).
// 0.5.3 and 0.9.1 reference: CMapObj::RenderGroupBsp (0.9.1 0x00292028, MapObjRender.cpp), which CMapObj::PrepareUpdate
// picks for every group while Enable_MapObjBSP is set (see there). For the first frustum only (index 0), it draws the
// group's faces by their SMOPoly flags in three passes, lit, in the material color:
//   - render faces that are not detail faces, opaque green (0xFF00FF00);
//   - detail faces, opaque blue (0xFF807FFF);
//   - collision faces, alpha blended red at 50% (0x80FF0000) over the first two.
// 3.3.5 split it into one renderer per face kind (RenderGroupRenderFaces, RenderGroupDetailFaces,
// RenderGroupCollisionFaces, ...: same colors, chosen by CMapObj::s_renderMode, which nothing in the release build
// sets). This follows them for the 3.3.5 group layout, whose faces are indexed (three m_indices per SMOPoly) where
// 0.9.1 had three vertices of its own per face.
static const uint32_t BSP_INDEX_BATCH = 3000; // indices per draw, as the 3.3.5 face renderers (FACE_DEBUG_INDEX_BATCH)
static const uint32_t BSP_RENDER_COLOR = 0xFF00FF00;
static const uint32_t BSP_DETAIL_COLOR = 0xFF807FFF;
static const uint32_t BSP_COLLISION_COLOR = 0x80FF0000;

static void DrawBspBatch(CGxBuf* indexBuf, uint32_t count, uint32_t vertexCount) {
    GxBufUnlock(indexBuf, 0);

    if (!count) {
        return;
    }

    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = count;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount - 1);
    g_theGxDevicePtr->Draw(&batch, 1);
}

// Draws the faces whose flags include one of require and none of exclude, with the group's vertices bound.
static void DrawBspFaces(CMapObjGroup* group, uint8_t require, uint8_t exclude) {
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), BSP_INDEX_BATCH);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    uint32_t count = 0;
    const SMOPoly* poly = group->m_polyList;

    for (uint32_t i = 0; i < static_cast<uint32_t>(group->m_polyListSize); i++, poly++) {
        if (!(poly->m_flags & require) || (poly->m_flags & exclude)) {
            continue;
        }

        indices[count] = group->m_indices[i * 3];
        indices[count + 1] = group->m_indices[i * 3 + 1];
        indices[count + 2] = group->m_indices[i * 3 + 2];
        count += 3;

        if (count == BSP_INDEX_BATCH) {
            DrawBspBatch(indexBuf, count, group->m_vertexListCount);
            indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), BSP_INDEX_BATCH);
            indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
            count = 0;
        }
    }

    DrawBspBatch(indexBuf, count, group->m_vertexListCount);
}

void CMapObj::RenderGroupBsp(CMapObjGroup* group, uint32_t index) {
    if (index || !group->m_vertexListCount || !group->m_polyList || !group->m_indices) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Fog, 0);
    CMapObj::SetRenderModeLight();
    GxRsSet(GxRs_VertexShader, 0);
    GxRsSet(GxRs_PixelShader, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, static_cast<void*>(nullptr));

    // The group's positions and normals, once for the three passes (as RenderGroupCollisionFaces streams them).
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPN), group->m_vertexListCount);
    CGxVertexPN* vertices = reinterpret_cast<CGxVertexPN*>(g_theGxDevicePtr->BufLock(vertexBuf));

    for (uint32_t i = 0; i < static_cast<uint32_t>(group->m_vertexListCount); i++) {
        vertices[i].m_position = group->m_vertexList[i];
        vertices[i].m_normal = group->m_normalList[i];
    }

    GxBufUnlock(vertexBuf, 0);
    GxPrimVertexPtr(vertexBuf, GxVBF_PN);

    GxRsSet(GxRs_BlendingMode, GxBlend_Opaque);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(BSP_RENDER_COLOR));
    DrawBspFaces(group, SMOPoly::F_RENDER, SMOPoly::F_DETAIL);
    GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(BSP_DETAIL_COLOR));
    DrawBspFaces(group, SMOPoly::F_DETAIL, 0);

    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(BSP_COLLISION_COLOR));
    DrawBspFaces(group, SMOPoly::F_COLLISION, 0);

    g_theGxDevicePtr->RsPop();
}

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
