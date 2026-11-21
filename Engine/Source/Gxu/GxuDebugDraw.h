#ifndef ENGINE_SOURCE_GXU_GXUDEBUGDRAW_H
#define ENGINE_SOURCE_GXU_GXUDEBUGDRAW_H

// Tool features: the Gxu debug-draw module, back-ported from 4.x.
//
// Source: the 4.1.0 (13850) Mac client, Engine/Source/Gxu/GxuDebugDrawShapes.cpp, GxuDebugDrawTexture.cpp and
// GxuBufferCache.cpp (paths from its asserts, names from its symbols). 3.3.x has no such module: the 3.3.3 (12213)
// Mac build lists every Gxu file it asserts in and none of these, and 3.3.5 draws each debug overlay by hand
// (CWorldScene::RenderCollisionDebug, CMapObj::RenderPortals, the debug-feature collision info display). So this is a
// later addition, kept here because it is exactly the shared helper those overlays lack.
//
// What 4.1 retail kept, and therefore what is known:
//   - shape factories (IGxuDebugShapeFactory: CreateVertexBuffer, CreateIndexBuffer, GetNumVertices, GetNumIndices,
//     GetRadius, GetIdxOffset, SetLOD): Null, Box, Sphere, Cone, Triangle, and the editor gizmos DirectionalSphere,
//     RotationRing, DirectionalDiamond. Shapes are unit sized (the box is the +-0.5 cube, 8 C3Vector vertices) and
//     placed by a transform; their vertex and index buffers live in a GxuBufferCache.
//   - four render-state blocks on GxuDebugShapeDrawer: s_renderStateBlockSolid, SolidBackfacing, SolidNoDepth,
//     Wireframe (GxuDebugShapeRenderStateBlock / ...NoDepth::Apply, Restore). Apply marks depth test, depth func,
//     depth write, polygon offset, lighting, color write, fog and culling dirty (the 4.1 EGxRenderState values 13, 14,
//     15, 0, 11, 16, 12, 17 match the 3.3.5 GxRs_* numbering) and sets master enable 8 (GxMasterEnable_PolygonFill
//     in 3.3.5), the wireframe switch.
//   - object lists TSFixedArray<GxuDebugShapeObject / GxuDebugTextObject / GxuDebugText3DObject /
//     GxuDebugTextureObject>, and GxuDebugTextureDrawer::Draw(const C3Vector&) (orthographic, GxuXformCreateOrtho).
//   - shutdown: GxuShutdownDebugDraw -> GxuShutdownDebugDrawList, ...ShapeDrawer, ...TextDrawer, ...Text3DDrawer,
//     ...TextureDrawer.
// 4.1 retail stripped every function that adds or draws an object, so the GxuDebugDraw* entry points below are
// named for this port, not recovered.
//
// Fitted to 3.3.5: render states through CGxDevice::RsPush/RsSet/RsPop, vertex and index buffers from CGxPool
// (GxuBufferCache), immediate-mode GxPrimLockVertexPtrs/GxDrawLockedElements for text-free shapes. The gizmo shapes
// (DirectionalSphere, RotationRing, DirectionalDiamond) belong to 4.x editing tools and are left out.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxTex;

enum GXUDEBUGSHAPE : int32_t {
    GXUDEBUGSHAPE_NULL = 0,
    GXUDEBUGSHAPE_BOX = 1, // unit cube, -0.5..0.5 (4.1 GxuBoxShapeFactory)
    GXUDEBUGSHAPE_SPHERE = 2, // unit radius, LOD-dependent tessellation (GxuSphereShapeFactory)
    GXUDEBUGSHAPE_CONE = 3, // GxuConeShapeFactory
    GXUDEBUGSHAPE_TRIANGLE = 4, // GxuTriangleShapeFactory
    NUM_GXUDEBUGSHAPES = 5,
};

// Which GxuDebugShapeDrawer render-state block a shape draws with.
enum GXUDEBUGSTYLE : int32_t {
    GXUDEBUGSTYLE_SOLID = 0, // s_renderStateBlockSolid
    GXUDEBUGSTYLE_SOLID_BACKFACING = 1, // s_renderStateBlockSolidBackfacing: culling off
    GXUDEBUGSTYLE_SOLID_NODEPTH = 2, // s_renderStateBlockSolidNoDepth: always on top
    GXUDEBUGSTYLE_WIREFRAME = 3, // s_renderStateBlockWireframe
    NUM_GXUDEBUGSTYLES = 4,
};

// Shared vertex and index buffers, keyed by a handle the factories keep (4.1 GxuBufferCache: entries of target, item
// size, item count, pool, buffer, reference count; 0xFFFF = no buffer yet).
class GxuBufferCache {
    public:
    struct BufferEntry {
        /* 0000 */ EGxPoolTarget m_target;
        /* 0004 */ uint32_t m_itemSize;
        /* 0008 */ uint32_t m_itemCnt;
        /* 000C */ void* m_pool; // CGxPool*
        /* 0010 */ void* m_buffer; // CGxBuf*
        /* 0014 */ uint32_t m_refs;
    };

