#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_GXU_DEBUG_DRAW

#include <cmath>
#include <cstring>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Gx.h"

// Tool features: Gxu debug shapes (4.1 Engine/Source/Gxu/GxuDebugDrawShapes.cpp, see GxuDebugDraw.h).
// 4.1 addresses are the 13850 Mac build's. Geometry, LOD tables and render states are 4.1's; the draw loop is this
// port's (4.1 retail kept no caller of the factories).

static const float PI = 3.14159265f;

// 4.1 0x012E7EA4..0x012E7EC0
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolid;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth;
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::s_renderStateBlockWireframe;

// 4.1 0x014D5700
GxuBufferCache s_debugShapeBufferCache;

static TSGrowableArray<GxuDebugShapeObject> s_shapeObjects;
static TSGrowableArray<GxuDebugLineObject> s_lineObjects;

static GxuNullShapeFactory s_nullFactory;
static GxuBoxShapeFactory s_boxFactory;
static GxuSphereShapeFactory s_sphereFactory;
static GxuConeShapeFactory s_coneFactory;
static GxuTriangleShapeFactory s_triangleFactory;

// 4.1 GxuGetLODDistModifier returns a global scale for every LOD distance; 3.3.5 has no such setting, so it is 1.
static float GxuGetLODDistModifier() {
    return 1.0f;
}

// ---- IGxuDebugShapeFactory (4.1: base GetIdxOffset 0x00AE6BD0, SetLOD 0x00AE6BC0)

IGxuDebugShapeFactory::IGxuDebugShapeFactory()
    : m_vertexBuffer(GxuBufferCache::INVALID_HANDLE)
    , m_indexBuffer(GxuBufferCache::INVALID_HANDLE) {
}

IGxuDebugShapeFactory::~IGxuDebugShapeFactory() {
}

// The 4.1 sequence: reuse a live buffer, else (re)create it under the same handle, lock it, write, unlock, mark it
// filled. The CPU copy is refreshed alongside so the drawer always has positions.
uint32_t IGxuDebugShapeFactory::CreateVertexBuffer() {
    if (s_debugShapeBufferCache.Exists(this->m_vertexBuffer)) {
        return this->m_vertexBuffer;
    }

    uint32_t count = this->GetNumVertices();
    this->m_drawVertices.SetCount(count);
    this->WriteVertices(this->m_drawVertices.m_data);

    this->m_vertexBuffer = s_debugShapeBufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), count);
    CGxBuf* buf = s_debugShapeBufferCache.AcquireBuffer(this->m_vertexBuffer);
    char* data = g_theGxDevicePtr->BufLock(buf);

    if (data) {
        memcpy(data, this->m_drawVertices.m_data, count * sizeof(C3Vector));
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
    s_debugShapeBufferCache.ReleaseBuffer(this->m_vertexBuffer);

    return this->m_vertexBuffer;
}

uint32_t IGxuDebugShapeFactory::CreateIndexBuffer() {
    if (s_debugShapeBufferCache.Exists(this->m_indexBuffer)) {
        return this->m_indexBuffer;
    }

    uint32_t count = this->GetIndexBufferCount();
    this->m_indexBuffer = s_debugShapeBufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), count);
    CGxBuf* buf = s_debugShapeBufferCache.AcquireBuffer(this->m_indexBuffer);
    char* data = g_theGxDevicePtr->BufLock(buf);

    if (data) {
        this->WriteIndices(reinterpret_cast<uint16_t*>(data));
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
    s_debugShapeBufferCache.ReleaseBuffer(this->m_indexBuffer);

    return this->m_indexBuffer;
}

uint32_t IGxuDebugShapeFactory::GetIdxOffset() const {
    return 0;
}

void IGxuDebugShapeFactory::SetLOD(float dist) {
    // One tessellation; only the sphere has LODs.
}

uint32_t IGxuDebugShapeFactory::GetIndexBufferCount() const {
    return this->GetNumIndices();
}

// ---- Null (4.1 0x00AEA2F0..0x00AEA330): no geometry, stands in for unknown shapes.

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

void GxuNullShapeFactory::WriteVertices(C3Vector* vertices) const {
}

