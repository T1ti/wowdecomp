#include "WowEdit/Source/WowEditTerrain.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cmath>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#include "WowEdit/Source/WowEditShaders.h"

// See WowEditTerrain.h.

static int32_t s_grid = WOWEDITGRID_OFF;
static bool s_slopes;
static int32_t s_overlay = WOWEDITOVERLAY_OFF;
static WowEditBrushSettings s_brush = { false, 10.0f, 0.5f };
static C3Vector s_brushPoint; // the terrain point under the cursor
static bool s_brushPointValid;
static CGxTex* s_brushTexture;

// Grid colors (RGBA), port defaults: the 8.1 editor sets them from its options, which are not known.
static const float s_cellColor[4] = { 1.0f, 1.0f, 1.0f, 0.35f }; // c7, cell lines and diagonals
static const float s_chunkColor[4] = { 1.0f, 0.85f, 0.1f, 0.8f }; // c5, chunk lines
static const float s_tileColor[4] = { 1.0f, 0.2f, 0.2f, 1.0f }; // c6, map tile lines

// Depth bias against the terrain the pass repeats (GxRs_PolygonOffset units, see GxuDebugDrawShapes.cpp), only when
// the terrain went through the fixed-function pipeline, whose depths the editor vertex shader cannot reproduce to the
// last bit. With the terrain vertex shaders the editor shader computes the position with the terrain's own constants
// and instructions (see TerrainVS), so the depths are equal and no bias is used: a constant bias is worth a few
// centimeters near the camera but several yards on distant hills, where it lets the overlay of a hill behind show
// through the hill in front.
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

// ---- Brush

// Brush look (port). The 8.1 shader draws nothing of its own: it lays the brush texture on the terrain and multiplies it
// by the tint, so the look is all in the texture, which is not known. Screenshots and videos of the editor show two
// dashed circles and nothing inside them: an outer one in red-orange and an inner one in yellow, with about the same
// number of dashes, shorter on the inner circle. That is one dashed-ring texture drawn twice with the same shader, at
// the brush radius and at the inner (falloff) radius, with two tints, which is what the port does.
//
// The texture: a white ring of BRUSH_DASHES dashes just inside the texture's edge, transparent elsewhere. The shader
// samples it with clamped addressing, which repeats the outermost texels forever outside the brush, so those (and
// everything past the ring) must be fully transparent.
static const uint32_t BRUSH_TEXTURE_SIZE = 256;
static const float BRUSH_RING_INNER = 0.955f; // ring band, in brush radii
static const float BRUSH_RING_OUTER = 0.975f;
static const float BRUSH_DASHES = 16.0f;
static const float BRUSH_DASH_DUTY = 0.6f; // part of each dash period that is drawn
static const float s_brushOuterTint[4] = { 1.0f, 0.35f, 0.2f, 1.0f };
static const float s_brushInnerTint[4] = { 1.0f, 0.85f, 0.2f, 1.0f };

// A linear ramp from 0 at edge0 to 1 at edge1, clamped.
static float Ramp(float value, float edge0, float edge1) {
    float t = (value - edge0) / (edge1 - edge0);
    return t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
}

