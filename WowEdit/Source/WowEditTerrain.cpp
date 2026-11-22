#include "WowEdit/Source/WowEditTerrain.h"

#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"
#include "Engine/Source/Gx/CGxDeviceD3d9Ex/CGxDeviceD3d9Ex.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/Gx.h"
#include <cstring>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#include "WowEdit/Source/WowEditShaders.h"

// See WowEditTerrain.h.

static int32_t s_grid = WOWEDITGRID_OFF;
static bool s_slopes;
static int32_t s_test = WOWEDITTEST_OFF;
static bool s_probe; // print the device state at the next pass's first draw
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

static IDirect3DDevice9* ProbeD3dDevice();
static void ProbeDevice(const CMapRenderChunk* chunk, const C44Matrix& viewProj, int32_t pixels, bool late);

// Draws each chunk once with the bound shaders (or, with fixedFunction, through the fixed-function pipeline with the
// world transform RenderSetup sets).
static void DrawChunks(CMapRenderChunk* const* chunks, uint32_t count, const C44Matrix& viewProj, bool fixedFunction = false) {
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

        if (fixedFunction) {
            GxXformSet(GxXform_World, world);
            BindChunkGeometry(chunk);

            if (chunk->m_batch.m_count) {
                device->Draw(&chunk->m_batch, 1);
            }

            continue;
        }

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

// Probe support: an occlusion query around the first draw group of the pass counts the pixels it wrote.
static IDirect3DQuery9* BeginProbeQuery() {
    IDirect3DDevice9* d3d = ProbeD3dDevice();
    IDirect3DQuery9* query = nullptr;

    if (d3d && SUCCEEDED(d3d->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query)) && query) {
        query->Issue(D3DISSUE_BEGIN);
    }

    return query;
}

static int32_t EndProbeQuery(IDirect3DQuery9* query) {
    if (!query) {
        return -1;
    }

    query->Issue(D3DISSUE_END);
    DWORD pixels = 0;
    HRESULT result = S_FALSE;
    DWORD start = GetTickCount();

    while ((result = query->GetData(&pixels, sizeof(pixels), D3DGETDATA_FLUSH)) == S_FALSE && GetTickCount() - start < 500) {
    }

    query->Release();
    return result == S_OK ? static_cast<int32_t>(pixels) : -1;
}

// Runs one draw group, under the probe's occlusion query when a probe is pending (the first group of the pass).
template <typename DRAW>
static void DrawGroup(CMapRenderChunk* const* chunks, const C44Matrix& viewProj, bool late, DRAW draw) {
    if (!s_probe) {
        draw();
        return;
    }

    s_probe = false;
    IDirect3DQuery9* query = BeginProbeQuery();
    draw();
    int32_t pixels = EndProbeQuery(query);
    ProbeDevice(chunks[0], viewProj, pixels, late);
}

