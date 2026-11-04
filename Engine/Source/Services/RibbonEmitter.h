#ifndef ENGINE_SOURCE_SERVICES_RIBBONEMITTER_H
#define ENGINE_SOURCE_SERVICES_RIBBONEMITTER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CImVector.h"

class C44Matrix;
class CTexture;

class CRibbonMat {
    public:
    /* 0000 */ uint32_t m_lit : 1;
    uint32_t m_fogged : 1;
    uint32_t m_depthTest : 1;
    uint32_t m_depthWrite : 1;
    uint32_t m_cull : 1;
    /* 0004 */ uint32_t m_blendMode;

    CRibbonMat();
};

class CRibbonEmitter {
    public:
    /* 0000 */ int32_t m_refCount;
    /* 0004 */ TSGrowableArray<float> m_edges;
    /* 0014 */ uint32_t m_writePos;
    /* 0018 */ uint32_t m_readPos;
    /* 001C */ float m_startTime;
    /* 0020 */ C3Vector m_prevPos;
    /* 002C */ C3Vector m_localPos;
    /* 0038 */ TSGrowableArray<CGxVertexPCT> m_gxVertices;
    /* 0048 */ TSGrowableArray<uint16_t> m_gxIndices;
    /* 0058 */ float m_ooLifeSpan;
    /* 005C */ float m_tmpDU;
    /* 0060 */ float m_tmpDV;
    /* 0064 */ float m_ooTmpDU;
    /* 0068 */ float m_ooTmpDV;
    /* 006C */ CAaRect m_texSlotBox;
    /* 007C */ C3Vector m_prevVertical;
    /* 0088 */ C3Vector m_currVertical;
    /* 0094 */ C3Vector m_prevDir;
    /* 00A0 */ C3Vector m_currDir;
    /* 00AC */ C3Vector m_prevDirScaled;
    /* 00B8 */ C3Vector m_currDirScaled;
    /* 00C4 */ C3Vector m_below0;
    /* 00D0 */ C3Vector m_below1;
    /* 00DC */ C3Vector m_above0;
    /* 00E8 */ C3Vector m_above1;
    /* 00F4 */ CAaBox m_worldBounds;
    /* 010C */ float m_edgesPerSec;
    /* 0110 */ float m_edgeLifeSpan;
    /* 0114 */ TSGrowableArray<CRibbonMat> m_materials;
    /* 0124 */ TSGrowableArray<CTexture*> m_textures;
    /* 0134 */ TSGrowableArray<uint32_t> m_replaces;
    /* 0144 */ CImVector m_diffuseClr;
    /* 0148 */ CAaRect m_texBox;
    /* 0158 */ uint32_t m_rows;
    /* 015C */ uint32_t m_cols;
    /* 0160 */ uint32_t m_flags;
    /* 0164 */ C3Vector m_currPos;
    /* 0170 */ uint32_t m_texSlot;
    /* 0174 */ float m_above;
    /* 0178 */ float m_below;
    /* 017C */ float m_gravity;

    CRibbonEmitter();
    ~CRibbonEmitter();
    void ChangeFrameOfReference(const C44Matrix* matrix);
    void CloseTextureHandles();
    void ConvertTexSlotToTexCoords();
    void DecRef();
    uint32_t GetNumBatches();
    uint32_t GetNumPrimitives();
    void InitInterpDeltas();
    void Initialize(float edgesPerSecond, float edgeLifetime, const CImVector& color, const TSGrowableArray<CTexture*>& textures, const TSGrowableArray<CRibbonMat>& materials, const TSGrowableArray<uint32_t>& textureTypes, const CAaRect& texCoords, uint32_t rows, uint32_t cols);
    void InterpEdge(float age, float t, uint32_t advance);
    int32_t IsDead();
    int32_t Render(const C44Matrix* frameOfReference);
    int32_t ReplaceTexture(uint32_t type, CTexture* texture);
    void SetAbove(float above);
    void SetAlpha(float alpha);
    void SetBelow(float below);
    void SetColor(float r, float g, float b);
    void SetDataEnabled(int32_t enabled);
    void SetGravity(float gravity);
    void SetPos(const C44Matrix& transform, const C3Vector& position, const C44Matrix* frameOfReference);
    void SetTexSlot(uint32_t slot);
    void SetUserEnabled(uint32_t enabled);
    void Update(float deltaTime, int32_t suppressNewEdges);
};

#endif
