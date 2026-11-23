#include "MapArea.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include <cstring>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#ifdef WOWDECOMP_TOOL_FEATURES
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#endif
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"

void CMapArea::Destroy() {
}

// VERIFY: 2c619432 manual
// OFFSET: 0x005EEB70
void CMapArea::Initialize() {
}

// VERIFY: 28aeb8e8 auto
// OFFSET: 0x007C2FF0
void CMapArea::FreeAsyncObject(CAsyncObject* object) {
    void* buffer = object->buffer;
    uint32_t size = object->size;
    AsyncFileReadDestroyObject(object);
    CMapArea::FreeAsyncLoadBuffer(buffer, size);
}

// VERIFY: ed92c35f auto
// OFFSET: 0x007C35D0
void CMapArea::PurgeChunk(CMapChunk* chunk) {
    chunk->Purge();
    CMap::FreeChunk(chunk);
}

// VERIFY: 3489b6fa manual
// OFFSET: 0x007C35F0
void CMapArea::Purge() {
    if (this->asyncObject) {
        if (!AsyncFile::Cancel(this->asyncObject, CMapArea::FreeAsyncObject)) {
            this->rawAreaData = nullptr;
        }

        this->asyncObject = nullptr;
        this->adtFile = nullptr;
    }

    for (CMapBaseObjLink* link = this->chunkLinkList.Head(); link;) {
        CMapChunk* chunk = static_cast<CMapChunk*>(link->owner);
        CMapBaseObjLink* next = this->chunkLinkList.Next(link);
        this->m_mapChunks[MAP_CHUNKS_PER_AREA_AXIS * chunk->aIndex.y + chunk->aIndex.x] = nullptr;
        this->m_chunkInfo[MAP_CHUNKS_PER_AREA_AXIS * chunk->aIndex.y + chunk->aIndex.x].m_flags = 0;
        CMap::FreeBaseObjLink(link);
        chunk->Purge();
        CMap::FreeChunk(chunk);
        link = next;
    }

    for (uint32_t i = 0; i < this->m_textures.m_count; i++) {
        if (this->m_textures.m_data[i].m_texture) {
            HandleClose(reinterpret_cast<HOBJECT>(this->m_textures.m_data[i].m_texture));
        }
    }

    this->m_textures.m_count = 0;

    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link;) {
        CMapBaseObjLink* next = this->m_parentLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }
}

// VERIFY: bafcd5df auto
// OFFSET: 0x007D6690
bool CWorldScene::InsideFrustumRect(CiRect* rect) {
    return rect->minX <= CWorldScene::s_frustumChunkRect.maxX && rect->minY <= CWorldScene::s_frustumChunkRect.maxY && rect->maxX >= CWorldScene::s_frustumChunkRect.minX && rect->maxY >= CWorldScene::s_frustumChunkRect.minY;
}

