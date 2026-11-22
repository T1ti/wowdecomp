#include "WowEdit/Source/WowEditTerrain.h"

#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#include "WowEdit/Source/WowEditShaders.h"

// See WowEditTerrain.h.

static int32_t s_grid = WOWEDITGRID_OFF;
static bool s_slopes;
static bool s_test;
static uint32_t s_passCount; // terrain passes drawn since an overlay was turned on
static uint32_t s_lastChunkCount; // chunks the last pass drew over

// Grid colors (RGBA), port defaults: the 8.1 editor sets them from its options, which are not known.
static const float s_cellColor[4] = { 1.0f, 1.0f, 1.0f, 0.35f }; // c7, cell lines and diagonals
static const float s_chunkColor[4] = { 1.0f, 0.85f, 0.1f, 0.8f }; // c5, chunk lines
static const float s_tileColor[4] = { 1.0f, 0.2f, 0.2f, 1.0f }; // c6, map tile lines

// Depth bias against the terrain the pass repeats (GxRs_PolygonOffset units, see GxuDebugDrawShapes.cpp). The pass
// goes through the editor vertex shader while the terrain may have gone through the fixed-function pipeline or the
// terrain vertex shader, so the depths can differ in the last bits.
static const float TERRAIN_OVERLAY_DEPTH_BIAS = 2.0f / 65536.0f;

static void SetGridConstants() {
    float constants[NUM_WOWEDITCONSTS][4] = {};
    // ProjectPosition rows: the position is already the world position, so they pick its x and y.
    constants[WOWEDITCONST_PROJECT_X][0] = 1.0f;
    constants[WOWEDITCONST_PROJECT_Y][1] = 1.0f;
    // The vertex shader already gives the grid coordinates in chunks.
    constants[WOWEDITCONST_GRID_SCALE][0] = 1.0f;
    constants[WOWEDITCONST_GRID_SCALE][1] = 1.0f;

    for (uint32_t i = 0; i < 4; i++) {
        constants[WOWEDITCONST_CHUNK_COLOR][i] = s_chunkColor[i];
        constants[WOWEDITCONST_TILE_COLOR][i] = s_tileColor[i];
        constants[WOWEDITCONST_CELL_COLOR][i] = s_cellColor[i];
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &constants[0][0], NUM_WOWEDITCONSTS);
}

// The comparison direction. The 8.1 constant's space is unknown; here the normal is cross(ddx, ddy) of the world
// position on a Direct3D 9 screen (x to the right, y down), which for ground seen from above points down: for a
// camera facing +x, moving right on screen goes along -y and moving down goes along -x, and (0, -1, 0) x (-1, 0, 0) is
// (0, 0, -1). So the direction is straight down, and flat ground compares as 1.
static void SetSlopesConstants() {
    float constants[NUM_WOWEDITCONSTS][4] = {};
    constants[0][2] = -1.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &constants[0][0], NUM_WOWEDITCONSTS);
}

// Binds a chunk's vertices and indices the way CMapRenderChunk::RenderSetup does, without its transform and lights.
static void BindChunkGeometry(CMapRenderChunk* chunk) {
    if (chunk->m_chunkBuf) {
        GxPrimVertexPtr(chunk->m_chunkBuf->m_vertexBuf, CMapRenderChunk::s_gxBufVertexFormat);
        g_theGxDevicePtr->PrimIndexPtr(chunk->m_chunkBuf->m_indexBuf);
    } else {
        chunk->UseStreamingBufs();
    }
}

static void DrawChunks(CMapRenderChunk* const* chunks, uint32_t count, const C44Matrix& viewProj) {
    CGxDevice* device = g_theGxDevicePtr;
    const C3Vector& viewOrigin = CWorldScene::s_activeWorldView;

    for (uint32_t i = 0; i < count; i++) {
        CMapRenderChunk* chunk = chunks[i];

        // Vertices are local to m_vec1 and drawn relative to the camera origin, as in RenderSetup.
        C44Matrix world;
        world.d0 = chunk->m_vec1.x - viewOrigin.x;
        world.d1 = chunk->m_vec1.y - viewOrigin.y;
        world.d2 = chunk->m_vec1.z - viewOrigin.z;
        C44Matrix worldViewProj = world * viewProj;

        float constants[5][4] = {
            { worldViewProj.a0, worldViewProj.a1, worldViewProj.a2, worldViewProj.a3 },
            { worldViewProj.b0, worldViewProj.b1, worldViewProj.b2, worldViewProj.b3 },
            { worldViewProj.c0, worldViewProj.c1, worldViewProj.c2, worldViewProj.c3 },
            { worldViewProj.d0, worldViewProj.d1, worldViewProj.d2, worldViewProj.d3 },
            { chunk->m_vec1.x, chunk->m_vec1.y, chunk->m_vec1.z, 1.0f },
        };
        device->ShaderConstantsSet(GxSh_Vertex, 0, &constants[0][0], 5);

        BindChunkGeometry(chunk);

        if (chunk->m_batch.m_count) {
            device->Draw(&chunk->m_batch, 1);
        }
    }
}