static void UpdateBrushTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    static uint32_t s_texels[BRUSH_TEXTURE_SIZE * BRUSH_TEXTURE_SIZE];

    if (command != GxTex_Latch) {
        return;
    }

    const float texel = 2.0f / static_cast<float>(BRUSH_TEXTURE_SIZE); // one texel, in brush radii
    const float pi = 3.14159265f;

    for (uint32_t y = 0; y < BRUSH_TEXTURE_SIZE; y++) {
        for (uint32_t x = 0; x < BRUSH_TEXTURE_SIZE; x++) {
            // Position from the center, 1 at the texture's edge (the brush radius), at the texel center.
            float dx = (static_cast<float>(x) + 0.5f) * texel - 1.0f;
            float dy = (static_cast<float>(y) + 0.5f) * texel - 1.0f;
            float r = sqrtf(dx * dx + dy * dy);

            // The ring band, its edges smoothed over a texel.
            float band = Ramp(r, BRUSH_RING_INNER - texel, BRUSH_RING_INNER) * (1.0f - Ramp(r, BRUSH_RING_OUTER, BRUSH_RING_OUTER + texel));

            // The dashes: the angle around the center in dash periods, each dash's ends smoothed over a texel of arc.
            float period = (atan2f(dy, dx) / (2.0f * pi) + 0.5f) * BRUSH_DASHES;
            float phase = period - floorf(period);
            float end = texel * BRUSH_DASHES / (2.0f * pi * (r > 0.5f ? r : 0.5f)); // a texel of arc, in periods
            float dash = Ramp(phase, 0.0f, end) * (1.0f - Ramp(phase, BRUSH_DASH_DUTY - end, BRUSH_DASH_DUTY));

            float alpha = band * dash;

            if (x == 0 || y == 0 || x == BRUSH_TEXTURE_SIZE - 1 || y == BRUSH_TEXTURE_SIZE - 1) {
                alpha = 0.0f;
            }

            uint32_t a = static_cast<uint32_t>(alpha * 255.0f + 0.5f);
            s_texels[y * BRUSH_TEXTURE_SIZE + x] = (a << 24) | 0x00FFFFFF;
        }
    }

    texels = s_texels;
    texelStrideInBytes = width * 4;
}

static CGxTex* BrushTexture() {
    if (!s_brushTexture) {
        // Linear filtering, clamped (CGxTexFlags wrap 0), no mipmaps.
        GxTexCreate(BRUSH_TEXTURE_SIZE, BRUSH_TEXTURE_SIZE, GxTex_Argb8888, CGxTexFlags(GxTex_Linear), nullptr, UpdateBrushTexture, s_brushTexture);
    }

    return s_brushTexture;
}

static void UpdateBrushPoint() {
    C3Vector start;
    C3Vector end;
    s_brushPointValid = false;
    const WorldEditView* editorView = WorldEditGetView();
    if (!editorView || !WowEditWorldGetBrushRay(start, end)) {
        return;
    }

    C3Vector hit;
    float distance = 1.0f;
    s_brushPointValid = World::Intersect(start, end, &hit, &distance, WORLD_QUERY_TERRAIN, nullptr) != 0;
    if (!s_brushPointValid) {
        return;
    }

    const C2Vector xy(hit.x, hit.y);
    const CMapChunk* chunk = CMap::GetChunkAtPosition(&xy);
    s_brushPointValid = chunk && editorView->IncludesTerrain(chunk->m_bbox2);

    if (s_brushPointValid) {
        s_brushPoint = hit;
    }
}

// ---- Overlays

// The overlays (port): the 8.1 overlays shader filled with 3.3.5 terrain data. The 8.1 editor drew it for a block of
// 2 x 2 chunks at a time with a color per chunk and per cell; the port draws it per render chunk (one or two map chunks,
// CMapRenderChunk::m_chunks), the block's corner on their outer corner so that both fall in the block on the same
// side of it.
//
// Area IDs (WOWEDITOVERLAY_AREAS): each map chunk in the color of its area (CMapChunk::m_areaId, the MCNK area ID, a
// zone or a subzone), a color of its own per area ID, so that area borders show as color changes along chunk edges.
// Chunks without area stay clear. The cell colors are unused (the area ID is per chunk): the cell texture is clear.
static const float CHUNK_SIZE = 33.333332f;
static const float OVERLAY_ALPHA = 0.4f;
static CGxTex* s_overlayCellTexture;

static void UpdateOverlayCellTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    static uint32_t s_texels[16 * 16]; // clear: no per-cell colors yet

    if (command == GxTex_Latch) {
        texels = s_texels;
        texelStrideInBytes = width * 4;
    }
}

static CGxTex* OverlayCellTexture() {
    if (!s_overlayCellTexture) {
        // Point sampled: one texel per cell.
        GxTexCreate(16, 16, GxTex_Argb8888, CGxTexFlags(GxTex_Nearest), nullptr, UpdateOverlayCellTexture, s_overlayCellTexture);
    }

    return s_overlayCellTexture;
}

