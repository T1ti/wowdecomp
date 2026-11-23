#include "MapLowDetail.h"
#include "Engine/Source/Tempest/Color.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CWorldOcclusion.h"
#include "WoW/Source/WorldClient/FVBBList.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "Engine/Source/Gx/CGxVertexPC.h"

// VERIFY: 3763adce auto
// OFFSET: 0x007C06E0
CMapAreaLow::CMapAreaLow() {
    this->m_sphere.r = 0.0f;
    this->m_index.x = 0;
    this->m_index.y = 0;
    this->m_horizonLink.m_prevLink = nullptr;
    this->m_horizonLink.m_next = nullptr;
    this->m_indexCount = 0;
    this->m_block = nullptr;
    this->m_flags = CMapAreaLow::FLAGS::NONE;
    this->m_heights = nullptr;
    this->m_holes = nullptr;
}

// VERIFY: 0b659a65 auto
// OFFSET: 0x007CC2C0
CMapLowDetail::CMapLowDetail() {
    this->m_data = nullptr;
    this->m_mapObjNames = nullptr;
    this->m_mapObjNameOffsets = nullptr;
    this->m_mapObjDefs = nullptr;
    this->m_mapObjDefCount = 0;
    this->m_areaOffsets = nullptr;
    memset(this->m_areaTable, 0, sizeof(this->m_areaTable));
}

#if defined(WOWDECOMP_ASSERTIONS)
CMapLowDetail::~CMapLowDetail() {
    STORM_ASSERT(m_data == nullptr); // 12213: "m_data == __null"

    for (uint32_t i = 0; i < MAP_AREA_COUNT; i++) {
        STORM_ASSERT(m_areaTable[i] == nullptr); // 12213: "m_areaTable[i] == __null"
    }

    STORM_ASSERT(m_mapObjDefList.Count() == 0);
}
#endif

// OFFSET: 0x007CBFE0
void CMapLowDetail::Purge() {
    for (uint32_t i = 0; i < 4096; i++) {
        CMapAreaLow* areaLow = this->m_areaTable[i];

        if (areaLow) {
            if (areaLow->m_block) {
                CMap::s_vbbList.Release(areaLow->m_block);
            }

            this->m_areaTable[i]->m_block = nullptr;
        }
    }
}

// VERIFY: 3b80ddc7 manual
// OFFSET: 0x007CC0B0
void CMapLowDetail::CullHorizonAreas(const C3Vector* pos) {
    float y = -(pos->y - 17066.666f);
    float x = -(pos->x - 17066.666f) * OO_COORD_TO_CHUNK;
    int32_t row = CMath::fint_(x - 0.5f);
    y = y * OO_COORD_TO_CHUNK;
    int32_t col = CMath::fint_(y - 0.5f);
    int32_t radius = static_cast<int32_t>(static_cast<int64_t>(World::s_horizonFarClipScale * World::farClip * 0.030000001f));

    int32_t minCol = ((col - radius) >> 4) - 2;
    int32_t minRow = ((row - radius) >> 4) - 2;
    int32_t maxCol = ((col + radius) >> 4) + 2;
    int32_t maxRow = ((radius + row) >> 4) + 2;

    if (minRow < 0) {
        minRow = 0;
    }

    if (minCol < 0) {
        minCol = 0;
    }

    if (maxCol >= MAP_AREAS_PER_AXIS) {
        maxCol = MAP_AREAS_PER_AXIS - 1;
    }

    if (maxRow >= MAP_AREAS_PER_AXIS) {
        maxRow = MAP_AREAS_PER_AXIS - 1;
    }

    for (int32_t r = minRow; r <= maxRow; r++) {
        for (int32_t c = minCol; c <= maxCol; c++) {
            CMapAreaLow* area = this->m_areaTable[CMap::GetAreaIndex(c, r)];

            if (!area || CWorldScene::FrustumCull(&area->m_bbox) || CWorldOcclusion::QueryVolumes(&area->m_sphere)) {
                continue;
            }

            if (World::farClip > 300.0f && CWorldOcclusion::QueryBuffer(&area->m_bbox, CWorldOcclusion::QUERY_FLAG_UNKNOWN_20 | CWorldOcclusion::QUERY_FLAG_TEST_NEAR)) {
                continue;
            }

            CWorldScene::AddHorizonChunk(area);
        }
    }

    for (uint32_t i = 0; i < this->m_mapObjDefList.Count(); i++) {
        CMapObjDef* def = this->m_mapObjDefList.m_data[i];

        if (def->mapObj->m_isGroupLoaded && !CWorldScene::FrustumCull(&def->m_bbox) && !CWorldOcclusion::QueryVolumes(&def->m_sphere)) {
            CWorldScene::AddHorizonMapObjDef(def);
        }
    }
}