// VERIFY: 8d38ed01 manual
// OFFSET: 0x007D6810
void CMapArea::BatchChunks(const C2iVector& pos) {
    int32_t index = MAP_CHUNKS_PER_AREA_AXIS * pos.y + pos.x;

    CMapChunk* chunks[MAP_CHUNKS_PER_BATCH];
    chunks[0] = this->m_mapChunks[index];
    chunks[1] = this->m_mapChunks[index + MAP_CHUNKS_PER_AREA_AXIS];
    chunks[2] = this->m_mapChunks[index + 1];
    chunks[3] = this->m_mapChunks[index + MAP_CHUNKS_PER_AREA_AXIS + 1];

    for (int32_t i = 0; i < MAP_CHUNKS_PER_BATCH; i++) {
        if (!chunks[i]) {
            return;
        }
    }

    CMapChunk* topLeft = chunks[0];
    CMapChunk* bottomLeft = chunks[1];
    CMapChunk* topRight = chunks[2];
    CMapChunk* bottomRight = chunks[3];

    int32_t mergeLeft = CMapChunk::CanMergeChunkLayers(bottomLeft, topLeft);
    int32_t mergeRight = CMapChunk::CanMergeChunkLayers(bottomRight, topRight);
    int32_t mergeTop = CMapChunk::CanMergeChunkLayers(topRight, topLeft);
    int32_t mergeBottom = CMapChunk::CanMergeChunkLayers(bottomRight, bottomLeft);

    int32_t vertical = mergeRight + mergeLeft;
    int32_t horizontal = mergeBottom + mergeTop;

    if (vertical || horizontal) {
        int32_t firstPartner = 2;
        int32_t secondStart = 1;
        int32_t firstMerge = mergeTop;
        int32_t secondMerge = mergeBottom;
        uint8_t orientation = CMapRenderChunk::BATCH_FLAGS::BATCH_PAIR_HORIZONTAL;

        if (vertical > horizontal) {
            firstMerge = mergeLeft;
            firstPartner = 1;
            secondStart = 2;
            secondMerge = mergeRight;
            orientation = CMapRenderChunk::BATCH_FLAGS::BATCH_PAIR_VERTICAL;
        }

        if (firstMerge) {
            CMapRenderChunk* renderChunk = CMap::AllocRenderChunk();
            CMapChunk* partner = chunks[firstPartner];
            topLeft->m_renderChunk = renderChunk;
            renderChunk->AddBatch(topLeft, partner, &topLeft->m_topLeftCoords, orientation);
            partner->m_renderChunk = topLeft->m_renderChunk;
        }

        if (secondMerge) {
            CMapRenderChunk* renderChunk = CMap::AllocRenderChunk();
            CMapChunk* first = chunks[secondStart];
            first->m_renderChunk = renderChunk;
            renderChunk->AddBatch(first, bottomRight, &first->m_topLeftCoords, orientation);
            bottomRight->m_renderChunk = first->m_renderChunk;
        }
    }

    for (int32_t i = 0; i < 4; i++) {
        CMapChunk* chunk = chunks[i];

        if (!chunk->m_renderChunk) {
            chunk->AllocRenderChunkAndBatch();
        }
    }
}

// VERIFY: bb694f33 manual
// OFFSET: 0x007D6980
void CMapArea::LoadTerrainTexture(CMapAreaTexture& mapAreaTexture, int32_t textureId) {
    STORM_ASSERT(mapAreaTexture.m_texture == NULL);  // 12213: "mapAreaTexture.m_texture == __null"

    bool shaders = g_theGxDevicePtr->Caps().m_texTarget[1] && g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Vertex] && g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel];

    uint8_t flags = 0;

    if (this->m_textureFlags) {
        flags = this->m_textureFlags[textureId];
    }

    if ((flags & TEXTURE_FLAG_NO_SPECULAR) || !CMap::enableSpecularTerrain) {
        if (!(flags & TEXTURE_FLAG_NO_SPECULAR) || shaders) {
            mapAreaTexture.m_texture = CMap::LoadTexture(mapAreaTexture.m_texName);
        } else {
            CImVector color;
            color.b = 0x00;
            color.g = 0x00;
            color.r = 0x00;
            color.a = 0xFF;
            mapAreaTexture.m_texture = TextureCreateSolid(color);
        }
    } else {
        char fileName[256];
        STORM_ASSERT((SStrLen(mapAreaTexture.m_texName) + 2) < sizeof(fileName)); // 12213: "(SStrLen(mapAreaTexture.m_texName) + 2) < 260"
        SStrCopy(fileName, mapAreaTexture.m_texName, STORM_MAX_STR);
        char* dot = SStrChrR(fileName, '.');
        STORM_ASSERT(dot);
        SStrCopy(dot, "_s.blp", STORM_MAX_STR);
        mapAreaTexture.m_texture = CMap::LoadTexture(fileName);
    }
}

// VERIFY: 3fc00f06 manual
// OFFSET: 0x007D6A90
void CMapArea::PurgeChunks(CiRect* chunkRect) {
    int32_t minY = chunkRect->minY - this->m_tileChunkIndex.y;
    int32_t minX = chunkRect->minX - this->m_tileChunkIndex.x;
    int32_t maxY = chunkRect->maxY - this->m_tileChunkIndex.y;
    int32_t maxX = chunkRect->maxX - this->m_tileChunkIndex.x;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMapChunk* chunk = this->m_mapChunks[MAP_CHUNKS_PER_AREA_AXIS * y + x];

            if (chunk) {
                this->m_mapChunks[MAP_CHUNKS_PER_AREA_AXIS * y + x] = nullptr;
                CMap::FreeBaseObjLink(chunk->m_parentLinkList.Head());
                CMapArea::PurgeChunk(chunk);
            }
        }
    }
}