// A color of its own for an area ID: a hue from a hash of the ID, so that neighboring areas (often consecutive IDs)
// differ, at full saturation and brightness.
static void AreaColor(int32_t areaId, float* color) {
    uint32_t hash = static_cast<uint32_t>(areaId) * 2654435761u;
    float hue = static_cast<float>(hash >> 8) / 16777216.0f * 6.0f;
    int32_t sector = static_cast<int32_t>(hue);
    float f = hue - static_cast<float>(sector);
    float rgb[6][3] = {
        { 1.0f, f, 0.0f }, { 1.0f - f, 1.0f, 0.0f }, { 0.0f, 1.0f, f },
        { 0.0f, 1.0f - f, 1.0f }, { f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f - f },
    };
    const float* c = rgb[sector < 6 ? sector : 5];
    color[0] = c[0];
    color[1] = c[1];
    color[2] = c[2];
    color[3] = OVERLAY_ALPHA;
}

// Per render chunk: the block corner and the chunk colors. False when no map chunk of it has an area.
static bool SetAreaOverlayConstants(CMapRenderChunk* renderChunk) {
    float constants[NUM_WOWEDITCONSTS][4] = {};
    constants[WOWEDITCONST_PROJECT_X][0] = 1.0f;
    constants[WOWEDITCONST_PROJECT_Y][1] = 1.0f;

    // The block corner: the largest x and y of the map chunks (CMapChunk::m_bbox.t, the corner world coordinates count
    // down from), so that every cell lies on the same side of it.
    float originX = -1.0e30f;
    float originY = -1.0e30f;

    for (uint32_t i = 0; i < MAP_CHUNKS_PER_RENDER_BATCH; i++) {
        CMapChunk* chunk = renderChunk->m_chunks[i];

        if (chunk) {
            originX = chunk->m_bbox.t.x > originX ? chunk->m_bbox.t.x : originX;
            originY = chunk->m_bbox.t.y > originY ? chunk->m_bbox.t.y : originY;
        }
    }

    constants[WOWEDITCONST_OVERLAY_ORIGIN][0] = originX;
    constants[WOWEDITCONST_OVERLAY_ORIGIN][1] = originY;
    bool any = false;

    for (uint32_t i = 0; i < MAP_CHUNKS_PER_RENDER_BATCH; i++) {
        CMapChunk* chunk = renderChunk->m_chunks[i];

        const int32_t areaId = chunk ? WowEditWorldGetAreaId(*chunk) : 0;
        if (!areaId) {
            continue;
        }

        // The chunk's place in the block, as the shader counts it: x along world y, y along world x.
        int32_t blockX = static_cast<int32_t>((originY - chunk->m_bbox.t.y) / CHUNK_SIZE + 0.5f);
        int32_t blockY = static_cast<int32_t>((originX - chunk->m_bbox.t.x) / CHUNK_SIZE + 0.5f);

        if (blockX < 0 || blockX > 1 || blockY < 0 || blockY > 1) {
            continue;
        }

        AreaColor(areaId, constants[WOWEDITCONST_OVERLAY_CHUNK_COLOR + blockX + 2 * blockY]);
        any = true;
    }

    if (any) {
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &constants[0][0], NUM_WOWEDITCONSTS);
    }

    return any;
}

// The area under the cursor, as a label above the ground: its name and ID, and its zone's when it is a subzone.
static void DrawAreaLabel() {
    if (!s_brushPointValid) {
        return;
    }

    const C2Vector xy(s_brushPoint.x, s_brushPoint.y);
    const CMapChunk* chunk = CMap::GetChunkAtPosition(&xy);
    const int32_t areaId = chunk ? WowEditWorldGetAreaId(*chunk) : 0;

    if (!areaId) {
        return;
    }

    const AreaTableRec* area = g_areaTableDB.GetRecord(areaId);
    const AreaTableRec* zone = area && area->m_parentAreaID ? g_areaTableDB.GetRecord(area->m_parentAreaID) : nullptr;
    char text[256];

    if (zone) {
        SStrPrintf(text, sizeof(text), "%s (%d), in %s (%d)", area->m_areaName ? area->m_areaName : "?", areaId, zone->m_areaName ? zone->m_areaName : "?", zone->m_id);
    } else {
        SStrPrintf(text, sizeof(text), "%s (%d)", area && area->m_areaName ? area->m_areaName : "?", areaId);
    }

    float color[4];
    AreaColor(areaId, color);
    CImVector textColor;
    textColor.r = static_cast<uint8_t>(color[0] * 255.0f);
    textColor.g = static_cast<uint8_t>(color[1] * 255.0f);
    textColor.b = static_cast<uint8_t>(color[2] * 255.0f);
    textColor.a = 0xFF;
    GxuDebugDrawText3D(text, C3Vector(s_brushPoint.x, s_brushPoint.y, s_brushPoint.z + 1.5f), textColor);
}

