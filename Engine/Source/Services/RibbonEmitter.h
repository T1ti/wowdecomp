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
    // Runtime word, separate from on-disk M2 flags. Bit 0x20 is cleared on update;
    // its distinct meaning is unproved. Keep raw storage and partial masks.
    enum RUNTIME_FLAGS : int32_t {
        POSITION_SET = 0x1,
        INITIALIZED = 0x2,
        DATA_ENABLED = 0x4,
        USER_ENABLED = 0x8,
        UPDATED = 0x10,
        UNKNOWN_0020 = 0x20,
        UPDATE_STATE_MASK = 0x30,
        CONSTRUCTOR_RESET_MASK = 0x12,
        INITIAL_ENABLED_MASK = 0xE,
    };

    /* 0000 */ int32_t m_refCount;
    /* 0004 */ TSGrowableArray<float> m_edges; // Ring of edge ages in seconds; newly interpolated ages can be negative.
    /* 0014 */ uint32_t m_writePos; // Next emitted edge's slot.
    /* 0018 */ uint32_t m_readPos; // Oldest live edge's slot.
    /* 001C */ float m_edgeFraction; // Fractional edge emission carried between updates.
    /* 0020 */ C3Vector m_prevPos;
    /* 002C */ C3Vector m_localPos; // Translation subtracted from the render world matrix.
    /* 0038 */ TSGrowableArray<CGxVertexPCT> m_gxVertices;
    /* 0048 */ TSGrowableArray<uint16_t> m_gxIndices;
    /* 0058 */ float m_ooLifeSpan; // "oo" = "one over": 1 / edge lifetime in seconds.
    /* 005C */ float m_tmpDU; // U span of one texture-atlas column.
    /* 0060 */ float m_tmpDV; // V span of one texture-atlas row.
    /* 0064 */ float m_ooTmpDU; // Reciprocal of the column's U span (1 / m_tmpDU).
    /* 0068 */ float m_ooTmpDV; // Reciprocal of the row's V span (1 / m_tmpDV).
    /* 006C */ CAaRect m_texSlotBox; // UV bounds of the selected atlas cell.
    /* 007C */ C3Vector m_prevVertical; // Previous orientation B-axis, used for ribbon width offsets.
    /* 0088 */ C3Vector m_currVertical; // Current orientation B-axis, not necessarily world-up.
    /* 0094 */ C3Vector m_prevDir;
    /* 00A0 */ C3Vector m_currDir;
    /* 00AC */ C3Vector m_prevDirScaled; // Previous orientation C-axis scaled by distance between emission positions.
    /* 00B8 */ C3Vector m_currDirScaled; // Current orientation C-axis scaled by the same distance.
    /* 00C4 */ C3Vector m_below0; // Previous position minus previous vertical * m_below.
    /* 00D0 */ C3Vector m_below1; // Current position minus current vertical * m_below.
    /* 00DC */ C3Vector m_above0; // Previous position plus previous vertical * m_above.
    /* 00E8 */ C3Vector m_above1; // Current position plus current vertical * m_above.
    /* 00F4 */ CAaBox m_worldBounds;
    /* 010C */ float m_edgesPerSec;
    /* 0110 */ float m_edgeLifeSpan;
    /* 0114 */ TSGrowableArray<CRibbonMat> m_materials;
    /* 0124 */ TSGrowableArray<CTexture*> m_textures;
    /* 0134 */ TSGrowableArray<uint32_t> m_replaces; // Texture replacement-type IDs, parallel to m_textures.
    /* 0144 */ CImVector m_diffuseClr; // Packed diffuse vertex color; "Clr" abbreviates color.
    /* 0148 */ CAaRect m_texBox; // UV bounds of the complete atlas, before row/column subdivision.
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
