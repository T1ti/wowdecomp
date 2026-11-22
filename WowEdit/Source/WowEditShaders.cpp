#include "WowEdit/Source/WowEditShaders.h"

#include <cstring>
#include <windows.h>
#include <d3dcompiler.h>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"

// See WowEditShaders.h.
//
// The 8.1 pixel shaders read a full terrain vertex output (SV_POSITION, COLOR0, TEXCOORD0..4); only two inputs are
// used by the ported functions, so the vertex shader writes only those:
//   - TEXCOORD0 "position": 8.1 dots it with cb11[0] and cb11[1] to get a world xy (ProjectPosition), and takes its
//     screen-space derivatives for the slope. Here it is the absolute world position, and c0 / c1 are the rows
//     (1, 0, 0, 0) and (0, 1, 0, 0), so ProjectPosition stays in the code and returns world x and y.
//   - TEXCOORD4 "gridCoord" (TEXCOORD1 here): 8.1 scales it by cb11[4] into chunk units, which the grid frequencies
//     show: 0.24 = 1 / 4.1667 (a cell, 1/8 chunk), 0.03 = 1 / 33.333 (a chunk), 0.001875 = 1 / 533.33 (a map tile),
//     and the line widths (derivative of the chunk coordinates times 8 for cells, 2 for chunks, 1/8 for tiles) only
//     come out as about one pixel each in those units. Here it is world xy / 33.333 and c4 is (1, 1).
// Terrain chunk, cell and tile borders lie on multiples of those sizes in absolute world coordinates (the map is
// 64 x 533.33 yards with its center at 0), so the absolute position lines the grid up with the terrain.

static const char* s_shaderSource = R"HLSL(
// ---- Vertex shader (port): CMapRenderChunk vertices (local to the chunk) to clip space.
// c0..c3: rows of world * view * projection (row vectors, as C44Matrix); c4.xyz: the chunk's world origin.
float4 vsWorldViewProj[4] : register(c0);
float4 vsChunkOrigin : register(c4);

struct TerrainVertex {
    float3 position : POSITION;
};

struct TerrainOutput {
    float4 screenPosition : POSITION;
    float3 position : TEXCOORD0; // world position
    float2 gridCoord : TEXCOORD1; // world xy in chunks
};

TerrainOutput TerrainVS(TerrainVertex input) {
    TerrainOutput output;
    output.screenPosition = input.position.x * vsWorldViewProj[0] + input.position.y * vsWorldViewProj[1] + input.position.z * vsWorldViewProj[2] + vsWorldViewProj[3];
    output.position = input.position + vsChunkOrigin.xyz;
    output.gridCoord = output.position.xy * 0.0300000011920928955078125f;
    return output;
}

// ---- Pixel shaders, from 8.1 (cb11[n] is c[n]).
float4 cb11[8] : register(c0);

struct PixelInput {
    float3 position : TEXCOORD0;
    float2 gridCoord : TEXCOORD1;
};

float2 ProjectPosition(float3 position) {
    float4 homogeneousPosition = float4(position, 1.0f);
    return float2(dot(cb11[0], homogeneousPosition), dot(cb11[1], homogeneousPosition));
}

// Keep both frac operations: replacing this with frac(x + 0.5) changes rounding.
float CenteredRepeat(float x) {
    return frac(frac(x) - 0.5f) - 0.5f;
}

float2 CenteredRepeat(float2 x) {
    return frac(frac(x) - 0.5f) - 0.5f;
}

float4 CenteredRepeat(float4 x) {
    return frac(frac(x) - 0.5f) - 0.5f;
}

static const float FineFrequency = 0.2400000095367431640625f; // 0x3e75c290, one cell
static const float MediumFrequency = 0.0300000011920928955078125f; // 0x3cf5c290, one chunk
static const float CoarseFrequency = 0.00187500007450580596923828125f; // 0x3af5c290, one map tile