void GxuNullShapeFactory::WriteIndices(uint16_t* indices) const {
}

// ---- Box (4.1 CreateVertexBuffer 0x00AE8630, CreateIndexBuffer 0x00AE8430)
//
// The +-0.5 cube. Vertices walk the four vertical edges around the cube (y=-0.5 side first), bottom then top corner
// of each: v0(-,-,-) v1(-,-,+) v2(+,-,-) v3(+,-,+) v4(+,+,-) v5(+,+,+) v6(-,+,-) v7(-,+,+). That makes the side walls
// one strip over indices 0..7 (wrapping), alternating winding, then two triangles each for the bottom and top.

void GxuBoxShapeFactory::WriteVertices(C3Vector* vertices) const {
    for (uint32_t i = 0; i < 2; i++) {
        float y = i ? 0.5f : -0.5f;

        for (uint32_t j = 0; j < 2; j++) {
            float x = (i == j) ? -0.5f : 0.5f;
            vertices[i * 4 + j * 2 + 0] = C3Vector(x, y, -0.5f);
            vertices[i * 4 + j * 2 + 1] = C3Vector(x, y, 0.5f);
        }
    }
}

void GxuBoxShapeFactory::WriteIndices(uint16_t* indices) const {
    uint32_t n = 0;

    for (uint32_t i = 0; i < 8; i++) {
        if (i & 1) {
            indices[n++] = static_cast<uint16_t>(i & 7);
            indices[n++] = static_cast<uint16_t>((i + 1) & 7);
            indices[n++] = static_cast<uint16_t>((i + 2) & 7);
        } else {
            indices[n++] = static_cast<uint16_t>((i + 2) & 7);
            indices[n++] = static_cast<uint16_t>((i + 1) & 7);
            indices[n++] = static_cast<uint16_t>(i & 7);
        }
    }

    static const uint16_t s_caps[12] = {
        4, 2, 0, 0, 6, 4, // bottom (z=-0.5)
        1, 3, 5, 5, 7, 1, // top (z=+0.5)
    };

    memcpy(&indices[n], s_caps, sizeof(s_caps));
}

uint32_t GxuBoxShapeFactory::GetNumVertices() const {
    return 8;
}

uint32_t GxuBoxShapeFactory::GetNumIndices() const {
    return 36;
}

float GxuBoxShapeFactory::GetRadius() const {
    return 0.5f;
}

// ---- Sphere (4.1 static init 0x00083080, CreateVertexBuffer 0x00AE7A70, CreateIndexBuffer 0x00AE7110,
// SetLOD 0x00AE7030, GetIdxOffset 0x00AE6C20)
//
// A 33 ring x 32 segment grid from pole (z=+1) to pole (z=-1); each pole is a ring of 32 coincident vertices. All
// three LODs index the same vertices: LOD l walks the grid in steps of 1 << l, and their indices sit back to back in
// one index buffer (m_idxOffset).

GxuSphereShapeFactory::GxuSphereShapeFactory()
    : m_lod(0)
    , m_segments(32)
    , m_rings(33)
    , m_numVertices(32 * 33)
    , m_totalIndices(0) {
    for (uint32_t lod = 0; lod < NUM_LODS; lod++) {
        this->m_numIndices[lod] = 3 * (this->m_segments >> lod) * ((2 * this->m_segments) >> lod);
        this->m_idxOffset[lod] = lod ? this->m_idxOffset[lod - 1] + this->m_numIndices[lod - 1] : 0;
        this->m_totalIndices += this->m_numIndices[lod];
    }
}

void GxuSphereShapeFactory::WriteVertices(C3Vector* vertices) const {
    for (uint32_t ring = 0; ring < this->m_rings; ring++) {
        float theta = static_cast<float>(ring) / static_cast<float>(this->m_rings - 1) * PI;
        float sinTheta = fabsf(sinf(theta));
        float cosTheta = cosf(theta);

        for (uint32_t segment = 0; segment < this->m_segments; segment++) {
            float phi = static_cast<float>(segment) / static_cast<float>(this->m_segments) * 2.0f * PI;
            vertices[ring * this->m_segments + segment] = C3Vector(cosf(phi) * sinTheta, sinTheta * sinf(phi), cosTheta);
        }
    }
}