// VERIFY: 4696cf2f manual
// OFFSET: 0x007D6B30
CMapChunk* CMapArea::PrepareChunk(int32_t x, int32_t y) {
    STORM_ASSERT(GetChunk(x, y) == NULL);

    int32_t index = x + MAP_CHUNKS_PER_AREA_AXIS * y;

    bool firstUse = true;

    if (this->m_chunkInfo[index].m_flags & SMChunkInfo::F_PREPARED) {
        firstUse = false;
    }

    this->m_chunkInfo[index].m_flags |= SMChunkInfo::F_PREPARED;

    CMapChunk* chunk = CMap::AllocMapChunk();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(chunk);
    link->ref = this;
    this->chunkLinkList.LinkToTail(link);

    chunk->aIndex.x = x;
    chunk->m_flags = MAPOBJ_FLAG_NONE;
    chunk->aIndex.y = y;
    chunk->m_cOffset.x = x + this->m_tileChunkIndex.x;
    chunk->m_cOffset.y = y + this->m_tileChunkIndex.y;
    chunk->m_sOffset.x = 8 * (x + this->m_tileChunkIndex.x);
    chunk->m_sOffset.y = 8 * (y + this->m_tileChunkIndex.y);
    chunk->Create(reinterpret_cast<SIffChunk*>(static_cast<uint8_t*>(this->rawAreaData) + this->m_chunkInfo[index].m_offset), firstUse);

    return chunk;
}

// VERIFY: b5606f47 manual
// OFFSET: 0x007D6BF0
void CMapArea::Update(int32_t addToScene, CiRect* chunkRect) {
#ifdef WOWDECOMP_TOOL_FEATURES
    const WorldEditView* editorView = WorldEditGetView();
#endif

    for (int32_t chunkY = chunkRect->minY; chunkY <= chunkRect->maxY; chunkY++) {
        int32_t localY = chunkY & MAP_CHUNK_INDEX_MASK;

        for (int32_t chunkX = chunkRect->minX; chunkX <= chunkRect->maxX; chunkX++) {
            int32_t localX = chunkX & MAP_CHUNK_INDEX_MASK;

            if (chunkX >= CWorld::gbChunkRect.minX && chunkX <= CWorld::gbChunkRect.maxX && chunkY >= CWorld::gbChunkRect.minY && chunkY <= CWorld::gbChunkRect.maxY) {
                CMapChunk** slot = &this->m_mapChunks[localX + MAP_CHUNKS_PER_AREA_AXIS * localY];
                CMapChunk* chunk = *slot;

                if (!chunk) {
                    this->PrepareChunk(localX, localY);
                    chunk = *slot;
                }

                if (addToScene && CWorldScene::InsideFrustumRect(chunkRect)) {
#ifdef WOWDECOMP_TOOL_FEATURES
                    if (editorView && !editorView->IncludesTerrain(chunk->m_bbox2)) {
                        continue;
                    }
#endif
                    CWorldScene::AddMapChunk(chunk);
                    chunk->CullAndBucketChunk();
                }
            } else {
                CMapChunk* chunk = this->m_mapChunks[MAP_CHUNKS_PER_AREA_AXIS * localY + localX];

                if (chunk) {
                    this->m_mapChunks[MAP_CHUNKS_PER_AREA_AXIS * chunk->aIndex.y + chunk->aIndex.x] = nullptr;
                    CMap::FreeBaseObjLink(chunk->m_parentLinkList.Head());
                    CMapArea::PurgeChunk(chunk);
                }
            }
        }
    }
}