    static const uint32_t INVALID_HANDLE = 0xFFFF;

    TSFixedArray<BufferEntry> m_allocatedBuffers;

    ~GxuBufferCache();
    uint32_t CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCnt);
    bool Exists(uint32_t handle);
    void* AcquireBuffer(uint32_t handle);
    void ReleaseBuffer(uint32_t handle);
    void ReleaseCache();
};

class IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer; // GxuBufferCache handle
    /* 0008 */ uint32_t m_indexBuffer; // GxuBufferCache handle

    IGxuDebugShapeFactory();
    virtual ~IGxuDebugShapeFactory();
    virtual uint32_t CreateVertexBuffer() = 0;
    virtual uint32_t CreateIndexBuffer() = 0;
    virtual uint32_t GetNumVertices() const = 0;
    virtual uint32_t GetNumIndices() const = 0;
    virtual float GetRadius() const = 0;
    virtual uint32_t GetIdxOffset() const;
    virtual void SetLOD(float lod);
};

class GxuNullShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
};

class GxuBoxShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
};

class GxuSphereShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 000C */ uint32_t m_rings;
    /* 0010 */ uint32_t m_segments;

    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
    uint32_t GetIdxOffset() const override;
    void SetLOD(float lod) override;
};

class GxuConeShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
};

class GxuTriangleShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
};

// Pushes the render states a style needs and pops them after the batch.
class GxuDebugShapeRenderStateBlock {
    public:
    /* 0004 */ int32_t m_depthTest;
    /* 0008 */ int32_t m_culling;
    /* 000C */ int32_t m_wireframe;

    GxuDebugShapeRenderStateBlock(int32_t depthTest, int32_t culling, int32_t wireframe);
    virtual ~GxuDebugShapeRenderStateBlock();
    virtual void Apply();
    virtual void Restore();
};

class GxuDebugShapeRenderStateBlockNoDepth : public GxuDebugShapeRenderStateBlock {
    public:
    GxuDebugShapeRenderStateBlockNoDepth();
    void Apply() override;
    void Restore() override;
};

struct GxuDebugShapeObject {
    /* 0000 */ GXUDEBUGSHAPE m_shape;
    /* 0004 */ GXUDEBUGSTYLE m_style;
    /* 0008 */ C44Matrix m_transform;
    /* 0048 */ CImVector m_color;
};

struct GxuDebugTextObject {
    /* 0000 */ char m_text[128];
    /* 0080 */ float m_x; // normalized screen coordinates
    /* 0084 */ float m_y;
    /* 0088 */ CImVector m_color;
};

struct GxuDebugText3DObject {
    /* 0000 */ char m_text[128];
    /* 0080 */ C3Vector m_position; // world position, projected at draw time
    /* 008C */ CImVector m_color;
};

struct GxuDebugTextureObject {
    /* 0000 */ CGxTex* m_texture;
    /* 0004 */ float m_x; // normalized screen rectangle
    /* 0008 */ float m_y;
    /* 000C */ float m_width;
    /* 0010 */ float m_height;
};

class GxuDebugShapeDrawer {
    public:
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolid;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolidBackfacing;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolidNoDepth;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockWireframe;

    static IGxuDebugShapeFactory* GetFactory(GXUDEBUGSHAPE shape);
    static GxuDebugShapeRenderStateBlock* GetStateBlock(GXUDEBUGSTYLE style);
    static void Draw(const TSGrowableArray<GxuDebugShapeObject>& objects);
};

class GxuDebugTextureDrawer {
    public:
    static void Draw(const C3Vector& screenSize);
};

// Port entry points (names chosen here; 4.1 retail stripped the originals). Objects queue up during the frame and are
// drawn and cleared by GxuDebugDrawRender, called once at the end of the world render (after the scene, before UI).
void GxuDebugDrawShape(GXUDEBUGSHAPE shape, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style);
void GxuDebugDrawText(const char* text, float x, float y, const CImVector& color);
void GxuDebugDrawText3D(const char* text, const C3Vector& position, const CImVector& color);
void GxuDebugDrawTexture(CGxTex* texture, float x, float y, float width, float height);
void GxuDebugDrawRender();

// 4.1 shutdown chain.
void GxuShutdownDebugDraw();
void GxuShutdownDebugDrawList();
void GxuShutdownDebugShapeDrawer();
void GxuShutdownDebugTextDrawer();
void GxuShutdownDebugText3DDrawer();
void GxuShutdownDebugTextureDrawer();

#endif

#endif