// Permutation 0: discard chunk interiors; the band within 5% of a chunk border outputs transparent black.
float4 GridPS0(PixelInput input) : COLOR0 {
    float2 cellFraction = frac(input.gridCoord * cb11[4].yx);

    if (cellFraction.x > 0.05f && cellFraction.x < 0.95f && cellFraction.y > 0.05f && cellFraction.y < 0.95f) {
        discard;
    }

    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

// Permutation 1: cell lines and both diagonals, color c7.
float4 GridPS1(PixelInput input) : COLOR0 {
    float2 projected = ProjectPosition(input.position);
    float sumDistance = abs(CenteredRepeat((projected.x + projected.y) * FineFrequency));
    float2 scaledCoord = input.gridCoord.yx * cb11[4].xy;
    float2 width = (abs(ddy(scaledCoord)) + abs(ddx(scaledCoord))) * 8.0f;

    float2 sumRatios = sumDistance / width;
    float sumFade = min(sumRatios.y, sumRatios.x);
    float2 axisRatios = abs(CenteredRepeat(projected * FineFrequency)) / width;
    float axisFade = min(axisRatios.y, axisRatios.x);
    float2 diagonals = float2(projected.x - projected.y, projected.y - projected.x);
    float2 diagonalRatios = abs(CenteredRepeat(diagonals * FineFrequency)) / width;
    float diagonalFade = min(diagonalRatios.y, diagonalRatios.x);

    float fade = min(min(sumFade, 1.0f), min(diagonalFade, 1.0f));
    fade = min(fade, min(axisFade, 1.0f));
    return fade * -cb11[7] + cb11[7];
}

// Permutation 2: chunk border band only, chunk lines (c5) and tile lines (c6).
float4 GridPS2(PixelInput input) : COLOR0 {
    float2 scaledCoord = input.gridCoord.yx * cb11[4].xy;
    float2 cellFraction = frac(scaledCoord.yx);

    if (cellFraction.x > 0.05f && cellFraction.x < 0.95f && cellFraction.y > 0.05f && cellFraction.y < 0.95f) {
        discard;
    }

    float2 width = abs(ddy(scaledCoord)) + abs(ddx(scaledCoord));
    float2 mediumWidth = width + width;
    float2 coarseWidth = width * 0.125f;
    float2 projected = ProjectPosition(input.position);
    float4 distances = abs(CenteredRepeat(projected.xyxy * float4(CoarseFrequency, CoarseFrequency, MediumFrequency, MediumFrequency)));
    float4 ratios = distances / float4(coarseWidth, mediumWidth);
    float coarseFade = min(min(ratios.y, ratios.x), 1.0f);
    float mediumFade = min(min(ratios.w, ratios.z), 1.0f);

    float4 mediumColor = mediumFade * -cb11[5] + cb11[5];
    return coarseFade * (mediumColor - cb11[6]) + cb11[6];
}

// Permutation 3: cell lines and diagonals (c7), chunk lines (c5) and tile lines (c6); no discard.
float4 GridPS3(PixelInput input) : COLOR0 {
    float2 projected = ProjectPosition(input.position);
    float sumDistance = abs(CenteredRepeat((projected.x + projected.y) * FineFrequency));
    float2 scaledCoord = input.gridCoord.yx * cb11[4].xy;
    float2 width = abs(ddy(scaledCoord)) + abs(ddx(scaledCoord));
    float2 coarseWidth = width * 0.125f;
    float2 fineWidth = width * 8.0f;
    float2 mediumWidth = width + width;

    float2 sumRatios = sumDistance / fineWidth;
    float sumFade = min(sumRatios.y, sumRatios.x);
    float2 diagonals = float2(projected.x - projected.y, projected.y - projected.x);
    float2 diagonalRatios = abs(CenteredRepeat(diagonals * FineFrequency)) / fineWidth;
    float diagonalFade = min(diagonalRatios.y, diagonalRatios.x);
    float fineFade = min(min(diagonalFade, 1.0f), min(sumFade, 1.0f));

    float4 coarseMediumDistances = abs(CenteredRepeat(projected.xyxy * float4(CoarseFrequency, CoarseFrequency, MediumFrequency, MediumFrequency)));
    float2 axisRatios = abs(CenteredRepeat(projected * FineFrequency)) / fineWidth;
    float2 coarseRatios = coarseMediumDistances.xy / coarseWidth;
    float2 mediumRatios = coarseMediumDistances.zw / mediumWidth;
    float mediumFade = min(min(mediumRatios.y, mediumRatios.x), 1.0f);
    float coarseFade = min(min(coarseRatios.y, coarseRatios.x), 1.0f);
    float axisFade = min(min(axisRatios.y, axisRatios.x), 1.0f);
    fineFade = min(fineFade, axisFade);

    float4 fineColor = fineFade * -cb11[7] + cb11[7];
    float4 fineMediumColor = mediumFade * (fineColor - cb11[5]) + cb11[5];
    return coarseFade * (fineMediumColor - cb11[6]) + cb11[6];
}

// Impassable slopes: the face normal from the position's screen-space derivatives, compared with c0.xyz against
// cos(50 degrees), the same walkable limit as the client's World::GetFacets (WORLD_QUERY_WALKABLE_ONLY).
// Port diagnostic, not an 8.1 shader: every terrain pixel the pass reaches, flat magenta.
float4 TestPS(PixelInput input) : COLOR0 {
    return float4(1.0f, 0.0f, 1.0f, 0.4f);
}

float4 SlopesPS(PixelInput input) : COLOR0 {
    float3 tangentX = ddx(input.position);
    float3 tangentY = ddy(input.position);
    float3 surfaceNormal = cross(tangentX, tangentY);
    surfaceNormal *= rsqrt(dot(surfaceNormal, surfaceNormal));
    float alignment = dot(surfaceNormal, cb11[0].xyz);
    const float slopeThreshold = 0.642787635326385498046875f; // 0x3f248dbb
    return (alignment < slopeThreshold) ? float4(1.0f, 0.0f, 0.0f, 0.25f) : float4(0.0f, 0.0f, 0.0f, 0.0f);
}
)HLSL";

