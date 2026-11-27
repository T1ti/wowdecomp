#include "MapObjGroup.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include "WoW/Source/WorldClient/BspQuery_SegmentLink.h"
#include "WoW/Source/WorldClient/Map.h"
#include "Engine/Source/Tempest/CFacet.h"
#include <cfloat>
#include <cstring>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include <cmath>
#include <malloc.h>
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CWFrustum.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/AsyncFile.h"

// STATIC: 0x00AEEE58
VBBList CMapObjGroup::vertexVBList;

// STATIC: 0x00AEEE80
VBBList CMapObjGroup::indexVBList;

// STATIC: 0x00AEEEA8
TSExplicitList<CMapObjGroup::VertArray, offsetof(CMapObjGroup::VertArray, m_link)> CMapObjGroup::s_freeVertArrays;

// STATIC: 0x00D25BF8
uint16_t BspQuery::testFaces[BspQuery::MAX_FACES];

// STATIC: 0x00D29BF8
uint16_t BspQuery::hitFaces[BspQuery::MAX_FACES];

// STATIC: 0x00D2DBF8
uint32_t BspQuery::testFaceSub;

// STATIC: 0x00D2DBFC
uint32_t BspQuery::hitFaceSub;

// STATIC: 0x00CB7528
uint32_t World::TriData::statusFlags;

// STATIC: 0x00CB752C
uint32_t World::TriData::nBatches;

// STATIC: 0x00CB7530
uint32_t World::TriData::faceIndexCursor;

// STATIC: 0x00CB7534
uint32_t World::TriData::indexCursor;

// STATIC: 0x00CB7538
uint32_t World::TriData::matrixPoolCount;

// STATIC: 0x00CB7540
uint16_t World::TriData::faceIndexPool[WORLD_TRI_POOL_LIMITS::TRI_QUERY_FACE_INDEX_CAPACITY];

// STATIC: 0x00CBF540
uint16_t World::TriData::indexPool[WORLD_TRI_POOL_LIMITS::TRI_QUERY_INDEX_CAPACITY];

// STATIC: 0x00CD8080
World::TriData::Batch World::TriData::batches[WORLD_TRI_POOL_LIMITS::TRI_QUERY_BATCH_CAPACITY];

// STATIC: 0x00CD7880
C44Matrix World::TriData::matrixPool[WORLD_TRI_POOL_LIMITS::TRI_QUERY_MATRIX_CAPACITY];

struct SpanRange {
    /* 0000 */ int32_t m_min;
    /* 0004 */ int32_t m_max;

    SpanRange Merge(const SpanRange& other);
};

class SpanRasterizer {
    public:
    SpanRasterizer(const C2iVector& size, SpanRange* spans);
    void AddRing(int32_t x, int32_t y, int32_t radius);

    protected:
    /* 0004 */ int32_t m_width;

    private:
    /* 0008 */ int32_t m_height;
    /* 000C */ SpanRange* m_spans;

    virtual void VisitSpan(int32_t x0, int32_t x1, int32_t y) = 0;
    void VisitRange(int32_t x0, int32_t x1, int32_t y);
    void AddSpan(int32_t x0, int32_t x1, int32_t y);
};

class LiquidSoundQuery : public SpanRasterizer {
    public:
    LiquidSoundQuery(const C2iVector& size, SpanRange* spans, const C3Vector& position, int32_t liquidType, const SMOLTile* tiles, const SMOLVert* verts, TSGrowableArray<World::LiquidQueryResult>* results);
    bool Found() const { return m_found != 0; } // (inferred)
    void SetRadiusSq(float radiusSq) { m_radiusSq = radiusSq; } // (inferred)

    private:
    /* 0010 */ C3Vector m_position;
    /* 001C */ int32_t m_liquidType;
    /* 0020 */ const SMOLTile* m_tiles;
    /* 0024 */ const SMOLVert* m_verts;
    /* 0028 */ TSGrowableArray<World::LiquidQueryResult>* m_results;
    /* 002C */ float m_radiusSq;
    /* 0030 */ uint8_t m_found;

    void VisitSpan(int32_t x0, int32_t x1, int32_t y) override;
};

// STATIC: 0x00A3FA48
static const float s_liquidSoundRings[26] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 24.0f, 28.0f, 32.0f, 36.0f, 40.0f, 44.0f, 48.0f, 52.0f, 56.0f, 60.0f };

// STATIC: 0x00ADF804
static float s_liquidTileScale = 0.24f;

// VERIFY: 23ad3913 auto
// OFFSET: 0x00431F30
int32_t CMapObjGroup::GetLiquidType() const {
    return this->m_liquidType;
}

// VERIFY: 3d3b1760 manual
// OFFSET: 0x0079B310
void SpanRasterizer::VisitRange(int32_t x0, int32_t x1, int32_t y) {
    if (x0 <= x1) {
        this->VisitSpan(x0, x1, y);
    }
}

// VERIFY: a611df78 auto
// OFFSET: 0x0079B400
SpanRange SpanRange::Merge(const SpanRange& other) {
    int32_t maximum = this->m_max > other.m_max ? this->m_max : other.m_max;
    int32_t minimum = this->m_min < other.m_min ? this->m_min : other.m_min;
    this->m_max = maximum;
    this->m_min = minimum;
    SpanRange result;
    result.m_min = minimum;
    result.m_max = maximum;
    return result;
}

// VERIFY: 3943ee70 auto
// OFFSET: 0x0079B470
SpanRasterizer::SpanRasterizer(const C2iVector& size, SpanRange* spans) {
    this->m_width = size.x;
    this->m_spans = spans;
    this->m_height = size.y;

    for (int32_t i = 0; i < this->m_height; i++) {
        this->m_spans[i].m_min = 0x7FFFFFFF;
        this->m_spans[i].m_max = static_cast<int32_t>(0x80000000);
    }
}

// VERIFY: 3fe4e8a0 manual
// OFFSET: 0x0079B4C0
void SpanRasterizer::AddSpan(int32_t x0, int32_t x1, int32_t y) {
    if (y < 0 || y >= this->m_height || x1 < 0 || x0 >= this->m_width) {
        return;
    }

    int32_t left = Clamp(x0, 0, this->m_width - 1);
    int32_t right = Clamp(x1, 0, this->m_width - 1);
    STORM_ASSERT(left <= right);
    SpanRange& span = this->m_spans[y];

    if (span.m_max >= span.m_min) {
        this->VisitRange(left, span.m_min - 1, y);
        this->VisitRange(this->m_spans[y].m_max + 1, right, y);
    } else {
        this->VisitRange(left, right, y);
    }

    SpanRange range;
    range.m_min = left;
    range.m_max = right;
    this->m_spans[y].Merge(range);
}

// VERIFY: 7f7f8ecb manual
// OFFSET: 0x0079B590
void SpanRasterizer::AddRing(int32_t x, int32_t y, int32_t radius) {
    if (!radius) {
        this->AddSpan(x, x, y);
        return;
    }

    if (radius == 1) {
        this->AddSpan(x - 1, x + 1, y);
        this->AddSpan(x - 1, x + 1, y - 1);
        this->AddSpan(x - 1, x + 1, y + 1);
        return;
    }

    int32_t right = x + radius - 1;
    int32_t error = radius - 1;
    int32_t limit = radius - 1;
    this->AddSpan(x - (radius - 1), right, y);
    int32_t step = 0;

    if (radius - 1 < 0) {
        return;
    }

    int32_t left = x - (radius - 1);
    int32_t dx = 1;
    int32_t dy = 1 - 2 * (radius - 1);
    int32_t nextDy = dy;
    int32_t up = x;
    int32_t down = x;
    int32_t offset = y - x;

    while (true) {
        step++;
        int32_t nextDx = dx + 2;
        up++;
        down--;
        error -= dx;

        if (error < 0) {
            limit--;
            left++;
            right--;
            error -= dy;
            nextDy = dy + 2;
        }

        this->AddSpan(left, right, offset + down);
        this->AddSpan(left, right, offset + up);
        this->AddSpan(down, up, offset + left);
        this->AddSpan(down, up, offset + right);

        if (step > limit) {
            break;
        }

        dx = nextDx;
        dy = nextDy;
    }
}

// VERIFY: fc3235a2 auto
// OFFSET: 0x0079B700
LiquidSoundQuery::LiquidSoundQuery(const C2iVector& size, SpanRange* spans, const C3Vector& position, int32_t liquidType, const SMOLTile* tiles, const SMOLVert* verts, TSGrowableArray<World::LiquidQueryResult>* results) : SpanRasterizer(size, spans) {
    this->m_position = position;
    this->m_liquidType = liquidType;
    this->m_results = results;
    this->m_tiles = tiles;
    this->m_verts = verts;
    this->m_found = 0;
}

// VERIFY: 04c2b9e5 manual
// OFFSET: 0x0079B760
void CMapObjGroup::QueryLiquidSounds(const C3Vector& pos, TSGrowableArray<World::LiquidQueryResult>& results) {
    if (!this->m_liquidType || !this->liquidTiles.x || !this->liquidTiles.y) {
        return;
    }

    C3Vector local;
    local.x = pos.x - this->m_liquidCorner.x;
    local.y = pos.y - this->m_liquidCorner.y;
    local.z = pos.z;
    int32_t tileX = static_cast<int32_t>(local.x * s_liquidTileScale);
    int32_t tileY = static_cast<int32_t>(s_liquidTileScale * local.y);
    SpanRange* spans = static_cast<SpanRange*>(alloca(sizeof(SpanRange) * this->liquidTiles.y));
    LiquidSoundQuery query(this->liquidTiles, spans, local, this->m_liquidType, this->m_liquidTileList, this->m_liquidVertexList, &results);

    for (uint32_t i = 0; i < sizeof(s_liquidSoundRings) / sizeof(s_liquidSoundRings[0]); i++) {
        if (query.Found()) {
            break;
        }

        float ring = s_liquidSoundRings[i];
        query.SetRadiusSq(SMOLTILE_SIZE * ring * (SMOLTILE_SIZE * ring));
        query.AddRing(tileX, tileY, static_cast<int32_t>(ring));
    }
}

// VERIFY: dd010149 manual
// OFFSET: 0x0079C1A0
void LiquidSoundQuery::VisitSpan(int32_t x0, int32_t x1, int32_t y) {
    const SMOLTile* tile = &this->m_tiles[x0 + y * this->m_width];
    const SMOLVert* vert = &this->m_verts[x0 + y * (this->m_width + 1)];

    for (int32_t x = x0; x <= x1; x++, tile++, vert++) {
        if ((tile->m_flags & SMOLTile::LIQUID_TYPE_MASK) == SMOLTile::LIQUID_TYPE_NONE) {
            continue;
        }

        World::LiquidQueryResult* result = nullptr;

        for (uint32_t i = 0; i < this->m_results->Count(); i++) {
            if (this->m_results->m_data[i].m_liquidType == this->m_liquidType) {
                result = &this->m_results->m_data[i];
                break;
            }
        }

        float dz = vert->m_waterVert.m_height - this->m_position.z;

        if (result) {
            if (dz * dz + this->m_radiusSq >= result->m_offset.z * result->m_offset.z + result->m_offset.y * result->m_offset.y + result->m_offset.x * result->m_offset.x) {
                continue;
            }
        } else {
            result = this->m_results->New();
        }

        result->m_liquidType = this->m_liquidType;
        result->m_offset.x = x * SMOLTILE_SIZE - this->m_position.x;
        result->m_offset.y = y * SMOLTILE_SIZE - this->m_position.y;
        result->m_offset.z = dz;
        this->m_found = 1;
    }
}

