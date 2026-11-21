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
//   - GxuBufferCache in full (CreateBuffer, Exists, AcquireBuffer, ReleaseBuffer, ReleaseCache).
//   - shape factories (IGxuDebugShapeFactory: CreateVertexBuffer, CreateIndexBuffer, GetNumVertices, GetNumIndices,
//     GetRadius, GetIdxOffset, SetLOD): Null, Box, Sphere, Cone, Triangle, and the editor gizmos DirectionalSphere,
//     RotationRing, DirectionalDiamond, with their exact geometry. Shapes are unit sized and placed by a transform.
//   - four render-state blocks on GxuDebugShapeDrawer: s_renderStateBlockSolid, SolidBackfacing, SolidNoDepth,
//     Wireframe (GxuDebugShapeRenderStateBlock / ...NoDepth::Apply, Restore). The 4.1 EGxRenderState values they
//     touch (13, 14, 15, 0, 11, 16, 12, 17) match the 3.3.5 GxRs_* numbering; master enable 8 is
//     GxMasterEnable_PolygonFill in 3.3.5.
//   - object lists TSFixedArray<GxuDebugShapeObject / GxuDebugTextObject / GxuDebugText3DObject /
//     GxuDebugTextureObject>, and GxuDebugTextureDrawer::Draw(const C3Vector&) (orthographic, shader based).
//   - shutdown: GxuShutdownDebugDraw -> ...TextDrawer, ...Text3DDrawer, ...TextureDrawer, ...ShapeDrawer, ...DrawList.
// 4.1 retail stripped every function that adds an object, the shape drawer's draw loop and both text drawers, so the
// GxuDebugDraw* entry points and those loops are written for this port, not recovered.
//
// Fitted to 3.3.5:
//   - render states go through CGxDevice::RsPush/RsSet/RsPop (4.1 pokes the device's state cache and marks entries
//     dirty by hand, between its own RsPush and RsPop);
//   - the factories' buffers are CGxPool/CGxBuf pairs made by CGxDevice::PoolCreate/BufCreate, like 4.1;
//   - 4.1 colors shapes and textures in vertex/pixel shader pairs; 3.3.5 has no such shaders, so the shape drawer
//     streams the factory's positions with a constant vertex color (GxPrimVertexPtr, color stride 0) and indexes them
//     from the cached index buffer (CGxDevice::PrimIndexPtr). Each factory therefore also keeps a CPU copy of its
//     vertices. Textures draw as a fixed-function textured quad;
//   - text uses GxuFont string batches. Text and textures use a 0..1 screen space with its origin at the top left
//     (4.1 GxuDebugTextureDrawer::Draw sets GxuXformCreateOrtho(0, 1, 1, 0, -1, 1)).
// The gizmo shapes (DirectionalSphere, RotationRing, DirectionalDiamond) belong to 4.x editing tools and are left out.
//
// Port addition: line objects (GxuDebugDrawLine). 4.1 has no line shape; the 3.3.5 overlays that need lines (collision
// info normals, unit pathing) draw GxPrim_Lines batches by hand, which is what the line drawer does too.
//
// Built with either debug or tool features: the debug features (model bounds, pathing) draw through it, and the tool
// features (the debugDrawTest command) exercise it.

#if defined(WOWDECOMP_DEBUG_FEATURES) || defined(WOWDECOMP_TOOL_FEATURES)
#define WOWDECOMP_GXU_DEBUG_DRAW 1
#endif

#ifdef WOWDECOMP_GXU_DEBUG_DRAW

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"

struct CGxBuf;
struct CGxPool;
class CGxTex;

enum GXUDEBUGSHAPE : int32_t {
    GXUDEBUGSHAPE_NULL = 0,
    GXUDEBUGSHAPE_BOX = 1, // unit cube, -0.5..0.5 (4.1 GxuBoxShapeFactory)
    GXUDEBUGSHAPE_SPHERE = 2, // unit radius, three LODs (GxuSphereShapeFactory)
    GXUDEBUGSHAPE_CONE = 3, // apex at z=+0.5, unit-radius base at z=-0.5 (GxuConeShapeFactory)
    GXUDEBUGSHAPE_TRIANGLE = 4, // double-sided, in the x=0 plane, apex at z=+0.5 (GxuTriangleShapeFactory)
    NUM_GXUDEBUGSHAPES = 5,
};

// Which GxuDebugShapeDrawer render-state block a shape draws with.
enum GXUDEBUGSTYLE : int32_t {
    GXUDEBUGSTYLE_SOLID = 0, // s_renderStateBlockSolid: back faces culled
    GXUDEBUGSTYLE_SOLID_BACKFACING = 1, // s_renderStateBlockSolidBackfacing: the other winding culled (the inside)
    GXUDEBUGSTYLE_SOLID_NODEPTH = 2, // s_renderStateBlockSolidNoDepth: no depth test, always on top
    GXUDEBUGSTYLE_WIREFRAME = 3, // s_renderStateBlockWireframe: no culling, polygon fill off
    NUM_GXUDEBUGSTYLES = 4,
};