void GxuSphereShapeFactory::WriteIndices(uint16_t* indices) const {
    uint32_t n = 0;
    uint32_t segments = this->m_segments;

    for (uint32_t lod = 0; lod < NUM_LODS; lod++) {
        uint32_t step = 1 << lod;
        uint32_t rowStride = segments * step;

        for (uint32_t row = 0; row < (segments >> lod); row++) {
            uint32_t base = row * rowStride;

            for (uint32_t column = 0; column < (segments >> lod); column++) {
                uint32_t a = base + column * step;
                uint32_t b = base + ((column + 1) * step) % segments;

                indices[n++] = static_cast<uint16_t>(a);
                indices[n++] = static_cast<uint16_t>(a + rowStride);
                indices[n++] = static_cast<uint16_t>(b);
                indices[n++] = static_cast<uint16_t>(b);
                indices[n++] = static_cast<uint16_t>(a + rowStride);
                indices[n++] = static_cast<uint16_t>(b + rowStride);
            }
        }
    }
}

uint32_t GxuSphereShapeFactory::GetIndexBufferCount() const {
    return this->m_totalIndices;
}

uint32_t GxuSphereShapeFactory::GetNumVertices() const {
    return this->m_numVertices;
}

uint32_t GxuSphereShapeFactory::GetNumIndices() const {
    return this->m_numIndices[this->m_lod];
}

float GxuSphereShapeFactory::GetRadius() const {
    return 1.0f;
}

uint32_t GxuSphereShapeFactory::GetIdxOffset() const {
    return this->m_idxOffset[this->m_lod];
}

// Full detail up to 50 units, half detail to 100, quarter beyond (times the LOD distance modifier).
void GxuSphereShapeFactory::SetLOD(float dist) {
    float ratio = dist / (GxuGetLODDistModifier() * 100.0f);
    this->m_lod = ratio < 1.0f ? static_cast<uint32_t>(ratio * 2.0f) : 2;
}

// ---- Cone (4.1 CreateVertexBuffer 0x00AE78A0, CreateIndexBuffer 0x00AE7D70, GetRadius 0x00AE6BE0,
// GetNumVertices 0x00AE6BF0, GetNumIndices 0x00AE6C00)
//
// Apex v0 at z=+0.5, base center v1 at z=-0.5, then a 12-segment unit circle at z=-0.5. Side fans from the apex, the
// base fans from its center with the opposite winding.

void GxuConeShapeFactory::WriteVertices(C3Vector* vertices) const {
    vertices[0] = C3Vector(0.0f, 0.0f, 0.5f);
    vertices[1] = C3Vector(0.0f, 0.0f, -0.5f);

    for (uint32_t k = 0; k < NUM_SEGMENTS; k++) {
        float angle = static_cast<float>(k) / static_cast<float>(NUM_SEGMENTS) * 2.0f * PI;
        vertices[2 + k] = C3Vector(cosf(angle), sinf(angle), -0.5f);
    }
}

void GxuConeShapeFactory::WriteIndices(uint16_t* indices) const {
    uint32_t n = 0;

    for (uint32_t k = 0; k < NUM_SEGMENTS; k++) {
        indices[n++] = 0;
        indices[n++] = static_cast<uint16_t>(2 + k);
        indices[n++] = static_cast<uint16_t>(2 + (k + 1) % NUM_SEGMENTS);
    }

    for (uint32_t k = 0; k < NUM_SEGMENTS; k++) {
        indices[n++] = 1;
        indices[n++] = static_cast<uint16_t>(2 + (k + 1) % NUM_SEGMENTS);
        indices[n++] = static_cast<uint16_t>(2 + k);
    }
}

uint32_t GxuConeShapeFactory::GetNumVertices() const {
    return NUM_SEGMENTS + 2;
}

uint32_t GxuConeShapeFactory::GetNumIndices() const {
    return NUM_SEGMENTS * 6;
}

float GxuConeShapeFactory::GetRadius() const {
    return 1.0f;
}

// ---- Triangle (4.1 CreateVertexBuffer 0x00AE82B0, CreateIndexBuffer 0x00AE8160, GetRadius 0x00AE6CC0)
//
// One triangle in the x=0 plane, written twice with opposite windings so it shows from both sides under culling.