static void TerrainPass(CMapRenderChunk* const* chunks, uint32_t count, bool late) {
    if (!(CWorld::enables & Enables::Enable_Chunks)) {
        return;
    }

    // The late test draws only in the late call; everything else only in the terrain call.
    if (late != (s_test == WOWEDITTEST_LATE)) {
        return;
    }

    CGxShader* vertexShader = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS);
    CGxShader* gridShader = !late && s_grid >= 0 ? WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) : nullptr;
    CGxShader* slopesShader = !late && s_slopes ? WowEditShaderGet(WOWEDITSHADER_SLOPES) : nullptr;
    bool shaderTest = s_test == WOWEDITTEST_SHADER || s_test == WOWEDITTEST_NO_DEPTH || s_test == WOWEDITTEST_LATE;
    CGxShader* testShader = shaderTest ? WowEditShaderGet(WOWEDITSHADER_TEST) : nullptr;
    bool fixedTest = s_test == WOWEDITTEST_FIXED;

    if (!fixedTest && (!vertexShader || (!gridShader && !slopesShader && !testShader))) {
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

    CGxMatrixStack& viewStack = device->m_xforms[GxXform_View];
    C44Matrix projection;
    GxXformProjNative(projection);
    C44Matrix viewProj = viewStack.m_mtx[viewStack.m_level] * projection;

    if (fixedTest) {
        device->RsSet(GxRs_VertexShader, static_cast<void*>(nullptr));
        device->RsSet(GxRs_PixelShader, static_cast<void*>(nullptr));
        device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Opaque));
        GxUpdateAlphaRef(GxRs_AlphaRef);
        device->XformPush(GxXform_World);
        DrawGroup(chunks, viewProj, late, [&] { DrawChunks(chunks, count, viewProj, true); });
        GxXformPop(GxXform_World);
        device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Alpha));
        GxUpdateAlphaRef(GxRs_AlphaRef);
    }

    if (!vertexShader) {
        device->RsPop();
        return;
    }

    device->RsSet(GxRs_VertexShader, vertexShader);

    if (testShader) {
        device->RsSet(GxRs_PixelShader, testShader);

        if (s_test == WOWEDITTEST_NO_DEPTH) {
            device->RsSet(GxRs_DepthTest, 0);
        }

        DrawGroup(chunks, viewProj, late, [&] { DrawChunks(chunks, count, viewProj); });
        device->RsSet(GxRs_DepthTest, 1);
    }

    if (slopesShader) {
        device->RsSet(GxRs_PixelShader, slopesShader);
        SetSlopesConstants();
        DrawGroup(chunks, viewProj, late, [&] { DrawChunks(chunks, count, viewProj); });
    }

    if (gridShader) {
        device->RsSet(GxRs_PixelShader, gridShader);
        SetGridConstants();
        DrawGroup(chunks, viewProj, late, [&] { DrawChunks(chunks, count, viewProj); });
    }

    device->RsPop();
}

static void UpdateTerrainPass() {
    s_passCount = 0;
    s_lastChunkCount = 0;
    WorldEditSetTerrainPass(s_grid >= 0 || s_slopes || s_test != WOWEDITTEST_OFF ? TerrainPass : nullptr);
}