static void SetBrushConstants(float radius, const float* tint) {
    float constants[NUM_WOWEDITCONSTS][4] = {};
    constants[WOWEDITCONST_PROJECT_X][0] = 1.0f;
    constants[WOWEDITCONST_PROJECT_Y][1] = 1.0f;
    constants[WOWEDITCONST_BRUSH_CENTER][0] = s_brushPoint.x;
    constants[WOWEDITCONST_BRUSH_CENTER][1] = s_brushPoint.y;
    constants[WOWEDITCONST_BRUSH_RADIUS][0] = radius;

    for (uint32_t i = 0; i < 4; i++) {
        constants[WOWEDITCONST_BRUSH_TINT][i] = tint[i];
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &constants[0][0], NUM_WOWEDITCONSTS);
}

// The chunks the brush reaches, from their bounding spheres (world coordinates), seen from above.
static uint32_t BrushChunks(CMapRenderChunk* const* chunks, uint32_t count, TSGrowableArray<CMapRenderChunk*>& reached) {
    reached.SetCount(0);

    for (uint32_t i = 0; i < count; i++) {
        const CAaSphere& sphere = chunks[i]->m_sphere;
        float dx = sphere.c.x - s_brushPoint.x;
        float dy = sphere.c.y - s_brushPoint.y;
        float reach = sphere.r + s_brush.radius;

        if (dx * dx + dy * dy <= reach * reach) {
            *reached.New() = chunks[i];
        }
    }

    return reached.Count();
}

// Binds a chunk's vertices and indices the way CMapRenderChunk::RenderSetup does, without its transform and lights.
static void BindChunkGeometry(CMapRenderChunk* chunk) {
    if (chunk->m_buf) {
        GxPrimVertexPtr(chunk->m_buf->m_vertexBuf, CMapRenderChunk::s_gxBufVertexFormat);
        g_theGxDevicePtr->PrimIndexPtr(chunk->m_buf->m_indexBuf);
    } else {
        chunk->UseStreamingBufs();
    }
}

// The origin a chunk's vertices are relative to. CMapChunk::CreateVertices builds them two ways: with the terrain
// vertex shaders (CMap::enableTerrainShaderVertex) as world positions (CreateVerticesWorld, drawn with the world
// transform CWorldScene::RenderChunks sets, the camera origin's opposite), otherwise local to the render chunk's
// m_vec1 (CreateVerticesLocal, drawn with the world transform of CMapRenderChunk::RenderSetup).
static C3Vector ChunkVertexOrigin(const CMapRenderChunk* chunk) {
    return CMap::enableTerrainShaderVertex ? C3Vector(0.0f, 0.0f, 0.0f) : chunk->m_vec1;
}

// The world transform of a chunk's vertices: to positions relative to the camera origin, as the terrain draws them.
static C44Matrix ChunkWorldTransform(const CMapRenderChunk* chunk) {
    C3Vector origin = ChunkVertexOrigin(chunk);
    C44Matrix world;
    world.d0 = origin.x - CWorldScene::s_activeWorldView.x;
    world.d1 = origin.y - CWorldScene::s_activeWorldView.y;
    world.d2 = origin.z - CWorldScene::s_activeWorldView.z;
    return world;
}