// OFFSET: 0x007C6C30
void BspQuery_Segment::operator()(uint16_t face) {
    uint16_t polyBits = *reinterpret_cast<const uint16_t*>(&this->m_faces[face]);

    if (this->m_faceIgnoreFlags & polyBits) {
        return;
    }

    uint8_t materialId = this->m_faces[face].m_materialId;
    uint16_t ignoreMask = (materialId == 0xFF || !this->m_materials[materialId].m_blendMode) ? SMOPoly::IGNORE_OPAQUE : SMOPoly::IGNORE_BLENDED;

    if (this->m_faceIgnoreFlags & ignoreMask) {
        return;
    }

    if (BspQuery::testFaceSub >= BspQuery::MAX_FACES) {
        if (this->m_overflowFlags) {
            *this->m_overflowFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        }

        return;
    }

    BspQuery::testFaces[BspQuery::testFaceSub++] = face;
    this->m_faces[face].m_flags |= SMOPoly::F_COLLIDE_HIT;

    float t = 0.0f;

    if (!NTempest::Intersect(this->m_ray, this->m_vertexList, &this->m_indices[3 * face], &t, nullptr, 0.002f)) {
        return;
    }

    if (t < 0.0f || t > this->m_maxT) {
        return;
    }

    this->m_maxT = t;
    BspQuery::hitFaces[0] = face;
    BspQuery::hitFaceSub = 1;
    *this->m_hitT = t * this->m_oosegMag;

    if (*this->m_hitT > this->m_origHitT) {
        *this->m_hitT = this->m_origHitT;
    }
}

// VERIFY: c5ba0518 manual
// OFFSET: 0x007C6D50
bool BspQuery_Segment::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    if (!g_BspDigestCache) {
        return false;
    }

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->m_faces, this->m_vertexList, this->m_indices);

    if (!digest) {
        return false;
    }

    float maxX = this->m_seg.end.x;
    float maxY = this->m_seg.end.y;
    float minX = this->m_seg.start.x;
    float maxZ = this->m_seg.end.z;

    if (minX > maxX) {
        minX = this->m_seg.end.x;
        maxX = this->m_seg.start.x;
    }

    float minY = this->m_seg.start.y;

    if (minY > maxY) {
        minY = this->m_seg.end.y;
        maxY = this->m_seg.start.y;
    }

    float minZ = this->m_seg.start.z;

    if (minZ > maxZ) {
        minZ = this->m_seg.end.z;
        maxZ = this->m_seg.start.z;
    }

    minX -= 0.01f;
    maxX += 0.01f;
    minY -= 0.01f;
    maxY += 0.01f;
    minZ -= 0.01f;
    maxZ += 0.01f;

    uint8_t outcodes[450];

    for (uint32_t i = 0; i < digest->m_nVertices; i++) {
        const C3Vector& vertex = digest->m_vertices[i];
        uint8_t code = 0;

        if (minX > vertex.x) {
            code = 0x20;
        }

        if (maxX < vertex.x) {
            code |= 0x10;
        }

        if (minY > vertex.y) {
            code |= 0x8;
        }

        if (maxY < vertex.y) {
            code |= 0x4;
        }

        if (minZ > vertex.z) {
            code |= 0x2;
        }

        if (maxZ < vertex.z) {
            code |= 0x1;
        }

        outcodes[i] = code;
    }

    for (uint32_t i = 0; i < digest->m_nFaces; i++) {
        uint16_t ignore = this->m_faceIgnoreFlags;
        uint16_t face = digest->m_faceIndices[i];

        if (ignore & digest->m_faceFlags[i]) {
            continue;
        }

        SMOPoly& poly = this->m_faces[face];

        if (ignore & poly.m_flags) {
            continue;
        }

        uint16_t ignoreMask = (poly.m_materialId == 0xFF || !this->m_materials[poly.m_materialId].m_blendMode) ? SMOPoly::IGNORE_OPAQUE : SMOPoly::IGNORE_BLENDED;

        if (ignore & ignoreMask) {
            continue;
        }

        if (BspQuery::testFaceSub >= BspQuery::MAX_FACES) {
            if (this->m_overflowFlags) {
                *this->m_overflowFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
            }

            break;
        }

        BspQuery::testFaces[BspQuery::testFaceSub++] = face;
        poly.m_flags |= SMOPoly::F_COLLIDE_HIT;

        const uint16_t* verts = &digest->m_faceVertexIndices[3 * i];

        if (outcodes[verts[0]] & outcodes[verts[1]] & outcodes[verts[2]] & 0x3F) {
            continue;
        }

        float t = 0.0f;

        if (!NTempest::Intersect(this->m_ray, digest->m_vertices, verts, &t, nullptr, 0.002f)) {
            continue;
        }

        if (t < 0.0f || t > this->m_maxT) {
            continue;
        }

        this->m_maxT = t;
        BspQuery::hitFaces[0] = face;
        BspQuery::hitFaceSub = 1;
        *this->m_hitT = t * this->m_oosegMag;

        if (*this->m_hitT > this->m_origHitT) {
            *this->m_hitT = this->m_origHitT;
        }
    }

    return true;
}