// VERIFY: 1e0fb652 manual
// OFFSET: 0x007D6D20
void CMapArea::LoadTextures(char* texNames, uint32_t fileNamesSize) {
    STORM_ASSERT(texNames);

    uint32_t offset = 0;
    int32_t streaming = SFile::IsStreamingMode();

    this->m_textures.m_chunk = 16;

    if (!fileNamesSize) {
        return;
    }

    do {
        CMapAreaTexture* areaTexture = this->m_textures.New();
        areaTexture->m_texName = &texNames[offset];
        areaTexture->m_texture = nullptr;

        if (!streaming) {
            this->LoadTerrainTexture(this->m_textures.m_data[this->m_textures.Count() - 1], this->m_textures.Count() - 1);
        }

        while (texNames[offset]) {
            offset++;
        }

        offset++;
    } while (offset < fileNamesSize);
}

// VERIFY: 2e3ea90a manual
// OFFSET: 0x007D6EF0
void CMapArea::ProcessIffChunks() {
    STORM_ASSERT(CMap::bActive);
    STORM_ASSERT(this->rawAreaData);  // 12213: "data"

    SIffChunk* mIffChunk = static_cast<SIffChunk*>(this->rawAreaData);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MVER); // 12213: "mIffChunk->token == 'MVER'"
#if defined(WOWDECOMP_ASSERTIONS)
    const uint32_t adtVersion = *reinterpret_cast<const uint32_t*>(&mIffChunk->m_data);
    STORM_ASSERT(adtVersion == 0x0012);
#endif

    auto* headerChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&mIffChunk->m_data) + mIffChunk->m_size);
    mIffChunk = headerChunk;
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MHDR); // 12213: "mIffChunk->token == 'MHDR'"
    uint8_t* header = reinterpret_cast<uint8_t*>(&headerChunk->m_data);
    this->m_header = reinterpret_cast<SMAreaHeader*>(header);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mcin);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MCIN); // 12213: "mIffChunk->token == 'MCIN'"
    this->m_chunkInfo = reinterpret_cast<SMChunkInfo*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtex);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MTEX); // 12213: "mIffChunk->token == 'MTEX'"
    char* textureNames = reinterpret_cast<char*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mddf);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MDDF); // 12213: "mIffChunk->token == 'MDDF'"
    this->m_doodadDef = reinterpret_cast<SMDoodadDef*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_modf);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MODF); // 12213: "mIffChunk->token == 'MODF'"
    this->m_mapObjDef = reinterpret_cast<SMMapObjDef*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mmdx);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MMDX); // 12213: "mIffChunk->token == 'MMDX'"
    this->m_m2FileNames = reinterpret_cast<char*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mwmo);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MWMO); // 12213: "mIffChunk->token == 'MWMO'"
    this->m_wmoFileNames = reinterpret_cast<char*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mmid);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MMID); // 12213: "mIffChunk->token == 'MMID'"
    this->m_modelFilenamesOffsets = reinterpret_cast<uint32_t*>(&mIffChunk->m_data);
    mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mwid);
    STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MWID); // 12213: "mIffChunk->token == 'MWID'"
    this->m_wmoFilenamesOffsets = reinterpret_cast<uint32_t*>(&mIffChunk->m_data);

    if (this->m_header->m_flags & SMAreaHeader::FLAGS::HAS_FLYING_BBOX) {
        mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mfbo);
        STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MFBO); // 12213: "mIffChunk->token == 'MFBO'"
        this->m_flyingBbox = reinterpret_cast<int16_t*>(&mIffChunk->m_data);
    }

    if (this->m_header->m_mh2o) {
        mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mh2o);
        STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MH2O); // 12213: "mIffChunk->token == 'MH2O'"
        if (mIffChunk->m_size) {
            void* mem = SMemAlloc(sizeof(Liquid::CAreaData), __FILE__, __LINE__, 0);
            Liquid::CAreaData* liquid = mem ? new (mem) Liquid::CAreaData(nullptr) : nullptr;
            this->m_liquid = liquid;
            liquid->SetData(&mIffChunk->m_data);
        }
    }

    if (this->m_header->m_mtxf) {
        mIffChunk = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtxf);
        STORM_ASSERT(mIffChunk->token == SIffChunk::TOKEN_MTXF); // 12213: "mIffChunk->token == 'MTXF'"
        this->m_textureFlags = reinterpret_cast<int32_t*>(&mIffChunk->m_data);
    }

    this->LoadTextures(textureNames, reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtex)->m_size);

    this->m_doodadDefCount = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mddf)->m_size / sizeof(SMDoodadDef);
    this->m_mapObjDefCount = reinterpret_cast<SIffChunk*>(header + this->m_header->m_modf)->m_size / sizeof(SMMapObjDef);
}

