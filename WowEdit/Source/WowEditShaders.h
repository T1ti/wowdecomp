#ifndef WOWEDIT_SOURCE_WOWEDITSHADERS_H
#define WOWEDIT_SOURCE_WOWEDITSHADERS_H

// WowEdit: the editor's terrain overlay shaders, as HLSL compiled at run time (port).
//
// Source: the pixel shaders of Blizzard's world editor 8.1.0 (27826), woweditgrid_internal, woweditimpassableslopes_
// internal, woweditoverlays_internal, woweditpathing_internal and woweditterrainbrush_internal (D3D10/11 bytecode in
// BLS containers, decompiled to HLSL with guessed names). The "_internal" suffix sets them apart from the client's
// shipped shaders: they belong to the tool. No 3.3.x build of them survives.
//
// Port: grid (all four permutations) and impassable slopes are rewritten for Shader Model 3 (ps_3_0, with a vs_3_0
// vertex shader, since D3D9 pairs a 3.0 pixel shader only with a 3.0 vertex shader). Their constants keep the 8.1
// register layout (8.1 cb11[n] is c[n] here), so each function can be compared with its 8.1 counterpart line by line.
// The overlay, pathing and brush shaders are not ported: they read editor data (per-cell colors, pathing and brush
// textures) this client does not have.
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
    WOWEDITSHADER_TEST = 6, // port diagnostic: flat magenta at 40%, to tell the pass from the overlay shaders
    NUM_WOWEDITSHADERS = 7,
};

// Pixel shader constant registers, after 8.1 cb11.
enum WOWEDITSHADERCONST : uint32_t {
    WOWEDITCONST_PROJECT_X = 0, // grid: row giving the world x of a position (slopes: the comparison direction)
    WOWEDITCONST_PROJECT_Y = 1, // grid: row giving the world y of a position
    WOWEDITCONST_GRID_SCALE = 4, // grid: scale of the chunk grid coordinates (xy)
    WOWEDITCONST_CHUNK_COLOR = 5, // grid: chunk lines (8.1 "medium")
    WOWEDITCONST_TILE_COLOR = 6, // grid: map tile lines (8.1 "coarse")
    WOWEDITCONST_CELL_COLOR = 7, // grid: cell lines and diagonals (8.1 "fine")
    NUM_WOWEDITCONSTS = 8,
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