// TerrainVS constants: c0..c3 the world-view rows, c4..c7 the projection rows, c8 the vertex origin. With the terrain
// vertex shaders, the world-view and projection are the very matrices the terrain was drawn with
// (CMapRenderChunk::s_vertexShaderConsts, set by CWorldScene::RenderChunks through InitializeVertexShaderConstants),
// in the same registers as the Terrain.bls shaders.
static void DrawChunks(CMapRenderChunk* const* chunks, uint32_t count, const C44Matrix& view, const C44Matrix& projection, bool (*perChunk)(CMapRenderChunk*) = nullptr) {
    CGxDevice* device = g_theGxDevicePtr;
    const CMapRenderChunk::SVertexShaderConstants& terrainConsts = CMapRenderChunk::s_vertexShaderConsts;

    for (uint32_t i = 0; i < count; i++) {
        CMapRenderChunk* chunk = chunks[i];

        // The overlays set their constants per chunk, and skip chunks with nothing to show.
        if (perChunk && !perChunk(chunk)) {
            continue;
        }

        C3Vector origin = ChunkVertexOrigin(chunk);
        C44Matrix world = ChunkWorldTransform(chunk);

        C44Matrix worldView = CMap::enableTerrainShaderVertex ? terrainConsts.m_worldView : world * view;
        const C44Matrix& chunkProjection = CMap::enableTerrainShaderVertex ? terrainConsts.m_projection : projection;
        device->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&worldView), 4);
        device->ShaderConstantsSet(GxSh_Vertex, 4, reinterpret_cast<const float*>(&chunkProjection), 4);
        float vertexOrigin[4] = { origin.x, origin.y, origin.z, 1.0f };
        device->ShaderConstantsSet(GxSh_Vertex, 8, vertexOrigin, 1);

        BindChunkGeometry(chunk);

        if (chunk->m_batch.m_count) {
            device->Draw(&chunk->m_batch, 1);
        }
    }
}

static void TerrainPass(CMapRenderChunk* const* chunks, uint32_t count) {
    if (!count || !(CWorld::enables & Enables::Enable_Chunks)) {
        s_brushPointValid = false;
        return;
    }

    CGxShader* vertexShader = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS);
    CGxShader* gridShader = s_grid >= 0 ? WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) : nullptr;
    CGxShader* slopesShader = s_slopes ? WowEditShaderGet(WOWEDITSHADER_SLOPES) : nullptr;
    CGxShader* brushShader = nullptr;
    CGxShader* overlaysShader = s_overlay != WOWEDITOVERLAY_OFF ? WowEditShaderGet(WOWEDITSHADER_OVERLAYS) : nullptr;

    // The cursor's terrain point, for the brush and the area label.
    if (s_brush.enabled || s_overlay == WOWEDITOVERLAY_AREAS) {
        UpdateBrushPoint();
    }

    if (s_brush.enabled) {
        brushShader = s_brushPointValid ? WowEditShaderGet(WOWEDITSHADER_BRUSH) : nullptr;
    }

    if (!vertexShader || (!gridShader && !slopesShader && !brushShader && !overlaysShader)) {
        return;
    }

    CGxDevice* device = g_theGxDevicePtr;
    device->RsPush();

    device->RsSet(GxRs_DepthTest, 1);
    device->RsSet(GxRs_DepthFunc, static_cast<int32_t>(GxCmp_LEqual));
    device->RsSet(GxRs_DepthWrite, 0);
    device->RsSet(GxRs_PolygonOffset, CMap::enableTerrainShaderVertex ? 0.0f : TERRAIN_OVERLAY_DEPTH_BIAS);
    device->RsSet(GxRs_Culling, 0);
    device->RsSet(GxRs_Fog, 0);
    device->RsSet(GxRs_Lighting, 0);
    device->RsSet(GxRs_ColorWrite, static_cast<int32_t>(GxColorWrite_All));
    device->RsSet(GxRs_Texture0, static_cast<void*>(nullptr));
    device->RsSet(GxRs_Texture1, static_cast<void*>(nullptr));
    device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Alpha));
    GxUpdateAlphaRef(GxRs_AlphaRef);

    CGxMatrixStack& viewStack = device->m_xforms[GxXform_View];
    const C44Matrix& view = viewStack.m_mtx[viewStack.m_level];
    C44Matrix projection;
    GxXformProjNative(projection);
    device->RsSet(GxRs_VertexShader, vertexShader);

    // The overlays first, under the slopes, grid and brush.
    CGxTex* cellTexture = overlaysShader ? OverlayCellTexture() : nullptr;

    if (cellTexture) {
        device->RsSet(GxRs_PixelShader, overlaysShader);
        device->RsSet(GxRs_Texture0, cellTexture);
        DrawChunks(chunks, count, view, projection, SetAreaOverlayConstants);
        device->RsSet(GxRs_Texture0, static_cast<void*>(nullptr));

        if (s_overlay == WOWEDITOVERLAY_AREAS) {
            DrawAreaLabel();
        }
    }

    if (slopesShader) {
        device->RsSet(GxRs_PixelShader, slopesShader);
        SetSlopesConstants();
        DrawChunks(chunks, count, view, projection);
    }

    if (gridShader) {
        device->RsSet(GxRs_PixelShader, gridShader);
        SetGridConstants();
        DrawChunks(chunks, count, view, projection);
    }

    // The brush last, over the other overlays, on the chunks it reaches.
    static TSGrowableArray<CMapRenderChunk*> s_brushChunks;
    CGxTex* brushTexture = brushShader ? BrushTexture() : nullptr;

    if (brushTexture && BrushChunks(chunks, count, s_brushChunks)) {
        device->RsSet(GxRs_PixelShader, brushShader);
        device->RsSet(GxRs_Texture0, brushTexture);
        CMapRenderChunk* const* reached = s_brushChunks.m_data;
        uint32_t reachedCount = s_brushChunks.Count();
        SetBrushConstants(s_brush.radius, s_brushOuterTint);
        DrawChunks(reached, reachedCount, view, projection);

        if (s_brush.innerFraction > 0.0f && s_brush.innerFraction < 1.0f) {
            SetBrushConstants(s_brush.radius * s_brush.innerFraction, s_brushInnerTint);
            DrawChunks(reached, reachedCount, view, projection);
        }
    }

    device->RsPop();
}

