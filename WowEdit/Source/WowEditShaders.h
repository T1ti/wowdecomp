#ifndef WOWEDIT_SOURCE_WOWEDITSHADERS_H
#define WOWEDIT_SOURCE_WOWEDITSHADERS_H

// WowEdit: the editor's terrain overlay shaders, as HLSL compiled at run time (port).
//
// Source: the pixel shaders of Blizzard's world editor 8.1.0 (27826), woweditgrid_internal, woweditimpassableslopes_
// internal, woweditoverlays_internal, woweditpathing_internal and woweditterrainbrush_internal (D3D10/11 bytecode in
// BLS containers, decompiled to HLSL with guessed names). The "_internal" suffix sets them apart from the client's
// shipped shaders: they belong to the tool. No 3.3.x build of them survives.
//
// Port: grid (all four permutations), impassable slopes and the terrain brush are rewritten for Shader Model 3 (ps_3_0,
// with a vs_3_0 vertex shader, since D3D9 pairs a 3.0 pixel shader only with a 3.0 vertex shader). Their constants keep
// the 8.1 register layout (8.1 cb11[n] is c[n] here), so each function can be compared with its 8.1 counterpart line
// by line. The brush reads a brush texture, which the port generates (a dashed ring, WowEditTerrain.cpp), the 8.1 ones
// being unknown. The overlays shader is ported with one change: its 256 per-cell colors (8.1 cb12) do not fit in the
// 224 pixel shader constants of Shader Model 3 and cannot be indexed there, so they come from a 16 x 16 texture; the
// editor data it showed is unknown, and the port fills it with 3.3.5 terrain data (area IDs). The pathing shader is not
// ported: 3.3.5 has no pathing data to show.
//
// Instead of BLS files, the HLSL text is compiled when first needed with D3DCompile from the system's
// d3dcompiler_47.dll, and the bytecode goes into ordinary CGxShader objects (CGxShader::m_code, then
// CGxDevice::IShaderCreate), so binding goes through GxRs_VertexShader / GxRs_PixelShader like every other shader.
// Direct3D devices only: the OpenGL device takes ARB programs, not D3D bytecode.

#include <cstdint>

class CGxShader;

enum WOWEDITSHADER : uint32_t {
    WOWEDITSHADER_TERRAIN_VS = 0, // vs_3_0: terrain chunk vertices to clip space, world position, chunk grid coordinates
    WOWEDITSHADER_GRID0 = 1, // 8.1 woweditgrid_internal permutation 0: chunk border band, transparent black
    WOWEDITSHADER_GRID1 = 2, // permutation 1: cell grid with both diagonals, color c7
    WOWEDITSHADER_GRID2 = 3, // permutation 2: chunk border band only, chunk (c5) and tile (c6) lines
    WOWEDITSHADER_GRID3 = 4, // permutation 3: cell grid with diagonals (c7), chunk (c5) and tile (c6) lines
    WOWEDITSHADER_SLOPES = 5, // 8.1 woweditimpassableslopes_internal: red at 25% where steeper than 50 degrees
    WOWEDITSHADER_BRUSH = 6, // 8.1 woweditterrainbrush_internal: the brush texture (s0) around c4.xy, radius c6.x, tint c5
    WOWEDITSHADER_OVERLAYS = 7, // 8.1 woweditoverlays_internal: per-chunk colors c5..c8 plus per-cell colors (s0)
    NUM_WOWEDITSHADERS = 8,
};

// Pixel shader constant registers, after 8.1 cb11.
enum WOWEDITSHADERCONST : uint32_t {
    WOWEDITCONST_PROJECT_X = 0, // grid: row giving the world x of a position (slopes: the comparison direction)
    WOWEDITCONST_PROJECT_Y = 1, // grid: row giving the world y of a position
    WOWEDITCONST_GRID_SCALE = 4, // grid: scale of the chunk grid coordinates (xy)
    WOWEDITCONST_CHUNK_COLOR = 5, // grid: chunk lines (8.1 "medium")
    WOWEDITCONST_TILE_COLOR = 6, // grid: map tile lines (8.1 "coarse")
    WOWEDITCONST_CELL_COLOR = 7, // grid: cell lines and diagonals (8.1 "fine")
    WOWEDITCONST_BRUSH_CENTER = 4, // brush: world xy of the brush center
    WOWEDITCONST_BRUSH_TINT = 5, // brush: color the texture is multiplied by
    WOWEDITCONST_BRUSH_RADIUS = 6, // brush: radius in yards (x)
    WOWEDITCONST_OVERLAY_ORIGIN = 4, // overlays: world xy of the 2 x 2 chunk block's corner the cells count from
    WOWEDITCONST_OVERLAY_CHUNK_COLOR = 5, // overlays: c5..c8, the color of each chunk of the block (x + 2 y)
    NUM_WOWEDITCONSTS = 9,
};

// True when the device can take the shaders (a Direct3D device; whether the card runs Shader Model 3 shows when a
// shader is built). reason, when given, receives a short explanation when it cannot.
bool WowEditShadersSupported(const char** reason);
// The shader, compiled on first use; nullptr when unsupported or when compilation failed (the console shows why).
CGxShader* WowEditShaderGet(WOWEDITSHADER shader);
// For the status command: "ready", "failed" or "not built", and the HLSL entry point.
const char* WowEditShaderState(WOWEDITSHADER shader);
const char* WowEditShaderName(WOWEDITSHADER shader);

#endif