// VERIFY: e3cbf054 manual
// OFFSET: 0x007C75E0
void CMapObjGroup::AsyncFailedCallback(CAsyncObject* object) {
    void* buffer = object->buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// VERIFY: 1ac7bf30 manual
// OFFSET: 0x007C7610
BspQuery::~BspQuery() {
    while (BspQuery::testFaceSub) {
        BspQuery::testFaceSub--;
        this->m_faces[BspQuery::testFaces[BspQuery::testFaceSub]].m_flags &= ~SMOPoly::F_COLLIDE_HIT;
    }

    BspQuery::hitFaceSub = 0;
    BspQuery::testFaceSub = 0;
}

// VERIFY: 8fcd1fbd auto
// OFFSET: 0x007C7710
World::TriData::Batch* __stdcall World::TriData::AllocBatch(uint32_t indexCount, uint32_t faceCount) {
    if (World::TriData::nBatches + 1 >= WORLD_TRI_POOL_LIMITS::TRI_QUERY_BATCH_CAPACITY || indexCount + World::TriData::indexCursor >= WORLD_TRI_POOL_LIMITS::TRI_QUERY_INDEX_CAPACITY || faceCount + World::TriData::faceIndexCursor >= WORLD_TRI_POOL_LIMITS::TRI_QUERY_FACE_INDEX_CAPACITY) {
        World::TriData::statusFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        return nullptr;
    }

    Batch* batch = &World::TriData::batches[World::TriData::nBatches++];
    batch->m_matrix = nullptr;
    batch->vertices = nullptr;
    batch->m_normalList = nullptr;
    batch->heights = nullptr;
    batch->vertexIndices = nullptr;
    batch->m_faceIndices = nullptr;
    batch->indexCount = 0;
    batch->m_faceCount = 0;
    batch->m_minVertexIndex = 0;
    batch->m_maxVertexIndex = 0;
    batch->m_def = nullptr;
    batch->m_minVertexIndex = 0xFFFF;

    return batch;
}

// VERIFY: 4f76965b auto
// OFFSET: 0x007C7790
static uint8_t CullLiquidVertexZ(const CAaBox& box, const C3Vector& vertex) {
    float below = vertex.z - box.b.z + 0.019444443f;
    float above = box.t.z - vertex.z + 0.019444443f;
    return ((*reinterpret_cast<uint32_t*>(&below) >> 29) & 0x4) | ((*reinterpret_cast<uint32_t*>(&above) >> 26) & 0x20);
}

// VERIFY: 1bcdfc19 auto
// OFFSET: 0x007C78E0
BspQuery_Segment::BspQuery_Segment(SMOPoly* faces, C3Vector* vertexList, uint16_t* indices, const C3Segment& seg, float* hitT, uint16_t faceIgnoreFlags, SMOMaterial* materials) {
    STORM_ASSERT(hitT);

    this->m_faces = faces;
    this->m_vertexList = vertexList;
    this->m_indices = indices;
    this->m_overflowFlags = nullptr;
    this->m_hitT = hitT;
    this->m_seg.start = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_seg.end = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_ray.origin = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_ray.dir = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_materials = materials;
    this->m_faceIgnoreFlags = faceIgnoreFlags | SMOPoly::F_COLLIDE_HIT;
    this->unused_0054 = CMap::s_useSSE;
    this->m_ray.origin = seg.start;
    this->m_ray.dir = C3Vector(seg.end.x - seg.start.x, seg.end.y - seg.start.y, seg.end.z - seg.start.z);

    float length = sqrtf(this->m_ray.dir.z * this->m_ray.dir.z + this->m_ray.dir.y * this->m_ray.dir.y + this->m_ray.dir.x * this->m_ray.dir.x);
    this->m_maxT = *hitT * length;
    this->m_seg = seg;
    this->m_origHitT = *hitT;

    float inv = 1.0f / length;
    this->m_oosegMag = inv;
    this->m_ray.dir.x = this->m_ray.dir.x * inv;
    this->m_ray.dir.y = this->m_ray.dir.y * inv;
    this->m_ray.dir.z = inv * this->m_ray.dir.z;
}

// VERIFY: c85c398d manual
// OFFSET: 0x007C7AE0
void CMapObjGroup::GetTrisFromQuery(World::TriData* triData, BspQuery& q, CMapObjDef* mapObjDef, uint32_t statusFlags) {
    if (CWorld::enables & Enables::Enable_CollisionDebug) {
        for (uint32_t i = 0; i < BspQuery::testFaceSub; i++) {
            uint32_t base = 3 * BspQuery::testFaces[i];
            C3Vector c = this->m_vertexList[this->m_indices[base + 2]] * mapObjDef->m_mat;
            C3Vector b = this->m_vertexList[this->m_indices[base + 1]] * mapObjDef->m_mat;
            C3Vector a = this->m_vertexList[this->m_indices[base]] * mapObjDef->m_mat;
            CFacet facet(a, b, c);
            CImVector color;
            *reinterpret_cast<uint32_t*>(&color) = 0x7FFF0000;
            CMap::TestQueryAdd(facet, color, nullptr);
        }

        for (uint32_t i = 0; i < BspQuery::hitFaceSub; i++) {
            uint32_t base = 3 * BspQuery::hitFaces[i];
            C3Vector c = this->m_vertexList[this->m_indices[base + 2]] * mapObjDef->m_mat;
            C3Vector b = this->m_vertexList[this->m_indices[base + 1]] * mapObjDef->m_mat;
            C3Vector a = this->m_vertexList[this->m_indices[base]] * mapObjDef->m_mat;
            CFacet facet(a, b, c);
            CImVector color;
            *reinterpret_cast<uint32_t*>(&color) = 0x7F00FF00;
            CMap::TestQueryAdd(facet, color, nullptr);
        }
    }

    World::TriData::statusFlags |= statusFlags;

    STORM_ASSERT(q.hitFaceSub <= BspQuery::MAX_FACES && "Too many triangles were queried in CMapObjGroup::GetTrisFromQuery()");

    uint32_t hitCount = BspQuery::hitFaceSub;

    if (!hitCount || hitCount > BspQuery::MAX_FACES) {
        return;
    }

    World::TriData::Batch* batch = World::TriData::AllocBatch(3 * hitCount, hitCount);

    if (!batch) {
        return;
    }

    batch->m_def = mapObjDef;

    uint16_t* indices;

    if (World::TriData::indexCursor + 3 * BspQuery::hitFaceSub >= WORLD_TRI_POOL_LIMITS::TRI_QUERY_INDEX_CAPACITY) {
        World::TriData::statusFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        indices = nullptr;
    } else {
        indices = &World::TriData::indexPool[World::TriData::indexCursor];
        World::TriData::indexCursor += 3 * BspQuery::hitFaceSub;
    }

    uint16_t* tris;

    if (World::TriData::faceIndexCursor + BspQuery::hitFaceSub >= WORLD_TRI_POOL_LIMITS::TRI_QUERY_FACE_INDEX_CAPACITY) {
        World::TriData::statusFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        tris = nullptr;
    } else {
        tris = &World::TriData::faceIndexPool[World::TriData::faceIndexCursor];
        World::TriData::faceIndexCursor += BspQuery::hitFaceSub;
    }

    batch->m_matrix = &mapObjDef->m_mat;
    batch->vertices = this->m_vertexList;
    batch->m_normalList = this->m_normalList;
    STORM_ASSERT(tris);
    batch->m_faceCount = BspQuery::hitFaceSub;
    STORM_ASSERT(indices);
    batch->indexCount = 3 * BspQuery::hitFaceSub;

    uint16_t* out = indices;

    for (uint32_t i = 0; i < BspQuery::hitFaceSub; i++) {
        uint16_t face = BspQuery::hitFaces[i];
        tris[i] = face;

        for (uint32_t j = 0; j < 3; j++) {
            uint16_t index = this->m_indices[3 * face + j];
            *out++ = index;

            if (index < batch->m_minVertexIndex) {
                batch->m_minVertexIndex = index;
            }

            if (index > batch->m_maxVertexIndex) {
                batch->m_maxVertexIndex = index;
            }
        }
    }

    batch->vertexIndices = indices;
    batch->m_faceIndices = tris;
}

// VERIFY: cdc665c6 auto
// OFFSET: 0x007C7F10
void CMapObjGroup::InitPtrs() {
    this->m_groupName = nullptr;
    this->m_polyList = nullptr;
    this->m_vertexList = nullptr;
    this->m_normalList = nullptr;
    this->m_textureVertexList = nullptr;
    this->m_textureVertexList2 = nullptr;
    this->m_indices = nullptr;
    this->m_stripIndices = nullptr;
    this->m_batchList = nullptr;
    this->m_stripBatches = nullptr;
    this->m_lightRefList = nullptr;
    this->m_doodadRefList = nullptr;
    this->cvertexList = nullptr;
    this->m_colorVertexListExtra = nullptr;
    this->m_liquidVertexList = nullptr;
    this->m_liquidTileList = nullptr;
    this->m_liquidVertArray = nullptr;
    this->m_portalStart = 0;
    this->m_portalCount = 0;
    this->unused_014C = 0;
    this->m_polyListSize = 0;
    this->m_vertexListCount = 0;
    this->m_normalListCount = 0;
    this->m_textureVertexListCount = 0;
    this->m_indicesCount = 0;
    this->m_stripIndexCount = 0;
    this->m_batchListCount = 0;
    this->m_lightRefListCount = 0;
    this->m_doodadRefListCount = 0;
    this->m_colorVertexListSize = 0;
    this->m_liquidVerts.x = 0;
    this->m_liquidVerts.y = 0;
    this->liquidTiles.x = 0;
    this->liquidTiles.y = 0;
}

// VERIFY: 88194c9d manual
// OFFSET: 0x007C8500
void VBBList::Block::Set(uint32_t itemSize, uint32_t itemCount, uint32_t offset) {
    STORM_ASSERT(this->bufPool==NULL);

    this->m_capacity = itemCount * itemSize;
    this->m_offset = offset;

    uint32_t index = offset;
    uint32_t remainder = offset - offset / itemSize * itemSize;

    if (remainder) {
        index = itemSize - remainder + offset;
    }

    GxBufBaseSet(this->buf, index);
    GxBufSizeSet(this->buf, itemSize, itemCount - 1);
    this->buf->m_hasData = 0;
}

// VERIFY: 79b8e8c1 manual
// OFFSET: 0x007C8560
void CMapObjGroup::FillVertexVB(CGxBuf* buf, EGxVertexBufferFormat format) {
    if (format == GxVBF_PNCT) {
        CGxVertexPNCT* vertex = reinterpret_cast<CGxVertexPNCT*>(g_theGxDevicePtr->BufLock(buf));

        if (this->cvertexList) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_vertexListCount); i++, vertex++) {
                vertex->m_position = this->m_vertexList[i];
                vertex->m_normal = this->m_normalList[i];
                CImVector& source = this->cvertexList[i];
                CImVector color;

                if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                    color.b = source.r;
                    color.g = source.g;
                    color.r = source.b;
                    color.a = source.a;
                } else {
                    color = source;
                }

                vertex->m_color = color;
                vertex->m_texture = this->m_textureVertexList[i];
            }
        } else {
            CImVector base;
            base.a = 0xFF;

            if (this->m_parent->m_header->m_flags & SMOHeader::FLAGS::UNIFIED_RENDER) {
                base.b = 0;
                base.g = 0;
                base.r = 0;
            } else {
                base.b = 0x7F;
                base.g = 0x7F;
                base.r = 0x7F;
            }

            for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_vertexListCount); i++, vertex++) {
                vertex->m_position = this->m_vertexList[i];
                vertex->m_normal = this->m_normalList[i];
                CImVector color;

                if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                    color.b = base.r;
                    color.g = base.g;
                    color.r = base.b;
                    color.a = 0xFF;
                } else {
                    color = base;
                }

                vertex->m_color = color;
                vertex->m_texture = this->m_textureVertexList[i];
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->m_hasData = 1;
        return;
    }

    if (format != GxVBF_PNC2T2) {
        return;
    }

    CGxVertexPNC2T2* vertex = reinterpret_cast<CGxVertexPNC2T2*>(g_theGxDevicePtr->BufLock(buf));

    if (this->cvertexList) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_vertexListCount); i++, vertex++) {
            vertex->m_position = this->m_vertexList[i];
            vertex->m_normal = this->m_normalList[i];
            CImVector& source = this->cvertexList[i];
            CImVector color0;

            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                color0.b = source.r;
                color0.g = source.g;
                color0.r = source.b;
                color0.a = source.a;
            } else {
                color0 = source;
            }

            vertex->m_color[0] = color0;
            CImVector zero;
            *reinterpret_cast<uint32_t*>(&zero) = 0;
            CImVector& extra = this->m_colorVertexListExtra ? this->m_colorVertexListExtra[i] : zero;
            CImVector color1;

            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                color1.b = extra.r;
                color1.g = extra.g;
                color1.r = extra.b;
                color1.a = extra.a;
            } else {
                color1 = extra;
            }

            vertex->m_color[1] = color1;
            vertex->m_texture[0] = this->m_textureVertexList[i];

            C2Vector noTexture(0.0f, 0.0f);
            C2Vector& texture1 = this->m_textureVertexList2 ? this->m_textureVertexList2[i] : noTexture;
            vertex->m_texture[1].x = texture1.x;
            vertex->m_texture[1].y = texture1.y;
        }
    } else {
        CImVector base;
        base.a = 0xFF;

        if (this->m_parent->m_header->m_flags & SMOHeader::FLAGS::UNIFIED_RENDER) {
            base.b = 0;
            base.g = 0;
            base.r = 0;
        } else {
            base.b = 0x7F;
            base.g = 0x7F;
            base.r = 0x7F;
        }

        for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_vertexListCount); i++, vertex++) {
            vertex->m_position = this->m_vertexList[i];
            vertex->m_normal = this->m_normalList[i];
            CImVector color0;

            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                color0.b = base.r;
                color0.g = base.g;
                color0.r = base.b;
                color0.a = 0xFF;
            } else {
                color0 = base;
            }

            vertex->m_color[0] = color0;
            CImVector zero;
            *reinterpret_cast<uint32_t*>(&zero) = 0;
            CImVector& extra = this->m_colorVertexListExtra ? this->m_colorVertexListExtra[i] : zero;
            CImVector color1;

            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                color1.b = extra.r;
                color1.g = extra.g;
                color1.r = extra.b;
                color1.a = extra.a;
            } else {
                color1 = extra;
            }

            vertex->m_color[1] = color1;
            vertex->m_texture[0] = this->m_textureVertexList[i];

            C2Vector noTexture(0.0f, 0.0f);
            C2Vector& texture1 = this->m_textureVertexList2 ? this->m_textureVertexList2[i] : noTexture;
            vertex->m_texture[1].x = texture1.x;
            vertex->m_texture[1].y = texture1.y;
        }
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
}

// VERIFY: 745d4cbb manual
// OFFSET: 0x007C8A70
void CMapObjGroup::FillTransparencyVB(CGxBuf* buf) {
    STORM_ASSERT(this->cvertexList);
    uint32_t count = this->m_batchList[this->transBatchCount - 1].m_vertexEnd + 1;
    CGxVertexPNCT* vertex = reinterpret_cast<CGxVertexPNCT*>(g_theGxDevicePtr->BufLock(buf));

    for (uint32_t i = 0; i < count; i++, vertex++) {
        vertex->m_position = this->m_vertexList[i];
        vertex->m_normal = this->m_normalList[i];

        CImVector gray;
        gray.b = 0x7F;
        gray.g = 0x7F;
        gray.r = 0x7F;
        gray.a = this->cvertexList[i].a;
        CImVector color;

        if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
            color.b = 0x7F;
            color.g = 0x7F;
            color.r = 0x7F;
            color.a = gray.a;
        } else {
            color = gray;
        }

        vertex->m_color = color;
        vertex->m_texture = this->m_textureVertexList[i];
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
}

// VERIFY: 58db9069 auto
// OFFSET: 0x007C8B90
void CMapObjGroup::UploadIndexBuffer(CGxBuf* buf) {
    char* data = g_theGxDevicePtr->BufLock(buf);
    memcpy(data, this->m_indices, sizeof(uint16_t) * this->m_indicesCount);
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
}

// VERIFY: 12c31afd auto
// OFFSET: 0x007C8D80
int32_t CMapObjGroup::FindLiquidType() const {
    int32_t count = this->liquidTiles.x * this->liquidTiles.y;

    for (int32_t i = 0; i < count; i++) {
        uint8_t type = this->m_liquidTileList[i].m_flags & SMOLTile::LIQUID_TYPE_MASK;

        if (type != SMOLTile::LIQUID_TYPE_NONE) {
            return type + 1;
        }
    }

    return 0;
}

// VERIFY: f0e93fe5 auto
// OFFSET: 0x007C9BC0
void CMapObjGroup::Init() {
    this->m_runtimeFlags &= ~CMapObjGroup::RUNTIME_FLAGS::RUNTIME_STATE_MASK;
    this->unused_0190 = 0.0f;
    this->m_flags = 0;
    this->m_bbox.b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_bbox.t = C3Vector(0.0f, 0.0f, 0.0f);
    this->unused_00D0 = 0;
    this->m_minimapTag = 0;
    this->rawData = nullptr;
    this->m_fileSize = 0;
    this->m_parent = nullptr;
    this->asyncObject = nullptr;
    this->m_portalStart = 0;
    this->m_portalCount = 0;
    for (uint32_t i = 0; i < sizeof(this->m_fogs) / sizeof(this->m_fogs[0]); i++) {
        this->m_fogs[i] = 0;
    }
    this->m_timer = 0.0f;
    this->m_liquidVerts.y = 0;
    this->m_distToCamera = FLT_MAX;
    this->m_liquidCorner.x = 0.0f;
    this->m_liquidCorner.y = 0.0f;
    this->m_liquidVerts.x = 0;
    this->m_liquidCorner.z = 0.0f;
    this->m_liquidMaterialId = 0;
    this->m_vertsBlock = nullptr;
    this->m_transparencyVertsBlock = nullptr;
    this->m_indicesBlock = nullptr;
    this->m_liquidVertsBlock = nullptr;
    this->m_liquidIndicesBlock = nullptr;
    this->m_liquidVBRefCount = 0;
    this->m_liquidType = 0;
    this->m_numSharedTiles = -1;
    this->InitPtrs();
}