// VERIFY: ff270d7a auto
// OFFSET: 0x007D7020
void CMapArea::AsyncCallback(void* param) {
    CMapArea* area = static_cast<CMapArea*>(param);

    area->ProcessIffChunks();
    AsyncFileReadDestroyObject(area->asyncObject);
    area->asyncObject = nullptr;
    area->adtFile = nullptr;
}

// VERIFY: e0ce6011 manual
// OFFSET: 0x007D7050
CMapArea::CMapArea() {
    this->m_index.x = 0;
    this->m_index.y = 0;
    this->m_tileChunkIndex.x = 0;
    this->m_tileChunkIndex.y = 0;
    this->chunkLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->adtFile = nullptr;
    this->asyncObject = nullptr;
    memset(this->m_mapChunks, 0, sizeof(this->m_mapChunks));
    this->rawAreaData = nullptr;
    this->rawAreaDataSize = 0;
    this->unused_008C = 0;
    this->m_doodadDef = nullptr;
    this->m_mapObjDef = nullptr;
    this->m_chunkInfo = nullptr;
    this->m_doodadDefCount = 0;
    this->m_mapObjDefCount = 0;
    this->m_m2FileNames = nullptr;
    this->m_wmoFileNames = nullptr;
    this->m_modelFilenamesOffsets = nullptr;
    this->m_wmoFilenamesOffsets = nullptr;
    this->m_flyingBbox = nullptr;
    this->m_textureFlags = nullptr;
    this->m_liquid = nullptr;
}

// VERIFY: 09d8cf56 auto
// OFFSET: 0x007D7150
void CMapArea::Load(const char* fileName) {
    CMap::SafeOpen(fileName, &this->adtFile);
    this->rawAreaDataSize = SFileGetFileSize(this->adtFile, nullptr);
    STORM_ASSERT(rawAreaDataSize > 0);
    STORM_ASSERT(rawAreaData == NULL);
    this->rawAreaData = CMapArea::AllocAsyncLoadBuffer(this->rawAreaDataSize);
    STORM_ASSERT(rawAreaData != NULL);
    this->asyncObject = AsyncFileReadAllocObject();
    STORM_ASSERT(asyncObject);
    this->asyncObject->file = this->adtFile;
    this->asyncObject->buffer = this->rawAreaData;
    this->asyncObject->size = this->rawAreaDataSize;
    this->asyncObject->userArg = this;
    this->asyncObject->userPostloadCallback = &CMapArea::AsyncCallback;
    AsyncFileReadObject(this->asyncObject, 0);
}

namespace {

struct LiquidChunkAndDistance {
    /* 0000 */ CMapChunk* m_chunk;
    /* 0004 */ CChunkLiquid* m_liquid;
    /* 0008 */ float m_distance;
};

}

// VERIFY: 38a1ef74 manual
// OFFSET: 0x0079BAD0
static void SortLiquidChunks(LiquidChunkAndDistance* array, uint32_t count) {
    if (count < 2) {
        return;
    }

    uint32_t start = (count >> 1) + 1;
    LiquidChunkAndDistance* last = &array[count - 1];

    for (;;) {
        if (start > 1) {
            start--;
        } else {
            LiquidChunkAndDistance swap = array[0];
            array[0] = *last;
            *last = swap;
            count--;

            if (count == 1) {
                return;
            }

            last--;
        }

        uint32_t index = start;
        LiquidChunkAndDistance* node = &array[start - 1];

        while (2 * index <= count) {
            index *= 2;
            LiquidChunkAndDistance* parent = node;
            node = &array[index - 1];

            if (index < count && node[0].m_distance - node[1].m_distance < 0.0f) {
                index++;
                node++;
            }

            if (parent->m_distance - node->m_distance >= 0.0f) {
                break;
            }

            LiquidChunkAndDistance swap = *parent;
            *parent = *node;
            *node = swap;
        }
    }
}