static void TerrainPass(CMapRenderChunk* const* chunks, uint32_t count) {
    if (!(CWorld::enables & Enables::Enable_Chunks)) {
        return;
    }

    CGxShader* vertexShader = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS);
    CGxShader* gridShader = s_grid >= 0 ? WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) : nullptr;
    CGxShader* slopesShader = s_slopes ? WowEditShaderGet(WOWEDITSHADER_SLOPES) : nullptr;
    CGxShader* testShader = s_test ? WowEditShaderGet(WOWEDITSHADER_TEST) : nullptr;

    if (!vertexShader || (!gridShader && !slopesShader && !testShader)) {
        return;
    }

    s_passCount++;
    s_lastChunkCount = count;

    CGxDevice* device = g_theGxDevicePtr;
    device->RsPush();

    device->RsSet(GxRs_DepthTest, 1);
    device->RsSet(GxRs_DepthFunc, static_cast<int32_t>(GxCmp_LEqual));
    device->RsSet(GxRs_DepthWrite, 0);
    device->RsSet(GxRs_PolygonOffset, TERRAIN_OVERLAY_DEPTH_BIAS);
    device->RsSet(GxRs_Culling, 0);
    device->RsSet(GxRs_Fog, 0);
    device->RsSet(GxRs_Lighting, 0);
    device->RsSet(GxRs_ColorWrite, static_cast<int32_t>(GxColorWrite_All));
    device->RsSet(GxRs_Texture0, static_cast<void*>(nullptr));
    device->RsSet(GxRs_Texture1, static_cast<void*>(nullptr));
    device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Alpha));
    GxUpdateAlphaRef(GxRs_AlphaRef);
    device->RsSet(GxRs_VertexShader, vertexShader);

    CGxMatrixStack& viewStack = device->m_xforms[GxXform_View];
    C44Matrix projection;
    GxXformProjNative(projection);
    C44Matrix viewProj = viewStack.m_mtx[viewStack.m_level] * projection;

    if (testShader) {
        device->RsSet(GxRs_PixelShader, testShader);
        DrawChunks(chunks, count, viewProj);
    }

    if (slopesShader) {
        device->RsSet(GxRs_PixelShader, slopesShader);
        SetSlopesConstants();
        DrawChunks(chunks, count, viewProj);
    }

    if (gridShader) {
        device->RsSet(GxRs_PixelShader, gridShader);
        SetGridConstants();
        DrawChunks(chunks, count, viewProj);
    }

    device->RsPop();
}

static void UpdateTerrainPass() {
    s_passCount = 0;
    s_lastChunkCount = 0;
    WorldEditSetTerrainPass(s_grid >= 0 || s_slopes || s_test ? TerrainPass : nullptr);
}

bool WowEditTerrainPrepare() {
    bool ready = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS) != nullptr;

    if (s_grid >= 0) {
        ready = WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) != nullptr && ready;
    }

    if (s_slopes) {
        ready = WowEditShaderGet(WOWEDITSHADER_SLOPES) != nullptr && ready;
    }

    if (s_test) {
        ready = WowEditShaderGet(WOWEDITSHADER_TEST) != nullptr && ready;
    }

    return ready;
}

void WowEditTerrainGetStats(uint32_t* passes, uint32_t* chunks) {
    *passes = s_passCount;
    *chunks = s_lastChunkCount;
}

void WowEditTerrainSetGrid(int32_t permutation) {
    s_grid = permutation >= 0 && permutation < WOWEDITGRID_PERMUTATIONS ? permutation : WOWEDITGRID_OFF;
    UpdateTerrainPass();
}

int32_t WowEditTerrainGetGrid() {
    return s_grid;
}

void WowEditTerrainSetSlopes(bool show) {
    s_slopes = show;
    UpdateTerrainPass();
}

bool WowEditTerrainGetSlopes() {
    return s_slopes;
}

void WowEditTerrainSetTest(bool show) {
    s_test = show;
    UpdateTerrainPass();
}

bool WowEditTerrainGetTest() {
    return s_test;
}