// VERIFY: 8696dab8 manual
// OFFSET: 0x007C9CB0
void CMapObjGroup::SetVertexVB() {
    EGxVertexBufferFormat format = GxVBF_PNCT;
    uint32_t vertexSize = sizeof(CGxVertexPNCT);

    if (this->m_runtimeFlags & CMapObjGroup::RUNTIME_FLAGS::EXTENDED_VERTEX_FORMAT) {
        format = GxVBF_PNC2T2;
        vertexSize = sizeof(CGxVertexPNC2T2);
    }

    CGxBuf* buf = this->m_vertsBlock ? this->m_vertsBlock->buf : g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, vertexSize, this->m_vertexListCount);

    if (!buf->m_hasData || !buf->m_valid) {
        this->FillVertexVB(buf, format);
    }

    GxPrimVertexPtr(buf, format);
}

// VERIFY: a3616c6f auto
// OFFSET: 0x007C9D20
void CMapObjGroup::SetTransparencyVB() {
    CGxBuf* buf = this->m_transparencyVertsBlock ? this->m_transparencyVertsBlock->buf : g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPNCT), this->m_batchList[this->transBatchCount - 1].m_vertexEnd + 1);

    if (!buf->m_hasData || !buf->m_valid) {
        this->FillTransparencyVB(buf);
    }

    GxPrimVertexPtr(buf, GxVBF_PNCT);
}

// VERIFY: 4dd1515d auto
// OFFSET: 0x007C9D80
void CMapObjGroup::SetIndexVB() {
    CGxBuf* buf = this->m_indicesBlock ? this->m_indicesBlock->buf : g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), this->m_indicesCount);

    if (!buf->m_hasData || !buf->m_valid) {
        this->UploadIndexBuffer(buf);
    }

    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// VERIFY: 13d55b84 manual
// OFFSET: 0x007CB0C0
bool CMapObjGroup::GetTris(const C3Segment& seg, float* dist, uint32_t flags, uint16_t faceIgnoreFlags, World::TriData* triData, CMapObjDef* mapObjDef) {
    SMOMaterial* materials = this->m_parent->materialList;
    uint32_t batchCount = World::TriData::nBatches;

    BspQuery_Segment query(this->m_polyList, this->m_vertexList, this->m_indices, seg, dist, faceIgnoreFlags, materials);
    CAaBsp_Query_Segment<BspQuery_Segment> bspQuery;
    bspQuery.m_aaBsp = &this->m_aaBsp;
    bspQuery.m_f = &query;
    bspQuery.GetFaceIndices(0, seg, this->m_aaBsp.m_aaBox);
    this->GetTrisFromQuery(triData, query, mapObjDef, WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_NONE);

        if ((flags & WORLD_QUERY_LIQUID_MASK) && (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIQUID)) {
        this->VectorIntersectLiquid(seg, dist, flags, triData, mapObjDef);
    }

    return World::TriData::nBatches != batchCount;
}

// VERIFY: 784b1f56 manual
// OFFSET: 0x007CB3B0
VBBList::Block* VBBList::AllocBlock() {
    if (this->singlePool) {
        VBBList::Block* block = this->unk_0010.Head();

        if (!block) {
            void* mem = SMemAlloc(sizeof(VBBList::Block), __FILE__, __LINE__, 0x8);
            block = mem ? new (mem) VBBList::Block : nullptr;
            this->unk_0010.LinkToTail(block);
            block->buf = g_theGxDevicePtr->BufCreate(this->pool, 0, 0, 0);
        }

        block->m_link.Unlink();
        block->clientPtr = nullptr;
        block->unused_0008 = 0.0f;
        block->bufPool = nullptr;
        return block;
    }

    void* mem = SMemAlloc(sizeof(VBBList::Block), __FILE__, __LINE__, 0x8);
    VBBList::Block* block = mem ? new (mem) VBBList::Block : nullptr;
    this->unk_001C.LinkToTail(block);
    block->unused_0008 = 0.0f;
    block->m_offset = 0;
    block->m_capacity = 0;
    block->clientPtr = nullptr;
    block->buf = nullptr;
    block->bufPool = nullptr;
    return block;
}

// VERIFY: 0a4b9b8f manual
// OFFSET: 0x007CB520
void CMapObjGroup::AllocVertArray(CMapObjGroup::VertArray** vertArray, uint32_t count) {
    *vertArray = CMapObjGroup::s_freeVertArrays.Head();

    if (!*vertArray) {
        void* mem = SMemAlloc(sizeof(CMapObjGroup::VertArray), __FILE__, __LINE__, 0x8);
        CMapObjGroup::VertArray* created = mem ? new (mem) CMapObjGroup::VertArray() : nullptr;
        CMapObjGroup::s_freeVertArrays.LinkToTail(created);
        *vertArray = created;
    }

    (*vertArray)->m_link.Unlink();
    (*vertArray)->m_verts.SetCount(count);
}

// VERIFY: 0ecccc4f manual
// OFFSET: 0x007CB700
CMapObjGroup::CMapObjGroup() {
    this->m_liquidBatch.m_primType = static_cast<EGxPrim>(6);
    this->m_liquidBatch.m_start = 0;
    this->m_liquidBatch.m_count = 0;
    this->m_liquidBatch.m_minIndex = 0;
    this->m_liquidBatch.m_maxIndex = 0;
    this->m_bbox.b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_bbox.t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_liquidVerts.x = 0;
    this->m_liquidVerts.y = 0;
    this->liquidTiles.x = 0;
    this->liquidTiles.y = 0;
    this->m_liquidCorner = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_liquidHeight = 0.0f;
    this->m_liquidMaxHeight = 0.0f;
    this->Init();
}

// VERIFY: 97fda949 auto
// OFFSET: 0x007CB990
void CMapObjGroup::Initialize() {
    CMapObjGroup::vertexVBList.singlePool = 0;
    CMapObjGroup::vertexVBList.m_target = GxPoolTarget_Vertex;
    CMapObjGroup::vertexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::s_worldPoolUsage);
    CMapObjGroup::vertexVBList.pool = nullptr;
    CMapObjGroup::indexVBList.singlePool = 0;
    CMapObjGroup::indexVBList.m_target = GxPoolTarget_Index;
    CMapObjGroup::indexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::s_worldPoolUsage);
    CMapObjGroup::indexVBList.pool = nullptr;
}

// VERIFY: 6dcd01f3 manual
// OFFSET: 0x007CB870
void VBBList::Destroy() {
    VBBList::Block* vbb = this->unk_001C.Head();
    uint32_t count = 0;

    while (!this->unk_001C.IsEnd(vbb)) {
        VBBList::Block* next = this->unk_001C.RawNext(vbb);
        CGxBuf* buffer = vbb->buf;

        if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buffer);
        }

        vbb->buf = nullptr;

        if (!this->singlePool) {
            STORM_ASSERT(vbb->bufPool != NULL);

            if (vbb->bufPool) {
                g_theGxDevicePtr->PoolDestroy(vbb->bufPool);
            }

            vbb->bufPool = nullptr;
        }

        this->unk_001C.DeleteNode(vbb);
        count++;
        vbb = next;
    }

    if (this->singlePool) {
        STORM_ASSERT(count == 1);

        vbb = this->unk_0010.Head();

        while (!this->unk_0010.IsEnd(vbb)) {
            VBBList::Block* next = this->unk_0010.RawNext(vbb);
            CGxBuf* buffer = vbb->buf;

            if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
                g_theGxDevicePtr->BufDestroy(buffer);
            }

            vbb->buf = nullptr;
            this->unk_0010.DeleteNode(vbb);
            vbb = next;
        }

        if (this->singlePool && this->pool) {
            g_theGxDevicePtr->PoolDestroy(this->pool);
        }
    }
}

// VERIFY: f8f8468a auto
// OFFSET: 0x007CB9D0
void CMapObjGroup::Destroy() {
    CMapObjGroup::vertexVBList.Destroy();
    CMapObjGroup::indexVBList.Destroy();
    CMapObjGroup::s_freeVertArrays.Clear();
}




