// VERIFY: ba76c774 manual
// OFFSET: 0x007CC310
void CMapLowDetail::Load(const char* mapPath, const char* mapName) {
    char fileName[256];
    SFileImpl* file;

    SStrPrintf(fileName, sizeof(fileName), "%s\\%s.wdl", mapPath, mapName);
    SFileOpenFile(fileName, &file);

    if (!file) {
        return;
    }

    uint32_t size = SFileGetFileSize(file, nullptr);
    this->m_data = static_cast<char*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    STORM_ASSERT(m_data);
    SFileReadFile(file, this->m_data, size, nullptr, nullptr, 0);
    SFileCloseFile(file);

    auto* iffChunk = reinterpret_cast<const SIffChunk*>(this->m_data);
    STORM_ASSERT(iffChunk->token == SIffChunk::TOKEN_MVER); // 12213: "iffChunk->token == 'MVER'"
#if defined(WOWDECOMP_ASSERTIONS)
    const auto* adtVersion = reinterpret_cast<const uint32_t*>(this->m_data + 8);
#endif
    STORM_ASSERT(*adtVersion == 0x0012);

    char* data = this->m_data + iffChunk->m_size + 8;
    iffChunk = reinterpret_cast<const SIffChunk*>(data);
    data += 8;

    if (iffChunk->token == SIffChunk::TOKEN_MWMO) {
        this->m_mapObjNames = data;
        data += iffChunk->m_size;
        iffChunk = reinterpret_cast<const SIffChunk*>(data);
        STORM_ASSERT(iffChunk->token == SIffChunk::TOKEN_MWID); // 12213: "iffChunk->token == 'MWID'"
        data += 8;
        this->m_mapObjNameOffsets = reinterpret_cast<const uint32_t*>(data);

        data += iffChunk->m_size;
        iffChunk = reinterpret_cast<const SIffChunk*>(data);
        STORM_ASSERT(iffChunk->token == SIffChunk::TOKEN_MODF); // 12213: "iffChunk->token == 'MODF'"
        data += 8;
        this->m_mapObjDefs = reinterpret_cast<SMMapObjDef*>(data);
        this->m_mapObjDefCount = iffChunk->m_size >> 6;
        data += iffChunk->m_size;
        iffChunk = reinterpret_cast<const SIffChunk*>(data);
        data += 8;
    }

    STORM_ASSERT(iffChunk->token == SIffChunk::TOKEN_MAOF); // 12213: "iffChunk->token == 'MAOF'"
    this->m_areaOffsets = reinterpret_cast<const uint32_t*>(data);

    uint32_t index = 0;

    for (uint32_t y = 0; y < MAP_CHUNKS_PER_MAP_AXIS; y += MAP_CHUNKS_PER_AREA_AXIS) {
        for (uint32_t x = 0; x < MAP_CHUNKS_PER_MAP_AXIS; x += MAP_CHUNKS_PER_AREA_AXIS) {
            if (this->m_areaOffsets[index]) {
                const char* mare = this->m_data + this->m_areaOffsets[index];
                iffChunk = reinterpret_cast<const SIffChunk*>(mare);
                STORM_ASSERT(iffChunk->token == SIffChunk::TOKEN_MARE); // 12213: "iffChunk->token == 'MARE'"
                CMapAreaLow* mapAreaLow = CMap::AllocAreaLow();
                STORM_ASSERT(mapAreaLow);
                this->m_areaTable[index] = mapAreaLow;
                mapAreaLow->m_heights = reinterpret_cast<const int16_t*>(mare + 8);
                const auto* next = reinterpret_cast<const SIffChunk*>(mare + iffChunk->m_size + 8);

                int32_t minHeight = 100000;
                int32_t maxHeight = -100000;

                for (uint32_t i = 0; i < LOW_DETAIL_VERTEX_COUNT; i++) {
                    int32_t height = mapAreaLow->m_heights[i];

                    if (height > maxHeight) {
                        maxHeight = height;
                    }

                    if (height < minHeight) {
                        minHeight = height;
                    }
                }

                mapAreaLow->m_index.x = x >> 4;
                mapAreaLow->m_index.y = y >> 4;

                float top = 17066.666f - static_cast<float>(y) * 33.333332f;
                mapAreaLow->m_topLeft.x = top;
                mapAreaLow->m_bbox.t.x = top;

                float left = static_cast<float>(x) * -33.333332f + 17066.666f;
                mapAreaLow->m_topLeft.y = left;
                mapAreaLow->m_bbox.t.y = left;
                mapAreaLow->m_bbox.t.z = static_cast<float>(maxHeight);
                mapAreaLow->m_bbox.b.x = top - 533.33331f;
                mapAreaLow->m_bbox.b.y = left - 533.33331f;
                mapAreaLow->m_bbox.b.z = static_cast<float>(minHeight);

                mapAreaLow->m_sphere.c.x = (mapAreaLow->m_bbox.t.x + mapAreaLow->m_bbox.b.x) * 0.5f;
                mapAreaLow->m_sphere.c.y = (mapAreaLow->m_bbox.b.y + mapAreaLow->m_bbox.t.y) * 0.5f;
                mapAreaLow->m_sphere.c.z = 0.5f * (mapAreaLow->m_bbox.b.z + mapAreaLow->m_bbox.t.z);
                float dz = mapAreaLow->m_bbox.t.z - mapAreaLow->m_sphere.c.z;
                float dy = mapAreaLow->m_bbox.t.y - mapAreaLow->m_sphere.c.y;
                float dx = mapAreaLow->m_bbox.t.x - mapAreaLow->m_sphere.c.x;
                mapAreaLow->m_sphere.r = sqrtf(dx * dx + dy * dy + dz * dz);

                if (next->token == SIffChunk::TOKEN_MAHO) {
                    const auto* holes = reinterpret_cast<const uint16_t*>(reinterpret_cast<const char*>(next) + 8);
                    mapAreaLow->m_holes = holes;
                    mapAreaLow->m_indexCount = 0;

                    for (uint32_t row = 0; row < LOW_DETAIL_CELLS_PER_AXIS; row++) {
                        for (uint32_t bit = 0; bit < LOW_DETAIL_CELLS_PER_AXIS; bit++) {
                            if (!((1 << bit) & holes[row])) {
                                mapAreaLow->m_indexCount += LOW_DETAIL_INDICES_PER_CELL;
                            }
                        }
                    }
                } else {
                    mapAreaLow->m_holes = nullptr;
                    mapAreaLow->m_indexCount = LOW_DETAIL_INDEX_COUNT;
                }
            }

            index++;
        }
    }

    C3Vector center(17066.666f, 17066.666f, 0.0f);
    this->m_mapObjDefList.SetCount(this->m_mapObjDefCount);

    for (uint32_t i = 0; i < this->m_mapObjDefCount; i++) {
        SMMapObjDef* def = &this->m_mapObjDefs[i];
        CMapObjDef* mapObjDef = CMap::CreateMapObjDef(this->m_mapObjNames + this->m_mapObjNameOffsets[def->m_nameId], def, &center, false);

        if (!CMap::s_isStreamingMode) {
            CMapObj* mapObj = mapObjDef->mapObj;

            if (!mapObj->m_isGroupLoaded) {
                mapObj->WaitLoad();
            }

            uint32_t groupCount = mapObj->m_groupInfoCount;

            for (uint32_t j = 0; j < groupCount; j++) {
                if (!mapObj->IsGroupLoaded(j)) {
                    STORM_ASSERT(mapObj->IsGroupLoading(j) == 0);
                    mapObj->ReadGroup(j);
                    mapObj->WaitLoadGroup(j);
                }
            }
        }

        mapObjDef->AddRef();
        this->m_mapObjDefList.m_data[i] = mapObjDef;
    }
}