bool WowEditTerrainPrepare() {
    bool ready = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS) != nullptr;

    if (s_grid >= 0) {
        ready = WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid)) != nullptr && ready;
    }

    if (s_slopes) {
        ready = WowEditShaderGet(WOWEDITSHADER_SLOPES) != nullptr && ready;
    }

    if (s_test == WOWEDITTEST_SHADER || s_test == WOWEDITTEST_NO_DEPTH || s_test == WOWEDITTEST_LATE) {
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

void WowEditTerrainSetTest(int32_t mode) {
    s_test = mode > WOWEDITTEST_OFF && mode < NUM_WOWEDITTESTS ? mode : WOWEDITTEST_OFF;
    UpdateTerrainPass();
}

int32_t WowEditTerrainGetTest() {
    return s_test;
}

void WowEditTerrainProbe() {
    s_probe = true;
}

// ---- Probe

static IDirect3DDevice9* ProbeD3dDevice() {
    CGxDevice* device = g_theGxDevicePtr;

    if (device->DeviceApi() == GxApi_D3d9) {
        return static_cast<CGxDeviceD3d*>(device)->m_d3dDevice;
    }

    if (device->DeviceApi() == GxApi_D3d9Ex) {
        return static_cast<CGxDeviceD3d9Ex*>(device)->m_d3dDevice;
    }

    return nullptr;
}

static void ProbeShader(const char* label, IUnknown* bound, HRESULT result, const char* entry, CGxShader* ours) {
    if (FAILED(result)) {
        ConsoleWriteA("  %s: cannot read back (0x%08X, pure device?)", WARNING_COLOR, label, static_cast<uint32_t>(result));
        return;
    }

    const char* match = "another shader";

    if (!bound) {
        match = "none (fixed function)";
    } else if (ours && bound == ours->m_apiSpecific) {
        match = entry;
    }

    ConsoleWriteA("  %s: %s", DEFAULT_COLOR, label, match);
}

// Prints the state the first draw of the pass ran with. Called right after that draw, so the device holds what
// CGxDeviceD3d::IStateSync sent for it.
static void ProbeDevice(const CMapRenderChunk* chunk, const C44Matrix& viewProj, int32_t pixels, bool late) {
    IDirect3DDevice9* d3d = ProbeD3dDevice();
    ConsoleWriteA("WowEdit probe (test mode %d, grid %d, slopes %d, %s pass):", DEFAULT_COLOR, s_test, s_grid, s_slopes ? 1 : 0, late ? "late" : "terrain");

    if (pixels >= 0) {
        ConsoleWriteA("  pixels written by the first draw group (occlusion query): %d", DEFAULT_COLOR, pixels);
    } else {
        ConsoleWrite("  pixels written: occlusion query unavailable", WARNING_COLOR);
    }

    C44Matrix world;
    world.d0 = chunk->m_vec1.x - CWorldScene::s_activeWorldView.x;
    world.d1 = chunk->m_vec1.y - CWorldScene::s_activeWorldView.y;
    world.d2 = chunk->m_vec1.z - CWorldScene::s_activeWorldView.z;
    C44Matrix worldViewProj = world * viewProj;

    if (!d3d) {
        ConsoleWrite("  not a Direct3D device", WARNING_COLOR);
        return;
    }

    // Shaders: the first draw of the pass is the test pass when one is on, then slopes, then the grid.
    CGxShader* vs = WowEditShaderGet(WOWEDITSHADER_TERRAIN_VS);
    CGxShader* ps = nullptr;

    if (s_test == WOWEDITTEST_SHADER || s_test == WOWEDITTEST_NO_DEPTH || s_test == WOWEDITTEST_LATE) {
        ps = WowEditShaderGet(WOWEDITSHADER_TEST);
    } else if (s_slopes) {
        ps = WowEditShaderGet(WOWEDITSHADER_SLOPES);
    } else if (s_grid >= 0) {
        ps = WowEditShaderGet(static_cast<WOWEDITSHADER>(WOWEDITSHADER_GRID0 + s_grid));
    }

    IDirect3DVertexShader9* boundVs = nullptr;
    IDirect3DPixelShader9* boundPs = nullptr;
    HRESULT vsResult = d3d->GetVertexShader(&boundVs);
    HRESULT psResult = d3d->GetPixelShader(&boundPs);
    ProbeShader("vertex shader", boundVs, vsResult, "TerrainVS", vs);
    ProbeShader("pixel shader", boundPs, psResult, "the overlay shader", ps);

    if (boundVs) {
        boundVs->Release();
    }

    if (boundPs) {
        boundPs->Release();
    }

    if (vs) {
        ConsoleWriteA("  TerrainVS: valid %d, loaded %d, %u bytes", DEFAULT_COLOR, vs->m_valid, vs->m_loaded, vs->m_code.Count());
    }

    if (ps) {
        ConsoleWriteA("  overlay pixel shader: valid %d, loaded %d, %u bytes", DEFAULT_COLOR, ps->m_valid, ps->m_loaded, ps->m_code.Count());
    }

    // Vertex shader constants c0..c4 as the device holds them.
    float constants[5][4] = {};

    if (SUCCEEDED(d3d->GetVertexShaderConstantF(0, &constants[0][0], 5))) {
        for (uint32_t i = 0; i < 5; i++) {
            ConsoleWriteA("  c%u: %.4f %.4f %.4f %.4f", DEFAULT_COLOR, i, constants[i][0], constants[i][1], constants[i][2], constants[i][3]);
        }
    }

    // Where the chunk origin lands: clip space, then normalized device coordinates (on screen: x and y in -1..1,
    // z in 0..1).
    float clipX = worldViewProj.d0;
    float clipY = worldViewProj.d1;
    float clipZ = worldViewProj.d2;
    float clipW = worldViewProj.d3;
    float invW = clipW != 0.0f ? 1.0f / clipW : 0.0f;
    ConsoleWriteA("  chunk origin (%.1f, %.1f, %.1f), camera origin (%.1f, %.1f, %.1f)", DEFAULT_COLOR, chunk->m_vec1.x, chunk->m_vec1.y, chunk->m_vec1.z, CWorldScene::s_activeWorldView.x, CWorldScene::s_activeWorldView.y, CWorldScene::s_activeWorldView.z);
    ConsoleWriteA("  chunk origin on screen: w %.3f, ndc %.3f %.3f %.5f", DEFAULT_COLOR, clipW, clipX * invW, clipY * invW, clipZ * invW);
    ConsoleWriteA("  batch: prim %d, start %u, count %u, indices %u..%u", DEFAULT_COLOR, static_cast<int32_t>(chunk->m_batch.m_primType), chunk->m_batch.m_start, chunk->m_batch.m_count, chunk->m_batch.m_minIndex, chunk->m_batch.m_maxIndex);

    // Vertex declaration and stream 0.
    IDirect3DVertexDeclaration9* decl = nullptr;

    if (SUCCEEDED(d3d->GetVertexDeclaration(&decl)) && decl) {
        D3DVERTEXELEMENT9 elements[MAXD3DDECLLENGTH + 1];
        UINT count = MAXD3DDECLLENGTH + 1;

        if (SUCCEEDED(decl->GetDeclaration(elements, &count))) {
            for (UINT i = 0; i < count && elements[i].Stream != 0xFF; i++) {
                ConsoleWriteA("  element %u: stream %u, offset %u, type %u, usage %u.%u", DEFAULT_COLOR, i, elements[i].Stream, elements[i].Offset, elements[i].Type, elements[i].Usage, elements[i].UsageIndex);
            }
        }

        decl->Release();
    } else {
        DWORD fvf = 0;
        d3d->GetFVF(&fvf);
        ConsoleWriteA("  no vertex declaration (FVF 0x%X)", WARNING_COLOR, fvf);
    }

    IDirect3DVertexBuffer9* stream = nullptr;
    UINT offset = 0;
    UINT stride = 0;

    if (SUCCEEDED(d3d->GetStreamSource(0, &stream, &offset, &stride))) {
        ConsoleWriteA("  stream 0: %s, offset %u, stride %u", DEFAULT_COLOR, stream ? "bound" : "none", offset, stride);

        // The first vertices of the last draw (position first, as the declaration has it) and where they land, taken
        // with the first chunk's transform: right only when the last chunk drawn is also the first.
        if (stream && stride >= sizeof(C3Vector)) {
            void* data = nullptr;
            HRESULT lock = stream->Lock(offset, stride * 3, &data, D3DLOCK_READONLY);

            if (SUCCEEDED(lock) && data) {
                for (uint32_t v = 0; v < 3; v++) {
                    C3Vector position;
                    memcpy(&position, static_cast<const uint8_t*>(data) + v * stride, sizeof(position));
                    float x = position.x * worldViewProj.a0 + position.y * worldViewProj.b0 + position.z * worldViewProj.c0 + worldViewProj.d0;
                    float y = position.x * worldViewProj.a1 + position.y * worldViewProj.b1 + position.z * worldViewProj.c1 + worldViewProj.d1;
                    float z = position.x * worldViewProj.a2 + position.y * worldViewProj.b2 + position.z * worldViewProj.c2 + worldViewProj.d2;
                    float w = position.x * worldViewProj.a3 + position.y * worldViewProj.b3 + position.z * worldViewProj.c3 + worldViewProj.d3;
                    float invW = w != 0.0f ? 1.0f / w : 0.0f;
                    ConsoleWriteA("  vertex %u: (%.2f, %.2f, %.2f), ndc %.3f %.3f %.5f", DEFAULT_COLOR, v, position.x, position.y, position.z, x * invW, y * invW, z * invW);
                }

                stream->Unlock();
            } else {
                ConsoleWriteA("  vertices: cannot read the vertex buffer (0x%08X)", WARNING_COLOR, static_cast<uint32_t>(lock));
            }
        }

        if (stream) {
            stream->Release();
        }
    }

    // Render states.
    struct ProbeState {
        D3DRENDERSTATETYPE m_state;
        const char* m_name;
        bool m_float;
    };

    static const ProbeState s_states[] = {
        { D3DRS_ZENABLE, "ZENABLE", false },
        { D3DRS_ZFUNC, "ZFUNC", false },
        { D3DRS_ZWRITEENABLE, "ZWRITEENABLE", false },
        { D3DRS_DEPTHBIAS, "DEPTHBIAS", true },
        { D3DRS_SLOPESCALEDEPTHBIAS, "SLOPESCALEDEPTHBIAS", true },
        { D3DRS_CULLMODE, "CULLMODE", false },
        { D3DRS_FILLMODE, "FILLMODE", false },
        { D3DRS_ALPHABLENDENABLE, "ALPHABLENDENABLE", false },
        { D3DRS_SRCBLEND, "SRCBLEND", false },
        { D3DRS_DESTBLEND, "DESTBLEND", false },
        { D3DRS_BLENDOP, "BLENDOP", false },
        { D3DRS_ALPHATESTENABLE, "ALPHATESTENABLE", false },
        { D3DRS_ALPHAFUNC, "ALPHAFUNC", false },
        { D3DRS_ALPHAREF, "ALPHAREF", false },
        { D3DRS_COLORWRITEENABLE, "COLORWRITEENABLE", false },
        { D3DRS_STENCILENABLE, "STENCILENABLE", false },
        { D3DRS_CLIPPLANEENABLE, "CLIPPLANEENABLE", false },
        { D3DRS_SCISSORTESTENABLE, "SCISSORTESTENABLE", false },
        { D3DRS_CLIPPING, "CLIPPING", false },
        { D3DRS_FOGENABLE, "FOGENABLE", false },
        { D3DRS_SRGBWRITEENABLE, "SRGBWRITEENABLE", false },
    };

    for (const ProbeState& state : s_states) {
        DWORD value = 0;

        if (FAILED(d3d->GetRenderState(state.m_state, &value))) {
            continue;
        }

        if (state.m_float) {
            float number;
            memcpy(&number, &value, sizeof(number));
            ConsoleWriteA("  %s: %g", DEFAULT_COLOR, state.m_name, number);
        } else {
            ConsoleWriteA("  %s: %u", DEFAULT_COLOR, state.m_name, static_cast<uint32_t>(value));
        }
    }

    // Viewport and targets.
    D3DVIEWPORT9 viewport;

    if (SUCCEEDED(d3d->GetViewport(&viewport))) {
        ConsoleWriteA("  viewport: %u,%u %ux%u, z %.3f..%.3f", DEFAULT_COLOR, static_cast<uint32_t>(viewport.X), static_cast<uint32_t>(viewport.Y), static_cast<uint32_t>(viewport.Width), static_cast<uint32_t>(viewport.Height), viewport.MinZ, viewport.MaxZ);
    }

    IDirect3DSurface9* surface = nullptr;
    D3DSURFACE_DESC desc;

    if (SUCCEEDED(d3d->GetRenderTarget(0, &surface)) && surface) {
        if (SUCCEEDED(surface->GetDesc(&desc))) {
            ConsoleWriteA("  render target: %ux%u, format %u, multisample %u", DEFAULT_COLOR, desc.Width, desc.Height, static_cast<uint32_t>(desc.Format), static_cast<uint32_t>(desc.MultiSampleType));
        }

        surface->Release();
    }

    surface = nullptr;

    if (SUCCEEDED(d3d->GetDepthStencilSurface(&surface)) && surface) {
        if (SUCCEEDED(surface->GetDesc(&desc))) {
            ConsoleWriteA("  depth buffer: %ux%u, format %u, multisample %u", DEFAULT_COLOR, desc.Width, desc.Height, static_cast<uint32_t>(desc.Format), static_cast<uint32_t>(desc.MultiSampleType));
        }

        surface->Release();
    }
}