void GxuTriangleShapeFactory::WriteVertices(C3Vector* vertices) const {
    vertices[0] = C3Vector(0.0f, 0.0f, 0.5f);
    vertices[1] = C3Vector(0.0f, 0.5f, -0.5f);
    vertices[2] = C3Vector(0.0f, -0.5f, -0.5f);
    vertices[3] = C3Vector(0.0f, 0.0f, 0.5f);
    vertices[4] = C3Vector(0.0f, -0.5f, -0.5f);
    vertices[5] = C3Vector(0.0f, 0.5f, -0.5f);
}

void GxuTriangleShapeFactory::WriteIndices(uint16_t* indices) const {
    for (uint16_t i = 0; i < 6; i++) {
        indices[i] = i;
    }
}

uint32_t GxuTriangleShapeFactory::GetNumVertices() const {
    return 6;
}

uint32_t GxuTriangleShapeFactory::GetNumIndices() const {
    return 6;
}

float GxuTriangleShapeFactory::GetRadius() const {
    return 0.5f;
}

// ---- Render-state blocks (4.1 Apply 0x00AE8E10 / NoDepth 0x00AE9A40, Restore 0x00AE7360 / 0x00AE70F0)
//
// 4.1 Apply: RsPush, then depth test on, depth func LEqual, depth write off, polygon offset 0, lighting off, color
// write all, fog off, vertex and pixel shader cleared, culling from the fill mode; it saves the PolygonFill master
// enable and turns it off for wireframe only. Restore puts the master enable back and pops. The NoDepth block is the
// same with depth test off and solid culling, and leaves the master enable alone.
//
// Port additions: alpha blending with the device alpha reference, so a color's alpha shows (4.1 blends in its pixel
// shader).

GxuDebugShapeRenderStateBlock::GxuDebugShapeRenderStateBlock(GXUDEBUGFILLMODE fillMode)
    : m_fillMode(fillMode)
    , m_savedPolygonFill(1) {
}

GxuDebugShapeRenderStateBlock::~GxuDebugShapeRenderStateBlock() {
}

static void ApplyCommonStates(int32_t depthTest) {
    CGxDevice* device = g_theGxDevicePtr;
    device->RsPush();
    device->RsSet(GxRs_DepthTest, depthTest);
    device->RsSet(GxRs_DepthFunc, static_cast<int32_t>(GxCmp_LEqual));
    device->RsSet(GxRs_DepthWrite, 0);
    device->RsSet(GxRs_PolygonOffset, 0);
    device->RsSet(GxRs_Lighting, 0);
    device->RsSet(GxRs_ColorWrite, static_cast<int32_t>(GxColorWrite_All));
    device->RsSet(GxRs_Fog, 0);
    device->RsSet(GxRs_VertexShader, static_cast<void*>(nullptr));
    device->RsSet(GxRs_PixelShader, static_cast<void*>(nullptr));
    device->RsSet(GxRs_Texture0, static_cast<void*>(nullptr));
    device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(GxBlend_Alpha));
    GxUpdateAlphaRef(GxRs_AlphaRef);
}

void GxuDebugShapeRenderStateBlock::Apply() {
    ApplyCommonStates(1);

    int32_t culling;

    switch (this->m_fillMode) {
        case GXUDEBUGFILL_SOLID:
            culling = 1;
            break;
        case GXUDEBUGFILL_SOLID_BACKFACING:
            culling = 2;
            break;
        case GXUDEBUGFILL_WIREFRAME:
            culling = 0;
            break;
        default:
            SErrDisplayError(STORM_ERROR_ASSERTION, __FILE__, __LINE__, "unknown fill mode", 1, 1, 0);
            culling = 1;
            break;
    }

    g_theGxDevicePtr->RsSet(GxRs_Culling, culling);

    this->m_savedPolygonFill = GxMasterEnable(GxMasterEnable_PolygonFill);
    GxMasterEnableSet(GxMasterEnable_PolygonFill, this->m_fillMode != GXUDEBUGFILL_WIREFRAME);
}