struct WowEditShaderDef {
    const char* m_name; // CGxShader name (the engine's shader table is keyed by name)
    const char* m_entry;
    const char* m_profile;
    EGxShTarget m_target;
};

static const WowEditShaderDef s_shaderDefs[NUM_WOWEDITSHADERS] = {
    { "WowEditTerrain", "TerrainVS", "vs_3_0", GxSh_Vertex },
    { "WowEditGrid0", "GridPS0", "ps_3_0", GxSh_Pixel },
    { "WowEditGrid1", "GridPS1", "ps_3_0", GxSh_Pixel },
    { "WowEditGrid2", "GridPS2", "ps_3_0", GxSh_Pixel },
    { "WowEditGrid3", "GridPS3", "ps_3_0", GxSh_Pixel },
    { "WowEditImpassableSlopes", "SlopesPS", "ps_3_0", GxSh_Pixel },
    { "WowEditTest", "TestPS", "ps_3_0", GxSh_Pixel },
};

static CGxShader* s_shaders[NUM_WOWEDITSHADERS];
static bool s_compileFailed[NUM_WOWEDITSHADERS];
static pD3DCompile s_d3dCompile;
static bool s_d3dCompileSearched;

bool WowEditShadersSupported(const char** reason) {
    CGxDevice* device = g_theGxDevicePtr;
    const char* why = nullptr;

    if (!device) {
        why = "no graphics device";
    } else if (device->DeviceApi() != GxApi_D3d9 && device->DeviceApi() != GxApi_D3d9Ex) {
        why = "the editor shaders need the Direct3D device (gxApi d3d or d3d9ex)";
    }

    // No check of Caps().m_shaderTargets: that is the profile the client picked for its own shaders, which the
    // hardware detection and the pixel shader override lower below what the card runs (CGxDeviceD3d::ICapsInit
    // replaces it with CGxFormat::m_fixedFunction, often ps_2_0). The editor shaders bring their own bytecode, so the
    // only test that matters is whether Direct3D accepts it (CreatePixelShader / CreateVertexShader in
    // IShaderCreate), which CompileShader reports.

    if (reason) {
        *reason = why;
    }

    return why == nullptr;
}

