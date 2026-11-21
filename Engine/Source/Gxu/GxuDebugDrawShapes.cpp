#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstring>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"

// Tool features: Gxu debug shapes (4.1 Engine/Source/Gxu/GxuDebugDrawShapes.cpp, see GxuDebugDraw.h).
// 4.1 addresses are the 13850 Mac build's.

// 4.1 0x012E7EA4..0x012E7EC0
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolid;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockWireframe;

// One cache for every factory's buffers (4.1 keeps a single static GxuBufferCache at 0x014D5700).
GxuBufferCache s_debugShapeBufferCache;

static TSGrowableArray<GxuDebugShapeObject> s_shapeObjects;

static GxuNullShapeFactory s_nullFactory;
static GxuBoxShapeFactory s_boxFactory;
static GxuSphereShapeFactory s_sphereFactory;
static GxuConeShapeFactory s_coneFactory;
static GxuTriangleShapeFactory s_triangleFactory;

// ---- IGxuDebugShapeFactory (4.1: base GetIdxOffset 0x00AE6BD0, SetLOD 0x00AE6BC0)

IGxuDebugShapeFactory::IGxuDebugShapeFactory()
    : m_vertexBuffer(GxuBufferCache::INVALID_HANDLE)
    , m_indexBuffer(GxuBufferCache::INVALID_HANDLE) {
}

IGxuDebugShapeFactory::~IGxuDebugShapeFactory() {
}

uint32_t IGxuDebugShapeFactory::GetIdxOffset() const {
    return 0;
}

void IGxuDebugShapeFactory::SetLOD(float lod) {
    // Fixed tessellation by default; spheres override it.
}

// ---- Null (4.1 0x00AEA2F0..0x00AEA330): a shape with no geometry, used for unknown types.

uint32_t GxuNullShapeFactory::CreateVertexBuffer() {
    return GxuBufferCache::INVALID_HANDLE;
}

uint32_t GxuNullShapeFactory::CreateIndexBuffer() {
    return GxuBufferCache::INVALID_HANDLE;
}

uint32_t GxuNullShapeFactory::GetNumVertices() const {
    return 0;
}

uint32_t GxuNullShapeFactory::GetNumIndices() const {
    return 0;
}

float GxuNullShapeFactory::GetRadius() const {
    return 0.0f;
}

// ---- Box (4.1 CreateVertexBuffer 0x00AE8630, CreateIndexBuffer 0x00AE8430)

uint32_t GxuBoxShapeFactory::CreateVertexBuffer() {
    // TODO: 4.1 0x00AE8630 creates the buffer once (GxuBufferCache::CreateBuffer(handle, vertex target, 12 bytes,
    // 8 items)), locks it and writes the eight +-0.5 corners as C3Vector.
    return this->m_vertexBuffer;
}

uint32_t GxuBoxShapeFactory::CreateIndexBuffer() {
    // TODO: 4.1 0x00AE8430: 36 indices (12 triangles) over the eight corners.
    return this->m_indexBuffer;
}

uint32_t GxuBoxShapeFactory::GetNumVertices() const {
    return 8;
}

uint32_t GxuBoxShapeFactory::GetNumIndices() const {
    return 36;
}

float GxuBoxShapeFactory::GetRadius() const {
    // Half the cube diagonal: sqrt(3) * 0.5.
    return 0.8660254f;
}

// ---- Sphere (4.1 CreateVertexBuffer 0x00AE7A70, CreateIndexBuffer 0x00AE7110, SetLOD 0x00AE7030)

uint32_t GxuSphereShapeFactory::CreateVertexBuffer() {
    // TODO: 4.1 0x00AE7A70: ring/segment grid on the unit sphere, sized by the current LOD.
    return this->m_vertexBuffer;
}

uint32_t GxuSphereShapeFactory::CreateIndexBuffer() {
    // TODO: 4.1 0x00AE7110.
    return this->m_indexBuffer;
}

uint32_t GxuSphereShapeFactory::GetNumVertices() const {
    return this->m_rings * this->m_segments + 2;
}

uint32_t GxuSphereShapeFactory::GetNumIndices() const {
    return this->m_rings * this->m_segments * 6;
}

float GxuSphereShapeFactory::GetRadius() const {
    return 1.0f;
}

uint32_t GxuSphereShapeFactory::GetIdxOffset() const {
    // TODO: 4.1 0x00AE6C20: every LOD's indices share one buffer; this is the current LOD's start.
    return 0;
}