// VERIFY: c20afbce manual
// OFFSET: 0x007D5E70
void CMap::RenderLowDetail(CMapAreaLow* area) {
    if (static_cast<int32_t>(area->m_indexCount) <= 0) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Fog, 1);
    GxRsSet(GxRs_DepthTest, 3);

    DayNight::DNInfo* info = DayNight::GetInfo();
    GxRsSet(GxRs_FogStart, 0.0f);
    GxRsSet(GxRs_FogEnd, 1.0f);
    CImVector color = info->m_fog.m_color;
    GxRsSet(GxRs_FogColor, *reinterpret_cast<int32_t*>(&color));
    GxRsSet(GxRs_Lighting, 0);

    if (!area->m_block) {
        CMap::s_vbbList.AcquireBlock(&area->m_block);
    }

    if (area->m_block) {
        CGxBuf* buf = area->m_block->m_buf;

        if (!buf->m_hasData || !buf->m_valid) {
            CMap::BuildLowDetailGridVertices(area, buf);
        }

        area->m_block->m_lastUsedFrame = CWorld::frameCnt;
        GxPrimVertexPtr(area->m_block->m_buf, GxVBF_PC);
    } else {
        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 16, LOW_DETAIL_VERTEX_COUNT);
        CMap::BuildLowDetailGridVertices(area, buf);
        GxPrimVertexPtr(buf, GxVBF_PC);
    }

    uint32_t count;

    if (!(area->m_flags & CMapAreaLow::FLAGS::UNKNOWN_1) && area->m_indexCount == LOW_DETAIL_INDEX_COUNT) {
        GxRsSet(GxRs_Culling, 0);
        CGxBuf* indexBuf = CMap::lowDetailIndexBuf;

        if (!indexBuf->m_hasData || !indexBuf->m_valid) {
            CMap::BuildLowDetailGridIndices(nullptr, 0, CMap::lowDetailIndexBuf);
            indexBuf = CMap::lowDetailIndexBuf;
        }

        g_theGxDevicePtr->PrimIndexPtr(indexBuf);
        count = LOW_DETAIL_INDEX_COUNT;
    } else {
        GxRsSet(GxRs_Culling, 0);
        CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, area->m_indexCount);
        CMap::BuildLowDetailGridIndices(area->m_holes, 0, indexBuf);
        g_theGxDevicePtr->PrimIndexPtr(indexBuf);

        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = area->m_indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = LOW_DETAIL_VERTEX_COUNT - 1;
        g_theGxDevicePtr->Draw(&batch, 1);

        GxRsSet(GxRs_Culling, 1);
        indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, LOW_DETAIL_INDEX_COUNT - area->m_indexCount);
        CMap::BuildLowDetailGridIndices(area->m_holes, 1, indexBuf);
        g_theGxDevicePtr->PrimIndexPtr(indexBuf);
        count = LOW_DETAIL_INDEX_COUNT - area->m_indexCount;
    }

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = count;
    batch.m_minIndex = 0;
    batch.m_maxIndex = LOW_DETAIL_VERTEX_COUNT - 1;
    g_theGxDevicePtr->Draw(&batch, 1);

    g_theGxDevicePtr->RsPop();
    g_theGxDevicePtr->m_xforms[GxXform_Tex0].Identity();
}