void GxuDebugShapeRenderStateBlock::Restore() {
    GxMasterEnableSet(GxMasterEnable_PolygonFill, this->m_savedPolygonFill);
    g_theGxDevicePtr->RsPop();
}

GxuDebugShapeRenderStateBlockNoDepth::GxuDebugShapeRenderStateBlockNoDepth()
    : GxuDebugShapeRenderStateBlock(GXUDEBUGFILL_SOLID) {
}

void GxuDebugShapeRenderStateBlockNoDepth::Apply() {
    ApplyCommonStates(0);
    g_theGxDevicePtr->RsSet(GxRs_Culling, 1);
}

void GxuDebugShapeRenderStateBlockNoDepth::Restore() {
    g_theGxDevicePtr->RsPop();
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

// 4.1 holds the four blocks as statics built at startup; here they are built on first use.
GxuDebugShapeRenderStateBlock* GxuDebugShapeDrawer::GetStateBlock(GXUDEBUGSTYLE style) {
    if (!s_renderStateBlockSolid) {
        s_renderStateBlockSolid = new GxuDebugShapeRenderStateBlock(GXUDEBUGFILL_SOLID);
        s_renderStateBlockSolidBackfacing = new GxuDebugShapeRenderStateBlock(GXUDEBUGFILL_SOLID_BACKFACING);
        s_renderStateBlockSolidNoDepth = new GxuDebugShapeRenderStateBlockNoDepth();
        s_renderStateBlockWireframe = new GxuDebugShapeRenderStateBlock(GXUDEBUGFILL_WIREFRAME);
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

// Port: draws each queued shape as one indexed triangle list. The world matrix is the shape's transform moved by
// -viewOrigin (the camera-relative origin the 3.3.5 world view expects, as in CWorldScene::RenderCollisionDebug).
// Sphere LOD comes from the distance to the view origin divided by the shape's largest axis scale, so a sphere
// coarsens as it shrinks on screen.
void GxuDebugShapeDrawer::Draw(const TSGrowableArray<GxuDebugShapeObject>& objects, const C3Vector& viewOrigin) {
    for (uint32_t i = 0; i < objects.m_count; i++) {
        const GxuDebugShapeObject& object = objects.m_data[i];
        IGxuDebugShapeFactory* factory = GetFactory(object.m_shape);

        if (!factory->GetNumVertices()) {
            continue;
        }

        factory->CreateVertexBuffer();
        uint32_t indexHandle = factory->CreateIndexBuffer();

        if (!s_debugShapeBufferCache.Exists(indexHandle) || !factory->m_drawVertices.m_count) {
            continue;
        }

        const C44Matrix& m = object.m_transform;
        C3Vector center(m.d0 - viewOrigin.x, m.d1 - viewOrigin.y, m.d2 - viewOrigin.z);
        float scaleA = m.a0 * m.a0 + m.a1 * m.a1 + m.a2 * m.a2;
        float scaleB = m.b0 * m.b0 + m.b1 * m.b1 + m.b2 * m.b2;
        float scaleC = m.c0 * m.c0 + m.c1 * m.c1 + m.c2 * m.c2;
        float scale = sqrtf(scaleA > scaleB ? (scaleA > scaleC ? scaleA : scaleC) : (scaleB > scaleC ? scaleB : scaleC));
        float dist = sqrtf(center.x * center.x + center.y * center.y + center.z * center.z);
        factory->SetLOD(scale > 0.0f ? dist / scale : dist);

        C44Matrix world = m;
        world.d0 = center.x;
        world.d1 = center.y;
        world.d2 = center.z;

        CImVector color = object.m_color;
        GxFormatColor(color);

        CGxBuf* indexBuf = s_debugShapeBufferCache.AcquireBuffer(indexHandle);
        GxuDebugShapeRenderStateBlock* block = GetStateBlock(object.m_style);
        block->Apply();
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

        uint32_t numVertices = factory->m_drawVertices.m_count;
        GxPrimVertexPtr(numVertices, factory->m_drawVertices.m_data, sizeof(C3Vector), nullptr, 0, &color, 0, nullptr, 0, nullptr, 0);
        g_theGxDevicePtr->PrimIndexPtr(indexBuf);

        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = factory->GetIdxOffset();
        batch.m_count = factory->GetNumIndices();
        batch.m_minIndex = 0;
        batch.m_maxIndex = numVertices - 1;
        g_theGxDevicePtr->Draw(&batch, 1);

        GxXformPop(GxXform_World);
        block->Restore();
        s_debugShapeBufferCache.ReleaseBuffer(indexHandle);
    }
}

// ---- Lines (port)
//
// Drawn like the 3.3.5 hand-written overlays (RenderCollisionInfo's normal lines): a PC vertex stream, a 0..n index
// stream and one GxPrim_Lines batch, under the Solid or NoDepth render-state block. Depth-tested lines go first so the
// always-on-top ones stay on top. Each batch holds at most LINE_BATCH_SIZE lines (16-bit indices).

static const uint32_t LINE_BATCH_SIZE = 0x4000;

static void DrawLineBatch(const TSGrowableArray<GxuDebugLineObject>& objects, bool noDepth) {
    static TSGrowableArray<C3Vector> s_positions;
    static TSGrowableArray<CImVector> s_colors;
    static TSGrowableArray<uint16_t> s_indices;

    uint32_t i = 0;

    while (i < objects.m_count) {
        s_positions.SetCount(0);
        s_colors.SetCount(0);

        for (; i < objects.m_count && s_positions.m_count < LINE_BATCH_SIZE * 2; i++) {
            const GxuDebugLineObject& line = objects.m_data[i];

            if ((line.m_style == GXUDEBUGSTYLE_SOLID_NODEPTH) != noDepth) {
                continue;
            }

            CImVector color = line.m_color;
            GxFormatColor(color);
            *s_positions.New() = line.m_start;
            *s_positions.New() = line.m_end;
            *s_colors.New() = color;
            *s_colors.New() = color;
        }

        uint32_t count = s_positions.m_count;

        if (!count) {
            continue;
        }

        if (s_indices.m_count < count) {
            uint32_t first = s_indices.m_count;
            s_indices.SetCount(count);

            for (uint32_t n = first; n < count; n++) {
                s_indices.m_data[n] = static_cast<uint16_t>(n);
            }
        }

        GxuDebugShapeRenderStateBlock* block = GxuDebugShapeDrawer::GetStateBlock(noDepth ? GXUDEBUGSTYLE_SOLID_NODEPTH : GXUDEBUGSTYLE_SOLID);
        block->Apply();
        g_theGxDevicePtr->RsSet(GxRs_Culling, 0);

        GxPrimVertexPtr(count, s_positions.m_data, sizeof(C3Vector), nullptr, 0, s_colors.m_data, sizeof(CImVector), nullptr, 0, nullptr, 0);
        GxPrimIndexPtr(count, s_indices.m_data);

        CGxBatch batch;
        batch.m_primType = GxPrim_Lines;
        batch.m_start = 0;
        batch.m_count = count;
        batch.m_minIndex = 0;
        batch.m_maxIndex = count - 1;
        g_theGxDevicePtr->Draw(&batch, 1);

        block->Restore();
    }
}

void GxuDebugShapeDrawer::DrawLines(const TSGrowableArray<GxuDebugLineObject>& objects, const C3Vector& viewOrigin) {
    C44Matrix world;
    world.Translate(C3Vector(-viewOrigin.x, -viewOrigin.y, -viewOrigin.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    DrawLineBatch(objects, false);
    DrawLineBatch(objects, true);

    GxXformPop(GxXform_World);
}

// ---- Port entry points

void GxuDebugDrawShape(GXUDEBUGSHAPE shape, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style) {
    GxuDebugShapeObject* object = s_shapeObjects.New();
    object->m_shape = shape;
    object->m_style = style;
    object->m_transform = transform;
    object->m_color = color;
}

void GxuDebugDrawLine(const C3Vector& start, const C3Vector& end, const CImVector& color, GXUDEBUGSTYLE style) {
    GxuDebugLineObject* object = s_lineObjects.New();
    object->m_start = start;
    object->m_end = end;
    object->m_color = color;
    object->m_style = style;
}

void GxuDebugDrawAaBoxLines(const C3Vector& minCorner, const C3Vector& maxCorner, const CImVector& color, GXUDEBUGSTYLE style) {
    C3Vector corners[8];

    for (uint32_t i = 0; i < 8; i++) {
        corners[i] = C3Vector((i & 1) ? maxCorner.x : minCorner.x, (i & 2) ? maxCorner.y : minCorner.y, (i & 4) ? maxCorner.z : minCorner.z);
    }

    // Each edge joins two corners that differ in one axis bit.
    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t bit = 1; bit < 8; bit <<= 1) {
            if (!(i & bit)) {
                GxuDebugDrawLine(corners[i], corners[i | bit], color, style);
            }
        }
    }
}

// The unit box scaled to the extent and moved to its center, then placed by transform (row vectors: local first).
void GxuDebugDrawBoundsBox(const C3Vector& minCorner, const C3Vector& maxCorner, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style) {
    C44Matrix local;
    local.a0 = maxCorner.x - minCorner.x;
    local.b1 = maxCorner.y - minCorner.y;
    local.c2 = maxCorner.z - minCorner.z;
    local.d0 = (minCorner.x + maxCorner.x) * 0.5f;
    local.d1 = (minCorner.y + maxCorner.y) * 0.5f;
    local.d2 = (minCorner.z + maxCorner.z) * 0.5f;

    GxuDebugDrawShape(GXUDEBUGSHAPE_BOX, local * transform, color, style);
}

void GxuDebugDrawBoundsSphere(const C3Vector& center, float radius, const C44Matrix& transform, const CImVector& color, GXUDEBUGSTYLE style) {
    C44Matrix local;
    local.a0 = radius;
    local.b1 = radius;
    local.c2 = radius;
    local.d0 = center.x;
    local.d1 = center.y;
    local.d2 = center.z;

    GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, local * transform, color, style);
}

// Shapes first, then lines, then 3D and 2D text and textures on top of them.
void GxuDebugDrawRender(const C3Vector& viewOrigin) {
    if (s_shapeObjects.m_count) {
        GxuDebugShapeDrawer::Draw(s_shapeObjects, viewOrigin);
        s_shapeObjects.SetCount(0);
    }

    if (s_lineObjects.m_count) {
        GxuDebugShapeDrawer::DrawLines(s_lineObjects, viewOrigin);
        s_lineObjects.SetCount(0);
    }

    GxuDebugTextureDrawer::Draw(viewOrigin);
    GxuDebugTextDrawer::Draw(viewOrigin);
}

// ---- Shutdown chain (4.1 GxuShutdownDebugDraw 0x00AE6B60)

void GxuShutdownDebugDrawList() {
    s_shapeObjects.SetCount(0);
    s_lineObjects.SetCount(0);
}

// 4.1 0x00AE6E80 releases the cache. The factories' handles are reset with it, since ReleaseCache drops the entries
// (see GxuBufferCache.cpp); the state blocks are freed too.
void GxuShutdownDebugShapeDrawer() {
    s_debugShapeBufferCache.ReleaseCache();

    IGxuDebugShapeFactory* factories[] = { &s_boxFactory, &s_sphereFactory, &s_coneFactory, &s_triangleFactory };

    for (IGxuDebugShapeFactory* factory : factories) {
        factory->m_vertexBuffer = GxuBufferCache::INVALID_HANDLE;
        factory->m_indexBuffer = GxuBufferCache::INVALID_HANDLE;
        factory->m_drawVertices.SetCount(0);
    }

    delete GxuDebugShapeDrawer::s_renderStateBlockSolid;
    delete GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing;
    delete GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth;
    delete GxuDebugShapeDrawer::s_renderStateBlockWireframe;
    GxuDebugShapeDrawer::s_renderStateBlockSolid = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockSolidBackfacing = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockSolidNoDepth = nullptr;
    GxuDebugShapeDrawer::s_renderStateBlockWireframe = nullptr;
}

// 4.1 order: text, 3D text, textures, shapes, then the object list.
void GxuShutdownDebugDraw() {
    GxuShutdownDebugTextDrawer();
    GxuShutdownDebugText3DDrawer();
    GxuShutdownDebugTextureDrawer();
    GxuShutdownDebugShapeDrawer();
    GxuShutdownDebugDrawList();
}

#endif