void GxuSphereShapeFactory::SetLOD(float lod) {
    // TODO: 4.1 0x00AE7030 picks the ring and segment counts from lod (scaled by GxuGetLODDistModifier).
}

// ---- Cone (4.1 CreateVertexBuffer 0x00AE78A0, CreateIndexBuffer 0x00AE7D70)

uint32_t GxuConeShapeFactory::CreateVertexBuffer() {
    // TODO: 4.1 0x00AE78A0: apex plus a base circle.
    return this->m_vertexBuffer;
}

uint32_t GxuConeShapeFactory::CreateIndexBuffer() {
    // TODO: 4.1 0x00AE7D70.
    return this->m_indexBuffer;
}

uint32_t GxuConeShapeFactory::GetNumVertices() const {
    return 0; // TODO: 4.1 0x00AE6BF0
}

uint32_t GxuConeShapeFactory::GetNumIndices() const {
    return 0; // TODO: 4.1 0x00AE6C00
}

float GxuConeShapeFactory::GetRadius() const {
    return 1.0f; // TODO: 4.1 0x00AE6BE0
}

// ---- Triangle (4.1 CreateVertexBuffer 0x00AE82B0, CreateIndexBuffer 0x00AE8160)

uint32_t GxuTriangleShapeFactory::CreateVertexBuffer() {
    // TODO: 4.1 0x00AE82B0.
    return this->m_vertexBuffer;
}

uint32_t GxuTriangleShapeFactory::CreateIndexBuffer() {
    // TODO: 4.1 0x00AE8160.
    return this->m_indexBuffer;
}

uint32_t GxuTriangleShapeFactory::GetNumVertices() const {
    return 3;
}

uint32_t GxuTriangleShapeFactory::GetNumIndices() const {
    return 3;
}

float GxuTriangleShapeFactory::GetRadius() const {
    return 1.0f; // TODO: 4.1 0x00AE6CC0
}

// ---- Render-state blocks (4.1 Apply 0x00AE8E10 / NoDepth 0x00AE9A40, Restore 0x00AE7360 / 0x00AE70F0)
//
// 4.1 Apply writes the device's render state cache directly and marks depth test, depth func, depth write, polygon
// offset, lighting, color write, fog and culling dirty; wireframe goes through the PolygonFill master enable. Here the
// same states go through RsPush/RsSet so Restore is a plain RsPop.

GxuDebugShapeRenderStateBlock::GxuDebugShapeRenderStateBlock(int32_t depthTest, int32_t culling, int32_t wireframe)
    : m_depthTest(depthTest)
    , m_culling(culling)
    , m_wireframe(wireframe) {
}

GxuDebugShapeRenderStateBlock::~GxuDebugShapeRenderStateBlock() {
}

void GxuDebugShapeRenderStateBlock::Apply() {
    CGxDevice* device = g_theGxDevicePtr;
    device->RsPush();
    device->RsSet(GxRs_Lighting, 0);
    device->RsSet(GxRs_Fog, 0);
    device->RsSet(GxRs_PolygonOffset, 0);
    device->RsSet(GxRs_ColorWrite, 15);
    device->RsSet(GxRs_DepthTest, this->m_depthTest);
    device->RsSet(GxRs_DepthWrite, 0);
    device->RsSet(GxRs_Culling, this->m_culling);
    device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Alpha));

    if (this->m_wireframe) {
        GxMasterEnableSet(GxMasterEnable_PolygonFill, 0);
    }
}

void GxuDebugShapeRenderStateBlock::Restore() {
    if (this->m_wireframe) {
        GxMasterEnableSet(GxMasterEnable_PolygonFill, 1);
    }

    g_theGxDevicePtr->RsPop();
}

GxuDebugShapeRenderStateBlockNoDepth::GxuDebugShapeRenderStateBlockNoDepth()
    : GxuDebugShapeRenderStateBlock(0, 1, 0) {
}

void GxuDebugShapeRenderStateBlockNoDepth::Apply() {
    // Port choice: also turn the DepthTest master enable off, so per-batch depth settings cannot bring the test back
    // (overlays drawn after the scene, like the collision display, rely on it). 4.1's NoDepth::Apply was not checked
    // for this; its Solid::Apply only toggles the PolygonFill master enable.
    GxuDebugShapeRenderStateBlock::Apply();
    GxMasterEnableSet(GxMasterEnable_DepthTest, 0);
}