static pD3DCompile FindD3DCompile() {
    if (!s_d3dCompileSearched) {
        s_d3dCompileSearched = true;
        const char* dlls[] = { "d3dcompiler_47.dll", "d3dcompiler_46.dll", "d3dcompiler_43.dll" };

        for (const char* dll : dlls) {
            HMODULE module = LoadLibraryA(dll);

            if (module) {
                s_d3dCompile = reinterpret_cast<pD3DCompile>(GetProcAddress(module, "D3DCompile"));

                if (s_d3dCompile) {
                    break;
                }
            }
        }
    }

    return s_d3dCompile;
}

// Compiles one entry point and hands the bytecode to a CGxShader from the device's shader table. ShaderCreate finds
// no BLS file for the name (none is shipped), so the shader comes back empty; the code is put in and the device
// creates the D3D shader from it, as it would from a loaded BLS.
static CGxShader* CompileShader(WOWEDITSHADER id) {
    const WowEditShaderDef& def = s_shaderDefs[id];
    pD3DCompile compile = FindD3DCompile();

    if (!compile) {
        ConsoleWrite("WowEdit: D3DCompile not found (d3dcompiler_47.dll)", ERROR_COLOR);
        return nullptr;
    }

    ID3DBlob* code = nullptr;
    ID3DBlob* errors = nullptr;
    HRESULT result = compile(s_shaderSource, strlen(s_shaderSource), "WowEditShaders.hlsl", nullptr, nullptr, def.m_entry, def.m_profile, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &errors);

    if (FAILED(result) || !code) {
        ConsoleWriteA("WowEdit: %s (%s) failed to compile", ERROR_COLOR, def.m_entry, def.m_profile);

        if (errors) {
            ConsoleWrite(static_cast<const char*>(errors->GetBufferPointer()), ERROR_COLOR);
        }
    }

    if (errors) {
        errors->Release();
    }

    if (FAILED(result) || !code) {
        if (code) {
            code->Release();
        }

        return nullptr;
    }

    CGxShader* shader = nullptr;
    g_theGxDevicePtr->ShaderCreate(&shader, def.m_target, def.m_target == GxSh_Vertex ? "Shaders\\Vertex" : "Shaders\\Pixel", def.m_name, 1);

    if (shader) {
        shader->m_code.SetCount(static_cast<uint32_t>(code->GetBufferSize()));
        memcpy(shader->m_code.m_data, code->GetBufferPointer(), code->GetBufferSize());
        g_theGxDevicePtr->IShaderCreate(shader);

        if (!shader->Valid()) {
            ConsoleWriteA("WowEdit: the device rejected %s (%s): the card does not run Shader Model 3", ERROR_COLOR, def.m_entry, def.m_profile);
            g_theGxDevicePtr->ShaderDestroy(shader);
            shader = nullptr;
        }
    }

    code->Release();
    return shader;
}

CGxShader* WowEditShaderGet(WOWEDITSHADER id) {
    if (id >= NUM_WOWEDITSHADERS || s_compileFailed[id] || !WowEditShadersSupported(nullptr)) {
        return nullptr;
    }

    if (!s_shaders[id]) {
        s_shaders[id] = CompileShader(id);
        s_compileFailed[id] = s_shaders[id] == nullptr;
    }

    return s_shaders[id];
}

const char* WowEditShaderState(WOWEDITSHADER id) {
    if (id >= NUM_WOWEDITSHADERS) {
        return "unknown";
    }

    if (s_shaders[id]) {
        return "ready";
    }

    return s_compileFailed[id] ? "failed" : "not built";
}

const char* WowEditShaderName(WOWEDITSHADER id) {
    return id < NUM_WOWEDITSHADERS ? s_shaderDefs[id].m_entry : "unknown";
}