// VERIFY: af527478 manual
// OFFSET: 0x007C8360
bool CMapObjGroup::QueryLiquid(const C3Vector& pos, int32_t* liquidType, float* level) {
    int32_t type = this->m_liquidType;

    if (!type) {
        return false;
    }

    if (!this->liquidTiles.x || !this->liquidTiles.y) {
        *liquidType = type;
        *level = FLT_MAX;
        return true;
    }

    float fx = (pos.x - this->m_liquidCorner.x) * OO_COORD_TO_SUBCHUNK;
    float fy = (pos.y - this->m_liquidCorner.y) * OO_COORD_TO_SUBCHUNK;

    C2iVector tile;
    tile.x = static_cast<int32_t>(floor(fx));
    tile.y = static_cast<int32_t>(floor(fy));

    if (tile.x < 0 || tile.y < 0 || !(tile < this->liquidTiles)) {
        return false;
    }

    if ((this->m_liquidTileList[tile.y * this->liquidTiles.x + tile.x].m_flags & SMOLTile::LIQUID_TYPE_MASK) == SMOLTile::LIQUID_TYPE_NONE) {
        return false;
    }

    float dx = fx - tile.x;
    float dy = fy - tile.y;
    int32_t v0 = tile.y * this->m_liquidVerts.x + tile.x;
    int32_t v1 = this->m_liquidVerts.x + v0;
    float offset = (g_liquidTypeDB.GetRecord(type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING) ? 0.01f : 0.0f;

    const SMOLVert* verts = this->m_liquidVertexList;
    float h0 = verts[v0].m_waterVert.m_height;
    float top = h0 + (verts[v0 + 1].m_waterVert.m_height - h0) * dx;
    float h2 = verts[v1].m_waterVert.m_height;
    float bottom = h2 + (verts[v1 + 1].m_waterVert.m_height - h2) * dx;
    float height = top + (bottom - top) * dy;

    if (offset + height <= pos.z) {
        return false;
    }

    *level = height;
    *liquidType = this->m_liquidType;
    return true;
}

// VERIFY: 0f477396 auto
// OFFSET: 0x007C8BF0
uint32_t CMapObjGroup::GetNumSharedTiles() {
    if (this->m_numSharedTiles > 0) {
        return this->m_numSharedTiles;
    }

    int32_t count = 0;
    const SMOLTile* tile = this->m_liquidTileList;

    for (int32_t y = 0; y < this->liquidTiles.y; y++) {
        for (int32_t x = 0; x < this->liquidTiles.x; x++) {
            if ((tile->m_flags & SMOLTile::LIQUID_TYPE_MASK) != SMOLTile::LIQUID_TYPE_NONE && (tile->m_flags & SMOLTile::SHARED_MASK)) {
                count++;
            }

            tile++;
        }
    }

    this->m_numSharedTiles = count;
    return count;
}

// VERIFY: e95ab74c manual
// OFFSET: 0x007C8C60
void CMapObjGroup::GenLiquidVerts() {
    float x = this->m_liquidCorner.x;
    float y = this->m_liquidCorner.y;
    C3Vector* out = this->m_liquidVertArray->m_verts.m_data;
    const SMOLVert* vert = this->m_liquidVertexList;

    this->m_liquidHeight = vert->m_waterVert.m_height;
    this->m_liquidMaxHeight = vert->m_waterVert.m_height;

    for (int32_t row = 0; row < this->m_liquidVerts.y; row++) {
        for (int32_t col = 0; col < this->m_liquidVerts.x; col++) {
            float height = vert->m_waterVert.m_height;
            out->x = x;
            out->y = y;
            out->z = height;
            out++;

            this->m_liquidHeight = height <= this->m_liquidHeight ? height : this->m_liquidHeight;
            this->m_liquidMaxHeight = height < this->m_liquidMaxHeight ? this->m_liquidMaxHeight : height;

            vert++;
            x += SMOLTILE_SIZE;
        }

        x = this->m_liquidCorner.x;
        y += SMOLTILE_SIZE;
    }
}

// VERIFY: 800e13fc manual
// OFFSET: 0x007CB9F0
void VBBList::FreeVBB(VBBList::Block* block) {
    if (!block) {
        return;
    }

    if (this->singlePool) {
        VBBList::Block* prev = block->m_link.Prev();

        if (prev && prev->clientPtr) {
            prev = nullptr;
        }

        VBBList::Block* next = block->m_link.Next();

        if (next && next->clientPtr) {
            next = nullptr;
        }

        uint32_t offset = block->m_offset;
        uint32_t capacity = block->m_capacity;

        if (prev) {
            offset = prev->m_offset;
            capacity += prev->m_capacity;
            prev->m_link.Unlink();
            this->unk_0010.LinkToTail(prev);
        }

        if (next) {
            capacity += next->m_capacity;
            next->m_link.Unlink();
            this->unk_0010.LinkToTail(next);
        }

        block->Set(capacity, 1, offset);
        *block->clientPtr = nullptr;
        block->unused_0008 = 0.0f;
        block->clientPtr = nullptr;
    } else {
        if (block->buf) {
            GxBufDestroy(block->buf);
            block->buf = nullptr;
        }

        if (block->bufPool) {
            g_theGxDevicePtr->PoolDestroy(block->bufPool);
            block->bufPool = nullptr;
        }

        *block->clientPtr = nullptr;
        this->unk_001C.DeleteNode(block);
    }

    if (this->singlePool) {
        uint32_t used = 0;
        uint32_t total = 0;

        for (VBBList::Block* node = this->unk_001C.Head(); node; node = node->m_link.Next()) {
            used += node->m_capacity;
            total += node->m_capacity;
        }
    }
}

// VERIFY: 371fd0ef manual
// OFFSET: 0x007CBB30
void VBBList::AssignBlock(VBBList::Block* block, VBBList::Block** slot, uint32_t itemSize, uint32_t itemCount) {
    STORM_ASSERT(singlePool);

    VBBList::Block* rest = this->AllocBlock();
    this->unk_001C.LinkNode(rest, TSLIST_LINK_AFTER, block);
    rest->Set(1, block->m_capacity - itemSize * (itemCount + 1), itemSize * (itemCount + 1) + block->m_offset);
    block->Set(itemSize, itemCount + 1, block->m_offset);
    block->clientPtr = slot;
    *slot = block;
}

// VERIFY: c44ec26d auto
// OFFSET: 0x007CBBC0
void VBBList::AllocVBB(VBBList::Block** slot, uint32_t itemSize, uint32_t itemCount) {
    if (this->singlePool) {
        STORM_ASSERT(pool);

        uint32_t size = itemSize * (itemCount + 1);

        while (true) {
            VBBList::Block* smallest = nullptr;

            for (VBBList::Block* block = this->unk_001C.Head(); block; block = this->unk_001C.Next(block)) {
                if (block->clientPtr) {
                    if (!smallest || block->m_capacity < smallest->m_capacity) {
                        smallest = block;
                    }

                    continue;
                }

                if (size < block->m_capacity) {
                    this->AssignBlock(block, slot, itemSize, itemCount);
                    return;
                }
            }

            STORM_ASSERT(smallest);

            if (size < smallest->m_capacity) {
                return;
            }

            this->FreeVBB(smallest);
        }
    }

    VBBList::Block* nbb = this->AllocBlock();
    const char* name = this->m_target ? "VBBList_idx" : "VBBList_vtx";
    uint32_t size = itemSize * (itemCount + 1);
    nbb->bufPool = g_theGxDevicePtr->PoolCreate(this->m_target, this->m_usage, size + 4096, GxPoolHintBit_Unk3, name);
    STORM_ASSERT(nbb->bufPool != NULL);
    nbb->buf = GxBufCreate(nbb->bufPool, itemSize, itemCount, 0);
    STORM_ASSERT(nbb->buf != NULL);
    nbb->m_offset = 0;
    nbb->m_capacity = size;
    nbb->clientPtr = slot;
    *slot = nbb;
}

// VERIFY: 6752260a manual
// OFFSET: 0x007CBCB0
void CMapObjGroup::AllocVB() {
    if (!this->m_vertsBlock) {
        uint32_t vertexSize = (this->m_runtimeFlags & CMapObjGroup::RUNTIME_FLAGS::EXTENDED_VERTEX_FORMAT) ? sizeof(CGxVertexPNC2T2) : sizeof(CGxVertexPNCT);
        CMapObjGroup::vertexVBList.AllocVBB(&this->m_vertsBlock, vertexSize, this->m_vertexListCount);
    }

    if (!CShaderEffect::s_enableShaders && (this->m_parent->m_header->m_flags & SMOHeader::FLAGS::UNIFIED_RENDER) && this->cvertexList && this->transBatchCount && !this->m_transparencyVertsBlock) {
        uint32_t count = this->m_batchList[this->transBatchCount - 1].m_vertexEnd + 1;
        CMapObjGroup::vertexVBList.AllocVBB(&this->m_transparencyVertsBlock, GxVertexSize(GxVBF_PNCT), count);
    }

    if (!this->m_indicesBlock) {
        CMapObjGroup::indexVBList.AllocVBB(&this->m_indicesBlock, sizeof(uint16_t), this->m_indicesCount);
    }
}

// VERIFY: 6de6bbad auto
// OFFSET: 0x007CBD70
void CMapObjGroup::FreeVB() {
    if (this->m_vertsBlock) {
        CMapObjGroup::vertexVBList.FreeVBB(this->m_vertsBlock);
    }

    if (this->m_transparencyVertsBlock) {
        CMapObjGroup::vertexVBList.FreeVBB(this->m_transparencyVertsBlock);
    }

    if (this->m_indicesBlock) {
        CMapObjGroup::indexVBList.FreeVBB(this->m_indicesBlock);
    }

    this->m_vertsBlock = nullptr;
    this->m_transparencyVertsBlock = nullptr;
    this->m_indicesBlock = nullptr;
}

// VERIFY: 5304b11e auto
// OFFSET: 0x007CBDC0
void CMapObjGroup::AllocLiquidVB(EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount, CGxBatch** batch) {
    if (!this->m_liquidVertsBlock) {
        CMapObjGroup::vertexVBList.AllocVBB(&this->m_liquidVertsBlock, GxVertexSize(format), vertexCount);
        this->m_liquidVertsBlock->buf->m_hasData = 0;
    }

    if (!this->m_liquidIndicesBlock) {
        CMapObjGroup::indexVBList.AllocVBB(&this->m_liquidIndicesBlock, 2, indexCount);
        this->m_liquidIndicesBlock->buf->m_hasData = 0;
    }

    *batch = &this->m_liquidBatch;
    this->m_liquidVBRefCount++;
}

// VERIFY: cd2471f5 auto
// OFFSET: 0x007CBE30
void CMapObjGroup::FreeLiquidVB() {
    if (!this->m_liquidVBRefCount) {
        return;
    }

    if (--this->m_liquidVBRefCount) {
        return;
    }

    if (this->m_liquidVertsBlock) {
        CMapObjGroup::vertexVBList.FreeVBB(this->m_liquidVertsBlock);
    }

    if (this->m_liquidIndicesBlock) {
        CMapObjGroup::indexVBList.FreeVBB(this->m_liquidIndicesBlock);
    }

    this->m_liquidVertsBlock = nullptr;
    this->m_liquidIndicesBlock = nullptr;
}

// VERIFY: 200c714f manual
// OFFSET: 0x007CBE80
void CMapObjGroup::Clear() {
    int32_t freeFile = 1;

    if (this->asyncObject) {
        freeFile = AsyncFile::Cancel(this->asyncObject, CMapObjGroup::AsyncFailedCallback);
        this->asyncObject = nullptr;
    }

    this->m_aaBsp.Clear();
    this->FreeVB();

    if (this->m_liquidVertArray) {
        CMapObjGroup::s_freeVertArrays.LinkToHead(this->m_liquidVertArray);
        this->m_liquidVertArray = nullptr;
    }

    this->InitPtrs();

    if (this->rawData) {
        if (freeFile) {
            SMemFree(this->rawData, __FILE__, __LINE__, 0);
        }

        this->rawData = nullptr;
    }

    CMapObjOccluder* occluder = this->m_occluderList.Head();

    while (occluder) {
        CMapObjOccluder* next = occluder->m_link.Next();
        CMapObj::FreeOccluder(occluder);
        occluder = next;
    }

    this->m_runtimeFlags &= ~CMapObjGroup::RUNTIME_FLAGS::LOADED;
}

// VERIFY: ed59eece manual
// OFFSET: 0x007CBF40
CMapObjGroup::~CMapObjGroup() {
    this->Clear();
}

// VERIFY: 0c68f770 auto
// OFFSET: 0x007D7310
int32_t CMapObjGroup::GetLiquidType(uint32_t groupLiquid) const {
    if (groupLiquid < NUM_BASIC_LIQUID_TYPES && groupLiquid) {
        switch ((groupLiquid - 1) & 3) {
            case LIQUID_BASIC_TYPE_WATER:
                return (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_USE_ALTERNATE_LIQUID_TYPE) ? LIQUID_TYPE_WMO_OCEAN : LIQUID_TYPE_WMO_WATER;

            case LIQUID_BASIC_TYPE_OCEAN:
                return LIQUID_TYPE_WMO_OCEAN;

            case LIQUID_BASIC_TYPE_MAGMA:
                return LIQUID_TYPE_WMO_MAGMA;

            case LIQUID_BASIC_TYPE_SLIME:
                return LIQUID_TYPE_WMO_SLIME;
        }
    }

    return groupLiquid;
}

// INLINED
// Name from the 12213 assertions; rounds the way the liquid tile walk does (inferred).
static inline int32_t Fast_ftol(float f) {
    return CMath::fint_(f - 0.5f);
}

// VERIFY: c9184804 manual
// OFFSET: 0x007C9110
void CMapObjGroup::VectorIntersectLiquidSX(const C3Vector& startPoint, const C3Vector& endPoint, uint32_t& numTiles, uint8_t* tiles, uint32_t maxTiles) {
    int32_t tileX = Fast_ftol(startPoint.x);
    int32_t tileY = Fast_ftol(startPoint.y);

    STORM_ASSERT(Fast_ftol(endPoint.y)==tileY);

    int32_t edgeX = Fast_ftol(endPoint.x);
    int32_t step = edgeX >= tileX ? 1 : -1;
    edgeX += step;

    while (tileX != edgeX) {
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileX;
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileY;
        tileX += step;
    }
}

// VERIFY: 4f16d5fa manual
// OFFSET: 0x007C91A0
void CMapObjGroup::VectorIntersectLiquidSY(const C3Vector& startPoint, const C3Vector& endPoint, uint32_t& numTiles, uint8_t* tiles, uint32_t maxTiles) {
    int32_t tileX = Fast_ftol(startPoint.x);
    int32_t tileY = Fast_ftol(startPoint.y);

    STORM_ASSERT(Fast_ftol(endPoint.x)==tileX);

    int32_t edgeY = Fast_ftol(endPoint.y);
    int32_t step = edgeY >= tileY ? 1 : -1;
    edgeY += step;

    while (tileY != edgeY) {
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileX;
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileY;
        tileY += step;
    }
}

// VERIFY: f3f15286 manual
// OFFSET: 0x007C9230
void CMapObjGroup::VectorIntersectLiquidDX(const C3Vector& startPoint, const C3Vector& endPoint, uint32_t& numTiles, uint8_t* tiles, uint32_t maxTiles) {
    int32_t tileX = Fast_ftol(startPoint.x);
    int32_t tileY = Fast_ftol(startPoint.y);
    int32_t step;
    float frac;

    if (startPoint.x > endPoint.x) {
        step = -1;
        frac = static_cast<float>(tileX + 1) - startPoint.x;
    } else {
        step = 1;
        frac = startPoint.x - static_cast<float>(tileX);
    }

    float slope = (endPoint.y - startPoint.y) / fabs(endPoint.x - startPoint.x);
    float startY = startPoint.y - frac * slope;
    float offset = 0.0f;
    int32_t edgeX = Fast_ftol(endPoint.x) + step;
    int32_t prevY = tileY;

    if (tileX != edgeX) {
        do {
            if (tileY != prevY) {
                STORM_ASSERT(numTiles<maxTiles);
                tiles[numTiles++] = tileX;
                STORM_ASSERT(numTiles<maxTiles);
                tiles[numTiles++] = prevY;
            }

            offset += slope;
            STORM_ASSERT(numTiles<maxTiles);
            tiles[numTiles++] = tileX;
            STORM_ASSERT(numTiles<maxTiles);
            tiles[numTiles++] = tileY;
            tileX += step;
            prevY = tileY;
            tileY = Fast_ftol(offset + startY);
        } while (tileX != edgeX);
    }

    if (tileY != prevY && tileY >= 0 && tileY < this->liquidTiles.y) {
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = edgeX - step;
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileY;
    }

    STORM_ASSERT(edgeX-step == Fast_ftol(endPoint.x));
    STORM_ASSERT(prevY == Fast_ftol(endPoint.y) || tileY == Fast_ftol(endPoint.y));
}

// VERIFY: 3612548b manual
// OFFSET: 0x007C9370
void CMapObjGroup::VectorIntersectLiquidDY(const C3Vector& startPoint, const C3Vector& endPoint, uint32_t& numTiles, uint8_t* tiles, uint32_t maxTiles) {
    int32_t tileX = Fast_ftol(startPoint.x);
    int32_t tileY = Fast_ftol(startPoint.y);
    int32_t step;
    float frac;

    if (startPoint.y > endPoint.y) {
        step = -1;
        frac = static_cast<float>(tileY + 1) - startPoint.y;
    } else {
        step = 1;
        frac = startPoint.y - static_cast<float>(tileY);
    }

    float slope = (endPoint.x - startPoint.x) / fabs(endPoint.y - startPoint.y);
    float startX = startPoint.x - frac * slope;
    float offset = 0.0f;
    int32_t edgeY = Fast_ftol(endPoint.y) + step;
    int32_t prevX = tileX;

    if (tileY != edgeY) {
        do {
            if (tileX != prevX) {
                STORM_ASSERT(numTiles<maxTiles);
                tiles[numTiles++] = prevX;
                STORM_ASSERT(numTiles<maxTiles);
                tiles[numTiles++] = tileY;
            }

            offset += slope;
            STORM_ASSERT(numTiles<maxTiles);
            tiles[numTiles++] = tileX;
            STORM_ASSERT(numTiles<maxTiles);
            tiles[numTiles++] = tileY;
            tileY += step;
            prevX = tileX;
            tileX = Fast_ftol(offset + startX);
        } while (tileY != edgeY);
    }

    if (tileX != prevX && tileX >= 0 && tileX < this->liquidTiles.x) {
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = tileX;
        STORM_ASSERT(numTiles<maxTiles);
        tiles[numTiles++] = edgeY - step;
    }

    STORM_ASSERT(prevX == Fast_ftol(endPoint.x) || tileX == Fast_ftol(endPoint.x));
    STORM_ASSERT(edgeY-step == Fast_ftol(endPoint.y));
}

// VERIFY: 8f1675f7 manual
// OFFSET: 0x007C8DD0
bool CMapObjGroup::VectorIntersectLiquidTiles(const C3Segment& seg, float* t, uint32_t flags, uint32_t* tileCount, uint8_t* tiles, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->liquidTiles.x || !this->liquidTiles.y) {
        return false;
    }

    if ((flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID) && !(g_liquidTypeDB.GetRecord(type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
        return false;
    }

    float dx = seg.end.x - seg.start.x;
    float dy = seg.end.y - seg.start.y;
    float dz = seg.end.z - seg.start.z;
    float invLength = 1.0f / sqrt(dx * dx + dz * dz + dy * dy);

    C3Ray ray;
    ray.origin = seg.start;
    ray.dir.x = dx * invLength;
    ray.dir.y = dy * invLength;
    ray.dir.z = invLength * dz;

    C3Vector* verts = this->m_liquidVertArray->m_verts.m_data;
    uint32_t tris[2][3];
    tris[0][0] = 0;
    tris[0][1] = this->m_liquidVerts.x + 1;
    tris[0][2] = this->m_liquidVerts.x;
    tris[1][0] = 0;
    tris[1][1] = 1;
    tris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;

    for (uint32_t i = 0; i < *tileCount; i += 2) {
        uint32_t tileX = tiles[0];
        uint32_t tileY = tiles[1];

        STORM_ASSERT(tileX>=0 && tileX<liquidTiles.x && tileY>=0 && tileY<liquidTiles.y);

        if ((this->m_liquidTileList[tileY * this->liquidTiles.x + tileX].m_flags & SMOLTile::LIQUID_TYPE_MASK) != SMOLTile::LIQUID_TYPE_NONE) {
            uint32_t base = tileX + tileY * this->m_liquidVerts.x;

            for (uint32_t j = 0; j < 2; j++) {
                uint32_t triIndices[3];
                triIndices[0] = base + tris[j][0];
                triIndices[1] = base + tris[j][1];
                triIndices[2] = base + tris[j][2];

                float hitT = 1.0f;

                if (!NTempest::Intersect(ray, verts, triIndices, &hitT, nullptr, 0.01f)) {
                    continue;
                }

                hitT *= invLength;

                if (hitT >= *t || hitT < 0.0f) {
                    continue;
                }

                *t = hitT;

                if (!triData) {
                    found = true;
                    continue;
                }

                if (!found) {
                    batch = triData->AllocBatch();

                    if (batch) {
                        batch->m_def = mapObjDef;
                        batch->m_matrix = &mapObjDef->m_mat;
                        batch->vertices = this->m_liquidVertArray->m_verts.m_data;
                        indices = triData->AllocIndices(3);
                        batch->vertexIndices = indices;
                    }
                }

                found = true;

                if (!batch || !batch->vertexIndices) {
                    continue;
                }

                batch->indexCount = 0;

                for (uint32_t k = 0; k < 3; k++) {
                    uint16_t index = base + tris[j][k];
                    indices[batch->indexCount++] = index;
                    batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                    batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                }

                batch->m_faceCount++;
            }
        }

        tiles += 2;
    }

    return found;
}

// VERIFY: 4c425802 auto
// OFFSET: 0x007C9DD0
bool CMapObjGroup::VectorIntersectLiquid(const C3Segment& seg, float* t, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    float sizeY = this->liquidTiles.y * SMOLTILE_SIZE - 0.1f;
    sizeY = sizeY > 0.0f ? sizeY : 0.0f;
    float sizeX = SMOLTILE_SIZE * this->liquidTiles.x - 0.1f;
    sizeX = sizeX > 0.0f ? sizeX : 0.0f;

    CAaBox box;
    box.b.x = this->m_liquidCorner.x;
    box.b.y = this->m_liquidCorner.y;
    box.b.z = this->m_liquidHeight;
    box.t.x = sizeX + this->m_liquidCorner.x;
    box.t.y = sizeY + this->m_liquidCorner.y;
    box.t.z = this->m_liquidMaxHeight;

    C3Vector startPoint = seg.start;
    C3Vector endPoint = seg.end;
    C3Vector dir;
    dir.x = endPoint.x - startPoint.x;
    dir.y = endPoint.y - startPoint.y;
    dir.z = endPoint.z - startPoint.z;

    float lengthSq = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (lengthSq > 2.3841858e-7f) {
        float invLength = 1.0f / sqrt(lengthSq);
        dir.x *= invLength;
        dir.y *= invLength;
        dir.z *= invLength;
    }

    if (!box.ContainsPoint(startPoint)) {
        C3Ray ray;
        ray.origin = seg.start;
        ray.dir = dir;
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &startPoint)) {
            return false;
        }
    }

    if (!box.ContainsPoint(endPoint)) {
        C3Ray ray;
        ray.origin = endPoint;
        dir.x = -dir.x;
        dir.y = -dir.y;
        dir.z = -dir.z;
        ray.dir = dir;
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &endPoint)) {
            return false;
        }
    }

    uint32_t tileCount = 0;
    uint8_t tiles[0x800];

    startPoint.x = (startPoint.x - this->m_liquidCorner.x) * OO_COORD_TO_SUBCHUNK;
    startPoint.y = (startPoint.y - this->m_liquidCorner.y) * OO_COORD_TO_SUBCHUNK;
    startPoint.z = startPoint.z * OO_COORD_TO_SUBCHUNK;

    STORM_ASSERT(startPoint.x>=0.f && startPoint.x<liquidTiles.x && startPoint.y>=0.f && startPoint.y<liquidTiles.y);

    endPoint.x = (endPoint.x - this->m_liquidCorner.x) * OO_COORD_TO_SUBCHUNK;
    endPoint.y = (endPoint.y - this->m_liquidCorner.y) * OO_COORD_TO_SUBCHUNK;
    endPoint.z = OO_COORD_TO_SUBCHUNK * endPoint.z;

    STORM_ASSERT(endPoint.x>=0.f && endPoint.x<liquidTiles.x && endPoint.y>=0.f && endPoint.y<liquidTiles.y);

    float deltaX = endPoint.x - startPoint.x;
    float deltaY = fabs(endPoint.y - startPoint.y);

    STORM_ASSERT(liquidTiles.x<256 && liquidTiles.y<256);

    if (deltaY < 2.3841858e-7f) {
        this->VectorIntersectLiquidSX(startPoint, endPoint, tileCount, tiles, sizeof(tiles));
    } else if (fabs(deltaX) < 2.3841858e-7f) {
        this->VectorIntersectLiquidSY(startPoint, endPoint, tileCount, tiles, sizeof(tiles));
    } else if (deltaY >= fabs(deltaX)) {
        this->VectorIntersectLiquidDY(startPoint, endPoint, tileCount, tiles, sizeof(tiles));
    } else {
        this->VectorIntersectLiquidDX(startPoint, endPoint, tileCount, tiles, sizeof(tiles));
    }

    return this->VectorIntersectLiquidTiles(seg, t, flags, &tileCount, tiles, triData, mapObjDef);
}