// Shared vertex and index buffers, keyed by a handle the factories keep (4.1 GxuBufferCache: 24-byte entries of
// target, item size, item count, pool, buffer, reference count; 0xFFFF = no buffer yet).
class GxuBufferCache {
    public:
    struct BufferEntry {
        /* 0000 */ EGxPoolTarget m_target;
        /* 0004 */ uint32_t m_itemSize;
        /* 0008 */ uint32_t m_itemCnt;
        /* 000C */ CGxPool* m_pool;
        /* 0010 */ CGxBuf* m_buffer;
        /* 0014 */ uint32_t m_refs;
    };

    static const uint32_t INVALID_HANDLE = 0xFFFF;

    TSFixedArray<BufferEntry> m_allocatedBuffers;

    ~GxuBufferCache();
    uint32_t CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCnt);
    bool Exists(uint32_t handle);
    CGxBuf* AcquireBuffer(uint32_t handle);
    void ReleaseBuffer(uint32_t handle);
    void ReleaseCache();
};

// The one cache every factory allocates from (4.1 static at 0x014D5700).
extern GxuBufferCache s_debugShapeBufferCache;

class IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer; // GxuBufferCache handle
    /* 0008 */ uint32_t m_indexBuffer; // GxuBufferCache handle
    // Port: CPU copy of the vertices for GxPrimVertexPtr (see the file comment).
    TSFixedArray<C3Vector> m_drawVertices;

    IGxuDebugShapeFactory();
    virtual ~IGxuDebugShapeFactory();
    // 4.1 implements these two in every factory with the same Exists / CreateBuffer / AcquireBuffer / BufLock /
    // write / BufUnlock / ReleaseBuffer sequence; here the sequence is shared and only the writes differ.
    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices() const = 0;
    virtual uint32_t GetNumIndices() const = 0;
    virtual float GetRadius() const = 0;
    virtual uint32_t GetIdxOffset() const;
    virtual void SetLOD(float dist);

    protected:
    // The body of the 4.1 Create*Buffer between BufLock and BufUnlock.
    virtual void WriteVertices(C3Vector* vertices) const = 0;
    virtual void WriteIndices(uint16_t* indices) const = 0;
    // Index buffer size when it differs from GetNumIndices (the sphere stores every LOD in one buffer).
    virtual uint32_t GetIndexBufferCount() const;
};

class GxuNullShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t CreateVertexBuffer() override;
    uint32_t CreateIndexBuffer() override;
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;

    protected:
    void WriteVertices(C3Vector* vertices) const override;
    void WriteIndices(uint16_t* indices) const override;
};

class GxuBoxShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;

    protected:
    void WriteVertices(C3Vector* vertices) const override;
    void WriteIndices(uint16_t* indices) const override;
};

// 4.1 layout (static init at 0x00083080): LOD, vertex and index handles, segments (32), rings (33), vertex count
// (1056), then per-LOD index counts and offsets. LOD l steps the same vertex grid by 1 << l.
class GxuSphereShapeFactory : public IGxuDebugShapeFactory {
    public:
    static const uint32_t NUM_LODS = 3;

    uint32_t m_lod;
    uint32_t m_segments;
    uint32_t m_rings;
    uint32_t m_numVertices;
    uint32_t m_numIndices[NUM_LODS];
    uint32_t m_idxOffset[NUM_LODS];
    uint32_t m_totalIndices;

    GxuSphereShapeFactory();
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;
    uint32_t GetIdxOffset() const override;
    void SetLOD(float dist) override;

    protected:
    void WriteVertices(C3Vector* vertices) const override;
    void WriteIndices(uint16_t* indices) const override;
    uint32_t GetIndexBufferCount() const override;
};

class GxuConeShapeFactory : public IGxuDebugShapeFactory {
    public:
    static const uint32_t NUM_SEGMENTS = 12;

    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;

    protected:
    void WriteVertices(C3Vector* vertices) const override;
    void WriteIndices(uint16_t* indices) const override;
};

class GxuTriangleShapeFactory : public IGxuDebugShapeFactory {
    public:
    uint32_t GetNumVertices() const override;
    uint32_t GetNumIndices() const override;
    float GetRadius() const override;

    protected:
    void WriteVertices(C3Vector* vertices) const override;
    void WriteIndices(uint16_t* indices) const override;
};

// Fill modes of the 4.1 blocks; Apply derives the culling mode and the PolygonFill master enable from them (any other
// value is the 4.1 "unknown fill mode" assert).
enum GXUDEBUGFILLMODE : int32_t {
    GXUDEBUGFILL_SOLID = 0, // GxRs_Culling 1
    GXUDEBUGFILL_SOLID_BACKFACING = 1, // GxRs_Culling 2
    GXUDEBUGFILL_WIREFRAME = 3, // GxRs_Culling 0, polygon fill off
};