// VERIFY: 11cb6f50 manual
// OFFSET: 0x0079C360
void CMapArea::QueryLiquidSounds(C3Vector* pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results) {
    float relY = -(pos->y - this->m_topLeft2.y);
    int32_t chunkX = CMath::fint_(-(pos->x - this->m_topLeft2.x) * OO_COORD_TO_CHUNK - 0.5f);
    int32_t chunkY = CMath::fint_(relY * OO_COORD_TO_CHUNK - 0.5f);
    // 12213: "radius / (150.0f/36.0f) < 256.0f"
    STORM_ASSERT(radius / SMOLTILE_SIZE < 256.0f);
    int32_t chunkRadius = CMath::fint_(static_cast<float>(ceil(radius * OO_COORD_TO_CHUNK_RADIUS)) - 0.5f);
    int32_t rowStart = chunkX - chunkRadius <= 0 ? 0 : chunkX - chunkRadius;
    int32_t colStart = chunkY - chunkRadius <= 0 ? 0 : chunkY - chunkRadius;
    int32_t rowEnd = chunkX + chunkRadius >= MAP_CHUNKS_PER_AREA_AXIS - 1 ? MAP_CHUNKS_PER_AREA_AXIS - 1 : chunkX + chunkRadius;
    int32_t colEnd = chunkY + chunkRadius >= MAP_CHUNKS_PER_AREA_AXIS - 1 ? MAP_CHUNKS_PER_AREA_AXIS - 1 : chunkY + chunkRadius;
    float radiusSq = radius * radius;

    TSGrowableArray<LiquidChunkAndDistance> chunks;

    for (int32_t row = rowStart; row <= rowEnd; row++) {
        for (int32_t col = colStart; col <= colEnd; col++) {
            CMapChunk* chunk = this->m_mapChunks[row * MAP_CHUNKS_PER_AREA_AXIS + col];

            if (!chunk) {
                continue;
            }

            for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
                float height = (liquid->m_height.m_h + liquid->m_height.m_l) * 0.5f;
                LiquidChunkAndDistance* entry = chunks.New();
                entry->m_chunk = chunk;
                entry->m_liquid = liquid;
                float dx = chunk->m_topLeftCoords.x - HALF_LIQUID_TILE_GRID_SIZE - pos->x;
                float dy = chunk->m_topLeftCoords.y - HALF_LIQUID_TILE_GRID_SIZE - pos->y;
                float dz = height - pos->z;
                entry->m_distance = dx * dx + dz * dz + dy * dy - HALF_LIQUID_TILE_GRID_SIZE_SQ;
            }
        }
    }

    if (!chunks.Count()) {
        return;
    }

    SortLiquidChunks(chunks.m_data, chunks.Count());

    for (LiquidChunkAndDistance* entry = chunks.m_data; entry != chunks.m_data + chunks.Count(); entry++) {
        CChunkLiquid* liquid = entry->m_liquid;

        if (liquid->m_tiles) {
            CMapChunk* chunk = entry->m_chunk;
            float dx = chunk->m_topLeftCoords.x - pos->x;
            float dy = chunk->m_topLeftCoords.y - pos->y;
            float dz = (liquid->m_height.m_h + liquid->m_height.m_l) * 0.5f - pos->z;

            for (uint32_t row = 0; row < LIQUID_TILES_PER_CHUNK_AXIS; row++) {
                for (uint32_t col = 0; col < LIQUID_TILES_PER_CHUNK_AXIS; col++) {
                    if ((reinterpret_cast<const uint8_t*>(liquid->m_tiles)[row * LIQUID_TILES_PER_CHUNK_AXIS + col] & 0xF) != 0xF) {
                        if (dx * dx + dy * dy + dz * dz > radiusSq) {
                            continue;
                        }

                        World::LiquidQueryResult* result = results.New();
                        result->m_liquidType = liquid->m_type;
                        result->m_offset.x = dx;
                        result->m_offset.y = dy;
                        result->m_offset.z = dz;
                    }

                    dy -= SMOLTILE_SIZE;
                }

                dy += LIQUID_TILE_GRID_SIZE;
                dx -= SMOLTILE_SIZE;
            }
        } else {
            CMapChunk* chunk = entry->m_chunk;
            float relTileY = -(pos->y - chunk->m_topLeftCoords.y);
            float relTileX = -(pos->x - chunk->m_topLeftCoords.x);
            int32_t tileRadius = CMath::fint_(static_cast<float>(ceil(radius * OOSMOLTILE_SIZE)) - 0.5f);
            int32_t tileY = CMath::fint_(relTileY * OO_COORD_TO_SUBCHUNK - 0.5f);
            int32_t tileX = CMath::fint_(relTileX * OO_COORD_TO_SUBCHUNK - 0.5f);
            int32_t tileRowStart = tileX - tileRadius;
            int32_t tileColStart = tileY - tileRadius;
            int32_t tileRowEnd = tileX + tileRadius + 1;
            int32_t tileColEnd = tileY + tileRadius + 1;

            if (tileColEnd >= liquid->m_tileEnd.y) {
                tileColEnd = liquid->m_tileEnd.y;
            }

            if (tileRowEnd >= liquid->m_tileEnd.x) {
                tileRowEnd = liquid->m_tileEnd.x;
            }

            if (tileColStart <= liquid->m_tileBegin.y) {
                tileColStart = liquid->m_tileBegin.y;
            }

            if (tileRowStart <= liquid->m_tileBegin.x) {
                tileRowStart = liquid->m_tileBegin.x;
            }

            if (tileRowStart >= tileRowEnd || tileColStart >= tileColEnd) {
                continue;
            }

            float dx = chunk->m_topLeftCoords.x - static_cast<float>(tileRowStart) * SMOLTILE_SIZE - HALF_SMOLTILE_SIZE - pos->x;
            float dy = chunk->m_topLeftCoords.y - static_cast<float>(tileColStart) * SMOLTILE_SIZE - HALF_SMOLTILE_SIZE - pos->y;
            float dz = (liquid->m_height.m_h + liquid->m_height.m_l) * 0.5f - pos->z;

            int32_t colBegin = tileColStart - liquid->m_tileBegin.y;
            int32_t colEnd = tileColEnd - liquid->m_tileBegin.y;
            int32_t colSpan = colEnd - colBegin;
            int32_t rowLast = tileRowEnd - liquid->m_tileBegin.x;
            int32_t stride = liquid->m_tileEnd.y - liquid->m_tileBegin.y;
            int32_t rowFirst = tileRowStart - liquid->m_tileBegin.x;

            if (rowFirst >= rowLast) {
                continue;
            }

            float rowStep = static_cast<float>(colSpan) * SMOLTILE_SIZE;
            int32_t bitIndex = rowFirst * stride;

            for (int32_t rows = rowLast - rowFirst; rows; rows--) {
                for (int32_t col = colBegin; col < colEnd; col++) {
                    if (liquid->m_tileBits.Get(bitIndex + col) && dy * dy + dx * dx + dz * dz <= radiusSq) {
                        World::LiquidQueryResult* result = results.New();
                        result->m_liquidType = liquid->m_type;
                        result->m_offset.x = dx;
                        result->m_offset.y = dy;
                        result->m_offset.z = dz;
                    }

                    dy -= SMOLTILE_SIZE;
                }

                bitIndex += stride;
                dy += rowStep;
                dx -= SMOLTILE_SIZE;
            }
        }
    }
}

// VERIFY: e54d69d6 manual
// OFFSET: [0x007D6E10, 0x007D7120]
CMapArea::~CMapArea() {
    STORM_ASSERT(adtFile == NULL);
    STORM_ASSERT(asyncObject == NULL);

    if (this->m_liquid) {
        SMemFree(this->m_liquid, __FILE__, __LINE__, 0);
        this->m_liquid = nullptr;
    }

    if (this->rawAreaData) {
        CMapArea::FreeAsyncLoadBuffer(this->rawAreaData, this->rawAreaDataSize);
    }

    this->unused_008C = 0;
    this->m_doodadDef = nullptr;
    this->m_mapObjDef = nullptr;
    this->m_chunkInfo = nullptr;
    this->m_doodadDefCount = 0;
    this->m_mapObjDefCount = 0;
    this->m_m2FileNames = nullptr;
    this->m_wmoFileNames = nullptr;
    this->m_modelFilenamesOffsets = nullptr;
    this->m_wmoFilenamesOffsets = nullptr;
}