// VERIFY: 894a0529 manual
// OFFSET: 0x007D5150
void CMap::BuildLowDetailGridVertices(CMapAreaLow* area, CGxBuf* buf) {
    CGxVertexPC* vertex = reinterpret_cast<CGxVertexPC*>(g_theGxDevicePtr->BufLock(buf));
    uint32_t index = 0;
    float x = area->m_topLeft.x;

    for (int32_t row = 0; row < LOW_DETAIL_OUTER_VERTICES_PER_AXIS; row++) {
        float y = area->m_topLeft.y;

        for (int32_t col = 0; col < LOW_DETAIL_OUTER_VERTICES_PER_AXIS; col++) {
            vertex->p.x = x;
            vertex->p.y = y;
            vertex->p.z = area->m_heights[index++];
            vertex->c = CImVector(COLOR_WHITE_BGRA);
            vertex++;
            y -= 33.333332f;
        }

        x -= 33.333332f;
    }

    x = area->m_topLeft.x - 16.666666f;

    for (int32_t row = 0; row < LOW_DETAIL_CELLS_PER_AXIS; row++) {
        float y = area->m_topLeft.y - 16.666666f;

        for (int32_t col = 0; col < LOW_DETAIL_CELLS_PER_AXIS; col++) {
            vertex->p.x = x;
            vertex->p.y = y;
            vertex->p.z = area->m_heights[index++];
            vertex->c = CImVector(COLOR_WHITE_BGRA);
            vertex++;
            y -= 33.333332f;
        }

        x -= 33.333332f;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
}

// VERIFY: 7d2e0b07 manual
// OFFSET: 0x007D5240
void CMap::BuildLowDetailGridIndices(const uint16_t* holes, int32_t pass, CGxBuf* buf) {
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));
    uint32_t outer = 0;
    uint32_t inner = LOW_DETAIL_OUTER_VERTEX_COUNT;

    for (uint32_t row = 0; row < LOW_DETAIL_CELLS_PER_AXIS; row++) {
        for (int32_t col = 0; col < LOW_DETAIL_CELLS_PER_AXIS; col++) {
            if (!holes || (pass ? (holes[row] & (1 << col)) != 0 : (holes[row] & (1 << col)) == 0)) {
                *indices++ = inner;
                *indices++ = outer + 1;
                *indices++ = outer;
                *indices++ = inner;
                *indices++ = outer + LOW_DETAIL_OUTER_VERTICES_PER_AXIS + 1;
                *indices++ = outer + 1;
                *indices++ = inner;
                *indices++ = outer + LOW_DETAIL_OUTER_VERTICES_PER_AXIS;
                *indices++ = outer + LOW_DETAIL_OUTER_VERTICES_PER_AXIS + 1;
                *indices++ = inner;
                *indices++ = outer;
                *indices++ = outer + LOW_DETAIL_OUTER_VERTICES_PER_AXIS;
            }

            outer++;
            inner++;
        }

        outer++;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->m_hasData = 1;
}

// VERIFY: f803b515 manual
// OFFSET: 0x007CC770
void CMapLowDetail::Unload() {
    for (uint32_t i = 0; i < 4096; i++) {
        if (this->m_areaTable[i]) {
            CMap::FreeAreaLow(this->m_areaTable[i]);
            this->m_areaTable[i] = nullptr;
        }
    }

    for (uint32_t i = 0; i < this->m_mapObjDefList.m_count; i++) {
        this->m_mapObjDefList.m_data[i]->ReleaseRef();
        CMap::PurgeMapObjDef(this->m_mapObjDefList.m_data[i]);
    }

    this->m_mapObjDefList.m_count = 0;
    this->m_mapObjNames = nullptr;
    this->m_mapObjNameOffsets = nullptr;
    this->m_mapObjDefs = nullptr;
    this->m_mapObjDefCount = 0;
    this->m_areaOffsets = nullptr;

    if (this->m_data) {
        SMemFree(this->m_data, nullptr, 0, 0);
        this->m_data = nullptr;
    }
}
