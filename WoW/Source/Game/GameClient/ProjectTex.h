#ifndef WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H
#define WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H

#include <cstddef>
#include <cstdint>

class C44Matrix;
class CAaBox;
class CGxTex;
class CImVector;

// Render flags for 2D projected textures; the query flags are WORLD_QUERY_FLAGS.
enum PROJECT_TEX_FLAGS : uint32_t {
    PROJECT_TEX_NONE = 0x0,
    PROJECT_TEX_DOODADS = 0x1, // Also draw onto doodads flagged MAPOBJ_FLAG_PROJ_TEX.
    PROJECT_TEX_MATERIAL_COLOR = 0x2, // Color through the material diffuse with normal vertices instead of per-vertex color.
    PROJECT_TEX_ALL_FACES = 0x4, // Keep every triangle; otherwise triangles failing the XY winding test are dropped.
};

namespace ProjectTex {
    void Render2d(const CAaBox& box, const CImVector& color, const C44Matrix* matrix, float fade, uint32_t queryFlags, uint32_t renderFlags, float polygonOffset);
}

CGxTex* ProjectTex2dGetAddFadeTex();
void ProjectTex2dDrawShaderEffect(const CAaBox& box, const CImVector& color, uint32_t queryFlags, uint32_t renderFlags, float polygonOffset);
void ProjectTex2dMakeMatrices(C44Matrix& texMatrix, C44Matrix& fadeMatrix, const CAaBox& box, const C44Matrix* rotation, float fade, int32_t local);

#endif