// VERIFY: 2533e68d manual
// OFFSET: 0x007C94B0
bool CMapObjGroup::GetLiquidTris(const CAaBox& box, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->liquidTiles.x || !this->liquidTiles.y) {
        return false;
    }

    if ((flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID) && !(g_liquidTypeDB.GetRecord(type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
        return false;
    }

    C3Vector points[2];
    points[0].x = box.b.x - this->m_liquidCorner.x;
    points[0].y = box.b.y - this->m_liquidCorner.y;
    points[0].z = box.b.z;
    points[1].x = box.t.x - this->m_liquidCorner.x;
    points[1].y = box.t.y - this->m_liquidCorner.y;
    points[1].z = box.t.z;

    CAaBox bounds = CAaBox::Bounding(points, 2);

    CiRect rect;
    rect.maxX = static_cast<int32_t>(floor(bounds.t.x * OO_COORD_TO_SUBCHUNK));
    rect.maxY = static_cast<int32_t>(floor(bounds.t.y * OO_COORD_TO_SUBCHUNK));
    rect.minY = static_cast<int32_t>(floor(bounds.b.y * OO_COORD_TO_SUBCHUNK));
    rect.minX = static_cast<int32_t>(floor(bounds.b.x * OO_COORD_TO_SUBCHUNK));

    if (rect.maxX < 0 || rect.maxY < 0 || rect.minX >= this->liquidTiles.x || rect.minY >= this->liquidTiles.y) {
        return false;
    }

    CiRect clip;
    clip.minY = 0;
    clip.minX = 0;
    clip.maxY = this->liquidTiles.y - 1;
    clip.maxX = this->liquidTiles.x - 1;
    rect &= clip;

    int32_t vertSkip = this->m_liquidVerts.x - rect.maxX + rect.minX - 2;
    int32_t height = rect.maxY - rect.minY;
    int32_t width = rect.maxX - rect.minX;
    uint32_t cullSize = (width + 2) * (height + 2);
    int32_t first = rect.minX + rect.minY * this->m_liquidVerts.x;

    if (cullSize > CMapObjGroup::LIQUID_CULL_VERTEX_LIMIT) {
        return false;
    }

    uint8_t* cull = static_cast<uint8_t*>(alloca(cullSize));
    const C3Vector* vert = this->m_liquidVertArray->m_verts.m_data + first;
    uint8_t* cullOut = cull;

    for (int32_t y = rect.minY; y <= rect.maxY + 1; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX + 1; x++) {
            *cullOut++ = CullLiquidVertexZ(box, *vert);
            vert++;
        }

        vert += vertSkip;
    }

    const C3Vector* tileVert = this->m_liquidVertArray->m_verts.m_data + first;
    const SMOLTile* tile = &this->m_liquidTileList[rect.minY * this->liquidTiles.x + rect.minX];
    int32_t tileSkip = this->liquidTiles.x - rect.maxX + rect.minX - 1;

    int32_t cullTris[2][3];
    cullTris[0][0] = 0;
    cullTris[0][1] = width + 3;
    cullTris[0][2] = width + 2;
    cullTris[1][0] = 0;
    cullTris[1][1] = 1;
    cullTris[1][2] = width + 3;

    int32_t vertTris[2][3];
    vertTris[0][0] = 0;
    vertTris[0][1] = this->m_liquidVerts.x + 1;
    vertTris[0][2] = this->m_liquidVerts.x;
    vertTris[1][0] = 0;
    vertTris[1][1] = 1;
    vertTris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;
    const uint8_t* cullTile = cull;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            if ((tile->m_flags & SMOLTile::LIQUID_TYPE_MASK) != SMOLTile::LIQUID_TYPE_NONE) {
                for (uint32_t j = 0; j < 2; j++) {
                    if (cullTile[cullTris[j][0]] & cullTile[cullTris[j][2]] & cullTile[cullTris[j][1]]) {
                        continue;
                    }

                    if (!found) {
                        batch = triData->AllocBatch();

                        if (batch) {
                            batch->m_def = mapObjDef;
                            batch->m_matrix = &mapObjDef->m_mat;
                            batch->vertices = this->m_liquidVertArray->m_verts.m_data;
                            indices = triData->AllocIndices(6 * (width + 1) * (height + 1));
                            batch->vertexIndices = indices;
                        }
                    }

                    found = true;

                    if (!batch || !batch->vertexIndices) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 3; k++) {
                        uint16_t index = (tileVert - this->m_liquidVertArray->m_verts.m_data) + vertTris[j][k];
                        indices[batch->indexCount++] = index;
                        batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                        batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                    }

                    batch->m_faceCount++;
                }
            }

            tileVert++;
            tile++;
            cullTile++;
        }

        tileVert += tileSkip + 1;
        tile += tileSkip;
        cullTile++;
    }

    return found;
}