void GxuDebugShapeRenderStateBlockNoDepth::Restore() {
    GxMasterEnableSet(GxMasterEnable_DepthTest, 1);
    GxuDebugShapeRenderStateBlock::Restore();
}

// ---- Drawer

IGxuDebugShapeFactory* GxuDebugShapeDrawer::GetFactory(GXUDEBUGSHAPE shape) {
    switch (shape) {
        case GXUDEBUGSHAPE_BOX:
            return &s_boxFactory;
        case GXUDEBUGSHAPE_SPHERE:
            return &s_sphereFactory;
        case GXUDEBUGSHAPE_CONE:
            return &s_coneFactory;
        case GXUDEBUGSHAPE_TRIANGLE:
            return &s_triangleFactory;
        default:
            return &s_nullFactory;
    }
}

GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::GetStateBlock(GXUDEBUGSTYLE style) {
    if (!s_renderStateBlockSolid) {
        s_renderStateBlockSolid = new GxuDebugShapeRenderStateBlock(1, 1, 0);
        s_renderStateBlockSolidBackfacing = new GxuDebugShapeRenderStateBlock(1, 0, 0);
        s_renderStateBlockSolidNoDepth = new GxuDebugShapeRenderStateBlockNoDepth();
        s_renderStateBlockWireframe = new GxuDebugShapeRenderStateBlock(1, 0, 1);
    }

    switch (style) {
        case GXUDEBUGSTYLE_SOLID_BACKFACING:
            return s_renderStateBlockSolidBackfacing;
        case GXUDEBUGSTYLE_SOLID_NODEPTH:
            return s_renderStateBlockSolidNoDepth;
        case GXUDEBUGSTYLE_WIREFRAME:
            return s_renderStateBlockWireframe;
        default:
            return s_renderStateBlockSolid;
    }
}

void GxuDebugShapeDrawer::Draw(const TSGrowableArray<GxuDebugShapeObject>& objects) {
    for (uint32_t i = 0; i < objects.m_count; i++) {
        const GxuDebugShapeObject& object = objects.m_data[i];
        IGxuDebugShapeFactory* factory = GetFactory(object.m_shape);

        if (!factory->GetNumIndices()) {
            continue;
        }

        GxuDebugShapeRenderStateBlock* block = GetStateBlock(object.m_style);
        block->Apply();
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, object.m_transform);

        // TODO: bind the factory's cached vertex/index buffers (CreateVertexBuffer/CreateIndexBuffer handles, sphere
        // indices from GetIdxOffset), set object.m_color as the diffuse material color and draw an indexed triangle
        // list. Until the factories fill their buffers nothing is drawn.

        GxXformPop(GxXform_World);
        block->Restore();
    }
}

// ---- Port entry points

void GxuDebugDrawShape(GXUDEBUGSHAPE shape, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style) {
    GxuDebugShapeObject* object = s_shapeObjects.New();
    object->m_shape = shape;
    object->m_style = style;
    object->m_transform = transform;
    object->m_color = color;
}

void GxuDebugDrawRender() {
    GxuDebugShapeDrawer::Draw(s_shapeObjects);
    s_shapeObjects.SetCount(0);

    // TODO: draw the queued text, 3D text and textures (GxuDebugDrawTexture.cpp) the same way.
}

// ---- Shutdown chain (4.1 GxuShutdownDebugDraw 0x00AE6B60)

void GxuShutdownDebugDrawList() {
    s_shapeObjects.SetCount(0);
}

void GxuShutdownDebugShapeDrawer() {
    // 4.1 0x00AE6E80 releases the factories' buffers and frees the state blocks.
    s_debugShapeBufferCache.ReleaseCache();

    delete GxuDebugShapeDrawer::s_renderStateBlockSolid;
    delete GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing;
    delete GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth;
    delete GxuDebugShapeDrawer::s_renderStateBlockWireframe;
    GxuDebugShapeDrawer::s_renderStateBlockSolid = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockWireframe = nullptr;
}

void GxuShutdownDebugDraw() {
    GxuShutdownDebugDrawList();
    GxuShutdownDebugShapeDrawer();
    GxuShutdownDebugTextDrawer();
    GxuShutdownDebugText3DDrawer();
    GxuShutdownDebugTextureDrawer();
}

#endif