static void UpdateTerrainPass() {
    WorldEditSetTerrainPass(s_grid >= 0 || s_slopes || s_brush.enabled || s_overlay != WOWEDITOVERLAY_OFF ? TerrainPass : nullptr);
}

bool WowEditTerrainPrepare() {
    bool ready = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS) != nullptr;

    if (s_grid >= 0) {
        ready = WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) != nullptr && ready;
    }

    if (s_slopes) {
        ready = WowEditShaderGet(WOWEDITSHADER_SLOPES) != nullptr && ready;
    }

    if (s_brush.enabled) {
        ready = WowEditShaderGet(WOWEDITSHADER_BRUSH) != nullptr && ready;
    }

    if (s_overlay != WOWEDITOVERLAY_OFF) {
        ready = WowEditShaderGet(WOWEDITSHADER_OVERLAYS) != nullptr && ready;
    }

    return ready;
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

void WowEditTerrainSetOverlay(int32_t overlay) {
    s_overlay = overlay > WOWEDITOVERLAY_OFF && overlay < NUM_WOWEDITOVERLAYS ? overlay : WOWEDITOVERLAY_OFF;
    UpdateTerrainPass();
}

int32_t WowEditTerrainGetOverlay() {
    return s_overlay;
}

void WowEditTerrainSetBrush(const WowEditBrushSettings& settings) {
    const bool starting = settings.enabled && settings.radius > 0.0f && !s_brush.enabled;
    s_brush.enabled = settings.enabled && settings.radius > 0.0f;
    if (settings.radius > 0.0f) {
        s_brush.radius = settings.radius < WOWEDITBRUSH_MAX_RADIUS ? settings.radius : WOWEDITBRUSH_MAX_RADIUS;
    }
    s_brush.innerFraction = settings.innerFraction > 0.0f ? (settings.innerFraction < 1.0f ? settings.innerFraction : 1.0f) : 0.0f;
    s_brushPointValid = false;
    UpdateTerrainPass();

    if (starting) {
        WowEditPrepareTerrainOverlays();
    }
}

WowEditBrushSettings WowEditTerrainGetBrush() {
    return s_brush;
}