// VERIFY: c6ccbb71 manual
// OFFSET: 0x007CAB70
bool CMapObjGroup::GetLiquidTris(const CFrustum& frustum, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->liquidTiles.x || !this->liquidTiles.y) {
        return false;
    }

    if ((flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID) && !(g_liquidTypeDB.GetRecord(type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
        return false;
    }

    C3Vector offset;
    offset.x = -this->m_liquidCorner.x;
    offset.y = -this->m_liquidCorner.y;
    offset.z = -this->m_liquidCorner.z;

    CWFrustum localFrustum;
    static_cast<CFrustum&>(localFrustum) = frustum;
    localFrustum.Translate(offset);

    CAaBox bounds = CAaBox::Bounding(localFrustum.corners, 8);

    CiRect rect;
    rect.maxX = static_cast<int32_t>(floor(bounds.t.x * OO_COORD_TO_SUBCHUNK));
    rect.maxY = static_cast<int32_t>(floor(bounds.t.y * OO_COORD_TO_SUBCHUNK));
    rect.minY = static_cast<int32_t>(floor(bounds.b.y * OO_COORD_TO_SUBCHUNK));
    rect.minX = static_cast<int32_t>(floor(bounds.b.x * OO_COORD_TO_SUBCHUNK));

    if (rect.maxX < 0 || rect.maxY < 0 || rect.minX >= this->liquidTiles.x || rect.minY >= this->liquidTiles.y) {
        return false;
    }

    CiRect clip;
    clip.minY = 0;
    clip.minX = 0;
    clip.maxY = this->liquidTiles.y - 1;
    clip.maxX = this->liquidTiles.x - 1;
    rect &= clip;

    int32_t vertSkip = this->m_liquidVerts.x - rect.maxX + rect.minX - 2;
    int32_t height = rect.maxY - rect.minY;
    int32_t width = rect.maxX - rect.minX;
    uint32_t cullSize = (width + 2) * (height + 2);
    int32_t first = rect.minX + rect.minY * this->m_liquidVerts.x;

    if (cullSize > CMapObjGroup::LIQUID_CULL_VERTEX_LIMIT) {
        return false;
    }

    uint8_t* cull = static_cast<uint8_t*>(alloca(cullSize));
    const C3Vector* vert = this->m_liquidVertArray->m_verts.m_data + first;
    uint8_t* cullOut = cull;

    for (int32_t y = rect.minY; y <= rect.maxY + 1; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX + 1; x++) {
            frustum.Cull(*vert, *cullOut);
            vert++;
            cullOut++;
        }

        vert += vertSkip;
    }

    const C3Vector* tileVert = this->m_liquidVertArray->m_verts.m_data + first;
    const SMOLTile* tile = &this->m_liquidTileList[rect.minY * this->liquidTiles.x + rect.minX];
    int32_t tileSkip = this->liquidTiles.x - rect.maxX + rect.minX - 1;

    int32_t cullTris[2][3];
    cullTris[0][0] = 0;
    cullTris[0][1] = width + 3;
    cullTris[0][2] = width + 2;
    cullTris[1][0] = 0;
    cullTris[1][1] = 1;
    cullTris[1][2] = width + 3;

    int32_t vertTris[2][3];
    vertTris[0][0] = 0;
    vertTris[0][1] = this->m_liquidVerts.x + 1;
    vertTris[0][2] = this->m_liquidVerts.x;
    vertTris[1][0] = 0;
    vertTris[1][1] = 1;
    vertTris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;
    const uint8_t* cullTile = cull;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            if ((tile->m_flags & SMOLTile::LIQUID_TYPE_MASK) != SMOLTile::LIQUID_TYPE_NONE) {
                for (uint32_t j = 0; j < 2; j++) {
                    if (cullTile[cullTris[j][0]] & cullTile[cullTris[j][2]] & cullTile[cullTris[j][1]]) {
                        continue;
                    }

                    if (!found) {
                        batch = triData->AllocBatch();

                        if (batch) {
                            batch->m_def = mapObjDef;
                            batch->m_matrix = &mapObjDef->m_mat;
                            batch->vertices = this->m_liquidVertArray->m_verts.m_data;
                            indices = triData->AllocIndices(6 * (width + 1) * (height + 1));
                            batch->vertexIndices = indices;
                        }
                    }

                    found = true;

                    if (!batch || !batch->vertexIndices) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 3; k++) {
                        uint16_t index = (tileVert - this->m_liquidVertArray->m_verts.m_data) + vertTris[j][k];
                        indices[batch->indexCount++] = index;
                        batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                        batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                    }

                    batch->m_faceCount++;
                }
            }

            tileVert++;
            tile++;
            cullTile++;
        }

        tileVert += tileSkip + 1;
        tile += tileSkip;
        cullTile++;
    }

    return found;
}

// VERIFY: be2ed44d manual
// OFFSET: 0x007CA110
bool CMapObjGroup::GetLiquidFacets(const CAaBox& box, uint32_t flags, World::FacetData* facets, CMapObjDef* mapObjDef) {
    World::TriData::ResetPools();

    World::TriData triData;
    this->GetLiquidTris(box, flags, &triData, mapObjDef);

    WOWGUID guid = mapObjDef->m_objectGuid;
    World::TriDataToFacetData(&triData, facets, guid);

    return World::TriData::nBatches != 0;
}

