#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H
#define ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"

class C3Vector;
class CGxShader;
class CGxTex;
class CImVector;
class CTexture;

class CSimpleBatchedMesh {
    public:
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ CGxTex* m_texSort;
    /* 0008 */ EGxBlend m_alphaMode;
    /* 000C */ CGxShader* m_shader;
    /* 0010 */ uint32_t m_positionCount;
    /* 0014 */ const C3Vector* m_position;
    /* 0018 */ const C2Vector* m_texCoord;
    /* 001C */ const CImVector* m_color;
    /* 0020 */ uint32_t m_colorCount;
    /* 0024 */ const uint16_t* m_indices;
    /* 0028 */ uint32_t m_indexCount;
    /* 002C */ int32_t m_onAtlas;
    /* 0030 */ float m_atlasScale;
    /* 0034 */ C2Vector m_atlasOffset;
};

#endif