// Pushes the render states a style needs and pops them after the batch (4.1 layout: vtable, fill mode, saved
// PolygonFill master enable).
class GxuDebugShapeRenderStateBlock {
    public:
    /* 0004 */ GXUDEBUGFILLMODE m_fillMode;
    /* 0008 */ int32_t m_savedPolygonFill;

    explicit GxuDebugShapeRenderStateBlock(GXUDEBUGFILLMODE fillMode);
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
    /* 0008 */ C44Matrix m_transform; // unit shape to world (row vectors, translation in d0..d2)
    /* 0048 */ CImVector m_color;
};

// Port: one world-space segment. The style picks the render-state block: GXUDEBUGSTYLE_SOLID_NODEPTH draws on top of
// everything, every other style depth tests (culling and fill mode do not apply to lines).
struct GxuDebugLineObject {
    /* 0000 */ C3Vector m_start;
    /* 000C */ C3Vector m_end;
    /* 0018 */ CImVector m_color;
    /* 001C */ GXUDEBUGSTYLE m_style;
};

struct GxuDebugTextObject {
    /* 0000 */ char m_text[128];
    /* 0080 */ float m_x; // 0..1 screen coordinates, origin top-left
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
    /* 0004 */ float m_x; // 0..1 screen rectangle, origin top-left
    /* 0008 */ float m_y;
    /* 000C */ float m_width;
    /* 0010 */ float m_height;
    /* 0014 */ EGxBlend m_blend; // 4.1 keeps a blend mode per texture object and sets it as GxRs_BlendingMode
};

class GxuDebugShapeDrawer {
    public:
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolid;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolidBackfacing;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockSolidNoDepth;
    static GxuDebugShapeRenderStateBlock* s_renderStateBlockWireframe;

    static IGxuDebugShapeFactory* GetFactory(GXUDEBUGSHAPE shape);
    static GxuDebugShapeRenderStateBlock* GetStateBlock(GXUDEBUGSTYLE style);
    static void Draw(const TSGrowableArray<GxuDebugShapeObject>& objects, const C3Vector& viewOrigin);
    // Port: all queued lines of one depth mode go out as a single GxPrim_Lines batch.
    static void DrawLines(const TSGrowableArray<GxuDebugLineObject>& objects, const C3Vector& viewOrigin);
};

class GxuDebugTextureDrawer {
    public:
    static void Draw(const C3Vector& viewOrigin);
};

// Port: 4.1 keeps only the text drawers' shutdown functions.
class GxuDebugTextDrawer {
    public:
    static void Draw(const C3Vector& viewOrigin);
};

// Port entry points (names chosen here; 4.1 retail stripped the originals). Objects queue up during the frame and are
// drawn and cleared by GxuDebugDrawRender, called once from CGWorldFrame::OnWorldRender after the scene, while the
// world view and projection are still set. viewOrigin is the camera-relative rendering origin
// (CWorldScene::s_activeWorldView): world positions are drawn relative to it.
void GxuDebugDrawShape(GXUDEBUGSHAPE shape, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style);
void GxuDebugDrawLine(const C3Vector& start, const C3Vector& end, const CImVector& color, GXUDEBUGSTYLE style = GXUDEBUGSTYLE_SOLID);
// Convenience on top of the primitives: the twelve edges of a world axis-aligned box, and the box or sphere of a
// local-space bounds placed by a transform (used by the model bounds display).
void GxuDebugDrawAaBoxLines(const C3Vector& minCorner, const C3Vector& maxCorner, const CImVector& color, GXUDEBUGSTYLE style = GXUDEBUGSTYLE_SOLID);
void GxuDebugDrawBoundsBox(const C3Vector& minCorner, const C3Vector& maxCorner, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style);
void GxuDebugDrawBoundsSphere(const C3Vector& center, float radius, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style);
void GxuDebugDrawText(const char* text, float x, float y, const CImVector& color);
void GxuDebugDrawText3D(const char* text, const C3Vector& position, const CImVector& color);
void GxuDebugDrawTexture(CGxTex* texture, float x, float y, float width, float height, EGxBlend blend = GxBlend_Alpha);
void GxuDebugDrawRender(const C3Vector& viewOrigin);

// 4.1 shutdown chain. GxuShutdownDebugDraw runs before GxuFontShutdown (IStockDestroy), since the text drawer owns a
// font, a batch and strings.
void GxuShutdownDebugDraw();
void GxuShutdownDebugDrawList();
void GxuShutdownDebugShapeDrawer();
void GxuShutdownDebugTextDrawer();
void GxuShutdownDebugText3DDrawer();
void GxuShutdownDebugTextureDrawer();

#endif

#endif