// VERIFY: 4b81fb7a manual
// OFFSET: 0x007C7A00
static bool TriangleOutsideAaBox(const CAaBox* box, const C3Vector* v0, const C3Vector* v1, const C3Vector* v2) {
    const float* b = &box->b.x;
    const float* t = &box->t.x;
    const float* p0 = &v0->x;
    const float* p1 = &v1->x;
    const float* p2 = &v2->x;

    for (uint32_t axis = 0; axis < 3; axis++) {
        if ((t[axis] - p0[axis] < 0.0f && t[axis] - p1[axis] < 0.0f && t[axis] - p2[axis] < 0.0f) || (p0[axis] - b[axis] < 0.0f && p1[axis] - b[axis] < 0.0f && p2[axis] - b[axis] < 0.0f)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x007C9B10
template <>
void BspQuery_Volume<CAaBox>::operator()(uint16_t face) {
    if (this->m_faceIgnoreFlags & *reinterpret_cast<const uint8_t*>(&this->m_faces[face])) {
        return;
    }

    if (BspQuery::testFaceSub >= BspQuery::MAX_FACES) {
        if (this->m_overflowFlags) {
            *this->m_overflowFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        }

        return;
    }

    BspQuery::testFaces[BspQuery::testFaceSub++] = face;
    this->m_faces[face].m_flags |= SMOPoly::F_COLLIDE_HIT;
    const uint16_t* indices = &this->m_indices[3 * face];

    if (!TriangleOutsideAaBox(this->m_volume, &this->m_vertexList[indices[0]], &this->m_vertexList[indices[1]], &this->m_vertexList[indices[2]])) {
        BspQuery::hitFaces[BspQuery::hitFaceSub++] = face;
    }
}

// VERIFY: afc790da manual
// OFFSET: 0x007C7230
template <>
bool BspQuery_Volume<CAaBox>::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    if (!g_BspDigestCache) {
        return false;
    }

    CAaBspNodeDigest* digest = g_BspDigestCache->GetDigest(aaBsp, node, this->m_faces, this->m_vertexList, this->m_indices);

    if (!digest) {
        return false;
    }

    const CAaBox* box = this->m_volume;
    float x0 = box->b.x;
    float y0 = box->b.y;
    float z0 = box->b.z;
    float x1 = box->t.x;
    float y1 = box->t.y;
    float z1 = box->t.z;

    STORM_ASSERT(x0 <= x1);
    STORM_ASSERT(y0 <= y1);
    STORM_ASSERT(z0 <= z1);

    uint8_t outcodes[452];

    for (uint32_t i = 0; i < digest->m_nVertices; i++) {
        const C3Vector& vertex = digest->m_vertices[i];
        uint8_t code = 0;

        if (x0 >= vertex.x) {
            code = 0x20;
        }

        if (x1 <= vertex.x) {
            code |= 0x10;
        }

        if (y0 >= vertex.y) {
            code |= 0x8;
        }

        if (y1 <= vertex.y) {
            code |= 0x4;
        }

        if (z0 >= vertex.z) {
            code |= 0x2;
        }

        if (z1 <= vertex.z) {
            code |= 0x1;
        }

        outcodes[i] = code;
    }

    for (uint32_t i = 0; i < digest->m_nFaces; i++) {
        uint16_t ignore = this->m_faceIgnoreFlags;
        uint16_t face = digest->m_faceIndices[i];

        if (ignore & digest->m_faceFlags[i]) {
            continue;
        }

        if (ignore & *reinterpret_cast<const uint8_t*>(&this->m_faces[face])) {
            continue;
        }

        if (BspQuery::testFaceSub >= BspQuery::MAX_FACES) {
            if (this->m_overflowFlags) {
                *this->m_overflowFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
            }

            break;
        }

        BspQuery::testFaces[BspQuery::testFaceSub++] = face;
        this->m_faces[face].m_flags |= SMOPoly::F_COLLIDE_HIT;
        const uint16_t* verts = &digest->m_faceVertexIndices[3 * i];

        if (!(outcodes[verts[0]] & outcodes[verts[1]] & outcodes[verts[2]] & 0x3F)) {
            BspQuery::hitFaces[BspQuery::hitFaceSub++] = face;
        }
    }

    return true;
}

// VERIFY: 9dd5a6b5 manual
// OFFSET: 0x007CB7B0
bool CMapObjGroup::GetTris(const CAaBox& box, uint32_t flags, uint16_t faceIgnoreFlags, World::TriData* triData, CMapObjDef* mapObjDef) {
    uint32_t batchCount = World::TriData::nBatches;
    int32_t overflow = WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_NONE;
    BspQuery_Volume<CAaBox> query;
    query.m_overflowFlags = &overflow;
    query.m_faces = this->m_polyList;
    query.m_vertexList = this->m_vertexList;
    query.m_indices = this->m_indices;
    query.m_volume = &box;
    query.m_faceIgnoreFlags = faceIgnoreFlags | SMOPoly::F_COLLIDE_HIT;
    CAaBsp_Query_AaBox<BspQuery_Volume<CAaBox>> bspQuery;
    bspQuery.m_aaBsp = &this->m_aaBsp;
    bspQuery.m_f = &query;
    bspQuery.GetFaceIndices(0, box, this->m_aaBsp.m_aaBox);
    this->GetTrisFromQuery(triData, query, mapObjDef, overflow);

    if (flags & WORLD_QUERY_LIQUID_MASK) {
        this->GetLiquidTris(box, flags, triData, mapObjDef);
    }

    return World::TriData::nBatches != batchCount;
}

// VERIFY: 52965840 manual
// OFFSET: 0x007CB2F0
bool CMapObjGroup::Intersect(C3Segment& seg, float* dist, uint32_t flags, uint16_t faceIgnoreFlags, int32_t* hitIndex) {
    bool hit = false;
    BspQuery_Segment query(this->m_polyList, this->m_vertexList, this->m_indices, seg, dist, faceIgnoreFlags, this->m_parent->materialList);
    CAaBsp_Query_Segment<BspQuery_Segment> bspQuery;
    bspQuery.m_aaBsp = &this->m_aaBsp;
    bspQuery.m_f = &query;
    bspQuery.GetFaceIndices(0, seg, this->m_aaBsp.m_aaBox);

    if (BspQuery::hitFaceSub) {
        *hitIndex = BspQuery::hitFaces[0];
        hit = true;
    }

    if ((flags & WORLD_QUERY_LIQUID_MASK) && (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIQUID) && this->VectorIntersectLiquid(seg, dist, flags, nullptr, nullptr)) {
        *hitIndex = 0;
        hit = true;
    }

    return hit;
}

// VERIFY: 1d5e9a65 manual
// OFFSET: 0x007CB260
bool CMapObjGroup::GetFacesForLinking(C3Segment& seg, float* t0, int32_t* face0, float* t1, int32_t* face1) {
    BspQuery_SegmentLink query(this->m_polyList, this->m_vertexList, this->m_indices, seg, *t0, *t1);
    CAaBsp_Query_Segment<BspQuery_SegmentLink> bspQuery;
    bspQuery.m_aaBsp = &this->m_aaBsp;
    bspQuery.m_f = &query;
    bspQuery.GetFaceIndices(0, seg, this->m_aaBsp.m_aaBox);
    return query.GetHits(t0, face0, t1, face1);
}

// VERIFY: 3a8057f6 manual
// OFFSET: 0x007C7FE0
bool CMapObjGroup::QueryLighting(const C3Vector& point, uint16_t face, CImVector* color, uint8_t* flag) {
    if (face >= static_cast<uint32_t>(this->m_polyListSize)) {
        *color = this->m_parent->m_argbColor;
        *flag = 0;
        return true;
    }

    // STATIC: 0x00D2DC00
    static uint32_t s_axisPairs[3][2] = { { 1, 2 }, { 2, 0 }, { 0, 1 } };

    const uint16_t* indices = &this->m_indices[3 * face];
    const C3Vector& p0 = this->m_vertexList[indices[0]];
    const C3Vector& p1 = this->m_vertexList[indices[1]];
    const C3Vector& p2 = this->m_vertexList[indices[2]];
    float ax = p2.x - p0.x;
    float ay = p2.y - p0.y;
    float az = p2.z - p0.z;
    float bx = p1.x - p0.x;
    float by = p1.y - p0.y;
    float bz = p1.z - p0.z;
    C3Vector normal = C3Vector(by * az - bz * ay, bz * ax - az * bx, ay * bx - ax * by);
    uint32_t axis = normal.MajorAxis();
    uint32_t i = s_axisPairs[axis][0];
    uint32_t j = s_axisPairs[axis][1];

    const float* v0 = &p0.x;
    const float* v1 = &p1.x;
    const float* v2 = &p2.x;
    const float* pt = &point.x;
    float e1i = v1[i] - v0[i];
    float e2i = v2[i] - v0[i];
    float di = pt[i] - v0[i];
    float e1j = v1[j] - v0[j];
    float e2j = v2[j] - v0[j];
    float inv = 1.0f / (e2j * e1i - e1j * e2i);
    float dj = pt[j] - v0[j];
    float u = (e2j * di - e2i * dj) * inv;
    float w = inv * (e1i * dj - di * e1j);

    int32_t weight1 = static_cast<int32_t>(u * 256.0f);
    int32_t weight2 = static_cast<int32_t>(w * 256.0f);
    int32_t weight0 = 256 - weight2 - weight1;

    if (weight1 < 0) {
        int32_t d = weight2 * weight1 / (weight2 + weight0);
        weight0 += weight1 - d;
        weight2 += d;
        weight1 = 0;
    }

    if (weight2 < 0) {
        int32_t d = weight2 * weight1 / (weight0 + weight1);
        weight1 += d;
        weight0 += weight2 - d;
        weight2 = 0;
    }

    if (weight0 < 0) {
        int32_t d = weight0 * weight1 / (weight2 + weight1);
        weight2 += weight0 - d;
        weight1 += d;
        weight0 = 0;
    }

    const CImVector* colors = this->cvertexList;
    color->a = static_cast<uint8_t>((weight1 * colors[indices[1]].a + weight0 * colors[indices[0]].a + weight2 * colors[indices[2]].a) >> 8);
    color->r = static_cast<uint8_t>((weight0 * colors[indices[0]].r + weight1 * colors[indices[1]].r + weight2 * colors[indices[2]].r) >> 8);
    color->g = static_cast<uint8_t>((weight1 * colors[indices[1]].g + weight0 * colors[indices[0]].g + weight2 * colors[indices[2]].g) >> 8);
    color->b = static_cast<uint8_t>((weight1 * colors[indices[1]].b + weight0 * colors[indices[0]].b + weight2 * colors[indices[2]].b) >> 8);

    uint32_t r = 2 * color->r;
    uint32_t g = 2 * color->g;
    uint32_t b = 2 * color->b;
    CMapObj* parent = this->m_parent;

    if (parent->m_header->m_flags & SMOHeader::FLAGS::UNIFIED_RENDER) {
        r += parent->m_argbColor.r;
        g += parent->m_argbColor.g;
        b += parent->m_argbColor.b;
    }

    color->r = r >= 0xFF ? 0xFF : static_cast<uint8_t>(r);
    color->g = g >= 0xFF ? 0xFF : static_cast<uint8_t>(g);
    color->b = b >= 0xFF ? 0xFF : static_cast<uint8_t>(b);
    *flag = this->m_polyList[face].m_flags & SMOPoly::F_TRANSITION;
    return true;
}

// INLINED
template <>
bool BspQuery_Volume<CWFrustum>::GetFaceIndicesUsingCache(const CAaBsp& aaBsp, const CAaBspNode* node) {
    return false;
}

// VERIFY: 2ac9f0dc auto
// OFFSET: 0x007C71E0
static bool QueryCull(const CWFrustum* frustum, const C3Vector* v0, const C3Vector* v1, const C3Vector* v2) {
    uint8_t outside0;
    uint8_t outside1;
    uint8_t outside2;
    frustum->Cull(*v0, outside0);
    frustum->Cull(*v1, outside1);
    frustum->Cull(*v2, outside2);
    return (outside1 & outside2 & outside0) != 0;
}

// OFFSET: 0x007C7660
template <>
void BspQuery_Volume<CWFrustum>::operator()(uint16_t face) {
    if (this->m_faceIgnoreFlags & *reinterpret_cast<const uint8_t*>(&this->m_faces[face])) {
        return;
    }

    if (BspQuery::testFaceSub >= BspQuery::MAX_FACES) {
        if (this->m_overflowFlags) {
            *this->m_overflowFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
        }

        return;
    }

    BspQuery::testFaces[BspQuery::testFaceSub++] = face;
    this->m_faces[face].m_flags |= SMOPoly::F_COLLIDE_HIT;
    const uint16_t* indices = &this->m_indices[3 * face];

    if (!QueryCull(this->m_volume, &this->m_vertexList[indices[0]], &this->m_vertexList[indices[1]], &this->m_vertexList[indices[2]])) {
        BspQuery::hitFaces[BspQuery::hitFaceSub++] = face;
    }
}

// VERIFY: dec307c3 manual
// OFFSET: 0x007CB180
bool CMapObjGroup::GetTris(const CWFrustum& frustum, uint32_t flags, uint16_t faceIgnoreFlags, World::TriData* triData, CMapObjDef* mapObjDef) {
    uint32_t batchCount = World::TriData::nBatches;

    if (flags & WORLD_QUERY_WMO_MASK) {
        int32_t overflow = WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_NONE;
        BspQuery_Volume<CWFrustum> query;
        query.m_overflowFlags = &overflow;
        query.m_faces = this->m_polyList;
        query.m_vertexList = this->m_vertexList;
        query.m_indices = this->m_indices;
        query.m_volume = &frustum;
        query.m_faceIgnoreFlags = faceIgnoreFlags | SMOPoly::F_COLLIDE_HIT;
        CAaBox box = CAaBox::Bounding(frustum.corners, 8);
        CAaBsp_Query_AaBox<BspQuery_Volume<CWFrustum>> bspQuery;
        bspQuery.m_aaBsp = &this->m_aaBsp;
        bspQuery.m_f = &query;
        bspQuery.GetFaceIndices(0, box, this->m_aaBsp.m_aaBox);
        this->GetTrisFromQuery(triData, query, mapObjDef, overflow);
    }

    if (flags & WORLD_QUERY_LIQUID_MASK) {
        this->GetLiquidTris(frustum, flags, triData, mapObjDef);
    }

    return World::TriData::nBatches != batchCount;
}
