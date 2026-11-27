#include "Map.h"
#include "WoW/Source/Object/ObjectClient/Types.h"
#include "WoW/Source/WorldClient/WorldLightingConstants.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#if defined(_MSC_VER)
#include <cfloat>
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include <cstring>
#include <cmath>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/CWFrustum.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/FVBBList.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapLoad.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/WaterRipple.h"
#include "WoW/Source/WorldClient/World.h"
#ifdef WOWDECOMP_TOOL_FEATURES
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#endif
#include "WoW/Source/WorldClient/WorldView.h"
#include "WoW/Source/DB/Rec/GroundEffectTextureRec.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/DB/Rec/TerrainTypeRec.h"

static const int32_t MAXALPHA = 127;
static const float FOOTPRINT_FADEOUT_RATE = 0.00016666666f;

static const float MD_RIVER_DEPTH_SCALE = 0.11111111f;

// STATIC: 0x00CF4938
TSGrowableArray<CGxVertexPC> CMap::debugVertexArray;

// STATIC: 0x00CF4948
TSGrowableArray<uint16_t> CMap::debugIndexArray;

// STATIC: 0x00CE04A8
CMapLight* CMap::s_mapLight;

// STATIC: 0x00ADF800
float CMap::s_liquidAlphaScale = 0.58333333f;

// STATIC: 0x00ADFBC4
int32_t s_mapId = -1;

// STATIC: 0x00ADFBCC
FVBBList CMap::s_vbbList;

// STATIC: 0x00AEED74
TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> CMap::s_mapRenderChunkFreeList;

// STATIC: 0x00AEED80
TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, m_link)> CMap::s_chunkBufFreeList;

// STATIC: 0x00AEED8C
TSList<CMapArea, TSGetExplicitLink<CMapArea> > CMap::areaList = TSExplicitList<CMapArea, offsetof(CMapArea, m_lameAssLink)>();

// STATIC: 0x00AEEDA4
TSList<CMapChunk, TSGetExplicitLink<CMapChunk> > CMap::chunkList = TSExplicitList<CMapChunk, offsetof(CMapChunk, m_lameAssLink)>();

// STATIC: 0x00AEEDBC
TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > CMap::mapObjDefGroupList = TSExplicitList<CMapObjDefGroup, offsetof(CMapObjDefGroup, m_lameAssLink)>();

// STATIC: 0x00AEEDC8
TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > CMap::entityList = TSExplicitList<CMapEntity, offsetof(CMapEntity, m_lameAssLink)>();

// STATIC: 0x00AEEDD4
TSList<CMapLight, TSGetExplicitLink<CMapLight> > CMap::lightList = TSExplicitList<CMapLight, offsetof(CMapLight, m_lameAssLink)>();

// STATIC: 0x00ADFBEC
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::areaLinkList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00ADFC28
TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> CMap::mapRenderChunkUpdateList;

// STATIC: 0x00CE049D
bool CMap::enableSpecularTerrain;

// STATIC: 0x00CE05D0
char CMap::wdtFilename[256];

// STATIC: 0x00CE06D0
char CMap::mapName[256];

// STATIC: 0x00CE07D0
char CMap::mapPath[256];

// STATIC: 0x00CDF7D0
float CMap::s_depthCoordTable0[256];

// STATIC: 0x00CDFBD0
float CMap::s_depthCoordTable1[256];

// STATIC: 0x00CDFFF8
CGxBuf* CMap::lowDetailIndexBuf;

// STATIC: 0x00CDFFFC
CGxPool* CMap::lowDetailIndexPool;

// STATIC: 0x00CE0000
CGxShader* CMap::pixelShader_TerrainSM;

// STATIC: 0x00CE0004
CGxShader* CMap::pixelShader_Terrain0_env;

// STATIC: 0x00CE0008
CGxShader* CMap::vertexShader_Terrain[128];

// STATIC: 0x00CE0208
CGxShader* CMap::pixelShader_Terrain3[96];

// STATIC: 0x00CE0388
CGxShader* CMap::pixelShader_Terrain2[32];

// STATIC: 0x00CE0408
CGxShader* CMap::pixelShader_Terrain1[32];

// STATIC: 0x00CE0488
CGxShader* CMap::pixelShader_Terrain0[3];

// STATIC: 0x00CE04A4
int32_t CMap::uniqueId;

// STATIC: 0x00CE04AC
int32_t CMap::oldSelectLightParm;

// STATIC: 0x00CE04C4
uint32_t CMap::s_queryTag;

// STATIC: 0x00CE04C8
uint32_t CMap::cCount;

// STATIC: 0x00CE08D0
CMapAreaMed* CMap::areaMedArray[MAP_AREA_COUNT];

// STATIC: 0x00CE48D0
CMapArea* CMap::m_areaTable[MAP_AREA_COUNT];

// STATIC: 0x00CE88D0
SMAreaInfo CMap::m_areaInfo[MAP_AREA_COUNT];

// STATIC: 0x00CF08F0
int32_t CMap::bActive;

// STATIC: 0x00CF08F4
int32_t CMap::bDungeon;

// STATIC: 0x00CF08F8
BOOL CMap::s_useSSE;

// STATIC: 0x00CF4928
TSGrowableArray<uint32_t> CMap::scCollideList;

// STATIC: 0x00D253A4
int32_t CMap::freeCounts[11];

// STATIC: 0x00D253D0
int32_t CMap::counts[11];

// STATIC: 0x00D253FC
uint32_t* CMap::lightHeap;

// STATIC: 0x00D25400
uint32_t* CMap::cacheLightHeap;

// STATIC: 0x00D25404
uint32_t* CMap::mapObjGroupHeap;

// STATIC: 0x00D25408
uint32_t* CMap::mapObjHeap;

// STATIC: 0x00D2540C
uint32_t* CMap::baseObjLinkHeap;

// STATIC: 0x00D25410
uint32_t* CMap::areaHeap;

// STATIC: 0x00D25414
uint32_t* CMap::areaMedHeap;

// STATIC: 0x00D25418
CMapAreaLowHeap* CMap::areaLowHeap;

// STATIC: 0x00D2541C
uint32_t* CMap::chunkHeap;

// STATIC: 0x00D25420
uint32_t* CMap::doodadDefHeap;

// STATIC: 0x00D25424
uint32_t* CMap::entityHeap;

// STATIC: 0x00CF4958
static uint32_t s_nextPlayerFootprint;

// STATIC: 0x00CF495C
static uint32_t s_nextFootprint;

// STATIC: 0x00A405E8
static const int32_t s_subVertexTriangles[12] = { 17, 9, 0, 9, 1, 0, 9, 17, 18, 9, 18, 1 };

// STATIC: 0x00A40618
static const int32_t s_subVertexIndexFlat[5] = { 0, 9, 17, 1, 18 };

// STATIC: 0x00ADFD04
static World::FacetData s_footprintFacets;

// STATIC: 0x00CF4960
static CMapFootprint s_footprints[144][4];

// STATIC: 0x00CFBE60
static TSGrowableArray<CMapFootprintTexture> s_footprintTextures;

// STATIC: 0x00D25428
uint32_t* CMap::mapObjDefGroupHeap;

// STATIC: 0x00D2542C
uint32_t* CMap::mapObjDefHeap;

// STATIC: 0x00D25430
uint32_t* CMap::chunkLiquidHeap;

// STATIC: 0x00ADFC10
TSList<CMapDoodadDef, TSGetExplicitLink<CMapDoodadDef> > CMap::doodadDefList = TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_doodadDefLink)>();

// STATIC: 0x00ADFC64
C3Vector CMap::s_subchunkCoords[5] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, -SMOLTILE_SIZE, 0.0f), C3Vector(-SMOLTILE_SIZE, -SMOLTILE_SIZE, 0.0f), C3Vector(-SMOLTILE_SIZE, 0.0f, 0.0f), C3Vector(-HALF_SMOLTILE_SIZE, -HALF_SMOLTILE_SIZE, 0.0f) };

// STATIC: 0x00A3FAF0
uint32_t CMap::s_holeMask[16] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

// STATIC: 0x00A3FB30
int32_t CMap::s_fanIndices[8] = { 17, 0, 0, 1, 18, 17, 1, 18 };

// STATIC: 0x00A3FB50
int32_t CMap::s_subchunkIndices[8] = { 3, 0, 0, 1, 2, 3, 1, 2 };

// STATIC: 0x00A3FB88
uint16_t CMap::s_2bitSplatMask[8] = { 0x3, 0xC, 0x30, 0xC0, 0x300, 0xC00, 0x3000, 0xC000 };

// STATIC: 0x00A3FB98
uint32_t CMap::s_2bitSplatShft[8] = { 0, 2, 4, 6, 8, 10, 12, 14 };

// STATIC: 0x00D25434
TSHashTable<CMapObjDef, HASHKEY_NONE> CMap::mapObjDefHash;

// STATIC: 0x00CE0498
int32_t CMap::enableChunkBatching;

// STATIC: 0x00ADFBB4
const DepthCoordTable* const CMap::s_depthCoordTables[DEPTH_TABLE_CNT] = { &CMap::s_depthCoordTable0, &CMap::s_depthCoordTable1 };

// STATIC: 0x00AEEDB0
TSList<CChunkLiquid, TSGetExplicitLink<CChunkLiquid> > CMap::chunkLiquidList = TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_link)>();

// STATIC: 0x00ADFC34
TSList<CChunkLiquid, TSGetExplicitLink<CChunkLiquid> > CMap::mapChunkLiquidUpdateList = TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_updateLink)>();

// STATIC: 0x00CF0900
CMapLowDetail CMap::s_lowDetail;

// STATIC: 0x00CF08D0
SMMapHeader CMap::header;

// STATIC: 0x00CE04CC
uint32_t CMap::version;

// STATIC: 0x00ADFBC8
int32_t CMap::bPreload = 1;

// STATIC: 0x00CE0494
BOOL CMap::s_isStreamingMode;

// STATIC: 0x00CE049C
bool CMap::s_enableTriangleStrips;

// STATIC: 0x00CE04A0
bool CMap::enableSpecular;

// STATIC: 0x00CE04A1
bool CMap::enablePixelShaders;

// STATIC: 0x00CE04A2
bool CMap::enableVertexShaders;

// STATIC: 0x00CE049E
bool CMap::gTerrainPixelShadersValid;

// STATIC: 0x00CE049F
bool CMap::enableTerrainShaderVertex;

// STATIC: 0x00CE04A3
HASHKEY_NONE CMap::s_mapObjDefHashKey;

// STATIC: 0x00ADFBF8
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::doodadDefLinkList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00ADFC04
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::mapObjDefLinkList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00AF16CC
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::s_globalLightList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00ADFC1C
TSList<CMapChunk, TSGetExplicitLink<CMapChunk> > CMap::mapChunkUpdateList = TSExplicitList<CMapChunk, offsetof(CMapChunk, m_sortListLink)>();

// STATIC: 0x00D2545C
TSHashTable<CMapDoodadDef, HASHKEY_DWORD> CMap::doodadDefHash;

// STATIC: 0x00AEEDE0
TSExplicitList<CDestructibleProxy, offsetof(CDestructibleProxy, m_link)> CMap::destructibleProxies;

// STATIC: 0x00CE04BC
int32_t CMap::mapGetFacetsCount;

// STATIC: 0x00CE04C0
int32_t CMap::bspRecurseCount;

// STATIC: 0x00ADFC40
TSList<CDetailDoodadInst, TSGetExplicitLink<CDetailDoodadInst> > CMap::mapDetailDoodadUpdateList = TSExplicitList<CDetailDoodadInst, offsetof(CDetailDoodadInst, m_link)>();

// STATIC: 0x00ADFD90
static const int32_t s_flightBoundsTriangles[24] = { 3, 0, 4, 0, 1, 4, 1, 2, 4, 2, 5, 4, 5, 8, 4, 8, 7, 4, 7, 6, 4, 6, 3, 4 };

// STATIC: 0x00ADFDF0
static const float s_flightBoundsOffsets[9][2] = {
    { 0.0f, 0.0f },
    { 0.0f, -266.66666f },
    { 0.0f, -MD_AREA_POLY_SIZE },
    { -266.66666f, 0.0f },
    { -266.66666f, -266.66666f },
    { -266.66666f, -MD_AREA_POLY_SIZE },
    { -MD_AREA_POLY_SIZE, 0.0f },
    { -MD_AREA_POLY_SIZE, -266.66666f },
    { -MD_AREA_POLY_SIZE, -MD_AREA_POLY_SIZE },
};

// STATIC: 0x00CD7768
WOWGUID s_lastCollisionGUID;

// VERIFY: 738eb1a4 manual
// OFFSET: 0x0079B440
CMapArea* CMap::GetLoadedArea(int32_t areaX, int32_t areaY) {
    CMapArea* area = CMap::GetArea(areaX, areaY);

    if (area && area->asyncObject) {
        return nullptr;
    }

    return area;
}

// VERIFY: ef54a4a5 auto
// OFFSET: 0x0079B870
const DepthCoordTable* CMap::GetDepthCoordTable(uint32_t liquidType) {
    if (!liquidType) {
        return nullptr;
    }

    LiquidTypeRec* liquidTypeRec = g_liquidTypeDB.GetRecord(liquidType);

    if (!liquidTypeRec) {
        return nullptr;
    }

    LiquidMaterialRec* materialRec = g_liquidMaterialDB.GetRecord(liquidTypeRec->m_materialID);

    if (!materialRec || (materialRec->m_lvf != LIQUID_VERTEX_HEIGHT_DEPTH && materialRec->m_lvf != LIQUID_VERTEX_DEPTH_ONLY)) {
        return nullptr;
    }

    STORM_ASSERT(liquidTypeRec->m_int[0]<DEPTH_TABLE_CNT);
    return CMap::s_depthCoordTables[liquidTypeRec->m_int[0]];
}

// VERIFY: e651a57d manual
// OFFSET: 0x0079E5C0
CGxShader* CMap::GetTerrainPixelShader(int32_t bigAlpha, int32_t layers, int32_t shadowMode, int32_t specular, int32_t unlitLayers) {
    const int32_t layerIndex = layers - 1;
    STORM_ASSERT(static_cast<uint32_t>(layerIndex) < 4); // 12213: "layers < 4"

    switch (shadowMode) {
        case 0:
            return CMap::pixelShader_Terrain1[16 * unlitLayers + 8 * bigAlpha + 2 * layerIndex + specular];

        case 1:
            if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == GxShPS_ps_2_0 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == GxShPS_arbfp1) {
                return CMap::pixelShader_Terrain2[16 * unlitLayers + 8 * bigAlpha + 2 * layerIndex + specular];
            }

            return CMap::pixelShader_Terrain3[48 * unlitLayers + 24 * bigAlpha + 2 * layerIndex + specular];

        case 2:
        case 3:
            return CMap::pixelShader_Terrain3[48 * unlitLayers - 8 + 24 * bigAlpha + 8 * shadowMode + 2 * layerIndex + specular];

        default:
            return nullptr;
    }
}

// VERIFY: 2fffff6e manual
// OFFSET: 0x0079E730
void CMap::ClearChunkDetailDoodads() {
    for (CMapChunk* chunk = CMap::chunkList.Head(); chunk; chunk = CMap::chunkList.Next(chunk)) {
        if (chunk->detailDoodadInst) {
            CDetailDoodad::FreeInst(chunk->detailDoodadInst);
            chunk->detailDoodadInst = nullptr;
        }
    }
}

// VERIFY: 77579752 manual
// OFFSET: 0x0079E780
void CMap::ClearAllRenderChunks() {
    for (CMapChunk* chunk = CMap::chunkList.Head(); chunk; chunk = CMap::chunkList.Next(chunk)) {
        if (chunk->m_renderChunk) {
            chunk->m_renderChunk->FreeBuf();
        }
    }
}

// VERIFY: 9b396a3d manual
// OFFSET: 0x0079EFF0
void CDestructibleProxy::FindOrCreate(CDestructibleProxy* proxy) {
    if (!proxy->m_uniqueId) {
        CMap::destructibleProxies.LinkToHead(proxy);
        return;
    }

    for (CDestructibleProxy* node = CMap::destructibleProxies.Head(); !CMap::destructibleProxies.IsEnd(node); node = CMap::destructibleProxies.RawNext(node)) {
        if (node->m_uniqueId == proxy->m_uniqueId) {
            return;
        }
    }

    CDestructibleProxy* created = CMap::destructibleProxies.NewNode(TSLIST_LINK_BEFORE, 0, 0);
    created->unused_0000 = proxy->unused_0000;
    created->m_sphere = proxy->m_sphere;
    created->m_uniqueId = proxy->m_uniqueId;
    CMap::destructibleProxies.LinkToHead(created);
}

// VERIFY: ee012d34 manual
// OFFSET: 0x0079F210
void CDestructibleProxy::FreeMatching(CDestructibleProxy* proxy) {
    CDestructibleProxy* next;

    for (CDestructibleProxy* node = CMap::destructibleProxies.Head(); node; node = next) {
        next = CMap::destructibleProxies.Next(node);

        if (proxy == node || (proxy->m_uniqueId && node->m_uniqueId == proxy->m_uniqueId)) {
            CMap::destructibleProxies.UnlinkNode(node);

            if (proxy != node) {
                CMap::destructibleProxies.DeleteNode(node);
            }
        }
    }
}

// VERIFY: e85ab889 manual
// OFFSET: 0x0079F320
void CMap::Destroy() {
    if (CMap::bActive) {
        CMap::Unload();
    }

    STORM_ASSERT(areaList.Head() == nullptr); // 12213: "areaList.Head() == __null"
#if defined(WOWDECOMP_ASSERTIONS)
    for (const CMapAreaMed* areaMed : areaMedArray) {
        STORM_ASSERT(areaMed == nullptr); // 12213: "areaMedList.Head() == __null"
    }
#endif
    STORM_ASSERT(chunkList.Head() == nullptr); // 12213: "chunkList.Head() == __null"
    STORM_ASSERT(entityList.Head() == nullptr); // 12213: "entityList.Head() == __null"
    STORM_ASSERT(mapObjDefHash.Head() == nullptr); // 12213: "mapObjDefHash.Head() == __null"
    STORM_ASSERT(mapObjDefGroupList.Head() == nullptr); // 12213: "mapObjDefGroupList.Head() == __null"
    STORM_ASSERT(doodadDefHash.Head() == nullptr); // 12213: "doodadDefHash.Head() == __null"
    STORM_ASSERT(chunkLiquidList.Head() == nullptr); // 12213: "chunkLiquidList.Head() == __null"
    STORM_ASSERT(areaLinkList.Head() == nullptr); // 12213: "areaLinkList.Head() == __null"
    STORM_ASSERT(doodadDefLinkList.Head() == nullptr); // 12213: "doodadDefLinkList.Head() == __null"
    STORM_ASSERT(mapObjDefLinkList.Head() == nullptr); // 12213: "mapObjDefLinkList.Head() == __null"
    STORM_ASSERT(destructibleProxies.Head() == nullptr); // 12213: "destructibleProxies.Head() == __null"

    WaterRipple::Destroy();
    CMapObj::Destroy();
    CMapChunk::Destroy();
    CMapArea::Destroy();
    CDetailDoodad::Destroy();
    CMap::FootprintDestroy();

    for (uint32_t i = 0; i < sizeof(CMap::vertexShader_Terrain) / sizeof(CMap::vertexShader_Terrain[0]); i++) {
        if (CMap::vertexShader_Terrain[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::vertexShader_Terrain[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain0) / sizeof(CMap::pixelShader_Terrain0[0]); i++) {
        if (CMap::pixelShader_Terrain0[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain0[i]);
        }
    }

    if (CMap::pixelShader_Terrain0_env) {
        g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain0_env);
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain1) / sizeof(CMap::pixelShader_Terrain1[0]); i++) {
        if (CMap::pixelShader_Terrain1[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain1[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]); i++) {
        if (CMap::pixelShader_Terrain2[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain2[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]); i++) {
        if (CMap::pixelShader_Terrain3[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain3[i]);
        }
    }

    if (CMap::pixelShader_TerrainSM) {
        g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_TerrainSM);
    }

    CMap::pixelShader_TerrainSM = nullptr;

    CGxBuf* indexBuf = CMap::lowDetailIndexBuf;

    if (indexBuf && indexBuf != g_theGxDevicePtr->BufStream(indexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(indexBuf);
    }

    if (CMap::lowDetailIndexPool) {
        g_theGxDevicePtr->PoolDestroy(CMap::lowDetailIndexPool);
    }

    CMap::s_vbbList.Destroy();

    if (CMap::lightHeap) {
        SMemFree(CMap::lightHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::cacheLightHeap) {
        SMemFree(CMap::cacheLightHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjGroupHeap) {
        SMemFree(CMap::mapObjGroupHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjHeap) {
        SMemFree(CMap::mapObjHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::baseObjLinkHeap) {
        SMemFree(CMap::baseObjLinkHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaHeap) {
        SMemFree(CMap::areaHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaMedHeap) {
        SMemFree(CMap::areaMedHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaLowHeap) {
        SMemFree(CMap::areaLowHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::chunkHeap) {
        SMemFree(CMap::chunkHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::doodadDefHeap) {
        SMemFree(CMap::doodadDefHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::entityHeap) {
        SMemFree(CMap::entityHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjDefGroupHeap) {
        SMemFree(CMap::mapObjDefGroupHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjDefHeap) {
        SMemFree(CMap::mapObjDefHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::chunkLiquidHeap) {
        SMemFree(CMap::chunkLiquidHeap, "delete", SLOG_FUNCTION, 0);
    }

    CMap::s_mapRenderChunkFreeList.Clear();
    CMap::s_chunkBufFreeList.Clear();
}

// VERIFY: f42871fd manual
// OFFSET: 0x0079E3C0
void CMap::InitializeLiquidTables() {
    float scale = 1.0f / (255.0f * CMap::s_liquidAlphaScale);

    for (uint32_t i = 0; i < sizeof(CMap::s_depthCoordTable1) / sizeof(CMap::s_depthCoordTable1[0]); i++) {
        float alpha = static_cast<float>(i) * CMap::s_liquidAlphaScale * scale;
        CMap::s_depthCoordTable1[i] = alpha >= 1.0f ? 1.0f : alpha;

        float depth = static_cast<float>(i) * MD_RIVER_DEPTH_SCALE;
        CMap::s_depthCoordTable0[i] = depth <= 4.6666665f ? depth * 0.21428572f : 1.0f;
    }

    WaterRipple::Initialize();
    Liquid::CreateProceduralTextures();
}

// VERIFY: cd6270d0 manual
// OFFSET: 0x0079E4F0
void CMap::InitializePCFShaders() {
    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]); i++) {
        if (CMap::pixelShader_Terrain2[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain2[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]); i++) {
        if (CMap::pixelShader_Terrain3[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain3[i]);
        }
    }

    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain2, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain2_pcf" : "Terrain2", sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]));
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain3, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain3_pcf" : "Terrain3", sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]));
}

// VERIFY: 98a7e674 manual
// OFFSET: 0x0079E7C0
void CMap::Initialize() {
    CMapArea::Initialize();
    CMapChunk::Initialize();
    CMapObj::Initialize();
    CMapObjGroup::Initialize();
    CDetailDoodad::Initialize();
    CMap::FootprintInitialize();

    memset(CMap::counts, 0, sizeof(CMap::counts));
    memset(CMap::freeCounts, 0, sizeof(CMap::freeCounts));
    memset(CMap::areaMedArray, 0, sizeof(CMap::areaMedArray));
    memset(CMap::m_areaTable, 0, sizeof(CMap::m_areaTable));

    for (uint32_t i = 0; i < sizeof(CMap::m_areaInfo) / sizeof(CMap::m_areaInfo[0]); i++) {
        CMap::m_areaInfo[i].m_flags = 0;
    }

    CMap::scCollideList.SetCount(CMap::COLLIDE_LIST_SIZE);

    CMap::cCount = 0;
    CMap::s_queryTag = 0;
    CMap::uniqueId = -2;
    s_mapId = -1;
    CMap::bDungeon = 0;
    CMap::bActive = 0;
    CMap::oldSelectLightParm = 0;

    CMap::InitializeLiquidTables();

    memset(CMap::vertexShader_Terrain, 0, sizeof(CMap::vertexShader_Terrain));

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain0) / sizeof(CMap::pixelShader_Terrain0[0]); i++) {
        CMap::pixelShader_Terrain0[i] = nullptr;
    }

    CMap::pixelShader_Terrain0_env = nullptr;

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain1) / sizeof(CMap::pixelShader_Terrain1[0]); i++) {
        CMap::pixelShader_Terrain1[i] = nullptr;
    }

    CMap::InitializePCFShaders();
    CMap::pixelShader_TerrainSM = nullptr;

    g_theGxDevicePtr->ShaderCreate(CMap::vertexShader_Terrain, GxSh_Vertex, "Shaders\\Vertex", "Terrain", 128);
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain0, GxSh_Pixel, "Shaders\\Pixel", "Terrain0", 3);
    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain0_env, GxSh_Pixel, "Shaders\\Pixel", "Terrain0_env", 1);

    switch (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel]) {
        case GxShPS_ps_1_1: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 6);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 6);
            break;
        }

        case GxShPS_ps_1_4: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 8);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[9], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[10], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[11], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[12], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[13], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[14], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_4", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[15], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_4", 1);
            break;
        }

        case GxShPS_nvts:
        case GxShPS_nvts2:
        case GxShPS_nvts3: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 4);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 4);
            break;
        }

        default: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 32);
            break;
        }
    }

    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_TerrainSM, GxSh_Pixel, "Shaders\\Pixel", "TerrainSM", 1);

    CMap::lowDetailIndexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Dynamic, 0x1800, static_cast<EGxPoolHintBits>(0), "CMap::lowDetailIndexPool");
    CMap::lowDetailIndexBuf = GxBufCreate(CMap::lowDetailIndexPool, sizeof(uint16_t), 0xC00, 0);
    CMap::s_vbbList.Initialize(GxPoolTarget_Vertex, GxPoolUsage_Dynamic, 16, 0x221, 24);

    CMap::lightHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapLight), 128, "WLIGHT", true));
    CMap::cacheLightHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapCacheLight), 256, "WCACHELIGHT", true));
    CMap::mapObjGroupHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjGroup), 128, "WMAPOBJGROUP", true));
    CMap::mapObjHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObj), 32, "WMAPOBJ", true));
    CMap::baseObjLinkHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapBaseObjLink), 10000, "WBASEOBJLINK", true));
    CMap::areaHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapArea), 16, "WAREA", true));
    CMap::areaMedHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapAreaMed), 16, "WAREAMED", true));
    CMap::areaLowHeap = new (__FILE__, __LINE__) CMapAreaLowHeap{ ObjectAllocAddHeap(sizeof(CMapAreaLow), 16, "WAREALOW", true) };
    CMap::chunkHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapChunk), 256, "WCHUNK", true));
    CMap::doodadDefHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapDoodadDef), 5000, "WDOODADDEF", true));
    CMap::entityHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapEntity), 128, "WENTITY", true));
    CMap::mapObjDefGroupHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjDefGroup), 128, "WMAPOBJDEFGROUP", true));
    CMap::mapObjDefHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjDef), 64, "WMAPOBJDEF", true));
    CMap::chunkLiquidHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CChunkLiquid), 64, "WCHUNKLIQUID", true));

    int32_t vendor;

    if (OsGetProcessorFeaturesEx(vendor) & OS_PROCESSOR_FEATURE_SSE) {
        CMap::s_useSSE = 1;
    }
}

// INLINED
CMapFootprintTexture::CMapFootprintTexture() {
    this->m_texture = nullptr;
    this->m_vertexCount = 0;
}

// VERIFY: fa585208 manual
// OFFSET: 0x0079F860
void CMap::FootprintDestroy() {
    for (uint32_t i = 0; i < 144; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            s_footprints[i][j].m_link.Unlink();
        }
    }

    for (uint32_t i = 0; i < s_footprintTextures.Count(); i++) {
        if (s_footprintTextures[i].m_texture) {
            HandleClose(reinterpret_cast<HOBJECT>(s_footprintTextures[i].m_texture));
        }
    }
}

// VERIFY: 3f19b093 manual
// OFFSET: 0x0079FA10
void CMap::ClearAllFootprints() {
    for (uint32_t i = 0; i < 144; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            s_footprints[i][j].m_vertices.SetCount(0);
            s_footprints[i][j].m_link.Unlink();
        }
    }
}

// VERIFY: bc21f929 manual
// OFFSET: 0x007A03C0
void CMap::FootprintInitialize() {
    CStatus status;

    s_nextFootprint = 64;
    s_nextPlayerFootprint = 0;
    s_footprintTextures.SetCount(g_footprintTexturesDB.GetMaxID() + 1);

    int32_t index = g_footprintTexturesDB.GetNumRecords();

    while (index) {
        index--;

        FootprintTexturesRec* record = (index < 0 || index >= g_footprintTexturesDB.GetNumRecords()) ? nullptr : &g_footprintTexturesDB.m_records[index];
        CMapFootprintTexture* footprint = &s_footprintTextures[record->m_id];

        if (!footprint->m_texture) {
            footprint->m_texture = TextureCreate(record->m_footstepFilename, CGxTexFlags(), &status, 1);
        }
    }
}


// VERIFY: 7ce5c762 manual
// OFFSET: 0x0079E4B0
CGxShader* CMap::GetPixelShader(bool bigAlpha, bool baseLayer, bool specular) {
    if (!bigAlpha) {
        return CMap::pixelShader_Terrain0[0];
    }

    if (!baseLayer) {
        return CMap::pixelShader_Terrain0[2];
    }

    if (specular) {
        return CMap::pixelShader_Terrain0_env;
    }

    return CMap::pixelShader_Terrain0[1];
}

// VERIFY: 70d4ac15 manual
// OFFSET: 0x007A06A0
bool CMap::QueryShadow(C3Vector* pos) {
    float y = -(pos->y - HALF_WORLD_SIZE);
    float x = -(pos->x - HALF_WORLD_SIZE);

    if (y < 0.0f || x < 0.0f || y >= 34133.332f || x >= 34133.332f) {
        return false;
    }

    int32_t cellX = CMath::fint_(y * OO_COORD_TO_CHUNK - 0.5f);
    int32_t cellY = CMath::fint_(x * OO_COORD_TO_CHUNK - 0.5f);
    CMapArea* area = CMap::GetLoadedArea((cellX >> 4) & MAP_AREA_INDEX_MASK, (cellY >> 4) & MAP_AREA_INDEX_MASK);

    if (!area) {
        return false;
    }

    CMapChunk* chunk = area->GetChunk(cellX, cellY);

    if (!chunk || !chunk->m_shadowMap) {
        return false;
    }

    int32_t shadowX = CMath::fint_(y * OO_COORD_TO_SHADOW - 0.5f) & 0x3F;
    int32_t shadowY = CMath::fint_(x * OO_COORD_TO_SHADOW - 0.5f) & 0x3F;
    uint8_t mamp = area->m_header->m_mampValue;
    int32_t bitX = shadowX >> mamp;

    return (chunk->m_shadowMap[(bitX >> 3) + (shadowY >> mamp) * (8 >> mamp)] & (1 << (bitX & 7))) != 0;
}

// VERIFY: cdb37c44 manual
// OFFSET: 0x007A0820
bool CMap::QueryLiquidTerrain(const C3Vector* pos, int32_t* liquidType, float* height, int32_t checkGround) {
    float cellYf = -(pos->y - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK;
    float cellXf = OO_COORD_TO_SUBCHUNK * -(pos->x - HALF_WORLD_SIZE);
    int32_t cellX = CMath::fint_(cellYf - 0.5f);
    int32_t cellY = CMath::fint_(cellXf - 0.5f);

    CMapArea* area = CMap::GetAreaForCell(cellX, cellY);

    if (!area || area->asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->GetChunkForCell(cellX, cellY);

    if (!chunk) {
        return false;
    }

    C2iVector tile;
    tile.x = cellX & MAP_CELL_INDEX_MASK;
    tile.y = cellY & MAP_CELL_INDEX_MASK;

    C2Vector frac(cellYf - cellX, cellXf - cellY);

    for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
        if (liquid->TileExists(tile.x, tile.y) && liquid->GetHeight(frac, tile, *height) && *height + 0.01f > pos->z) {
            if (!checkGround) {
                *liquidType = liquid->m_type;
                return true;
            }

            float ground = -10000.0f;
            CMap::GetHeightTerrain(chunk, *const_cast<C3Vector*>(pos), cellX, cellY, &ground);

            if (pos->z + 0.01f > ground) {
                *liquidType = liquid->m_type;
                return true;
            }
        }
    }

    return false;
}

// VERIFY: c7588881 manual
// OFFSET: 0x007A09D0
bool CMap::QueryLiquidStatusMapObjsExt(C3Vector* pos, int32_t* liquidType, float* height) {
    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) {
            continue;
        }

        const CAaBox& box = mapObjDef->m_bbox;

        if (box.b.x > pos->x || box.b.y > pos->y || box.b.z > pos->z || box.t.x < pos->x || box.t.y < pos->y || box.t.z < pos->z) {
            continue;
        }

        C3Vector local = *pos * mapObjDef->m_invMat;
        CMapObj* mapObj = mapObjDef->mapObj;

        if (mapObj->ContainsPointInBounds(local) && mapObj->QueryLiquidStatus(CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x2000, local, liquidType, height)) {
            C3Vector level(local.x, local.y, *height);
            level *= mapObjDef->m_mat;
            *height = level.z;
            return true;
        }
    }

    return false;
}

// VERIFY: 7c5ab3e1 auto
// OFFSET: 0x007A0B00
bool CMap::QueryLiquidStatus(C3Vector* pos, int32_t* liquidType, float* height, int32_t* unknown4, int32_t checkGround) {
    *unknown4 = 0;

    if (CMap::QueryLiquidStatusMapObjsExt(pos, liquidType, height)) {
        return true;
    }

    return CMap::QueryLiquidTerrain(pos, liquidType, height, checkGround);
}

// VERIFY: 2f11b480 manual
// OFFSET: 0x007A2070
static int32_t CompareIntersectCandidates(const void* a, const void* b) {
    const MapObjIntersectCandidate* ca = static_cast<const MapObjIntersectCandidate*>(a);
    const MapObjIntersectCandidate* cb = static_cast<const MapObjIntersectCandidate*>(b);

    if (cb->t > ca->t)
        return -1;

    if (cb->t < ca->t)
        return 1;

    return 0;
}

// VERIFY: 24e6cf3e auto
// OFFSET: 0x007A20B0
static const char* PathFileName(const char* path) {
    const char* name = path;

    for (const char* separator = SStrStrI(path, "\\"); separator; separator = SStrStrI(separator + 1, "\\")) {
        name = separator + 1;
    }

    return name;
}

// VERIFY: 85e5829e manual
// OFFSET: 0x007A20E0
void CMap::VectorIntersectSX(CiRect& rect) {
    int32_t x = rect.minX;

    if (x <= rect.maxX) {
        do {
            CMap::AddCollideCell(x, rect.minY);
            x++;
        } while (x <= rect.maxX);
    } else {
        do {
            CMap::AddCollideCell(x, rect.minY);
            x--;
        } while (x >= rect.maxX);
    }
}

// VERIFY: 8a929a73 manual
// OFFSET: 0x007A2180
void CMap::VectorIntersectSY(CiRect& rect) {
    int32_t y = rect.minY;

    if (y <= rect.maxY) {
        do {
            CMap::AddCollideCell(rect.minX, y);
            y++;
        } while (y <= rect.maxY);
    } else {
        do {
            CMap::AddCollideCell(rect.minX, y);
            y--;
        } while (y >= rect.maxY);
    }
}

// VERIFY: 6d1fef26 manual
// OFFSET: 0x007A2230
void CMap::VectorIntersectDX(C3Vector& start, C3Vector& end, CiRect& cells) {
    int32_t y = cells.minY;
    float slope = (end.y - start.y) / (end.x - start.x);
    int32_t x = cells.minX;
    float intercept = start.y - start.x * slope;

    int32_t next;
    int32_t step;

    if (cells.maxX <= x) {
        next = cells.minX;
        step = -1;
    } else {
        next = x + 1;
        step = 1;
    }

    float edge = next * SMOLTILE_SIZE;

    CMap::AddCollideCell(x, y);

    while (x != cells.maxX + step) {
        if (CMap::cCount >= CMap::COLLIDE_LIST_SIZE - 4) {
            return;
        }

        int32_t edgeY = CMath::fint_((slope * edge + intercept) * OO_COORD_TO_SUBCHUNK - 0.5f);

        if (edgeY != y) {
            CMap::AddCollideCell(x, edgeY);
        }

        x += step;
        edge = step * SMOLTILE_SIZE + edge;

        CMap::AddCollideCell(x, edgeY);

        y = edgeY;
    }

    if (y != cells.maxY) {
        CMap::AddCollideCell(cells.maxX, cells.maxY);
    }
}

// VERIFY: 6264b08b manual
// OFFSET: 0x007A23E0
void CMap::VectorIntersectDY(C3Vector& start, C3Vector& end, CiRect& cells) {
    float slope = (end.y - start.y) / (end.x - start.x);
    int32_t x = cells.minX;
    int32_t y = cells.minY;
    float intercept = start.y - start.x * slope;
    float invSlope = 1.0f / slope;

    int32_t next;
    int32_t step;

    if (cells.maxY <= cells.minY) {
        next = cells.minY;
        step = -1;
    } else {
        next = y + 1;
        step = 1;
    }

    float edge = next * SMOLTILE_SIZE;

    CMap::AddCollideCell(cells.minX, cells.minY);

    while (y != cells.maxY + step) {
        if (CMap::cCount >= CMap::COLLIDE_LIST_SIZE - 4) {
            return;
        }

        int32_t edgeX = CMath::fint_((edge - intercept) * invSlope * OO_COORD_TO_SUBCHUNK - 0.5f);

        if (edgeX != x) {
            CMap::AddCollideCell(edgeX, y);
        }

        edge = step * SMOLTILE_SIZE + edge;
        y += step;

        CMap::AddCollideCell(edgeX, y);

        x = edgeX;
    }

    if (x != cells.maxX) {
        CMap::AddCollideCell(cells.maxX, cells.maxY);
    }
}


// VERIFY: 136b6340 manual
// OFFSET: 0x007A25A0
World::ObjectCollisionData::ObjectCollisionData() {
}

// VERIFY: 09bec685 manual
// OFFSET: 0x007A25F0
static void AddHitDebugModel(World::HitTestDebug* hitInfo, int32_t depth, CM2Model* model) {
    for (; model; model = model->m_attachNext) {
        if (hitInfo->m_numModels == 16) {
            break;
        }

        World::HitTestDebug::Model* entry = &hitInfo->m_models[hitInfo->m_numModels++];
        entry->m_name = PathFileName(model->m_shared->m_fileName);
        entry->m_depth = depth;
        entry->m_bbox = model->GetCurrentBoundingBox();
        entry->m_sphere = model->GetCurrentBoundingSphere();
        entry->m_numGeometryBatches = model->GetNumGeometryBatches();
        entry->m_numPrimitives = model->GetNumPrimitives();
        entry->m_numOtherBatches = model->GetNumBatches() - entry->m_numGeometryBatches;
        entry->m_numBones = (model->m_runtimeFlags & M2_FLAGS::M2_INITIALIZED) ? model->m_shared->m_data->m_bones.m_count : 0;

        model->EnsureInitialized();

        entry->m_numLights = model->m_shared->m_data->m_lights.m_count;
        entry->m_numCurrentLights = model->GetCurrentLightCount();
        entry->m_attachmentId = model->m_attachmentId;
        entry->m_attachmentScale = 1.0f;

        if (model->m_attachmentId != MODEL_ATTACHMENT_ID::ATTACH_NONE && model->m_attachParent) {
            entry->m_attachmentScale = model->m_attachParent->GetAttachmentWorldScale(model->m_attachmentId);
        }

        entry->m_model = model;
        entry->m_numCombinableBatches = model->GetNumCombinableDoodadBatches();
        entry->m_instanceCountMax = model->GetInstanceCountMax();
        entry->m_groupName = nullptr;
        entry->m_groupNum = 0;
        entry->m_groupFlags = 0;
        AddHitDebugModel(hitInfo, depth + 1, model->m_attachList);
    }
}

// VERIFY: 0869a654 manual
// OFFSET: 0x007A2760
void CMap::VectorIntersectDoodadDefs(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, uint32_t flags) {
    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if ((flags & WORLD_QUERY_VISIBLE_OPAQUE) && doodadDef->m_culled != CMapStaticEntity::CULL_VISIBLE) {
            continue;
        }

        if ((doodadDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) || !(doodadDef->m_flags & MAPOBJ_FLAG_PREPARED) || doodadDef->m_queryTag == CMap::s_queryTag) {
            continue;
        }

        CM2Model* model = doodadDef->model;

        if (!model) {
            continue;
        }

        int32_t mode = -1;

        if (doodadDef->m_collisionGuid) {
            if (flags & WORLD_QUERY_OBJECT_COLLISION) {
                mode = CM2Model::HIT_TEST_COLLISION;
            } else if (flags & (WORLD_QUERY_OBJECT_RENDER | WORLD_QUERY_OBJECT_UNKNOWN_0x400000)) {
                mode = CM2Model::HIT_TEST_RENDER;
            }
        } else if (flags & WORLD_QUERY_M2_COLLISION) {
            mode = CM2Model::HIT_TEST_COLLISION;
        } else if (flags & (WORLD_QUERY_M2_RENDER | WORLD_QUERY_M2_UNKNOWN_0x4 | WORLD_QUERY_M2_NON_OPAQUE)) {
            if (flags & WORLD_QUERY_M2_NON_OPAQUE) {
                mode = CM2Model::HIT_TEST_NON_OPAQUE;
            } else {
                mode = (flags & WORLD_QUERY_VISIBLE_OPAQUE) ? CM2Model::HIT_TEST_OPAQUE : CM2Model::HIT_TEST_RENDER;
            }
        }

        STORM_ASSERT(model->m_scene->IsHitTesting()); // 12213: "m_scene->IsHitTesting()"

        if (mode >= 0 && (model->m_runtimeFlags & M2_FLAGS::M2_INITIALIZED)) {
            if (!model->m_hitTestPrev) {
                model->m_hitTestPrev = &model->m_scene->m_hitList;
                model->m_hitTestNext = model->m_scene->m_hitList;
                model->m_scene->m_hitList = model;

                if (model->m_hitTestNext) {
                    model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
                }
            }

            model->m_hitTestMode = mode;
            model->m_hitTestOwner = doodadDef;
            model->m_hitTestGroup = 0;
        }

        doodadDef->m_queryTag = CMap::s_queryTag;
    }
}

// VERIFY: 690f8e3c manual
// OFFSET: 0x007A2960
void CMap::VectorIntersectEntitys(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, uint32_t flags) {
    if (!World::s_objectCollisionCallback) {
        return;
    }

    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->owner);

        if ((flags & WORLD_QUERY_VISIBLE_OPAQUE) && entity->m_culled != CMapStaticEntity::CULL_VISIBLE) {
            continue;
        }

        if (!(entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_COLLIDABLE)) {
            return;
        }

        if (entity->m_queryTag == CMap::s_queryTag || (entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_HIDDEN)) {
            continue;
        }

        World::ObjectCollisionData data;
        WOWGUID guid = { entity->m_funcParam64 };

        if (!World::s_objectCollisionCallback(&guid, entity->m_funcParam32, &data, flags)) {
            continue;
        }

        CM2Model* model = data.m_model;

        if (!model) {
            continue;
        }

        STORM_ASSERT(model->m_scene->IsHitTesting()); // 12213: "m_scene->IsHitTesting()"

        if (model->m_runtimeFlags & M2_FLAGS::M2_INITIALIZED) {
            if (!model->m_hitTestPrev) {
                model->m_hitTestPrev = &model->m_scene->m_hitList;
                model->m_hitTestNext = model->m_scene->m_hitList;
                model->m_scene->m_hitList = model;

                if (model->m_hitTestNext) {
                    model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
                }
            }

            model->m_hitTestMode = (flags & WORLD_QUERY_OBJECT_COLLISION) ? CM2Model::HIT_TEST_COLLISION : CM2Model::HIT_TEST_RENDER;
            model->m_hitTestGroup = 0;
            model->m_hitTestOwner = entity;
        }

        entity->m_queryTag = CMap::s_queryTag;
    }
}

// VERIFY: 3fe9ef3a manual
// OFFSET: 0x007A3570
bool CMap::VectorIntersectSubChunkList(C3Vector* start, C3Vector* end, float* distance, uint32_t queryFlags, CMapChunk** hitChunk) {
    // 12213: "queryFlags & (CWorld::WQF_allLiquidFlags | CWorld::WQF_doodadMask | CWorld::WQF_gameObjMask | CWorld::WQF_terrain | CWorld::WQF_allObjsAllGeo)"
    STORM_ASSERT(queryFlags & (WORLD_QUERY_LIQUID_MASK | WORLD_QUERY_DOODAD_MASK | WORLD_QUERY_TERRAIN | WORLD_QUERY_ALL_OBJS_ALL_GEO));
    const uint32_t* scPtr = CMap::scCollideList.m_data;
    STORM_ASSERT(scPtr);
    // The binary seeds the current chunk from the first scPtr's out-of-range bit, with X and Y crossed.
    // In-range cells give 0 for both, so a walk starting in the world's corner chunk never loads a chunk.
    uint32_t lastChunkCellY = scPtr[0] & MAP_CELLS_PER_MAP_AXIS;
    uint32_t lastChunkCellX = scPtr[1] & MAP_CELLS_PER_MAP_AXIS;
    uint32_t remaining = CMap::cCount;
    CMapChunk* chunk = nullptr;
    CMapChunk* bestChunk = nullptr;
    uint32_t modelFlags = queryFlags & WORLD_QUERY_MODEL_MASK;

    if (modelFlags) {
        World::s_m2Scene->BeginHitTest();
    }

    float dx = end->x - start->x;
    float dy = end->y - start->y;
    float dz = end->z - start->z;
    C3Vector origin = *start;
    float invLength = 1.0f / sqrt(dz * dz + dy * dy + dx * dx);
    C3Vector dir(dx * invLength, dy * invLength, invLength * dz);
    float bestDist = *distance;

    while (remaining) {
        uint32_t cellX = scPtr[0];
        uint32_t cellY = scPtr[1];
        remaining -= 2;
        scPtr += 2;

        if (cellX > MAP_CELLS_PER_MAP_AXIS || cellY > MAP_CELLS_PER_MAP_AXIS) {
            break;
        }

        uint32_t chunkCellX = cellX & MAP_CELL_CHUNK_MASK;
        uint32_t chunkCellY = cellY & MAP_CELL_CHUNK_MASK;

        if (chunkCellX != lastChunkCellX || chunkCellY != lastChunkCellY) {
            CMapArea* area = CMap::GetAreaForCell(cellX, cellY);

            if (!area || area->asyncObject) {
                break;
            }

            chunk = area->GetChunkForCell(cellX, cellY);

            if (!chunk) {
                break;
            }

            lastChunkCellX = chunkCellX;
            lastChunkCellY = chunkCellY;
            origin.x = start->x - chunk->m_topLeftCoords.x;
            origin.y = start->y - chunk->m_topLeftCoords.y;
            origin.z = start->z - chunk->m_topLeftCoords.z;

            if (modelFlags) {
                CMap::VectorIntersectDoodadDefs(&chunk->doodadDefLinkList, queryFlags);
            }

            if (queryFlags & WORLD_QUERY_ENTITY_MASK) {
                CMap::VectorIntersectEntitys(&chunk->entityLinkList, queryFlags);
            }
        }

        uint32_t subX = cellX & MAP_CELL_INDEX_MASK;
        uint32_t subY = cellY & MAP_CELL_INDEX_MASK;
        C3Ray ray;
        ray.origin = origin;
        ray.dir = dir;

        if (queryFlags & WORLD_QUERY_TERRAIN) {
            float t = FLT_MAX;

            if (chunk->Intersect(subX, subY, ray, &t)) {
                float dist = t * invLength;

                if (bestDist > dist && dist >= 0.0f) {
                    bestDist = dist;
                    bestChunk = chunk;
                }
            }
        }

        if (queryFlags & WORLD_QUERY_LIQUID_MASK) {
            int32_t checkType = (queryFlags & WORLD_QUERY_WATER_WALK_LIQUID) && !(queryFlags & WORLD_QUERY_LIQUID);

            for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
                if (checkType && !(g_liquidTypeDB.GetRecord(liquid->m_type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
                    continue;
                }

                if (!liquid->TileExists(subX, subY)) {
                    continue;
                }

                uint32_t tilesY = liquid->m_tileEnd.y - liquid->m_tileBegin.y + 1;
                uint32_t vertsY = liquid->m_tileEnd.y - liquid->m_tileBegin.y + 2;
                uint32_t base = subX + tilesY * (subY - liquid->m_tileBegin.x) - liquid->m_tileBegin.y;
                uint32_t pattern[6] = { 0, vertsY, tilesY, 0, 1, vertsY };

                for (uint32_t i = 0; i < 2; i++) {
                    float t = 0.0f;
                    uint32_t tri[3] = { base + pattern[3 * i], base + pattern[3 * i + 1], base + pattern[3 * i + 2] };

                    if (NTempest::Intersect(ray, liquid->m_verts, tri, &t, nullptr, 0.01f)) {
                        float dist = t * invLength;

                        if (bestDist > dist && dist >= 0.0f) {
                            bestDist = dist;
                            bestChunk = chunk;
                        }
                    }
                }
            }
        }
    }

    if (modelFlags) {
        if (queryFlags & WORLD_QUERY_RENDER_HIT_TEST_MASK) {
            C3Vector localStart = *start * CWorldScene::camTransportView;
            C3Vector localEnd = *end * CWorldScene::camTransportView;
            World::s_m2Scene->EndHitTest(localStart, localEnd, bestDist, 0);
        } else {
            World::s_m2Scene->EndHitTestCollisionWorld(*start, *end, bestDist);
        }
    }

    if (bestDist >= *distance) {
        return false;
    }

    *distance = bestDist;

    if (hitChunk) {
        *hitChunk = bestChunk;
    }

    return true;
}

// VERIFY: 2f69e7ee auto
// OFFSET: 0x007A39F0
bool CMap::VectorIntersectTerrain(C3Vector* start, C3Vector* end, float* distance, uint32_t flags, CMapChunk** hitChunk) {
    float startX = -(start->y - HALF_WORLD_SIZE);
    float startY = -(start->x - HALF_WORLD_SIZE);
    float endX = -(end->y - HALF_WORLD_SIZE);
    float endY = -(end->x - HALF_WORLD_SIZE);

    float dX = endX - startX;
    float dY = endY - startY;

    CiRect cells;
    cells.maxY = CMath::fint_(endY * OO_COORD_TO_SUBCHUNK - 0.5f);
    cells.minX = CMath::fint_(startX * OO_COORD_TO_SUBCHUNK - 0.5f);
    cells.minY = CMath::fint_(startY * OO_COORD_TO_SUBCHUNK - 0.5f);
    cells.maxX = CMath::fint_(endX * OO_COORD_TO_SUBCHUNK - 0.5f);

    CMap::cCount = 0;

    if (fabsf(dX) < 0.00000023841858f || cells.minX == cells.maxX) {
        CMap::VectorIntersectSY(cells);
    } else if (fabsf(dY) < 0.00000023841858f || cells.minY == cells.maxY) {
        CMap::VectorIntersectSX(cells);
    } else {
        C3Vector e(endX, endY, 0.0f);
        C3Vector s(startX, startY, 0.0f);

        if (fabsf(dY) >= fabsf(dX)) {
            CMap::VectorIntersectDY(s, e, cells);
        } else {
            CMap::VectorIntersectDX(s, e, cells);
        }
    }

    return CMap::VectorIntersectSubChunkList(start, end, distance, flags, hitChunk);
}


// VERIFY: 35b33716 manual
// OFFSET: 0x007A4270
CFacet* CMap::BuildImpassableFacets(World::FacetData* facets, C3Vector* up, C3Vector* edge, C3Vector* normal, C3Vector* origin) {
    float d = -(normal->z * origin->z + normal->x * origin->x + normal->y * origin->y);

    CFacet* facet = facets->facets.New();
    facet->plane.n = *normal;
    facet->plane.d = d;
    facet->v[0] = *origin;
    facet->v[1] = { origin->x + edge->x, origin->y + edge->y, origin->z + edge->z };
    facet->v[2] = { origin->x + edge->x + up->x, origin->y + edge->y + up->y, origin->z + edge->z + up->z };

    facet = facets->facets.New();
    facet->plane.n = *normal;
    facet->plane.d = d;
    facet->v[0] = *origin;
    facet->v[1] = { origin->x + edge->x + up->x, origin->y + edge->y + up->y, origin->z + edge->z + up->z };
    facet->v[2] = { origin->x + up->x, origin->y + up->y, origin->z + up->z };

    return facet;
}

// VERIFY: f65ff37c manual
// OFFSET: 0x007A43D0
void CMap::CreateImpassableFacets(CMapChunk* chunk, CAaBox* box, World::FacetData* facets) {
    C3Vector up = { 0.0f, 0.0f, 32000.0f };
    C3Vector origin = { 0.0f, 0.0f, 0.0f };
    C3Vector normal = { 0.0f, 0.0f, 0.0f };
    C3Vector edge = { 0.0f, 0.0f, 0.0f };

    if (chunk->m_bbox.b.y > box->b.y) {
        origin = { chunk->m_bbox.b.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z };
        normal = { 0.0f, -1.0f, 0.0f };
        edge = { chunk->m_bbox.t.x - chunk->m_bbox.b.x, 0.0f, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.t.y < box->t.y) {
        origin = { chunk->m_bbox.t.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z };
        normal = { 0.0f, 1.0f, 0.0f };
        edge = { chunk->m_bbox.b.x - chunk->m_bbox.t.x, 0.0f, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.b.x > box->b.x) {
        origin = { chunk->m_bbox.b.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z };
        normal = { -1.0f, 0.0f, 0.0f };
        edge = { 0.0f, chunk->m_bbox.b.y - chunk->m_bbox.t.y, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.t.x < box->t.x) {
        origin = { chunk->m_bbox.t.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z };
        normal = { 1.0f, 0.0f, 0.0f };
        edge = { 0.0f, chunk->m_bbox.t.y - chunk->m_bbox.b.y, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }
}


// VERIFY: 2b594300 manual
// OFFSET: 0x007A4AF0
void CMap::AppendMapObjFacets(CMapDoodadDef* def, CAaBox* box, World::FacetData* facets) {
    uint32_t first = facets->facets.Count();

    def->model->GetCollisionFacets(*box, def->m_mat, facets->facets);

    uint32_t count = facets->facets.Count();
    facets->facetIds.SetCount(count);

    for (uint32_t i = first; i < count; i++) {
        facets->facetIds.m_data[i] = def->m_collisionGuid.m_guid;
    }
}

// VERIFY: e2b4677b manual
// OFFSET: 0x007A4C10
void CMap::TestQueryAdd(const CFacet& facet, CImVector color, const C44Matrix* mat) {
    C44Matrix identity;
    uint16_t base = static_cast<uint16_t>(CMap::debugVertexArray.Count());

    if (!mat) {
        mat = &identity;
    }

    for (uint32_t i = 0; i < 3; i++) {
        CGxVertexPC* vertex = CMap::debugVertexArray.New();
        vertex->p = facet.v[i] * *mat;
        vertex->c = color;
    }

    *CMap::debugIndexArray.New() = base;
    *CMap::debugIndexArray.New() = base + 1;
    *CMap::debugIndexArray.New() = base + 2;
}

// VERIFY: de8ab0e3 manual
// OFFSET: 0x007AD3B0
bool CMap::GetHeightTerrain(CMapChunk* chunk, C3Vector& pos, int32_t cellX, int32_t cellY, float* outHeight) {
    int32_t col = cellX & MAP_CELL_INDEX_MASK;
    int32_t row = cellY & MAP_CELL_INDEX_MASK;

    if (chunk->m_header->m_holes & CMap::s_holeMask[4 * (row >> 1) + (col >> 1)]) {
        return false;
    }

    float originX = row * -SMOLTILE_SIZE;
    float originY = -SMOLTILE_SIZE * col;

    float dx = pos.x - chunk->m_topLeftCoords.x;
    float dy = pos.y - chunk->m_topLeftCoords.y;

    int32_t fan = 0;

    if (((originX + CMap::s_subchunkCoords[2].x) - (originX + CMap::s_subchunkCoords[0].x)) * ((originY + CMap::s_subchunkCoords[2].y) - dy) - ((originY + CMap::s_subchunkCoords[2].y) - (originY + CMap::s_subchunkCoords[0].y)) * ((originX + CMap::s_subchunkCoords[2].x) - dx) <= 0.0f) {
        fan = 1;
    }

    if (((originX + CMap::s_subchunkCoords[3].x) - (originX + CMap::s_subchunkCoords[1].x)) * ((originY + CMap::s_subchunkCoords[3].y) - dy) - ((originY + CMap::s_subchunkCoords[3].y) - (originY + CMap::s_subchunkCoords[1].y)) * ((originX + CMap::s_subchunkCoords[3].x) - dx) <= 0.0f) {
        fan += 2;
    }

    float* height = &chunk->m_height[TERRAIN_CHUNK_VERTEX_ROW_STRIDE * row + col];

    C3Vector a(originX + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan]].x, originY + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan]].y, height[CMap::s_fanIndices[2 * fan]]);
    C3Vector b(originX + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan + 1]].x, originY + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan + 1]].y, height[CMap::s_fanIndices[2 * fan + 1]]);
    C3Vector c(originX + CMap::s_subchunkCoords[4].x, originY + CMap::s_subchunkCoords[4].y, height[9]);

    if (CMap::s_useSSE) {
        float ax = a.x - c.x;
        float ay = a.y - c.y;
        float az = a.z - c.z;
        float bx = b.x - c.x;
        float by = b.y - c.y;
        float bz = b.z - c.z;

        float nx = bz * ay - az * by;
        float ny = az * bx - bz * ax;
        float nz = ax * by - ay * bx;

        float scale = 1.0f / sqrtf(ny * ny + nz * nz + 0.0f * 0.0f + nx * nx);

        *outHeight = chunk->m_topLeftCoords.z - (ny * scale * dy + nx * scale * dx - (c.z * (nz * scale) + c.y * (ny * scale) + c.x * (nx * scale))) / (nz * scale);

        return true;
    }

    C4Plane plane;
    plane.From3Pos(c, a, b);

    *outHeight = chunk->m_topLeftCoords.z - (plane.n.y * dy + plane.n.x * dx + plane.d) / plane.n.z;

    return true;
}

// VERIFY: 468f4c9b manual
// OFFSET: 0x007B4090
void CMapObjDefGroup::UpdateLights() {
    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        link->owner->m_flags = static_cast<MapObjFlag>(link->owner->m_flags | MAPOBJ_FLAG_UNPLACED);
    }

    for (CMapBaseObjLink* link = this->entityLinkList.Head(); link; link = this->entityLinkList.Next(link)) {
        link->owner->m_flags = static_cast<MapObjFlag>(link->owner->m_flags | MAPOBJ_FLAG_UNPLACED);
    }
}

// VERIFY: 7a5c5c66 manual
// OFFSET: 0x007B47F0
static int32_t CompareAreaEntries(const void* a, const void* b) {
    const CMapAreaEntry* entryA = static_cast<const CMapAreaEntry*>(a);
    const CMapAreaEntry* entryB = static_cast<const CMapAreaEntry*>(b);

    if (entryB->m_dist > entryA->m_dist) {
        return -1;
    }

    return entryB->m_dist < entryA->m_dist;
}

// VERIFY: 9faa97d5 manual
// OFFSET: 0x007B4960
CMapArea* CMap::GetAreaAtPosition(const C2Vector* pos) {
    float tx = -(pos->y - HALF_WORLD_SIZE);
    float ty = -(pos->x - HALF_WORLD_SIZE);
    // 12213: "tx > 0.0f && tx < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(tx > 0.0f && tx < 2.0f * HALF_WORLD_SIZE);
    // 12213: "ty > 0.0f && ty < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(ty > 0.0f && ty < 2.0f * HALF_WORLD_SIZE);

    return CMap::GetArea(CMath::fint_(tx * 0.001875f - 0.5f), CMath::fint_(ty * 0.001875f - 0.5f));
}

// VERIFY: 8231832c manual
// OFFSET: 0x007B49C0
CMapChunk* CMap::GetChunkAtPosition(const C2Vector* pos) {
    float tx = -(pos->y - HALF_WORLD_SIZE);
    float ty = -(pos->x - HALF_WORLD_SIZE);
    // 12213: "tx > 0.0f && tx < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(tx > 0.0f && tx < 2.0f * HALF_WORLD_SIZE);
    // 12213: "ty > 0.0f && ty < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(ty > 0.0f && ty < 2.0f * HALF_WORLD_SIZE);

    int32_t chunkY = CMath::fint_(ty * OO_COORD_TO_CHUNK - 0.5f);
    int32_t chunkX = CMath::fint_(tx * OO_COORD_TO_CHUNK - 0.5f);
    CMapArea* area = CMap::GetAreaForChunk(chunkX, chunkY);

    if (!area) {
        return nullptr;
    }

    return area->GetChunk(chunkX, chunkY);
}

// VERIFY: f1ab17a0 manual
// OFFSET: 0x007B4BC0
void CMap::UpdateBarriers() {
    int32_t mode = CWorldScene::s_barrier.m_mode;
    int32_t minX = CWorld::gbChunkRect.minX >> 4;
    int32_t maxX = CWorld::gbChunkRect.maxX >> 4;
    int32_t minY = CWorld::gbChunkRect.minY >> 4;
    int32_t maxY = CWorld::gbChunkRect.maxY >> 4;

    if (!mode) {
        return;
    }

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            int32_t index = CMap::GetAreaIndex(x, y);

            if (!(CMap::m_areaInfo[index].m_flags & SMAreaInfo::FLAG_EXISTS)) {
                continue;
            }

            CMapArea* area = CMap::m_areaTable[index];

            if (!area || area->asyncObject) {
                continue;
            }

            CMapArea* neighbor = CMap::m_areaTable[index - 1];

            if (!neighbor || neighbor->asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.b.x, area->m_bounds.t.y);
                C2Vector end(area->m_bounds.t.x, area->m_bounds.t.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index - MAP_AREAS_PER_AXIS];

            if (!neighbor || neighbor->asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.t.x, area->m_bounds.t.y);
                C2Vector end(area->m_bounds.t.x, area->m_bounds.b.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index + 1];

            if (!neighbor || neighbor->asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.t.x, area->m_bounds.b.y);
                C2Vector end(area->m_bounds.b.x, area->m_bounds.b.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index + MAP_AREAS_PER_AXIS];

            if (!neighbor || neighbor->asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.b.x, area->m_bounds.b.y);
                C2Vector end(area->m_bounds.b.x, area->m_bounds.t.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }
        }
    }
}

// VERIFY: 9eee5402 auto
// OFFSET: 0x007B4DF0
void CMap::UpdateArea(int32_t addToScene, CMapArea* area, CiRect* chunkRect, int32_t depth) {
    if (chunkRect->minX > CWorld::gbChunkRect.maxX || chunkRect->minY > CWorld::gbChunkRect.maxY || chunkRect->maxX < CWorld::gbChunkRect.minX || chunkRect->maxY < CWorld::gbChunkRect.minY) {
        area->PurgeChunks(chunkRect);
    } else if (depth == 2) {
        area->Update(addToScene, chunkRect);
    } else {
        CiRect rect;
        rect.minY = chunkRect->minY;
        rect.minX = chunkRect->minX;
        rect.maxY = chunkRect->minY + ((chunkRect->maxY - chunkRect->minY) >> 1);
        rect.maxX = chunkRect->minX + ((chunkRect->maxX - chunkRect->minX) >> 1);
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minX = ((chunkRect->maxX - chunkRect->minX) >> 1) + chunkRect->minX + 1;
        rect.maxX = chunkRect->maxX;
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minY = ((chunkRect->maxY - chunkRect->minY) >> 1) + chunkRect->minY + 1;
        rect.maxY = chunkRect->maxY;
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minX = chunkRect->minX;
        rect.maxX = chunkRect->minX + ((chunkRect->maxX - chunkRect->minX) >> 1);
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);
    }
}

// VERIFY: a9b440d7 manual
// OFFSET: 0x007B50B0
uint32_t CMap::ComputeMapObjLoadProgress(CMapChunk* chunk, float* progress, uint32_t total) {
    CAaBox box;
    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.b.z = 0.0f;
    box.t.x = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;
    *progress = 0.0f;
    float scale = 0.0f;

    if (total) {
        scale = 1.0f / static_cast<float>(total);
    }

    TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list;

    if (CMap::bDungeon) {
        box.b.x = World::s_currentWorldPos.x - 16.666666f;
        box.b.y = World::s_currentWorldPos.y - 16.666666f;
        box.b.z = World::s_currentWorldPos.z - 16.666666f;
        box.t.x = World::s_currentWorldPos.x + 16.666666f;
        box.t.y = World::s_currentWorldPos.y + 16.666666f;
        box.t.z = World::s_currentWorldPos.z + 16.666666f;
        list = &CMap::mapObjDefLinkList;
    } else {
        if (!chunk) {
            return 0;
        }

        box = chunk->m_bbox;
        box.b.z = World::s_currentWorldPos.z - 16.666666f;
        box.t.z = World::s_currentWorldPos.z + 16.666666f;
        list = &chunk->mapObjDefLinkList;
    }

    uint32_t remaining = 0;

    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->owner);

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) || !box.Intersects(mapObjDef->m_bbox)) {
            continue;
        }

        for (CMapBaseObjLink* groupLink = mapObjDef->groupLinkList.Head(); groupLink; groupLink = mapObjDef->groupLinkList.Next(groupLink)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(groupLink->owner);

            if (box.t.x < defGroup->aaBox.b.x || box.t.y < defGroup->aaBox.b.y || box.t.z < defGroup->aaBox.b.z || box.b.x > defGroup->aaBox.t.x || box.b.y > defGroup->aaBox.t.y || box.b.z > defGroup->aaBox.t.z) {
                continue;
            }

            if (!(defGroup->m_flags & MAPOBJ_FLAG_PREPARED)) {
                if (!mapObjDef->mapObj->IsGroupLoading(defGroup->groupNum)) {
                    continue;
                }

                CMapObjGroup* group = mapObjDef->mapObj->GetGroup(defGroup->groupNum, 0);

                if (group && group->m_distToCamera > 0.0f) {
                    group->m_distToCamera = 0.0f;
                }

                remaining++;
                continue;
            }

            int32_t pending = 0;

            for (CMapBaseObjLink* doodadLink = defGroup->doodadDefLinkList.Head(); doodadLink; doodadLink = defGroup->doodadDefLinkList.Next(doodadLink)) {
                CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(doodadLink->owner);

                if ((!doodadDef->model || !(doodadDef->m_flags & MAPOBJ_FLAG_PREPARED)) && !(doodadDef->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING)) {
                    doodadDef->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING;

                    if (!pending) {
                        pending = 1;
                    }
                }
            }

            if (pending) {
                remaining++;
            }
        }
    }

    *progress += static_cast<float>(total - remaining) * scale;
    return remaining;
}

// VERIFY: f7ac6b15 manual
// OFFSET: 0x007B53B0
void CMap::ClearChunkSortList() {
    CMapChunk* chunk = CMap::mapChunkUpdateList.Head();

    while (chunk) {
        CMapChunk* next = CMap::mapChunkUpdateList.Next(chunk);
        chunk->m_sortListLink.Unlink();
        chunk = next;
    }
}

// VERIFY: fa65766b manual
// OFFSET: 0x007B5420
void CMap::ProcessChunkLiquidUpdateList() {
    CChunkLiquid* liquid = CMap::mapChunkLiquidUpdateList.Head();

    while (liquid) {
        CChunkLiquid* next = CMap::mapChunkLiquidUpdateList.Next(liquid);
        liquid->UpdateResourcePurging();

        if (liquid->m_resPurgeTimer < 0.0f) {
            liquid->m_updateLink.Unlink();
        }

        liquid = next;
    }
}

// VERIFY: cd78794a manual
// OFFSET: 0x007B54A0
void CMap::ProcessDetailDoodadUpdateList() {
    CDetailDoodadInst* inst = CMap::mapDetailDoodadUpdateList.Head();

    while (inst) {
        CDetailDoodadInst* next = CMap::mapDetailDoodadUpdateList.Next(inst);

        if (++inst->m_age > 10) {
            if (inst->HasBatches()) {
                inst->FreeBuffers();
            }

            if (inst->m_age > 20) {
                CDetailDoodad::FreeInst(inst);
            }
        }

        inst = next;
    }
}

// VERIFY: 21d0bf10 auto
// OFFSET: 0x007B5500
void CMap::ProcessRenderChunkUpdateList() {
    CMapRenderChunk* renderChunk = CMap::mapRenderChunkUpdateList.Head();
#ifdef WOWDECOMP_TOOL_FEATURES
    const WorldEditView* editorView = WorldEditGetView();
#endif

    while (renderChunk) {
        renderChunk->m_lastUpdateTime = FrameTime::s_tickTimeSec + renderChunk->m_lastUpdateTime;
        CMapRenderChunk* next = CMap::mapRenderChunkUpdateList.Next(renderChunk);

#ifdef WOWDECOMP_TOOL_FEATURES
        bool expired = renderChunk->m_lastUpdateTime > 2.0f;
        if (!expired && editorView) {
            const CMapChunk* first = renderChunk->m_chunks[0];
            const CMapChunk* second = renderChunk->m_chunks[1];
            expired = (!first || !editorView->IncludesTerrain(first->m_bbox2)) &&
                      (!second || !editorView->IncludesTerrain(second->m_bbox2));
        }
        if (expired) {
#else
        if (renderChunk->m_lastUpdateTime > 2.0f) {
#endif
            renderChunk->FreeBuf();
        }

        if (!renderChunk->m_buf) {
            renderChunk->m_renderChunkLink.Unlink();
        }

        renderChunk = next;
    }
}

// VERIFY: a5870e86 manual
// OFFSET: 0x007B5590
void CMap::PrepareEntitys(int32_t addToScene) {
    if (!addToScene) {
        return;
    }

    for (CMapEntity* entity = CMap::entityList.Head(); entity; entity = CMap::entityList.Next(entity)) {
        if (!(entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_HIDDEN)) {
            CWorldScene::AddEntityToSortTable(entity);
        }
    }
}

// VERIFY: c3245130 manual
// OFFSET: 0x007B5630
void CMap::PrepareMapDoodadDefs() {
    int32_t streaming = SFile::IsStreamingMode();
    CMapDoodadDef* doodadDef = CMap::doodadDefList.Head();

    while (doodadDef) {
        CM2Model* model = doodadDef->model;
        CMapDoodadDef* next = CMap::doodadDefList.Next(doodadDef);

        if (!model || model->IsLoaded(0, 0)) {
            if (!(doodadDef->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_EXTERNAL)) {
                doodadDef->UpdateBounds();

                if (!(doodadDef->m_flags & MAPOBJ_FLAG_INTERIOR) && CMap::QueryShadow(&doodadDef->m_position)) {
                    doodadDef->m_diffuseLightScale = 0.5f;
                }

                doodadDef->ExtendChunkBounds();
                doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags | MAPOBJ_FLAG_PREPARED | MAPOBJ_FLAG_UNPLACED);
            }

            doodadDef->m_doodadDefLink.Unlink();
        } else if (streaming && (doodadDef->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING)) {
            doodadDef->model->LinkToAsyncHead(1);
            doodadDef->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING;
        }

        doodadDef = next;
    }
}

// INLINED (name inferred)
static void AddAreaToSortList(CMapAreaEntry* entries, int32_t& sortCount, int32_t maxCount, CMapArea* area) {
    STORM_ASSERT(area);
    const C2Vector pos(World::s_currentWorldPos.x, World::s_currentWorldPos.y);
    entries[sortCount].m_area = area;
    entries[sortCount].m_dist = NTempest::DistanceSquared(area->m_bounds, pos);
    ++sortCount;
    STORM_ASSERT(sortCount < maxCount);
}

// VERIFY: d6c4755a manual
// OFFSET: 0x007B5950
void CMap::PreUpdateAreas(bool addToScene) {
    int32_t maxY = CWorld::gbChunkRect.maxY >> 4;
    int32_t minX = CWorld::gbChunkRect.minX >> 4;
    int32_t minY = CWorld::gbChunkRect.minY >> 4;
    int32_t maxX = CWorld::gbChunkRect.maxX >> 4;
    int32_t sortCount = 0;
    constexpr int32_t maxCount = 1024;
    CMapAreaEntry* entries = static_cast<CMapAreaEntry*>(alloca(maxCount * sizeof(CMapAreaEntry)));

    CMap::ClearChunkSortList();
    CMap::ProcessChunkLiquidUpdateList();
    CMap::ProcessRenderChunkUpdateList();
    CMap::ProcessDetailDoodadUpdateList();

    int32_t waitForLoad = !(CMap::s_isStreamingMode || CMap::bPreload);
#ifdef WOWDECOMP_TOOL_FEATURES
    if (const WorldEditView* view = WorldEditGetView(); view && view->m_allowPendingMapAssets) {
        waitForLoad = 0;
    }
#endif
    CMapBaseObjLink* link = CMap::areaLinkList.Head();

    while (link) {
        CMapArea* area = static_cast<CMapArea*>(link->owner);
        CMapBaseObjLink* next = CMap::areaLinkList.Next(link);

        if (area->m_index.x >= minX && area->m_index.x <= maxX && area->m_index.y >= minY && area->m_index.y <= maxY) {
            AddAreaToSortList(entries, sortCount, maxCount, area);
        } else if (!area->asyncObject || !area->asyncObject->isCurrent) {
            CMap::FreeBaseObjLink(link);
            CMap::PurgeArea(area);
        }

        link = next;
    }

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            int32_t index = CMap::GetAreaIndex(x, y);

            if ((CMap::m_areaInfo[index].m_flags & SMAreaInfo::FLAG_EXISTS) && !CMap::m_areaTable[index]) {
                CMapArea* area = CMap::PrepareArea(x, y);
                AddAreaToSortList(entries, sortCount, maxCount, area);
            }
        }
    }

    if (sortCount) {
        qsort(entries, sortCount, sizeof(CMapAreaEntry), CompareAreaEntries);
    }

    int32_t lastPending = -1;
    CiRect rect;
    memset(&rect, 0, sizeof(rect));

    for (int32_t i = 0; i < sortCount; i++) {
        CMapArea* area = entries[i].m_area;

        if (!area->rawAreaData) {
            CMap::LoadArea(area);
        }

        if (area->m_index.x < minX || area->m_index.x > maxX || area->m_index.y < minY || area->m_index.y > maxY) {
            continue;
        }

        rect.minX = MAP_CHUNKS_PER_AREA_AXIS * area->m_index.x;
        rect.minY = MAP_CHUNKS_PER_AREA_AXIS * area->m_index.y;
        rect.maxX = MAP_CHUNKS_PER_AREA_AXIS * area->m_index.x + MAP_CHUNKS_PER_AREA_AXIS - 1;
        rect.maxY = MAP_CHUNKS_PER_AREA_AXIS * area->m_index.y + MAP_CHUNKS_PER_AREA_AXIS - 1;

        if (rect.minX <= CWorld::chunkRect.maxX && rect.minY <= CWorld::chunkRect.maxY && rect.maxX >= CWorld::chunkRect.minX && rect.maxY >= CWorld::chunkRect.minY && waitForLoad && area->asyncObject) {
            AsyncFileReadWait(area->asyncObject);
        }

        if (area->asyncObject) {
            if (CMap::s_isStreamingMode && entries[i].m_dist < 71111.109f) {
                lastPending = i;
            }
        } else {
            rect.minX = area->m_tileChunkIndex.x;
            rect.minY = area->m_tileChunkIndex.y;
            rect.maxX = area->m_tileChunkIndex.x + 15;
            rect.maxY = area->m_tileChunkIndex.y + 15;
            CMap::UpdateArea(addToScene, area, &rect, 0);
        }
    }

    if (CMap::s_isStreamingMode) {
        AsyncFile::EnterQueueLock();

        for (int32_t i = lastPending; i >= 0; i--) {
            CMapArea* area = entries[i].m_area;

            if (area->asyncObject && (entries[i].m_dist < 1111.1111f || (entries[i].m_dist < 71111.109f && !CWorldScene::FrustumCull(&area->m_bounds)))) {
                AsyncFileReadLinkObject(area->asyncObject, 1);
            }
        }

        AsyncFile::LeaveQueueLock();
    }

    if (addToScene) {
        CMap::UpdateBarriers();
    }
}

// VERIFY: 11c34df4 manual
// OFFSET: 0x007B5D00
void CMap::PrepareMapObjDef(CMapObjDef* mapObjDef, CMapObj* mapObj) {
    CAaBox bounds;
    mapObjDef->m_flags = static_cast<MapObjFlag>(mapObjDef->m_flags | MAPOBJ_FLAG_PREPARED);
    mapObj->GetBounds(mapObjDef->m_sphere);
    mapObjDef->m_sphere.c *= mapObjDef->m_mat;
    mapObj->GetBounds(&bounds);
    CWorldMath::TransformAABox(mapObjDef->m_mat, bounds, mapObjDef->m_bbox);
    mapObjDef->m_argbColor = *reinterpret_cast<uint32_t*>(&mapObj->m_argbColor);
    mapObjDef->m_lightsArray.SetCount(mapObj->m_ligtsCount);

    for (uint32_t i = 0; i < mapObjDef->m_lightsArray.Count(); i++) {
        mapObjDef->m_lightsArray[i] = nullptr;
    }

    CMap::CreateMapObjDefGroups(mapObj, mapObjDef);
}

// VERIFY: 1db2d5f4 manual
// OFFSET: 0x007B5E80
uint32_t CMap::ComputeDoodadLoadProgress(CMapChunk* chunk, float* progress, uint32_t total) {
    *progress = 0.0f;
    float scale = 0.0f;

    if (total) {
        scale = 1.0f / static_cast<float>(total);
    }

    CAaBox box;
    TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list;

    if (CMap::bDungeon) {
        box.b.x = World::s_currentWorldPos.x - 16.666666f;
        box.b.y = World::s_currentWorldPos.y - 16.666666f;
        box.b.z = World::s_currentWorldPos.z - 16.666666f;
        box.t.x = World::s_currentWorldPos.x + 16.666666f;
        box.t.y = World::s_currentWorldPos.y + 16.666666f;
        box.t.z = World::s_currentWorldPos.z + 16.666666f;
        list = &CMap::doodadDefLinkList;
    } else {
        if (!chunk) {
            return 0;
        }

        box = chunk->m_bbox;
        box.b.z = World::s_currentWorldPos.z - 16.666666f;
        box.t.z = World::s_currentWorldPos.z + 16.666666f;
        list = &chunk->doodadDefLinkList;
    }

    uint32_t remaining = 0;

    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if ((doodadDef->model && (doodadDef->m_flags & MAPOBJ_FLAG_PREPARED)) || (doodadDef->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING)) {
            continue;
        }

        if (total) {
            int64_t loaded = 0;
            int64_t size = 0;
            doodadDef->model->ComputeLoadProgress(loaded, size);
            *progress += static_cast<float>(loaded) / static_cast<float>(size) * scale;
        }

        doodadDef->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING;
        remaining++;
    }

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        if (!mapObj->m_asyncObject) {
            continue;
        }

        if (box.Intersects(mapObjDef->m_bbox)) {
            remaining++;

            if (mapObj->m_distToCamera > 0.0f) {
                mapObj->m_distToCamera = 0.0f;
            }
        }
    }

    *progress += static_cast<float>(total - remaining) * scale;
    return remaining;
}

// VERIFY: 61aca6d6 manual
// OFFSET: 0x007B6110
void CMap::PrepareMapObjDefs(bool addToScene) {
    int32_t waitLoad = 1;

    if (CMap::s_isStreamingMode || CMap::bPreload) {
        waitLoad = 0;
    }

#ifdef WOWDECOMP_TOOL_FEATURES
    if (const WorldEditView* view = WorldEditGetView(); view && view->m_allowPendingMapAssets) {
        waitLoad = 0;
    }
#endif

    CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head();

    while (mapObjDef) {
        CMapObj* mapObj = mapObjDef->mapObj;
        CAaBox& defBox = mapObjDef->m_bbox;

        if (CWorld::s_objectAreaOfInterest.b.x <= defBox.t.x && CWorld::s_objectAreaOfInterest.b.y <= defBox.t.y && CWorld::s_objectAreaOfInterest.b.z <= defBox.t.z && CWorld::s_objectAreaOfInterest.t.x >= defBox.b.x && CWorld::s_objectAreaOfInterest.t.y >= defBox.b.y && CWorld::s_objectAreaOfInterest.t.z >= defBox.b.z) {
            if (waitLoad && !mapObj->m_isGroupLoaded) {
                mapObj->WaitLoad();
            }

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) && mapObj->m_isGroupLoaded) {
                CMap::PrepareMapObjDef(mapObjDef, mapObj);
            }
        }

        float dist = NTempest::DistanceSquared(defBox, World::s_currentWorldPos);

        if (dist < mapObj->m_distToCamera) {
            mapObj->m_distToCamera = dist;
        }

        for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->owner);
            CMapObjGroup* group = mapObj->GetGroup(defGroup->groupNum, 1);
            CAaBox& box = defGroup->aaBox;

            if (CWorld::s_objectAreaOfInterest.b.x <= box.t.x && CWorld::s_objectAreaOfInterest.b.y <= box.t.y && CWorld::s_objectAreaOfInterest.b.z <= box.t.z && CWorld::s_objectAreaOfInterest.t.x >= box.b.x && CWorld::s_objectAreaOfInterest.t.y >= box.b.y && CWorld::s_objectAreaOfInterest.t.z >= box.b.z) {
                if (!(group->IsLoaded())) {
                    if (!group->asyncObject) {
                        mapObj->ReadGroup(defGroup->groupNum);
                    }

                    if (waitLoad && CWorld::s_groupAreaOfInterest.b.x <= box.t.x && CWorld::s_groupAreaOfInterest.b.y <= box.t.y && CWorld::s_groupAreaOfInterest.b.z <= box.t.z && CWorld::s_groupAreaOfInterest.t.x >= box.b.x && CWorld::s_groupAreaOfInterest.t.y >= box.b.y && CWorld::s_groupAreaOfInterest.t.z >= box.b.z) {
                        mapObj->WaitLoadGroup(defGroup->groupNum);
                    }
                }

                group->unused_0190 = 0.0f;

                if (group->IsLoaded()) {
                    if (!(defGroup->m_flags & MAPOBJ_FLAG_GROUP_INIT)) {
                        CMapObj::InitDefGroup(mapObj, group, mapObjDef, defGroup);
                    }

                    if (!(defGroup->m_flags & MAPOBJ_FLAG_REFS_CREATED)) {
                        CMapObj::CreateRefs(mapObj, group, mapObjDef, defGroup);
                        CMap::FreeBaseObjLinksInBounds(&box);
                    }
                }
            }

            float groupDist = NTempest::DistanceSquared(box, World::s_currentWorldPos);

            if (groupDist < group->m_distToCamera) {
                group->m_distToCamera = groupDist;
            }

            if (!addToScene || !(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
                continue;
            }

            if (!(group->IsLoaded())) {
                CWorldScene::s_barrier.AddBarrierMapObjDefGroup(defGroup, 50.0f);
                continue;
            }

            if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
                continue;
            }

            if (CWorldScene::boundingBox.Intersects(box)) {
                CWorldScene::AddMapObjDefGroup(mapObjDef, defGroup);
            }

            for (CMapObjOccluder* occluder = group->m_occluderList.Head(); occluder; occluder = group->m_occluderList.Next(occluder)) {
                C3Vector a = occluder->m_start * mapObjDef->m_mat;
                C3Vector b = occluder->m_end * mapObjDef->m_mat;
                CWorldView::AddOccluder(a, b);
            }
        }

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
            CWorldScene::s_barrier.AddBarrier(defBox, 50.0f);
        }

        mapObjDef = CMap::mapObjDefHash.Next(mapObjDef);
    }
}

// VERIFY: 6908aa4f manual
// OFFSET: 0x007B6B00
void CMap::PrepareUpdate(bool addToScene) {
    if (CWorld::status.m_chunksChanged) {
        CMap::PurgeMaps();
    }

    CMap::bspRecurseCount = 0;
    CMap::mapGetFacetsCount = 0;
    CMap::oldSelectLightParm = 0;
    CMapChunkLiquidBuf::Update(FrameTime::s_tickTimeSec);
    CMapObj::PrepareUpdate();
    CMapRenderChunk::PurgeChunkBufBlocks();
    CMap::PreUpdateAreas(addToScene);
    CMap::PrepareMapObjDefs(addToScene);
    CMap::PrepareMapDoodadDefs();
    CMap::PrepareEntitys(addToScene);

    if (!CMap::bPreload) {
        return;
    }

    if (!CMap::s_isStreamingMode) {
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.25f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(addToScene);
        CMap::PrepareMapObjDefs(addToScene);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.5f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(addToScene);
        CMap::PrepareMapObjDefs(addToScene);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.66f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(addToScene);
        CMap::PrepareMapObjDefs(addToScene);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.75f, World::s_loadProgressParam);
        }

        return;
    }

    CMapArea* area = CMap::GetAreaAtPosition(reinterpret_cast<const C2Vector*>(&World::s_currentWorldPos));

    if (area && area->asyncObject) {
        char fileName[260];
        SFile::GetActualFileName(area->asyncObject->file, fileName, sizeof(fileName));
        AsyncFile::EnterQueueLock();
        AsyncFileReadLinkObject(area->asyncObject, 1);
        AsyncFile::LeaveQueueLock();

        while (area->asyncObject) {
            int64_t local = 0;
            int64_t total = 0;
            SFile::FileGetIsLocalAmount(fileName, local, total);
            AsyncFileReadPollHandler(nullptr, nullptr);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(static_cast<float>(local) / static_cast<float>(total) * 0.2f, World::s_loadProgressParam);
            }

            OsSleep(10);
        }
    }

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(0.2f, World::s_loadProgressParam);
    }

    CMap::PreUpdateAreas(addToScene);
    CMap::PrepareMapObjDefs(addToScene);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);

    CMapChunk* chunk = CMap::GetChunkAtPosition(reinterpret_cast<const C2Vector*>(&World::s_currentWorldPos));
    float progress = 0.0f;
    uint32_t doodadTotal = CMap::ComputeDoodadLoadProgress(chunk, &progress, 0);

    if (doodadTotal) {
        uint32_t remaining;

        do {
            OsSleep(10);
            AsyncFileReadPollHandler(nullptr, nullptr);
            CMap::PreUpdateAreas(addToScene);
            CMap::PrepareMapObjDefs(addToScene);
            CMapObj::PrepareUpdate();
            CMap::PrepareMapDoodadDefs();
            remaining = CMap::ComputeDoodadLoadProgress(chunk, &progress, doodadTotal);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(progress * 0.25f + 0.2f, World::s_loadProgressParam);
            }
        } while (remaining);
    }

    CMap::PreUpdateAreas(addToScene);
    CMap::PrepareMapObjDefs(addToScene);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);

    uint32_t mapObjTotal = CMap::ComputeMapObjLoadProgress(chunk, &progress, 0);
    CMapObj::PrepareUpdate();

    if (mapObjTotal) {
        uint32_t remaining;

        do {
            OsSleep(10);
            AsyncFileReadPollHandler(nullptr, nullptr);
            CMap::PreUpdateAreas(addToScene);
            CMap::PrepareMapObjDefs(addToScene);
            CMapObj::PrepareUpdate();
            CMap::PrepareMapDoodadDefs();
            remaining = CMap::ComputeMapObjLoadProgress(chunk, &progress, mapObjTotal);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(static_cast<float>(mapObjTotal - remaining) * 0.3f / static_cast<float>(mapObjTotal) + 0.45f, World::s_loadProgressParam);
            }
        } while (remaining);
    }

    CMap::PreUpdateAreas(addToScene);
    CMap::PrepareMapObjDefs(addToScene);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);
}

// VERIFY: 6cf7464a auto
// OFFSET: 0x007BD480
int32_t CMap::SafeOpen(const char* fileName, SFileImpl** file) {
    int32_t tries = 10;

    while (!SFileOpenFile(fileName, file)) {
        DebugLog("CMap::SafeOpen() failed %s\n", fileName);

        if (!--tries) {
            SErrDisplayAppFatal("CMap::SafeOpen() failed %s", fileName);
            return 0;
        }
    }

    return 1;
}

// VERIFY: 3ace6e8d manual
// OFFSET: 0x007BD4D0
int32_t CMap::SafeRead(const char* filename, SFileImpl* file, void* buffer, uint32_t size) {
    STORM_ASSERT(filename);

    int32_t tries = 10;

    while (!SFileReadFile(file, buffer, size, nullptr, nullptr, 0)) {
        DebugLog("CMap::SafeRead() failed %s\n", filename);

        if (!--tries) {
            DebugLog("CMap::SafeRead() failed %s", filename);
            return 0;
        }
    }

    return 1;
}


// VERIFY: 69b638d4 auto
// OFFSET: 0x007BD540
void CMap::LoadTex() {
    char fileName[256];
    SStrCopy(fileName, CMap::wdtFilename, STORM_MAX_STR);
    char* ext = strrchr(fileName, '.');
    STORM_ASSERT(ext);
    SStrCopy(ext, ".tex", STORM_MAX_STR);
    TextureLoadBlob(fileName);
}

// VERIFY: 4c68f31b manual
// OFFSET: 0x007BD8A0
void CMap::ValidateShaders() {
    CMap::enableVertexShaders = CWorld::enables2 & Enable_VertexShader;
    CMap::enablePixelShaders = (CWorld::enables & Enables::Enable_PixelShaders) != 0;
    CMap::enableSpecular = (CWorld::enables & Enables::Enable_Specular) && (CWorld::enables & Enables::Enable_PixelShaders);

    bool valid = false;
    bool bigAlpha = (CMap::header.m_flags & SMMapHeader::MAP_FLAGS::BIG_ALPHA) != 0;
    CMap::gTerrainPixelShadersValid = false;
    CMap::enableSpecularTerrain = false;

    if (CWorld::enables & Enables::Enable_PixelShaders) {
        CGxShader* baseLayerShader = CMap::GetPixelShader(bigAlpha, true, false);
        CGxShader* layerShader = CMap::GetPixelShader(bigAlpha, false, false);
        valid = baseLayerShader && baseLayerShader->Valid() && layerShader && layerShader->Valid();
        CMap::gTerrainPixelShadersValid = valid;
    }

    if (CMap::enableSpecular && valid) {
        CGxShader* baseLayerShader = CMap::GetPixelShader(bigAlpha, true, false);
        CGxShader* layerShader = CMap::GetPixelShader(bigAlpha, false, false);
        CMap::enableSpecularTerrain = baseLayerShader && baseLayerShader->Valid() && layerShader && layerShader->Valid();
    }

    if (CMap::enableVertexShaders) {
        CMap::enableTerrainShaderVertex = CMap::gTerrainPixelShadersValid && CMap::vertexShader_Terrain[0] && CMap::vertexShader_Terrain[0]->Valid();
    }

    CMap::enableChunkBatching = 0;

    if (CMap::gTerrainPixelShadersValid) {
        CMap::enableChunkBatching = CMap::enableTerrainShaderVertex != 0;
    }
}

// VERIFY: a8f937fc auto
// OFFSET: 0x007BD9F0
void CMap::Preload() {
    CMap::PurgeMaps();
    CMapObj::ClearCache(0);
    CMap::ClearAllFootprints();
    TextureGxCacheFlush();
    CMap::bPreload = 1;
    CMap::PrepareUpdate(false);

    if (!CMap::s_isStreamingMode) {
        AsyncFileReadWaitAll();
    }

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(1.0f, World::s_loadProgressParam);
    }

    CMap::bPreload = 0;
    CWorld::status.m_prepareAll = 1;
    CShadowCache::s_needsRebuild = 1;
}

// VERIFY: 4426576e auto
// OFFSET: 0x007BDD70
void CMapObj::InitDefGroup(CMapObj* mapObj, CMapObjGroup* mapObjGroup, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    mapObjDefGroup->m_flags = static_cast<MapObjFlag>(mapObjDefGroup->m_flags | MAPOBJ_FLAG_GROUP_INIT);
}

// VERIFY: 5b940caf manual
// OFFSET: 0x007BDE50
void CMap::CreateMapObjDefGroups(CMapObj* mapObj, CMapObjDef* mapObjDef) {
    STORM_ASSERT(mapObj);
    STORM_ASSERT(mapObjDef);

    mapObjDef->m_defGroups.SetCount(mapObj->m_groupInfoCount);

    for (int32_t i = 0; i < mapObj->m_groupInfoCount; i++) {
        CMapObjDefGroup* mapObjDefGroup = CMap::AllocMapObjDefGroup();
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDefGroup);
        link->ref = mapObjDef;
        mapObjDef->groupLinkList.LinkToTail(link);
        mapObjDef->m_defGroups[i] = mapObjDefGroup;

        CAaBox box;
        mapObj->GetGroupBounds(&mapObjDefGroup->m_sphere, i);
        mapObjDefGroup->m_sphere.c *= mapObjDef->m_mat;
        mapObj->GetGroupBounds(&box, i);
        CWorldMath::TransformAABox(mapObjDef->m_mat, box, mapObjDefGroup->aaBox);

        STORM_ASSERT(mapObjDefGroup->aaBox.b != mapObjDefGroup->aaBox.t);

        mapObjDefGroup->groupNum = i;
        mapObjDefGroup->m_ambientColor = mapObjDef->m_argbColor;
        mapObjDefGroup->m_flags = MAPOBJ_FLAG_NONE;
        mapObjDefGroup->m_flags = static_cast<MapObjFlag>(mapObjDefGroup->m_flags | ((mapObj->GetGroupFlags(i) & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR_CLASSIFICATION_MASK) ? MAPOBJ_FLAG_EXTERIOR : MAPOBJ_FLAG_INTERIOR));
    }
}

// VERIFY: a7bf14e1 manual
// OFFSET: 0x007BECD0
CMapDoodadDef* CMap::CreateDoodadDef(const char* fileName, SMDoodadDef* def, C3Vector* center) {
    STORM_ASSERT(bActive);

    HASHKEY_DWORD key = { 0 };
    CMapDoodadDef* doodadDef = CMap::doodadDefHash.Ptr(def->m_uniqueId, key);

    if (doodadDef) {
        return doodadDef;
    }

    doodadDef = CMap::AllocDoodadDef();
    STORM_ASSERT(doodadDef);
    HASHKEY_DWORD insertKey = { 0 };
    CMap::doodadDefHash.Insert(doodadDef, def->m_uniqueId, insertKey);
    CMap::doodadDefList.LinkToTail(doodadDef);

    doodadDef->m_position.x = -def->m_position.z;
    doodadDef->m_position.y = -def->m_position.x;
    doodadDef->m_position.z = def->m_position.y;
    doodadDef->m_position.x = doodadDef->m_position.x + center->x;
    doodadDef->m_position.y = center->y + doodadDef->m_position.y;
    doodadDef->m_position.z = center->z + doodadDef->m_position.z;

    doodadDef->m_sphere.c = doodadDef->m_position;
    doodadDef->m_bbox.b = doodadDef->m_position;
    doodadDef->m_bbox.t = doodadDef->m_position;
    doodadDef->m_scale = def->m_scale * 0.0009765625f;
    doodadDef->m_sphere.r = 0.0f;
    doodadDef->m_flags = MAPOBJ_FLAG_UNPLACED;

    if (def->m_flags & SMDoodadDef::F_BIODOME) {
        doodadDef->m_flags = static_cast<MapObjFlag>(MAPOBJ_FLAG_BIODOME | MAPOBJ_FLAG_UNPLACED);
    }

    doodadDef->model = nullptr;

    float rotX = def->m_rotation.z * DEG_TO_RAD;
    float rotY = def->m_rotation.x * DEG_TO_RAD;
    float rotZ = DEG_TO_RAD * def->m_rotation.y;

    doodadDef->m_mat = C44Matrix();
    doodadDef->m_mat.d0 = doodadDef->m_position.x;
    doodadDef->m_mat.d1 = doodadDef->m_position.y;
    doodadDef->m_mat.d2 = doodadDef->m_position.z;
    doodadDef->m_mat.RotateAroundZ(rotZ + PI);
    doodadDef->m_mat.RotateAroundY(rotY);
    doodadDef->m_mat.RotateAroundX(rotX);
    doodadDef->m_mat.Scale(doodadDef->m_scale);
    doodadDef->m_lMat = C44Matrix();

    CM2Model* model = World::s_m2Scene->CreateModel(fileName, CM2Model::INSTANCE_INITIALIZE_ON_DEMAND);
    doodadDef->model = model;

    if (model) {
        model->SetWorldTransform(doodadDef->m_mat);
        doodadDef->model->SetEventCallback(CMapStaticEntity::ModelEventCallback, reinterpret_cast<uintptr_t>(doodadDef));
        doodadDef->model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        doodadDef->model->m_lightingArg = doodadDef;
        doodadDef->model->SetBoneSequence(M2_KEY_BONE_NONE, ANIM_STAND, M2_RANDOM_VARIATION, 0, 1.0f, 1, 1);
    }

    return doodadDef;
}

// VERIFY: 84c1a3e9 manual
// OFFSET: 0x007BEB40
CMapDoodadDef* CMap::CreateDoodadDef(const char* fileName, const C3Vector* position, float facing, BOOL waitForLoad) {
    STORM_ASSERT(fileName);
    STORM_ASSERT(bActive);

    int32_t id = CMap::uniqueId--;
    HASHKEY_DWORD key = { 0 };
    CMapDoodadDef* doodadDef = CMap::doodadDefHash.Ptr(id, key);
    STORM_ASSERT(doodadDef == nullptr); // 12213: "doodadDef == __null"
    doodadDef = CMap::AllocDoodadDef();
    STORM_ASSERT(doodadDef);
    HASHKEY_DWORD insertKey = { 0 };
    CMap::doodadDefHash.Insert(doodadDef, id, insertKey);
    doodadDef->m_position = *position;
    doodadDef->m_scale = 1.0f;
    doodadDef->m_sphere.r = 0.0f;
    doodadDef->m_sphere.c = doodadDef->m_position;
    doodadDef->m_bbox.b = doodadDef->m_position;
    doodadDef->m_bbox.t = doodadDef->m_position;
    doodadDef->m_mat = C44Matrix();
    doodadDef->m_mat.d0 = position->x;
    doodadDef->m_mat.d1 = position->y;
    doodadDef->m_mat.d2 = position->z;
    doodadDef->m_mat.RotateAroundZ(facing);
    doodadDef->m_lMat = C44Matrix();
    doodadDef->m_flags = MAPOBJ_FLAG_UNPLACED;
    doodadDef->model = nullptr;
    CMapDoodadDef::CreateModel(fileName, doodadDef, waitForLoad, 0);

    return doodadDef;
}

// VERIFY: ea13d7f6 manual
// OFFSET: 0x007BEF40
CMapDoodadDef* CMap::CreateDoodadDef(uint32_t doodadRef, SMODoodadDef* def, char* name, uint32_t key, C44Matrix* mat, uint16_t doodadSet) {
    STORM_ASSERT(bActive);

    HASHKEY_DWORD hashKey = { key };
    CMapDoodadDef* doodadDef = CMap::doodadDefHash.Ptr(doodadRef, hashKey);

    if (doodadDef) {
        return doodadDef;
    }

    doodadDef = CMap::AllocDoodadDef();
    STORM_ASSERT(doodadDef);
    CMap::doodadDefHash.Insert(doodadDef, doodadRef, hashKey);
    CMap::doodadDefList.LinkToTail(doodadDef);

    doodadDef->m_position = def->m_position;
    doodadDef->m_position *= *mat;
    doodadDef->m_scale = def->m_scale;
    doodadDef->m_sphere.c = doodadDef->m_position;
    doodadDef->m_sphere.r = 0.0f;
    doodadDef->m_bbox.b = doodadDef->m_position;
    doodadDef->m_bbox.t = doodadDef->m_position;
    doodadDef->wmoSpecialDoodadSet = doodadSet;
    doodadDef->m_flags = MAPOBJ_FLAG_UNPLACED;

    if (def->m_flags & SMODoodadDef::F_ACCEPT_PROJ_TEX) {
        doodadDef->m_flags = static_cast<MapObjFlag>(MAPOBJ_FLAG_PROJ_TEX | MAPOBJ_FLAG_UNPLACED);
    }

    doodadDef->model = nullptr;
    doodadDef->m_mat = C44Matrix();
    doodadDef->m_mat.Translate(def->m_position);
    doodadDef->m_mat.Rotate(def->m_orientation);
    doodadDef->m_mat.Scale(doodadDef->m_scale);
    doodadDef->m_lMat = doodadDef->m_mat;
    doodadDef->m_mat *= *mat;

    CImVector limit;
    *reinterpret_cast<uint32_t*>(&limit) = MIN_DOODAD_DIRCOLOR;
    doodadDef->AdjustLightmap(&def->m_color, &doodadDef->m_m2DiffuseColor, limit, &doodadDef->m_m2AmbientColor, MAX_AMBIENT);

    CM2Model* model = World::s_m2Scene->CreateModel(name, CM2Model::INSTANCE_INITIALIZE_ON_DEMAND);
    doodadDef->model = model;

    if (model) {
        model->SetWorldTransform(doodadDef->m_mat);
        doodadDef->model->SetEventCallback(CMapStaticEntity::ModelEventCallback, reinterpret_cast<uintptr_t>(doodadDef));
        doodadDef->model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        doodadDef->model->m_lightingArg = doodadDef;
        doodadDef->model->SetBoneSequence(M2_KEY_BONE_NONE, ANIM_STAND, M2_RANDOM_VARIATION, 0, 1.0f, 1, 1);
    }

    return doodadDef;
}

// VERIFY: 140b192a manual
// OFFSET: 0x007BF460
CMapObjDef* CMap::CreateMapObjDef(char* fileName, SMMapObjDef* mapObjectDef, C3Vector* center, bool cached) {
    CMapObjDef* mapObjDef;

    if (cached && (mapObjDef = CMap::mapObjDefHash.Ptr(mapObjectDef->m_uniqueId, CMap::s_mapObjDefHashKey)) != nullptr) {
        return mapObjDef;
    }

    mapObjDef = CMap::AllocMapObjDef();

    STORM_ASSERT(mapObjDef);

    if (cached) {
        CMap::mapObjDefHash.Insert(mapObjDef, mapObjectDef->m_uniqueId, CMap::s_mapObjDefHashKey);
    }

    mapObjDef->m_position.x = -mapObjectDef->m_position.z;
    mapObjDef->m_position.y = -mapObjectDef->m_position.x;
    mapObjDef->m_position.z = mapObjectDef->m_position.y;
    mapObjDef->m_position.x = center->x + mapObjDef->m_position.x;
    mapObjDef->m_position.y = mapObjDef->m_position.y + center->y;
    mapObjDef->m_position.z = center->z + mapObjDef->m_position.z;

    float rotX = mapObjectDef->m_rotation.z * DEG_TO_RAD;
    float rotY = mapObjectDef->m_rotation.x * DEG_TO_RAD;
    float rotZ = DEG_TO_RAD * mapObjectDef->m_rotation.y;

    mapObjDef->m_flags = MAPOBJ_FLAG_NONE;
    mapObjDef->m_nameId = mapObjectDef->m_nameId;
    mapObjDef->m_doodadSet = mapObjectDef->m_doodadSet;
    mapObjDef->m_nameSet = mapObjectDef->m_nameSet;
    mapObjDef->m_objectGuid = WOWGUID();
    mapObjDef->m_doodadSetOverrides[0] = 0;
    mapObjDef->m_doodadSetOverrides[1] = 0;
    mapObjDef->m_doodadSetOverrides[2] = 0;

    mapObjDef->m_mat.Identity();
    mapObjDef->m_mat.d0 = mapObjDef->m_position.x;
    mapObjDef->m_mat.d1 = mapObjDef->m_position.y;
    mapObjDef->m_mat.d2 = mapObjDef->m_position.z;
    mapObjDef->m_mat.RotateAroundZ(rotZ + PI);
    mapObjDef->m_mat.RotateAroundY(rotY);
    mapObjDef->m_mat.RotateAroundX(rotX);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

    mapObjDef->m_bbox.b.x = -mapObjectDef->m_extents.t.z;
    mapObjDef->m_bbox.b.y = -mapObjectDef->m_extents.t.x;
    mapObjDef->m_bbox.b.z = mapObjectDef->m_extents.b.y;
    mapObjDef->m_bbox.b.x = center->x + mapObjDef->m_bbox.b.x;
    mapObjDef->m_bbox.b.y = mapObjDef->m_bbox.b.y + center->y;
    mapObjDef->m_bbox.b.z = center->z + mapObjDef->m_bbox.b.z;
    mapObjDef->m_bbox.t.x = -mapObjectDef->m_extents.b.z;
    mapObjDef->m_bbox.t.y = -mapObjectDef->m_extents.b.x;
    mapObjDef->m_bbox.t.z = mapObjectDef->m_extents.t.y;
    mapObjDef->m_bbox.t.x = center->x + mapObjDef->m_bbox.t.x;
    mapObjDef->m_bbox.t.y = mapObjDef->m_bbox.t.y + center->y;
    mapObjDef->m_bbox.t.z = mapObjDef->m_bbox.t.z + center->z;

    mapObjDef->m_sphere.c.x = (mapObjDef->m_bbox.b.x + mapObjDef->m_bbox.t.x) * 0.5f;
    mapObjDef->m_sphere.c.y = (mapObjDef->m_bbox.t.y + mapObjDef->m_bbox.b.y) * 0.5f;
    mapObjDef->m_sphere.c.z = 0.5f * (mapObjDef->m_bbox.t.z + mapObjDef->m_bbox.b.z);
    float dz = mapObjDef->m_bbox.t.z - mapObjDef->m_sphere.c.z;
    float dy = mapObjDef->m_bbox.t.y - mapObjDef->m_sphere.c.y;
    float dx = mapObjDef->m_bbox.t.x - mapObjDef->m_sphere.c.x;
    mapObjDef->m_sphere.r = sqrtf(dx * dx + dy * dy + dz * dz);

    mapObjDef->m_lightsArray.m_count = 0;
    mapObjDef->mapObj = CMapObj::Create(fileName);

    return mapObjDef;
}

// VERIFY: 5ac717a0 manual
// OFFSET: 0x007BF120
CMapObjDef* CMap::CreateMapObjDef(const char* fileName, const C3Vector* position, float facing, BOOL waitLoad, uint32_t doodadSetCount, const int16_t* doodadSets, float* radius, uint32_t id) {
    STORM_ASSERT(fileName);

    uint32_t uniqueId = id;

    if (!id) {
        uniqueId = CMap::uniqueId--;
    }

    CMapObjDef* mapObjDef = CMap::mapObjDefHash.Ptr(uniqueId, CMap::s_mapObjDefHashKey);
    STORM_ASSERT(mapObjDef == nullptr); // 12213: "mapObjDef == __null"
    mapObjDef = CMap::AllocMapObjDef();
    STORM_ASSERT(mapObjDef);
    CMap::mapObjDefHash.Insert(mapObjDef, uniqueId, CMap::s_mapObjDefHashKey);
    CMapObj* mapObj = CMapObj::Create(const_cast<char*>(fileName));
    STORM_ASSERT(mapObj);
    mapObjDef->mapObj = mapObj;

    if (waitLoad && !mapObj->m_isGroupLoaded) {
        mapObj->WaitLoad();
    }

    mapObjDef->m_flags = MAPOBJ_FLAG_NONE;

    if (doodadSetCount) {
        mapObjDef->m_flags = MAPOBJ_FLAG_MAPOBJDEF;
    }

    mapObjDef->m_nameId = 0;
    mapObjDef->m_doodadSet = 0;
    mapObjDef->m_nameSet = 0;
    mapObjDef->m_position = *position;
    mapObjDef->m_objectGuid = WOWGUID();

    for (int32_t i = 0; i < 3; i++) {
        mapObjDef->m_doodadSetOverrides[i] = i < static_cast<int32_t>(doodadSetCount) ? doodadSets[i] : 0;
    }

    mapObjDef->m_mat = C44Matrix();
    mapObjDef->m_mat.d0 = position->x;
    mapObjDef->m_mat.d1 = position->y;
    mapObjDef->m_mat.d2 = position->z;
    mapObjDef->m_mat.RotateAroundZ(facing);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

    if (mapObj->m_isGroupLoaded) {
        CAaBox bounds(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
        mapObj->GetBounds(mapObjDef->m_sphere);
        mapObjDef->m_sphere.c *= mapObjDef->m_mat;
        mapObj->GetBounds(&bounds);
        CWorldMath::TransformAABox(mapObjDef->m_mat, bounds, mapObjDef->m_bbox);
        mapObjDef->m_lightsArray.m_count = 0;
        return mapObjDef;
    }

    float sphereRadius;

    if (radius) {
        CAaBox extents;
        extents.b.x = *radius * -1.0f;
        extents.b.y = extents.b.x;
        extents.b.z = extents.b.x;
        extents.t.x = *radius;
        extents.t.y = *radius;
        extents.t.z = *radius;
        CWorldMath::TransformAABox(mapObjDef->m_mat, extents, mapObjDef->m_bbox);
        sphereRadius = *radius;
    } else {
        sphereRadius = 0.0f;
        mapObjDef->m_bbox.b = *position;
        mapObjDef->m_bbox.t = *position;
    }

    mapObjDef->m_sphere.c = *position;
    mapObjDef->m_sphere.r = sphereRadius;
    mapObjDef->m_lightsArray.m_count = 0;

    return mapObjDef;
}

// VERIFY: aade514a manual
// OFFSET: 0x007BF8B0
void CMap::LoadWdt() {
    SFileImpl* file = nullptr;
    SFileOpenFile(CMap::wdtFilename, &file);

    if (!file) {
        SErrDisplayAppFatal("CMap::LoadWdt() failed %s\n", CMap::wdtFilename);
    }

    SIffChunk iffChunk;
    iffChunk.token = 0;
    iffChunk.m_size = 0;

    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    STORM_ASSERT(iffChunk.token == SIffChunk::TOKEN_MVER); // 12213: "iffChunk.token=='MVER'"
    SFileReadFile(file, &CMap::version, sizeof(CMap::version), nullptr, nullptr, 0);
    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    STORM_ASSERT(iffChunk.token == SIffChunk::TOKEN_MPHD); // 12213: "iffChunk.token=='MPHD'"
    SFileReadFile(file, &CMap::header, sizeof(CMap::header), nullptr, nullptr, 0);
    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    STORM_ASSERT(iffChunk.token == SIffChunk::TOKEN_MAIN); // 12213: "iffChunk.token=='MAIN'"
    SFileReadFile(file, CMap::m_areaInfo, sizeof(CMap::m_areaInfo), nullptr, nullptr, 0);

    if (CMap::header.m_flags & SMMapHeader::MAP_FLAGS::HAS_GLOBAL_WMO) {
        char fileName[256];
        SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
        STORM_ASSERT(iffChunk.token == SIffChunk::TOKEN_MWMO); // 12213: "iffChunk.token == 'MWMO'"
        SFileReadFile(file, fileName, iffChunk.m_size, nullptr, nullptr, 0);
        SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);

        STORM_ASSERT(iffChunk.token == SIffChunk::TOKEN_MODF); // 12213: "iffChunk.token == 'MODF'"

        SMMapObjDef def;
        SFileReadFile(file, &def, sizeof(def), nullptr, nullptr, 0);
        def.m_uniqueId = CMap::uniqueId--;

        CMapObjDef* mapObjDef = CMap::mapObjDefHash.Ptr(def.m_uniqueId, CMap::s_mapObjDefHashKey);

        if (!mapObjDef) {
            mapObjDef = CMap::AllocMapObjDef();
            CMap::mapObjDefHash.Insert(mapObjDef, def.m_uniqueId, CMap::s_mapObjDefHashKey);

            mapObjDef->m_position.x = -def.m_position.z;
            mapObjDef->m_position.y = -def.m_position.x;
            mapObjDef->m_position.z = def.m_position.y;

            float rotX = def.m_rotation.z * DEG_TO_RAD;
            float rotY = def.m_rotation.x * DEG_TO_RAD;
            float rotZ = DEG_TO_RAD * def.m_rotation.y;

            mapObjDef->m_flags = MAPOBJ_FLAG_NONE;
            mapObjDef->m_nameId = def.m_nameId;
            mapObjDef->m_doodadSet = def.m_doodadSet;
            mapObjDef->m_nameSet = def.m_nameSet;
            mapObjDef->m_objectGuid = WOWGUID();
            mapObjDef->m_doodadSetOverrides[0] = 0;
            mapObjDef->m_doodadSetOverrides[1] = 0;
            mapObjDef->m_doodadSetOverrides[2] = 0;

            mapObjDef->m_mat.Identity();
            mapObjDef->m_mat.d0 = mapObjDef->m_position.x;
            mapObjDef->m_mat.d1 = mapObjDef->m_position.y;
            mapObjDef->m_mat.d2 = mapObjDef->m_position.z;
            mapObjDef->m_mat.RotateAroundZ(rotZ + PI);
            mapObjDef->m_mat.RotateAroundY(rotY);
            mapObjDef->m_mat.RotateAroundX(rotX);
            mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

            mapObjDef->m_bbox.b.x = -def.m_extents.t.z;
            mapObjDef->m_bbox.b.y = -def.m_extents.t.x;
            mapObjDef->m_bbox.b.z = def.m_extents.b.y;
            mapObjDef->m_bbox.t.x = -def.m_extents.b.z;
            mapObjDef->m_bbox.t.y = -def.m_extents.b.x;
            mapObjDef->m_bbox.t.z = def.m_extents.t.y;

            mapObjDef->m_sphere.c.x = (mapObjDef->m_bbox.b.x + mapObjDef->m_bbox.t.x) * 0.5f;
            mapObjDef->m_sphere.c.y = (mapObjDef->m_bbox.t.y + mapObjDef->m_bbox.b.y) * 0.5f;
            mapObjDef->m_sphere.c.z = 0.5f * (mapObjDef->m_bbox.t.z + mapObjDef->m_bbox.b.z);
            float dz = mapObjDef->m_bbox.t.z - mapObjDef->m_sphere.c.z;
            float dy = mapObjDef->m_bbox.t.y - mapObjDef->m_sphere.c.y;
            float dx = mapObjDef->m_bbox.t.x - mapObjDef->m_sphere.c.x;
            mapObjDef->m_sphere.r = sqrtf(dx * dx + dy * dy + dz * dz);

            mapObjDef->m_lightsArray.m_count = 0;
            mapObjDef->mapObj = CMapObj::Create(fileName);
        }

        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDef);
        link->ref = nullptr;
        CMap::mapObjDefLinkList.LinkToTail(link);
        CMap::bDungeon = 1;
    }

    if (CMap::header.m_flags & SMMapHeader::MAP_FLAGS::VERTEX_COLORS) {
        CMapRenderChunk::SetVertexFormat(GxVBF_PNC);
    } else {
        CMapRenderChunk::SetVertexFormat(GxVBF_PN);
    }

    CMap::ValidateShaders();
    SFileCloseFile(file);
}

// VERIFY: 99e76495 manual
// OFFSET: 0x007BFCE0
void CMap::Load(const char* mapName, int32_t zoneID) {
    CMap::s_enableTriangleStrips = false;

    uint32_t length = SStrCopy(CMap::mapPath, "World\\Maps\\", STORM_MAX_STR);
    SStrCopy(&CMap::mapPath[length], mapName, STORM_MAX_STR);
    SStrCopy(CMap::mapName, mapName, STORM_MAX_STR);
    SStrPrintf(CMap::wdtFilename, sizeof(CMap::wdtFilename), "%s\\%s.wdt", CMap::mapPath, CMap::mapName);

    CMap::s_mapLight = CMap::CreateLight(1, 0);
    CMap::s_mapLight->m_light.SetLightType(M2LIGHT_DIRECT);
    CMap::EnableLight(CMap::s_mapLight);
    CMap::UpdateLight(CMap::s_mapLight);
    CMap::PurgeMaps();
    CMapObj::ClearCache(1);
    CMap::ClearAllFootprints();

    s_mapId = zoneID;
    CMap::bActive = 1;
    CMap::bDungeon = 0;
    CMap::bPreload = 1;

    int32_t streaming = SFile::IsStreamingMode();
    CMap::s_isStreamingMode = SFile::IsTrial() | streaming;
    bool wait = CMap::s_isStreamingMode == 0;

    CMap::s_lowDetail.Load(CMap::mapPath, CMap::mapName);
    CMap::LoadWdt();
    CMap::LoadTex();
    DayNight::LoadMap(zoneID);
    CMap::PrepareUpdate(false);

    if (wait) {
        AsyncFileReadWaitAll();
    }

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(1.0f, World::s_loadProgressParam);
    }

    CMap::bPreload = 0;
    World::s_loadProgressCallback = nullptr;
    CWorld::status.m_prepareAll = 1;
#if defined(WOWDECOMP_ASSERTIONS)
    CMapChunkLiquidBuf::AssertCacheEmpty();
#endif
}

// VERIFY: 512418e2 auto
// OFFSET: 0x007BFF20
CMapObj* CMap::AllocMapObj() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObj* mapObj = new (object) CMapObj();
    mapObj->m_objectIndex = memHandle;

    return mapObj;
}

// VERIFY: a3468102 manual
// OFFSET: 0x007BFF70
void CMap::FreeMapObj(CMapObj* mapObj) {
    STORM_ASSERT(mapObj);

    mapObj->m_asyncLoadLink.Unlink();
    mapObj->~CMapObj();
    ObjectFree(*CMap::mapObjHeap, mapObj->m_objectIndex);
}

// VERIFY: 3e380871 auto
// OFFSET: 0x007BFFE0
CMapObjGroup* CMap::AllocMapObjGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjGroupHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObjGroup* mapObjGroup = new (object) CMapObjGroup();
    mapObjGroup->m_objectIndex = memHandle;

    return mapObjGroup;
}

// VERIFY: 13962642 manual
// OFFSET: 0x007C0030
void CMap::FreeMapObjGroup(CMapObjGroup* group) {
    STORM_ASSERT(group);

    group->m_asyncLoadLink.Unlink();
    group->~CMapObjGroup();
    ObjectFree(*CMap::mapObjGroupHeap, group->m_objectIndex);
}

// VERIFY: 8cfa8101 manual
// OFFSET: 0x007C00A0
void CMap::FreeArea(CMapArea* area) {
    STORM_ASSERT(area);

    STORM_ASSERT(area->m_parentLinkList.Head() == NULL);
    STORM_ASSERT(area->chunkLinkList.Head() == NULL);
    STORM_ASSERT(area->m_textures.Count() == 0);

    area->m_lameAssLink.Unlink();
    area->~CMapArea();
    ObjectFree(*CMap::areaHeap, area->m_objectIndex);
}

// VERIFY: 53440967 manual
// OFFSET: 0x007C0110
void CMap::FreeAreaMed(CMapAreaMed* areaMed) {
    STORM_ASSERT(areaMed);

    areaMed->m_lameAssLink.Unlink();
    areaMed->~CMapAreaMed();
    ObjectFree(*CMap::areaMedHeap, areaMed->m_objectIndex);
}

// VERIFY: 27b1c9a8 manual
// OFFSET: 0x007C0180
void CMap::FreeChunk(CMapChunk* chunk) {
    STORM_ASSERT(chunk);

    STORM_ASSERT(chunk->m_parentLinkList.Head() == NULL);
    STORM_ASSERT(chunk->doodadDefLinkList.Head() == NULL);
    STORM_ASSERT(chunk->mapObjDefLinkList.Head() == NULL);
    STORM_ASSERT(chunk->entityLinkList.Head() == NULL);
    STORM_ASSERT(chunk->detailDoodadInst == NULL);
    STORM_ASSERT(chunk->m_renderChunk == NULL);

    chunk->m_lameAssLink.Unlink();
    chunk->~CMapChunk();
    ObjectFree(*CMap::chunkHeap, chunk->m_objectIndex);
}

// VERIFY: d1b91d2a auto
// OFFSET: 0x007C01F0
CMapDoodadDef* CMap::AllocDoodadDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::doodadDefHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapDoodadDef* doodadDef = new (object) CMapDoodadDef();
    doodadDef->m_objectIndex = memHandle;

    return doodadDef;
}

// VERIFY: ef01a812 manual
// OFFSET: 0x007C0240
void CMap::FreeDoodadDef(CMapDoodadDef* doodadDef) {
    STORM_ASSERT(doodadDef);

    SI2::StopSound(&doodadDef->m_soundKitObject, 0, -1.0f, 1);
    STORM_ASSERT(doodadDef->m_parentLinkList.Head() == NULL);
    STORM_ASSERT(doodadDef->model == NULL);

    doodadDef->m_lameAssLink.Unlink();
    CMap::doodadDefHash.Unlink(doodadDef);
    uint32_t* heap = CMap::doodadDefHeap;
    doodadDef->~CMapDoodadDef();
    ObjectFree(*heap, doodadDef->m_objectIndex);
}

// VERIFY: d7c82155 manual
// OFFSET: 0x007C02D0
void CMap::FreeEntity(CMapEntity* entity) {
    STORM_ASSERT(entity);

    STORM_ASSERT(entity->m_parentLinkList.Head() == NULL);

    entity->m_lameAssLink.Unlink();
    entity->~CMapEntity();
    ObjectFree(*CMap::entityHeap, entity->m_objectIndex);
}

// VERIFY: 56cb0afe auto
// OFFSET: 0x007C0340
void CMap::FreeLight(CMapLight* light) {
    STORM_ASSERT(light);
    STORM_ASSERT(light->m_parentLinkList.Head() == NULL);

    light->~CMapLight();
    ObjectFree(*CMap::lightHeap, light->m_objectIndex);
}

// VERIFY: eeef95f1 manual
// OFFSET: 0x007C0370
void CMap::FreeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    STORM_ASSERT(mapObjDefGroup);
    STORM_ASSERT(mapObjDefGroup->GetRefCount() == 0);

    STORM_ASSERT(mapObjDefGroup->m_parentLinkList.Head() == NULL);
    STORM_ASSERT(mapObjDefGroup->doodadDefLinkList.Head() == NULL);
    STORM_ASSERT(mapObjDefGroup->entityLinkList.Head() == NULL);
    STORM_ASSERT(mapObjDefGroup->lightLinkList.Head() == NULL);

    mapObjDefGroup->m_lameAssLink.Unlink();
    mapObjDefGroup->~CMapObjDefGroup();
    ObjectFree(*CMap::mapObjDefGroupHeap, mapObjDefGroup->m_objectIndex);
}

// VERIFY: fe23a5b5 auto
// OFFSET: 0x007C03E0
CMapObjDef* CMap::AllocMapObjDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjDefHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObjDef* mapObjDef = new (object) CMapObjDef();
    mapObjDef->m_objectIndex = memHandle;

    return mapObjDef;
}

// VERIFY: eb574028 manual
// OFFSET: 0x007C0430
void CMap::FreeMapObjDef(CMapObjDef* mapObjDef) {
    STORM_ASSERT(mapObjDef);

    STORM_ASSERT(mapObjDef->m_parentLinkList.Head() == NULL);
    STORM_ASSERT(mapObjDef->groupLinkList.Head() == NULL);

    mapObjDef->m_lameAssLink.Unlink();
    CMap::mapObjDefHash.Unlink(mapObjDef);
    uint32_t* heap = CMap::mapObjDefHeap;
    mapObjDef->~CMapObjDef();
    ObjectFree(*heap, mapObjDef->m_objectIndex);
}

// VERIFY: 37cad926 manual
// OFFSET: 0x007C04A0
void CMap::FreeChunkLiquid(CChunkLiquid* chunkLiquid) {
    STORM_ASSERT(chunkLiquid);

    chunkLiquid->m_link.Unlink();
    chunkLiquid->~CChunkLiquid();
    ObjectFree(*CMap::chunkLiquidHeap, chunkLiquid->m_objectIndex);
}

// VERIFY: 03529b4c manual
// OFFSET: 0x007C0500
CMapRenderChunk* CMap::AllocRenderChunk() {
    CMapRenderChunk* renderChunk = CMap::s_mapRenderChunkFreeList.Head();

    if (!renderChunk) {
        renderChunk = CMap::s_mapRenderChunkFreeList.NewNode(TSLIST_LINK_BEFORE, 0, 0);
        renderChunk->m_renderChunkLink.Unlink();
        return renderChunk;
    }

    renderChunk->m_renderChunkLink.Unlink();
    new (renderChunk) CMapRenderChunk();

    return renderChunk;
}

// VERIFY: 8d425b50 manual
// OFFSET: 0x007C0610
void CMap::FreeRenderChunk(CMapRenderChunk* renderChunk) {
    STORM_ASSERT(renderChunk);

    renderChunk->m_renderChunkLink.Unlink();
    renderChunk->~CMapRenderChunk();
    CMap::s_mapRenderChunkFreeList.LinkToHead(renderChunk);
}

// VERIFY: d11dd1c9 auto
// OFFSET: 0x007C0670
CMapEntity* CMap::AllocEntity(int32_t linkToHead) {
    uint32_t memHandle;
    void* object = nullptr;
    CMapEntity* entity;

    if (ObjectAlloc(*CMap::entityHeap, &memHandle, &object, false)) {
        entity = new (object) CMapEntity();
        entity->m_objectIndex = memHandle;
    } else {
        entity = nullptr;
    }

    if (linkToHead) {
        CMap::entityList.LinkToHead(entity);
    } else {
        CMap::entityList.LinkToTail(entity);
    }

    return entity;
}

// VERIFY: 3adb704b manual
// OFFSET: 0x007C0750
CMapBaseObjLink* CMap::AllocBaseObjLink(CMapBaseObj* owner) {
    STORM_ASSERT(owner);

    uint32_t memHandle;
    void* object = nullptr;
    CMapBaseObjLink* link;

    if (ObjectAlloc(*CMap::baseObjLinkHeap, &memHandle, &object, false)) {
        link = new (object) CMapBaseObjLink;
        link->m_objectIndex = memHandle;
    } else {
        link = nullptr;
    }

    owner->m_refCount++;
    link->owner = owner;
    link->ref = nullptr;
    owner->m_parentLinkList.LinkToTail(link);

    return link;
}

// VERIFY: a8354093 auto
// OFFSET: 0x007C07C0
CMapArea* CMap::AllocArea() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::areaHeap, &memHandle, &object, false)) {
        CMap::areaList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapArea* area = new (object) CMapArea();
    area->m_objectIndex = memHandle;
    CMap::areaList.LinkToTail(area);

    return area;
}

// VERIFY: 2acaa66b auto
// OFFSET: 0x007C0830
CMapChunk* CMap::AllocMapChunk() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::chunkHeap, &memHandle, &object, false)) {
        CMap::chunkList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapChunk* chunk = new (object) CMapChunk();
    chunk->m_objectIndex = memHandle;
    CMap::chunkList.LinkToTail(chunk);

    return chunk;
}

// VERIFY: 430261e3 auto
// OFFSET: 0x007C08A0
CMapLight* CMap::AllocLight() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::lightHeap, &memHandle, &object, false)) {
        CMap::lightList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapLight* light = new (object) CMapLight();
    light->m_objectIndex = memHandle;
    CMap::lightList.LinkToTail(light);

    return light;
}

// VERIFY: 7128fd21 auto
// OFFSET: 0x007C0910
CMapObjDefGroup* CMap::AllocMapObjDefGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjDefGroupHeap, &memHandle, &object, false)) {
        CMap::mapObjDefGroupList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapObjDefGroup* group = new (object) CMapObjDefGroup();
    group->m_objectIndex = memHandle;
    CMap::mapObjDefGroupList.LinkToTail(group);

    return group;
}

// VERIFY: ecff775f auto
// OFFSET: 0x007C0980
CChunkLiquid* CMap::AllocChunkLiquid() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::chunkLiquidHeap, &memHandle, &object, false)) {
        CMap::chunkLiquidList.LinkToTail(nullptr);
        return nullptr;
    }

    CChunkLiquid* liquid = new (object) CChunkLiquid();
    liquid->m_objectIndex = memHandle;
    CMap::chunkLiquidList.LinkToTail(liquid);

    return liquid;
}

// VERIFY: c9309208 manual
// OFFSET: 0x007C09F0
void CMap::FreeBaseObjLink(CMapBaseObjLink* link) {
    STORM_ASSERT(link);

    link->m_ownerLink.Unlink();
    link->m_refLink.Unlink();
    CMapBaseObj* owner = link->owner;
    STORM_ASSERT(owner);
    owner->m_refCount--;
    link->ref = nullptr;
    link->owner = nullptr;
    link->~CMapBaseObjLink();
    ObjectFree(*CMap::baseObjLinkHeap, link->m_objectIndex);
}

// VERIFY: f22f4d25 auto
// OFFSET: 0x007C0A90
CMapAreaLow* CMap::AllocAreaLow() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(CMap::areaLowHeap->m_heapId, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapAreaLow* areaLow = new (object) CMapAreaLow();
    areaLow->m_objectIndex = memHandle;

    return areaLow;
}

// VERIFY: 41bd42d1 manual
// OFFSET: 0x007C0AE0
void CMapAreaLowHeap::Delete(CMapAreaLow* areaLow) {
    areaLow->m_horizonLink.Unlink();
    ObjectFree(this->m_heapId, areaLow->m_objectIndex);
}

// VERIFY: 51f512f8 auto
// OFFSET: 0x007C0C60
void CMap::FreeAreaLow(CMapAreaLow* areaLow) {
    STORM_ASSERT(areaLow);
    STORM_ASSERT(areaLow->m_block == NULL); // 12213: "vbb == __null"

    CMap::areaLowHeap->Delete(areaLow);
}

// VERIFY: 9da97e61 auto
// OFFSET: 0x007C15F0
void CMap::ClassifyStaticEntityLink(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, uint32_t faceInfo, C3Vector* pos) {
    CMapObjGroup* group = mapObjDef->mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

    if (!group) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else if (group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else if (group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR_LIT) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_INTERIOR);
    }

    if (entity->m_flags & MAPOBJ_FLAG_INTERIOR) {
        entity->QueryInteriorLighting(mapObjDef, mapObjDefGroup->groupNum, reinterpret_cast<uint16_t*>(&faceInfo), pos);
    }
}

// VERIFY: d84c44fe manual
// OFFSET: 0x007C1660
bool CMap::LinkStaticEntityGetChunk(C3Vector& pos, float* outHeight, CMapChunk** outChunk) {
    *outChunk = nullptr;

    float mx = -(pos.y - HALF_WORLD_SIZE);
    float my = -(pos.x - HALF_WORLD_SIZE);
    STORM_ASSERT(mx >= 0.0f && my >= 0.0f);
    // 12213: "mx < ((64*16)*((150.0f/36.0f)*8)) && my < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(mx < 2.0f * HALF_WORLD_SIZE && my < 2.0f * HALF_WORLD_SIZE);

    int32_t cellX = CMath::fint_(mx * OO_COORD_TO_SUBCHUNK - 0.5f);
    int32_t cellY = CMath::fint_(OO_COORD_TO_SUBCHUNK * my - 0.5f);

    CMapArea* area = CMap::GetAreaForCell(cellX, cellY);

    if (!area || area->asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->GetChunkForCell(cellX, cellY);

    if (!chunk) {
        return false;
    }

    *outChunk = chunk;

    return CMap::GetHeightTerrain(chunk, pos, cellX, cellY, outHeight);
}

// VERIFY: abd054b1 manual
// OFFSET: 0x007C1DC0
void CMap::LinkIntersectMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, C3Vector& start, C3Vector& end, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    CMapObj* mapObj = mapObjDef->mapObj;
    CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

    if (!group) {
        return;
    }

    int32_t interiorFace = -1;
    int32_t groundFace = -1;

    C3Segment seg;
    seg.start = start;
    seg.end = end;

    int16_t interior = !(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR);

    if (group->GetFacesForLinking(seg, &interiorHit->t, &interiorFace, &groundHit->t, &groundFace)) {
        if (interiorFace != -1) {
            interiorHit->mapObjDef = mapObjDef;
            interiorHit->group = mapObjDefGroup;
            interiorHit->faceIndex = interiorFace;
            interiorHit->interior = interior;
        }

        if (groundFace != -1) {
            groundHit->faceIndex = groundFace;
            groundHit->mapObjDef = mapObjDef;
            groundHit->group = mapObjDefGroup;
            groundHit->interior = interior;
        }
    }

    if (!interior) {
        return;
    }

    float t = 1.05f;
    int32_t groups[2];

    if (mapObj->VectorIntersectPortals(mapObjDefGroup->groupNum, seg, &t, groups) && t - interiorHit->t < 0.0001f) {
        interiorHit->mapObjDef = mapObjDef;
        CMapObjDefGroup* portalGroup = mapObjDef->m_defGroups[groups[0]];
        interiorHit->t = t;
        interiorHit->group = portalGroup;
        interiorHit->faceIndex = -1;
        interiorHit->interior = !(mapObj->GetGroup(portalGroup->groupNum, 0)->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR);
    }
}

// VERIFY: e5163286 manual
// OFFSET: 0x007C1F20
void CMap::FreeBaseObjLinksInBounds(CAaBox* box) {
    for (CMapEntity* entity = CMap::entityList.Head(); entity; entity = CMap::entityList.Next(entity)) {
        if (box->t.x >= entity->m_bbox.b.x && box->t.y >= entity->m_bbox.b.y && box->t.z >= entity->m_bbox.b.z && box->b.x <= entity->m_bbox.t.x && box->b.y <= entity->m_bbox.t.y && box->b.z <= entity->m_bbox.t.z) {
            CMapBaseObjLink* link = entity->m_parentLinkList.Head();

            while (link) {
                CMapBaseObjLink* next = entity->m_parentLinkList.Next(link);
                CMap::FreeBaseObjLink(link);
                link = next;
            }
        }
    }
}

// VERIFY: 86bfa752 auto
// OFFSET: 0x007C1FF0
void CMap::LinkObjectToMapObjDefGroup(CMapStaticEntity* object, CMapObjDefGroup* mapObjDefGroup) {
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(object);
    link->ref = mapObjDefGroup;

    if (object->GetType() & CMapBaseObj::Type_Entity) {
        if (object->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_COLLIDABLE) {
            mapObjDefGroup->entityLinkList.LinkToHead(link);
            return;
        }

        mapObjDefGroup->entityLinkList.LinkToTail(link);
    } else if (object->GetType() & CMapBaseObj::Type_DoodadDef) {
        mapObjDefGroup->doodadDefLinkList.LinkToTail(link);
    } else {
        STORM_ASSERT(0);
    }
}

// VERIFY: d4db675a auto
// OFFSET: 0x007C2040
bool CMap::LinkObjectToMapExterior(CMapStaticEntity* object) {
    bool linked = false;

    if (CMap::bDungeon) {
        return false;
    }

    float maxX = -(object->m_bbox.t.x - HALF_WORLD_SIZE);
    float maxY = -(object->m_bbox.t.y - HALF_WORLD_SIZE);
    float minX = -(object->m_bbox.b.x - HALF_WORLD_SIZE);

    int32_t lastCol = CMath::fint_(-(object->m_bbox.b.y - HALF_WORLD_SIZE) * OO_COORD_TO_CHUNK - 0.5f);
    int32_t lastRow = CMath::fint_(minX * OO_COORD_TO_CHUNK - 0.5f);
    int32_t firstCol = CMath::fint_(maxY * OO_COORD_TO_CHUNK - 0.5f);
    int32_t firstRow = CMath::fint_(maxX * OO_COORD_TO_CHUNK - 0.5f);

    for (int32_t row = firstRow; row <= lastRow; row++) {
        for (int32_t col = firstCol; col <= lastCol; col++) {
            CMapArea* area = CMap::GetAreaForChunk(col, row);

            if (!area || area->asyncObject) {
                continue;
            }

            CMapChunk* chunk = area->GetChunk(col, row);

            if (!chunk || chunk->m_bbox.b.z > object->m_bbox.t.z) {
                continue;
            }

            CMapBaseObjLink* link = CMap::AllocBaseObjLink(object);
            link->ref = chunk;

            if (object->GetType() & CMapBaseObj::Type_Entity) {
                if (object->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_COLLIDABLE) {
                    chunk->entityLinkList.LinkToHead(link);
                } else {
                    chunk->entityLinkList.LinkToTail(link);
                }
            } else if (object->GetType() & CMapBaseObj::Type_DoodadDef) {
                chunk->doodadDefLinkList.LinkToTail(link);
            } else {
                STORM_ASSERT(0);
            }

            object->m_flags = static_cast<MapObjFlag>(object->m_flags | MAPOBJ_FLAG_EXTERIOR);
            linked = true;
        }
    }

    return linked;
}

// VERIFY: 4d233b7e manual
// OFFSET: 0x007C25D0
bool CMap::LinkIntersectMapObjDef(CMapObjDef* mapObjDef, C3Vector& start, C3Vector& end, C3Vector& mid, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    CMapObj* mapObj = mapObjDef->mapObj;

    if (!mapObj) {
        return false;
    }

    C3Vector localStart = start * mapObjDef->m_invMat;
    C3Vector localEnd = end * mapObjDef->m_invMat;
    C3Vector localMid = mid * mapObjDef->m_invMat;

    uint32_t noDepthSort = mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT;

    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);
        uint32_t groupFlags = mapObj->GetGroupFlags(group->groupNum);

        if (!(groupFlags & (CMapObjGroup::MOGP_FLAGS::F_ALWAYS_DRAW | CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400000 | CMapObjGroup::MOGP_FLAGS::F_UNREACHABLE)) && mapObj->TestGroupBounds(localStart, localEnd, group->groupNum) && ((!noDepthSort && (groupFlags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) || mapObj->TestGroupBounds(localMid, group->groupNum))) {
            CMap::LinkIntersectMapObjDefGroup(mapObjDef, group, localStart, localEnd, &interiorHit[noDepthSort == 0], &groundHit[noDepthSort == 0]);
        }
    }

    return true;
}

// VERIFY: c04d5774 manual
// OFFSET: 0x007C2700
bool CMap::LinkIntersectMapObjDefs(C3Vector& start, C3Vector& end, C3Vector& mid, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit, CMapChunk* chunk) {
    groundHit[0].t = 1.05f;
    groundHit[0].mapObjDef = nullptr;
    groundHit[1].t = 1.05f;
    groundHit[1].mapObjDef = nullptr;
    interiorHit[0].t = 1.05f;
    interiorHit[0].mapObjDef = nullptr;
    interiorHit[1].t = 1.05f;
    interiorHit[1].mapObjDef = nullptr;

    if (chunk) {
        for (CMapBaseObjLink* link = chunk->mapObjDefLinkList.Head(); link; link = chunk->mapObjDefLinkList.Next(link)) {
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->owner);

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) && mapObjDef->TestAABox(start, end)) {
                CMap::LinkIntersectMapObjDef(mapObjDef, start, end, mid, interiorHit, groundHit);
            }
        }
    } else {
        for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) && mapObjDef->TestAABox(start, end)) {
                CMap::LinkIntersectMapObjDef(mapObjDef, start, end, mid, interiorHit, groundHit);
            }
        }
    }

    if (!interiorHit[0].mapObjDef && groundHit[0].mapObjDef) {
        interiorHit[0] = groundHit[0];
    }

    if (!interiorHit[1].mapObjDef && groundHit[1].mapObjDef) {
        interiorHit[1] = groundHit[1];
    }

    if (!interiorHit[0].mapObjDef) {
        if (!interiorHit[1].mapObjDef) {
            return false;
        }

        interiorHit[0] = interiorHit[1];
        groundHit[0] = groundHit[1];
        interiorHit[1].mapObjDef = nullptr;
        groundHit[1].mapObjDef = nullptr;
    }

    if (!groundHit[0].mapObjDef) {
        groundHit[0] = interiorHit[0];
        groundHit[0].faceIndex = -1;
    }

    if (!groundHit[1].mapObjDef) {
        groundHit[1] = interiorHit[1];
        groundHit[1].faceIndex = -1;
    }

    return true;
}

// VERIFY: fbe3b25a manual
// OFFSET: 0x007C28F0
void CMap::LinkStaticEntity(CMapStaticEntity* entity, C3Vector& top, C3Vector& bottom, C3Vector& mid, int32_t& outInterior, int32_t& outHit, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    float height = 100000.0f;
    float terrainT = 2.0f;

    interiorHit[0].mapObjDef = nullptr;
    groundHit[0].mapObjDef = nullptr;
    interiorHit[1].mapObjDef = nullptr;
    groundHit[1].mapObjDef = nullptr;

    CMapChunk* chunk = nullptr;
    bool terrain = false;

    if (!CMap::bDungeon) {
        terrain = CMap::LinkStaticEntityGetChunk(top, &height, &chunk);
        terrainT = (top.z - height) * 0.001f;

        if (terrainT < 0.0f) {
            terrain = false;
        }
    }

    bool hit;

    if (entity->m_flags & MAPOBJ_FLAG_NO_MAPOBJ_LINK) {
        hit = false;
    } else {
        hit = CMap::LinkIntersectMapObjDefs(top, bottom, mid, interiorHit, groundHit, nullptr);
    }

    if (!terrain && !hit) {
        top = entity->visCenter;
        bottom = entity->visCenter;
        bottom.z = bottom.z + 1000.0f;
        CMap::LinkIntersectMapObjDefs(top, bottom, mid, interiorHit, groundHit, nullptr);
    }

    if (entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS) {
        interiorHit[1].mapObjDef = nullptr;
        groundHit[1].mapObjDef = nullptr;
    }

    if (terrain) {
        if (terrainT < interiorHit[0].t) {
            interiorHit[0].mapObjDef = nullptr;
            groundHit[0].mapObjDef = nullptr;
        }

        if (terrainT < interiorHit[1].t) {
            interiorHit[1].mapObjDef = nullptr;
            groundHit[1].mapObjDef = nullptr;
        }
    }

    outInterior = 0;
    outHit = 0;

    if (interiorHit[0].mapObjDef) {
        outHit = 1;
    }

    if (interiorHit[1].mapObjDef) {
        outHit = 1;
    }

    if (interiorHit[0].mapObjDef) {
        outInterior = interiorHit[0].interior;
    } else if (interiorHit[1].mapObjDef) {
        outInterior = interiorHit[1].interior;
    }
}

// VERIFY: c4018711 auto
// OFFSET: 0x007C2A70
void CMap::LinkStaticEntitySingle2(CMapStaticEntity* entity) {
    // 12213: "entity->visCenter != C3Vector(9999999.9f)"
    STORM_ASSERT(entity->visCenter != C3Vector(9999999.9f, 9999999.9f, 9999999.9f));

    C3Vector top(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z + 0.1f);
    C3Vector bottom(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z - 1000.0f);

    int32_t lightingIndex = 0;
    int32_t interior = 0;
    BOOL hit = 0;
    MapObjIntersectData placement[2];
    MapObjIntersectData lighting[2];

    CMap::LinkStaticEntity(entity, top, bottom, top, interior, hit, placement, lighting);

    uint32_t isEntity = entity->GetType() & CMapBaseObj::Type_Entity;
    uint32_t groundType = 0xFFFFFFFF;

    if (!hit) {
        STORM_ASSERT(!interior);
        CMap::LinkObjectToMapExterior(entity);

        if (isEntity) {
            static_cast<CMapEntity*>(entity)->m_groundType = CMap::QueryGroundTypeTerrain(&entity->m_position, &groundType) ? groundType : -1;
        }

        entity->m_flags = static_cast<MapObjFlag>((entity->m_flags & ~MAPOBJ_FLAG_IN_MAPOBJ) | MAPOBJ_FLAG_EXTERIOR);
        return;
    }

    if (!lighting[0].mapObjDef) {
        lightingIndex = 1;
    }

    STORM_ASSERT(lighting[lightingIndex].mapObjDef);

    if (placement[0].mapObjDef) {
        CMap::LinkObjectToMapObjDefGroup(entity, placement[0].group);
    }

    if (placement[1].mapObjDef) {
        CMap::LinkObjectToMapObjDefGroup(entity, placement[1].group);
    }

    if (isEntity) {
        static_cast<CMapEntity*>(entity)->m_groundType = lighting[lightingIndex].mapObjDef->GetGroundType(lighting[lightingIndex].group->groupNum, lighting[lightingIndex].faceIndex);
    }

    entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_IN_MAPOBJ);

    if (!interior) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
        return;
    }

    entity->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_INSIDE;

    C3Vector pos(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z - (bottom.z - top.z) * lighting[lightingIndex].t);
    CMap::ClassifyStaticEntityLink(entity, lighting[lightingIndex].mapObjDef, lighting[lightingIndex].group, lighting[lightingIndex].faceIndex, &pos);
}

// VERIFY: 68d8dacb manual
// OFFSET: 0x007C2BF0
void CMap::LinkStaticEntityMultiple2ToMapObjDefExterior(CMapStaticEntity* entity) {
    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        if (!mapObj || !mapObj->m_isGroupLoaded) {
            continue;
        }

        CAaBox box;
        CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, box);

        if (!mapObj->TestBounds(box)) {
            continue;
        }

        for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
            CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);
            uint32_t groupFlags = mapObj->GetGroupFlags(group->groupNum);

            if (!(groupFlags & (CMapObjGroup::MOGP_FLAGS::F_ALWAYS_DRAW | CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400000 | CMapObjGroup::MOGP_FLAGS::F_UNREACHABLE)) && (groupFlags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) && mapObj->TestGroupBounds(box, group->groupNum, 1)) {
                CMap::LinkObjectToMapObjDefGroup(entity, group);
            }
        }
    }
}

// VERIFY: 3091ac75 manual
// OFFSET: 0x007C2D30
void CMap::LinkStaticEntityMultiple2ToMapObjDefInterior(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    CMapObj* mapObj = mapObjDef->mapObj;

    if (!mapObj || !mapObj->m_isGroupLoaded) {
        return;
    }

    CAaBox box;
    CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, box);

    if (!mapObj->TestBounds(box)) {
        return;
    }

    CMap::LinkObjectToMapObjDefGroup(entity, mapObjDefGroup);

    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);

        if (group != mapObjDefGroup && !(mapObj->GetGroupFlags(group->groupNum) & (CMapObjGroup::MOGP_FLAGS::F_ALWAYS_DRAW | CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400000 | CMapObjGroup::MOGP_FLAGS::F_UNREACHABLE | CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) && mapObj->TestGroupBounds(box, group->groupNum, 1)) {
            CMap::LinkObjectToMapObjDefGroup(entity, group);
        }
    }
}

// VERIFY: e348a76f auto
// OFFSET: 0x007C2E70
void CMap::LinkStaticEntityMultiple2(CMapStaticEntity* entity) {
    C3Vector top(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z + 4.0f);
    C3Vector bottom(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z - 1000.0f);
    C3Vector mid(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z + 0.15f);

    float boxTop = entity->m_bbox.t.z + 0.1f;

    if (boxTop < top.z) {
        top.z = boxTop;
    }

    int32_t interior = 0;
    BOOL hit;
    MapObjIntersectData placement[2];
    MapObjIntersectData lighting[2];

    CMap::LinkStaticEntity(entity, top, bottom, mid, interior, hit, placement, lighting);

    if (interior) {
        STORM_ASSERT(placement[0].mapObjDef);
        STORM_ASSERT(lighting[0].mapObjDef);
        CMap::LinkStaticEntityMultiple2ToMapObjDefInterior(entity, placement[0].mapObjDef, placement[0].group);
        entity->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_INSIDE;

        C3Vector pos(entity->visCenter.x, entity->visCenter.y, entity->visCenter.z - (bottom.z - top.z) * lighting[0].t);
        CMap::ClassifyStaticEntityLink(entity, lighting[0].mapObjDef, lighting[0].group, lighting[0].faceIndex, &pos);
    } else {
        CMap::LinkStaticEntityMultiple2ToMapObjDefExterior(entity);
        CMap::LinkObjectToMapExterior(entity);
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    }
}

// VERIFY: d35a4a66 manual
// OFFSET: 0x007C2F80
void CMap::LinkStaticEntity(CMapStaticEntity* entity) {
    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    while (link) {
        CMapBaseObjLink* next = entity->m_parentLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    if (entity->GetType() & CMapBaseObj::Type_Entity) {
        static_cast<CMapEntity*>(entity)->m_groundType = -1;
    }

    if (entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS) {
        CMap::LinkStaticEntityMultiple2(entity);
    } else {
        CMap::LinkStaticEntitySingle2(entity);
    }
}

// VERIFY: 04f6c26e manual
// OFFSET: 0x007C3150
void CMap::PurgeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    for (CMapBaseObjLink* link = mapObjDefGroup->doodadDefLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->doodadDefLinkList.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);
        CMap::FreeBaseObjLink(link);

        if (!(doodadDef->m_flags & MAPOBJ_FLAG_IN_MAPOBJ)) {
            CMapDoodadDef::Purge(doodadDef);
        }

        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->entityLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->entityLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->lightLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->lightLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->m_unkLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->m_unkLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    CMap::FreeMapObjDefGroup(mapObjDefGroup);
}

// VERIFY: e0cd9ea4 manual
// OFFSET: 0x007C3250
void CMap::PurgeMapObjDef(CMapObjDef* mapObjDef) {
    if (mapObjDef->m_refCount) {
        return;
    }

    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDef->groupLinkList.Next(link);
        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(link->owner);
        CMap::FreeBaseObjLink(link);
        CMap::PurgeMapObjDefGroup(mapObjDefGroup);
        link = next;
    }

    if (mapObjDef->m_defGroups.m_count == 0xFFFFFFFF) {
        mapObjDef->m_defGroups.m_dynamic.m_count = 0;
    } else {
        mapObjDef->m_defGroups.m_count = 0;
    }

    for (uint32_t i = 0; i < mapObjDef->m_lightsArray.m_count; i++) {
        if (mapObjDef->m_lightsArray.m_data[i]) {
            CMap::DestroyLight(mapObjDef->m_lightsArray.m_data[i]);
        }
    }

    mapObjDef->m_lightsArray.m_count = 0;
    CMapObj::Delete(mapObjDef->mapObj);
    mapObjDef->mapObj = nullptr;
    CMap::FreeMapObjDef(mapObjDef);

    DayNight::DNInfo* info = DayNight::GetInfo();

    if (info->m_interiorMapObjDef == mapObjDef) {
        info->m_interiorMapObjDef = nullptr;
        info->m_interiorFogBlend = 0.0f;
        info->m_interiorGroups.m_count = 0;
    }
}

// INLINED
void CMap::RemoveAreaFromTable(const CMapArea* area) {
    int32_t index = CMap::GetAreaIndex(area->m_index.x, area->m_index.y);
    STORM_ASSERT(CMap::m_areaTable[index] == area);
    CMap::m_areaTable[index] = nullptr;
}

// VERIFY: ad81d8da auto
// OFFSET: 0x007C3700
void CMap::PurgeArea(CMapArea* area) {
    CMap::RemoveAreaFromTable(area);
    area->Purge();
    CMap::FreeArea(area);
}

// VERIFY: e2dae8ec manual
// OFFSET: 0x007C3730
void CMap::PurgeMaps() {
    CMapBaseObjLink* link = CMap::areaLinkList.Head();

    while (link) {
        CMapBaseObjLink* next = CMap::areaLinkList.Next(link);
        CMapArea* area = static_cast<CMapArea*>(link->owner);
        CMap::FreeBaseObjLink(link);
        CMap::RemoveAreaFromTable(area);
        area->Purge();
        CMap::FreeArea(area);
        link = next;
    }

    for (uint32_t i = 0; i < 4096; i++) {
        if (CMap::areaMedArray[i]) {
            CMap::FreeAreaMed(CMap::areaMedArray[i]);
            CMap::areaMedArray[i] = nullptr;
        }
    }

    CMap::s_lowDetail.Purge();
}

// VERIFY: 7e85d0ed manual
// OFFSET: 0x007D59B0
bool CMap::LocateViewerMapObjs(C3Vector& start, C3Vector& end, float dist, CMapObjDef** outDefs, uint32_t* outGroups) {
    float dists[2] = { dist, dist };
    uint32_t slot = 0;

    outDefs[0] = nullptr;
    outDefs[1] = nullptr;
    outGroups[1] = 0xFFFF;
    outGroups[0] = 0xFFFF;
    outGroups[3] = 0xFFFF;
    outGroups[2] = 0xFFFF;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
            continue;
        }

        if (mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT) {
            slot = 1;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        if (!mapObjDef->TestAABox(start, end) || !mapObj) {
            slot = 0;
            continue;
        }

        C3Vector localStart = start * mapObjDef->m_invMat;
        C3Vector localEnd = end * mapObjDef->m_invMat;

        if (!mapObj->TestBounds(localStart, localEnd)) {
            slot = 0;
            continue;
        }

        uint32_t exterior = 0;

        for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->owner);
            CMapObjGroup* group = mapObj->GetGroup(defGroup->groupNum, 0);

            if (!group || (group->m_flags & (CMapObjGroup::MOGP_FLAGS::F_ALWAYS_DRAW | CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400000 | CMapObjGroup::MOGP_FLAGS::F_UNREACHABLE)) || !mapObj->TestGroupBounds(localStart, localEnd, defGroup->groupNum)) {
                continue;
            }

            C3Segment seg;
            seg.start = localStart;
            seg.end = localEnd;
            World::TriData::ResetPools();

            World::TriData triData;

            if (group->GetTris(seg, &dists[slot], 0, 0, &triData, mapObjDef)) {
                exterior = (group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) >> 3;
                outDefs[slot] = mapObjDef;
                outGroups[2 * slot] = defGroup->groupNum;
                outGroups[2 * slot + 1] = 0xFFFF;
            }
        }

        float portalT = 1.05f;
        C3Segment seg;
        seg.start = localStart;
        seg.end = localEnd;
        uint32_t portalGroups[2];

        if (mapObj->VectorIntersectPortal(seg, &portalT, portalGroups, 0) && portalT - dists[slot] < 0.0001f) {
            exterior = (mapObj->GetGroupInfo(portalGroups[0])->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) >> 3;
            dists[slot] = portalT;
            outDefs[slot] = mapObjDef;
            outGroups[2 * slot] = portalGroups[0];

            if (mapObj->GetGroupInfo(portalGroups[1])->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) {
                outGroups[2 * slot + 1] = 0xFFFF;
            } else {
                outGroups[2 * slot + 1] = portalGroups[1];
            }
        }

        if (exterior) {
            outDefs[slot] = nullptr;
        }

        slot = 0;
    }

    if (outDefs[0]) {
        return true;
    }

    if (!outDefs[1]) {
        return false;
    }

    outDefs[0] = outDefs[1];
    outGroups[0] = outGroups[2];
    outGroups[1] = outGroups[3];
    outDefs[1] = nullptr;
    outGroups[2] = 0;
    outGroups[3] = 0;

    return true;
}

// VERIFY: 6db55b9e manual
// OFFSET: 0x007D9990
CTexture* CMap::LoadTexture(const char* fileName) {
    CStatus status;
    CTexture* texture = TextureCreate(fileName, CGxTexFlags(GxTex_LinearMipLinear, 1, 1, 0, 0, 0, 1), &status, 0);
    SysMsgAdd(&status);

    if (!texture) {
        DebugLog("Failed to load texture: %s\n", fileName);
    }

    return texture;
}

// VERIFY: fedf0bee auto
// OFFSET: 0x007D9A20
void CMap::LoadArea(CMapArea* area) {
    char fileName[256];
    SStrPrintf(fileName, sizeof(fileName), "%s\\%s_%d_%d.adt", CMap::mapPath, CMap::mapName, area->m_index.x, area->m_index.y);
    area->Load(fileName);
}

// VERIFY: a0b22da5 auto
// OFFSET: 0x007D9A70
CMapArea* CMap::PrepareArea(int32_t areaIndexX, int32_t areaIndexY) {
    STORM_ASSERT(CMap::m_areaInfo[areaIndexY * 64 + areaIndexX].m_flags & SMAreaInfo::FLAG_EXISTS); // 12213: "areaInfo[index].flags & SMAreaInfo::FLAG_EXISTS"

    CMapArea* area = CMap::AllocArea();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(area);
    CMap::areaLinkList.LinkToTail(link);

    area->m_tileChunkIndex.x = MAP_CHUNKS_PER_AREA_AXIS * areaIndexX;
    area->m_index.y = areaIndexY;
    area->m_flags = MAPOBJ_FLAG_NONE;
    area->m_index.x = areaIndexX;
    area->m_tileChunkIndex.y = MAP_CHUNKS_PER_AREA_AXIS * areaIndexY;

    area->m_topLeft2.x = HALF_WORLD_SIZE - area->m_tileChunkIndex.y * 33.333332f;
    area->m_bounds.t.x = area->m_topLeft2.x;
    area->m_topLeft2.y = area->m_tileChunkIndex.x * -33.333332f + HALF_WORLD_SIZE;
    area->m_bounds.t.y = area->m_topLeft2.y;
    area->m_bounds.b.x = area->m_topLeft2.x - MD_AREA_POLY_SIZE;
    area->m_bounds.b.y = area->m_topLeft2.y - MD_AREA_POLY_SIZE;

    int32_t index = CMap::GetAreaIndex(areaIndexX, areaIndexY);
    // 12213: "CMap::m_areaTable[index] == __null"
    STORM_ASSERT(CMap::m_areaTable[index] == nullptr);
    CMap::m_areaTable[index] = area;

    return area;
}

// VERIFY: 34c22451 manual
// OFFSET: 0x007A50C0
bool CMap::GetDoodadDefFacets(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* linkList, CAaBox* box, World::FacetData* facets, uint32_t flags) {
    if (!(flags & WORLD_QUERY_DOODAD_MASK)) {
        return true;
    }

    for (CMapBaseObjLink* link = linkList->Head(); link; link = linkList->Next(link)) {
        CMapDoodadDef* def = static_cast<CMapDoodadDef*>(link->owner);

        if ((def->m_flags & MAPOBJ_FLAG_NO_HITTEST) || def->m_queryTag == CMap::s_queryTag || !def->model) {
            continue;
        }

        if (def->m_collisionGuid.m_guid ? !(flags & WORLD_QUERY_OBJECT_MASK) : !(flags & WORLD_QUERY_M2_MASK)) {
            continue;
        }

        CAaBox bbox = def->m_collideExt;

        if (!(def->m_flags & MAPOBJ_FLAG_PREPARED)) {
            def->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_STREAMING_LOAD_PENDING;
            CAaBox modelBox = def->model->m_shared->m_boundingBox;
            CWorldMath::TransformAABox(def->m_mat, modelBox, bbox);
        }

        if (box->Intersects(bbox)) {
            if (def->m_flags & MAPOBJ_FLAG_PREPARED) {
                CMap::AppendMapObjFacets(def, box, facets);
            } else {
                World::AddAaBoxFacets(&bbox, facets);
            }
        }

        def->m_queryTag = CMap::s_queryTag;
    }

    return true;
}

// VERIFY: e531c817 manual
// OFFSET: 0x007A4B80
void CMap::AppendDestructibleFacets(World::ObjectCollisionData* data, CAaBox* box, World::FacetData* facets, WOWGUID guid) {
    uint32_t first = facets->facets.Count();
    data->m_model->GetCollisionFacets(*box, data->m_mat, facets->facets);
    uint32_t count = facets->facets.Count();
    facets->facetIds.SetCount(count);

    for (uint32_t i = first; i < count; i++) {
        facets->facetIds.m_data[i] = guid.m_guid;
    }
}

// VERIFY: cee45fe5 manual
// OFFSET: 0x007A5240
void CMap::QueryDestructibleFacets(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, CAaBox* box, World::FacetData* facets, uint32_t flags) {
    if (!World::s_objectCollisionCallback || !(flags & WORLD_QUERY_OBJECT_MASK)) {
        return;
    }

    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->owner);

        if (!(entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_COLLIDABLE)) {
            break;
        }

        if (entity->m_queryTag == CMap::s_queryTag || (entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_HIDDEN)) {
            continue;
        }

        World::ObjectCollisionData data;
        WOWGUID guid = WOWGUID{ entity->m_funcParam64 };

        if (World::s_objectCollisionCallback(&guid, entity->m_funcParam32, &data, flags) && box->Intersects(data.m_bounds)) {
            CMap::AppendDestructibleFacets(&data, box, facets, guid);
        }

        entity->m_queryTag = CMap::s_queryTag;
    }
}

// VERIFY: aa0cecc5 manual
// OFFSET: 0x007A3CF0
bool CMap::GetChunkLiquidFacets(CMapChunk* chunk, CAaBox* box, CiRect* rect, CChunkLiquid* liquid, World::FacetData* facets) {
    World::TriData::ResetPools();
    World::TriData triData;

    if (!liquid->GetTris(&triData, *box, *rect)) {
        return false;
    }

    return World::TriDataToFacetData(&triData, facets, WOWGUID{ 0 }) != 0;
}

// VERIFY: 92053872 manual
// OFFSET: 0x007A5A60
bool CMap::GetChunkFacets(int32_t chunkX, int32_t chunkY, CiRect* subRect, CAaBox* queryBox, CAaBox* box, World::FacetData* facetData, uint32_t flags) {
    STORM_ASSERT(facetData);

    uint32_t first = facetData->facets.Count();
    int32_t areaIndex = CMap::GetAreaIndex((chunkX >> 4) & MAP_AREA_INDEX_MASK, (chunkY >> 4) & MAP_AREA_INDEX_MASK);
    CMapArea* area = CMap::m_areaTable[areaIndex];

    if (area && !area->asyncObject) {
        CMapChunk* chunk = area->GetChunk(chunkX, chunkY);

        if (!chunk) {
            return false;
        }

        if (chunk->m_flags & MAPOBJ_FLAG_IMPASSABLE) {
            CMap::CreateImpassableFacets(chunk, box, facetData);
        }

        CiRect rect;
        rect.minY = subRect->minY - 8 * chunkY;
        rect.minX = subRect->minX - 8 * chunkX;
        rect.maxY = subRect->maxY - 8 * chunkY;
        rect.maxX = subRect->maxX - 8 * chunkX;

        if (rect.minY < 0) {
            rect.minY = 0;
        }

        if (rect.minX < 0) {
            rect.minX = 0;
        }

        if (rect.maxY >= 8) {
            rect.maxY = 7;
        }

        if (rect.maxX >= 8) {
            rect.maxX = 7;
        }

        CAaBox localBox;
        localBox.b = box->b - chunk->m_topLeftCoords;
        localBox.t = box->t - chunk->m_topLeftCoords;

        if (flags & WORLD_QUERY_TERRAIN) {
            chunk->Intersect(&rect, &localBox, &facetData->facets);
        }

        if (flags & WORLD_QUERY_LIQUID_MASK) {
            bool walkableOnly = (flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID);

            for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
                if (!walkableOnly || (g_liquidTypeDB.GetRecord(liquid->m_type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
                    CMap::GetChunkLiquidFacets(chunk, &localBox, &rect, liquid, facetData);
                }
            }
        }

        uint32_t count = facetData->facets.Count();
        facetData->facetIds.SetCount(count);

        if (count > first) {
            memset(&facetData->facetIds.m_data[first], 0, sizeof(uint64_t) * (count - first));
        }

        if ((flags & WORLD_QUERY_DOODAD_MASK) && !CMap::GetDoodadDefFacets(&chunk->doodadDefLinkList, box, facetData, flags)) {
            return false;
        }

        if (flags & WORLD_QUERY_OBJECT_MASK) {
            CMap::QueryDestructibleFacets(&chunk->entityLinkList, box, facetData, flags);
        }

        return true;
    }

    if (!(CMap::m_areaInfo[areaIndex].m_flags & SMAreaInfo::FLAG_EXISTS)) {
        return true;
    }

    if (!(flags & WORLD_QUERY_UNLOADED_AS_BOUNDS)) {
        return false;
    }

    CAaBox cellBox = *box;
    cellBox.b.x = (chunkY + 1) * -33.333332f + HALF_WORLD_SIZE;
    cellBox.b.y = (chunkX + 1) * -33.333332f + HALF_WORLD_SIZE;
    cellBox.b.z = box->b.z - 1000.0f;
    cellBox.t.x = chunkY * -33.333332f + HALF_WORLD_SIZE;
    cellBox.t.y = -33.333332f * chunkX + HALF_WORLD_SIZE;
    cellBox.t.z = box->t.z + 1000.0f;

    if (cellBox.Intersects(*queryBox)) {
        return false;
    }

    World::AddAaBoxFacets(&cellBox, facetData);
    return true;
}

// VERIFY: c903d39d manual
// OFFSET: 0x007A55E0
bool CMap::GetMapObjFacets(CAaBox* queryBox, CAaBox* box, World::FacetData* facets, uint32_t flags, uint32_t* status) {
    CAaBox localBox;
    localBox.b = C3Vector(0.0f, 0.0f, 0.0f);
    localBox.t = C3Vector(0.0f, 0.0f, 0.0f);
    C3Vector center;
    center.x = (box->t.x + box->b.x) * 0.5f;
    center.y = (box->t.y + box->b.y) * 0.5f;
    center.z = 0.5f * (box->t.z + box->b.z);
    CAaBox centered;
    centered.b = box->b - center;
    centered.t = box->t - center;

    for (CDestructibleProxy* proxy = CMap::destructibleProxies.Head(); proxy; proxy = CMap::destructibleProxies.Next(proxy)) {
        C3Vector extent = box->t - box->b;
        float radius = sqrtf(extent.x * extent.x + (extent.y * extent.y + extent.z * extent.z)) + proxy->m_sphere.r;
        C3Vector delta = proxy->m_sphere.c - center;

        if (delta.SquaredMag() < radius * radius) {
            return false;
        }
    }

    for (CMapObjDef* def = CMap::mapObjDefHash.m_fulllist.Head(); def; def = CMap::mapObjDefHash.m_fulllist.Next(def)) {
        if (def->m_flags & MAPOBJ_FLAG_NO_HITTEST) {
            continue;
        }

        CMapObj* owner = def->mapObj;

        if (!owner) {
            continue;
        }

        if (box->t.x < def->m_bbox.b.x || box->t.y < def->m_bbox.b.y || box->t.z < def->m_bbox.b.z || box->b.x > def->m_bbox.t.x || box->b.y > def->m_bbox.t.y || box->b.z > def->m_bbox.t.z) {
            continue;
        }

        if (!owner->m_isGroupLoaded || !def->groupLinkList.Head()) {
            if (!(flags & WORLD_QUERY_UNLOADED_AS_BOUNDS) || def->m_bbox.Intersects(*queryBox)) {
                return false;
            }

            World::AddAaBoxFacets(&def->m_bbox, facets);
            continue;
        }

        C3Vector localCenter = center * def->m_invMat;
        C33Matrix rotation(def->m_invMat);
        CWorldMath::TransformAABox(rotation, centered, localBox);
        localBox.b += localCenter;
        localBox.t += localCenter;

        if (owner->TestBounds(localBox)) {
            if (flags & WORLD_QUERY_WMO_MASK) {
                World::TriData::ResetPools();
                World::TriData triData;
                owner->GetTris(localBox, flags, &triData, def);
                World::TriDataToFacetData(&triData, facets, def->m_objectGuid);

                if (status) {
                    *status |= World::TriData::statusFlags;
                }
            }

            if (flags & WORLD_QUERY_LIQUID_MASK) {
                owner->GetLiquidFacets(localBox, flags, facets, def);
            }
        }

        for (CMapBaseObjLink* link = def->groupLinkList.Head(); link; link = def->groupLinkList.Next(link)) {
            CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);

            if (box->t.x < group->aaBox.b.x || box->t.y < group->aaBox.b.y || box->t.z < group->aaBox.b.z || box->b.x > group->aaBox.t.x || box->b.y > group->aaBox.t.y || box->b.z > group->aaBox.t.z) {
                continue;
            }

            if (owner->IsGroupLoaded(group->groupNum)) {
                if ((flags & WORLD_QUERY_DOODAD_MASK) && !CMap::GetDoodadDefFacets(&group->doodadDefLinkList, box, facets, flags)) {
                    return false;
                }

                if (flags & WORLD_QUERY_OBJECT_MASK) {
                    CMap::QueryDestructibleFacets(&group->entityLinkList, box, facets, flags);
                }
            } else {
                if (!(flags & WORLD_QUERY_UNLOADED_AS_BOUNDS)) {
                    return false;
                }

                World::AddAaBoxFacets(&group->aaBox, facets);
            }
        }
    }

    return true;
}

// VERIFY: 5c2f5676 manual
// OFFSET: 0x007A4590
bool CMap::CreateFlightBoundsFacets(int32_t areaX, int32_t areaY, CAaBox* box, World::FacetData* facets) {
    CMapArea* area = CMap::GetArea(areaX, areaY);

    if (!area || area->asyncObject) {
        return false;
    }

    const int16_t* heights = area->m_flyingBbox;

    if (!heights) {
        return true;
    }

    uint32_t first = facets->facets.Count();
    int16_t top = static_cast<int16_t>(static_cast<int32_t>(box->t.z + 1.0f));
    int32_t above[9];

    for (uint32_t i = 0; i < 9; i++) {
        above[i] = top < heights[i];
    }

    for (uint32_t tri = 0; tri < sizeof(s_flightBoundsTriangles) / sizeof(s_flightBoundsTriangles[0]); tri += 3) {
        int32_t i0 = s_flightBoundsTriangles[tri];
        int32_t i1 = s_flightBoundsTriangles[tri + 1];
        int32_t i2 = s_flightBoundsTriangles[tri + 2];

        if (above[i0] && above[i1] && above[i2]) {
            continue;
        }

        C3Vector v0(s_flightBoundsOffsets[i0][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i0][1] + area->m_topLeft2.y, static_cast<float>(heights[i0]));
        C3Vector v1(s_flightBoundsOffsets[i1][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i1][1] + area->m_topLeft2.y, static_cast<float>(heights[i1]));
        C3Vector v2(s_flightBoundsOffsets[i2][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i2][1] + area->m_topLeft2.y, static_cast<float>(heights[i2]));
        CFacet* facet = facets->facets.New();
        facet->plane.From3Pos(v0, v1, v2);
        facet->v[0] = v0;
        facet->v[1] = v1;
        facet->v[2] = v2;
    }

    int16_t bottom = static_cast<int16_t>(static_cast<int32_t>(box->b.z - 1.0f));
    int32_t below[9];

    for (uint32_t i = 0; i < 9; i++) {
        below[i] = bottom > heights[9 + i];
    }

    for (uint32_t tri = 0; tri < sizeof(s_flightBoundsTriangles) / sizeof(s_flightBoundsTriangles[0]); tri += 3) {
        int32_t i0 = s_flightBoundsTriangles[tri];
        int32_t i1 = s_flightBoundsTriangles[tri + 1];
        int32_t i2 = s_flightBoundsTriangles[tri + 2];

        if (below[i0] && below[i1] && below[i2]) {
            continue;
        }

        C3Vector v0(s_flightBoundsOffsets[i0][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i0][1] + area->m_topLeft2.y, static_cast<float>(heights[9 + i0]));
        C3Vector v1(s_flightBoundsOffsets[i1][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i1][1] + area->m_topLeft2.y, static_cast<float>(heights[9 + i1]));
        C3Vector v2(s_flightBoundsOffsets[i2][0] + area->m_topLeft2.x, s_flightBoundsOffsets[i2][1] + area->m_topLeft2.y, static_cast<float>(heights[9 + i2]));
        CFacet* facet = facets->facets.New();
        facet->plane.From3Pos(v0, v2, v1);
        facet->v[0] = v0;
        facet->v[1] = v2;
        facet->v[2] = v1;
    }

    uint32_t count = facets->facets.Count();
    facets->facetIds.SetCount(count);

    if (count > first) {
        memset(&facets->facetIds.m_data[first], 0, sizeof(uint64_t) * (count - first));
    }

    return true;
}

// VERIFY: f40f6721 manual
// OFFSET: 0x007A5F20
bool CMap::GetFacets(CAaBox* queryBox, CAaBox* box, World::FacetData* facetData, uint32_t flags, uint32_t* status) {
    STORM_ASSERT(facetData);

    CMap::mapGetFacetsCount++;
    CMap::s_queryTag++;
    facetData->facets.SetCount(0);

    if (!CMap::GetMapObjFacets(queryBox, box, facetData, flags, status)) {
        return false;
    }

    if (CMap::bDungeon) {
        return true;
    }

    float topX = -(box->t.x - HALF_WORLD_SIZE);
    float topY = -(box->t.y - HALF_WORLD_SIZE);
    float bottomX = -(box->b.x - HALF_WORLD_SIZE);

    if (topY < 0.0f) {
        return false;
    }

    float bottomY = -(box->b.y - HALF_WORLD_SIZE);

    if (topX < 0.0f || bottomY >= 34133.332f || bottomX >= 34133.332f) {
        return false;
    }

    CiRect rect;
    rect.maxX = CMath::fint_(bottomY * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.maxY = CMath::fint_(bottomX * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.minX = CMath::fint_(topY * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.minY = CMath::fint_(topX * OO_COORD_TO_SUBCHUNK - 0.5f);
    bool result = true;

    for (int32_t y = rect.minY >> 3; y <= rect.maxY >> 3; y++) {
        for (int32_t x = rect.minX >> 3; x <= rect.maxX >> 3; x++) {
            if (!CMap::GetChunkFacets(x, y, &rect, queryBox, box, facetData, flags)) {
                result = false;
            }
        }
    }

    if (flags & WORLD_QUERY_FLIGHT_BOUNDS) {
        for (int32_t y = (rect.minY >> 3) >> 4; y <= (rect.maxY >> 3) >> 4; y++) {
            for (int32_t x = (rect.minX >> 3) >> 4; x <= (rect.maxX >> 3) >> 4; x++) {
                if (!CMap::CreateFlightBoundsFacets(x, y, box, facetData)) {
                    result = false;
                }
            }
        }
    }

    return result;
}

// VERIFY: 4b94dd93 manual
// OFFSET: 0x007A2C60
void CMap::SetHitTestDebug(World::HitTestDebug* hitInfo, CMapBaseObj* hitObject) {
    hitInfo->m_bbox.b.x = 0.0f;
    hitInfo->m_bbox.b.y = 0.0f;
    hitInfo->m_bbox.b.z = 0.0f;
    hitInfo->m_bbox.t.x = 0.0f;
    hitInfo->m_bbox.t.y = 0.0f;
    hitInfo->m_bbox.t.z = 0.0f;
    hitInfo->m_sphere.c.x = 0.0f;
    hitInfo->m_sphere.c.y = 0.0f;
    hitInfo->m_sphere.c.z = 0.0f;
    hitInfo->m_sphere.r = 0.0f;
    hitInfo->m_labelPos.x = 0.0f;
    hitInfo->m_labelPos.y = 0.0f;
    hitInfo->m_type = World::HitTestDebug::TYPE_NONE;
    hitInfo->m_numModels = 0;
    hitInfo->m_numParents = 0;
    hitInfo->m_labelPos.z = 0.0f;

    if (!hitObject) {
        return;
    }

    CM2Model* model = nullptr;

    if (hitObject->GetType() & CMapBaseObj::Type_DoodadDef) {
        CMapStaticEntity* entity = static_cast<CMapStaticEntity*>(hitObject);
        hitInfo->m_type = World::HitTestDebug::TYPE_DOODAD;
        hitInfo->m_bbox = entity->m_bbox;
        hitInfo->m_sphere = entity->m_sphere;
        hitInfo->m_labelPos = entity->visCenter;
        hitInfo->m_labelPos.z += 0.1f;
        model = entity->model;
    } else if (hitObject->GetType() & CMapBaseObj::Type_Entity) {
        CMapStaticEntity* entity = static_cast<CMapStaticEntity*>(hitObject);
        hitInfo->m_type = World::HitTestDebug::TYPE_ENTITY;
        hitInfo->m_bbox = entity->m_bbox;
        hitInfo->m_sphere = entity->m_sphere;
        hitInfo->m_labelPos = entity->visCenter;

        if (entity->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS) {
            float raised = hitInfo->m_labelPos.z + 4.0f;
            float top = entity->m_bbox.t.z + 0.1f;
            hitInfo->m_labelPos.z = top > raised ? raised : top;
        }

        model = entity->model;
    } else if (hitObject->GetType() & CMapBaseObj::Type_MapObjDefGroup) {
        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(hitObject);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(defGroup->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & CMapBaseObj::Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        CMapObjGroup* group = mapObj->GetGroup(defGroup->groupNum, 0);
        World::HitTestDebug::Model* entry = &hitInfo->m_models[hitInfo->m_numModels++];
        hitInfo->m_type = World::HitTestDebug::TYPE_MAP_OBJ_GROUP;
        entry->m_name = PathFileName(mapObj->m_wmoName);
        entry->m_depth = 0;
        entry->m_bbox = defGroup->aaBox;
        entry->m_sphere = defGroup->m_sphere;
        entry->m_numGeometryBatches = group->m_batchListCount;
        entry->m_numPrimitives = group->m_polyListSize;
        entry->m_model = nullptr;
        entry->m_attachmentScale = 1.0f;
        entry->m_numOtherBatches = 0;
        entry->m_numBones = 0;
        entry->m_numLights = 0;
        entry->m_numCurrentLights = 0;
        entry->m_attachmentId = MODEL_ATTACHMENT_ID::ATTACH_SHIELD;
        entry->m_numCombinableBatches = 0;
        entry->m_instanceCountMax = 0;
        entry->m_groupName = group->m_groupName;
        entry->m_groupNum = defGroup->groupNum;
        entry->m_groupFlags = group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR;
        entry->m_doodadSet = mapObjDef->m_doodadSet;
    }

    CMapBaseObj* object = hitObject;

    if (object->GetType() & CMapBaseObj::Type_MapObjDefGroup) {
        object = object->m_parentLinkList.Head()->ref;
    }

    for (CMapBaseObjLink* link = object->m_parentLinkList.Head(); link; link = object->m_parentLinkList.Next(link)) {
        CMapBaseObj* parent = link->ref;

        if (!parent) {
            continue;
        }

        if (parent->GetType() & CMapBaseObj::Type_Chunk) {
            if (hitInfo->m_numParents < 8) {
                World::HitTestDebug::Parent* entry = &hitInfo->m_parents[hitInfo->m_numParents++];
                entry->m_name = "Terrain";
                entry->m_groupName = nullptr;
                entry->m_chunkOffset = static_cast<CMapChunk*>(parent)->m_cOffset;
            }
        } else if (parent->GetType() & CMapBaseObj::Type_MapObjDefGroup) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(parent);
            const CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(defGroup->m_parentLinkList.Head()->ref);
            STORM_ASSERT(mapObjDef->GetType() & CMapBaseObj::Type_MapObjDef);
            CMapObj* mapObj = mapObjDef->mapObj;
            STORM_ASSERT(mapObj);
            CMapObjGroup* group = mapObj->GetGroup(defGroup->groupNum, 0);

            if (hitInfo->m_numParents < 8) {
                World::HitTestDebug::Parent* entry = &hitInfo->m_parents[hitInfo->m_numParents++];
                entry->m_name = PathFileName(mapObj->m_wmoName);
                entry->m_groupName = group->m_groupName;
                entry->m_chunkOffset.x = 0;
                entry->m_chunkOffset.y = 0;
            }
        }
    }

    if (model) {
        while (model->m_attachParent) {
            model = model->m_attachParent;
        }

        AddHitDebugModel(hitInfo, 0, model);
    }
}

// VERIFY: a85f0fdc manual
// OFFSET: 0x007A30D0
bool CMap::VectorIntersect(C3Vector* start, C3Vector* end, uint32_t flags, uint32_t defIgnoreFlags, float* distance, uint16_t* hitIndex, CMapObj** outMapObj, CMapObjDef** outMapObjDef, CMapObjDefGroup** outMapObjDefGroup) {
    uint32_t modelFlags = flags & WORLD_QUERY_MODEL_MASK;

    if (modelFlags) {
        World::s_m2Scene->BeginHitTest();
    }

    C3Vector delta = *end - *start;
    float length = sqrtf(delta.x * delta.x + (delta.y * delta.y + delta.z * delta.z));
    CAaBox box;
    box.b = C3Vector(0.0f, 0.0f, 0.0f);
    box.t = C3Vector(0.0f, 0.0f, 0.0f);
    MapObjIntersectCandidate* candidates = static_cast<MapObjIntersectCandidate*>(alloca(sizeof(MapObjIntersectCandidate) * 500));
    uint32_t testListCount = 0;

    for (CMapObjDef* def = CMap::mapObjDefHash.m_fulllist.Head(); def; def = CMap::mapObjDefHash.m_fulllist.Next(def)) {
        if (defIgnoreFlags & def->m_flags) {
            continue;
        }

        CMapObj* owner = def->mapObj;

        if (!owner || !owner->m_isGroupLoaded || !CWorldMath::VectorIntersectAABox2(def->m_bbox, *start, *end)) {
            continue;
        }

        uint32_t groupCount = def->m_defGroups.m_count == 0xFFFFFFFF ? def->m_defGroups.m_dynamic.m_count : def->m_defGroups.m_count;

        for (uint32_t i = 0; i < groupCount; i++) {
            CMapObjDefGroup* group = def->m_defGroups[i];

            if (!owner->IsGroupLoaded(i) || !CWorldMath::VectorIntersectAABox2(group->aaBox, *start, *end)) {
                continue;
            }

            MapObjIntersectCandidate& candidate = candidates[testListCount++];
            candidate.def = def;
            candidate.group = group;
            STORM_ASSERT(testListCount < 500);
            C3Vector localStart = *start * def->m_invMat;

            if (owner->TestGroupBounds(localStart, i)) {
                candidate.t = 0.0f;
                continue;
            }

            owner->GetGroupBounds(&box, i);
            float x = localStart.x >= box.b.x ? (box.t.x >= localStart.x ? localStart.x : box.t.x) : box.b.x;
            float y = localStart.y >= box.b.y ? (box.t.y >= localStart.y ? localStart.y : box.t.y) : box.b.y;
            float z = localStart.z >= box.b.z ? (box.t.z >= localStart.z ? localStart.z : box.t.z) : box.b.z;
            candidate.t = sqrtf((localStart.x - x) * (localStart.x - x) + (localStart.z - z) * (localStart.z - z) + (localStart.y - y) * (localStart.y - y)) / length;
        }
    }

    qsort(candidates, testListCount, sizeof(MapObjIntersectCandidate), CompareIntersectCandidates);
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);
    float bestT = *distance;
    bool hit = false;
    int32_t bestFace = -1;

    for (uint32_t i = 0; i < testListCount; i++) {
        CMapObjDef* def = candidates[i].def;
        CMapObjDefGroup* group = candidates[i].group;
        CMapObj* mapObj = def->mapObj;
        C3Vector localStart = *start * def->m_invMat;
        C3Vector localEnd = *end * def->m_invMat;

        if (candidates[i].t <= bestT && mapObj->TestGroupBounds(localStart, localEnd, group->groupNum) && mapObj->Intersect(localStart, localEnd, &bestT, flags, ignoreFlags, group->groupNum, &bestFace)) {
            hit = true;

            if (outMapObj) {
                *outMapObj = mapObj;
            }

            if (outMapObjDef) {
                *outMapObjDef = def;
            }

            if (outMapObjDefGroup) {
                *outMapObjDefGroup = group;
            }
        }

        if (modelFlags) {
            CMap::VectorIntersectDoodadDefs(&group->doodadDefLinkList, flags);
        }

        if (flags & WORLD_QUERY_ENTITY_MASK) {
            CMap::VectorIntersectEntitys(&group->entityLinkList, flags);
        }
    }

    if (hit) {
        *distance = bestT;

        if (hitIndex) {
            *hitIndex = static_cast<uint16_t>(bestFace);
        }
    }

    if (modelFlags) {
        C3Vector localStart = *start * CWorldScene::camTransportView;
        C3Vector localEnd = *end * CWorldScene::camTransportView;
        float t = *distance;
        CMapBaseObj* owner = static_cast<CMapBaseObj*>(World::s_m2Scene->EndHitTest(localStart, localEnd, t, 0));

        if (t < *distance) {
            if ((owner->GetType() & CMapBaseObj::Type_DoodadDef) && static_cast<CMapDoodadDef*>(owner)->m_collisionGuid.m_guid) {
                s_lastCollisionGUID = static_cast<CMapDoodadDef*>(owner)->m_collisionGuid;
            }

            *distance = t;

            if (hitIndex) {
                *hitIndex = 0xFFFF;
            }

            return true;
        }
    }

    if (!hit && hitIndex) {
        *hitIndex = 0xFFFF;
    }

    return hit;
}

// VERIFY: b625f45f manual
// OFFSET: 0x007A3B70
bool CMap::Intersect(C3Vector* start, C3Vector* end, C3Vector* hitPoint, float* dist, uint32_t flags, World::HitTestDebug* hitInfo) {
    STORM_ASSERT(dist);

    CMap::s_queryTag++;
    bool hit = false;
    CMapObjDef* mapObjDef = nullptr;
    CMapObjDefGroup* mapObjDefGroup = nullptr;

    if (flags & (WORLD_QUERY_MODEL_MASK | WORLD_QUERY_WMO_MASK | WORLD_QUERY_LIQUID_MASK)) {
        World::s_m2Scene->m_lastHit.m_model = nullptr;
        s_lastCollisionGUID = WOWGUID{ 0 };
        CMapObj* mapObj;

        if (CMap::VectorIntersect(start, end, flags, MAPOBJ_FLAG_NO_HITTEST, dist, nullptr, &mapObj, &mapObjDef, &mapObjDefGroup)) {
            if (mapObjDef && mapObjDef->m_objectGuid.m_guid) {
                s_lastCollisionGUID = mapObjDef->m_objectGuid;
            }

            if (hitInfo) {
                if (World::s_m2Scene->m_lastHit.m_model) {
                    CMap::SetHitTestDebug(hitInfo, static_cast<CMapBaseObj*>(World::s_m2Scene->m_lastHitOwner));
                } else {
                    CMap::SetHitTestDebug(hitInfo, mapObjDefGroup);
                }
            }

            hit = true;
        }
    }

    if (flags & (WORLD_QUERY_MODEL_MASK | WORLD_QUERY_TERRAIN | WORLD_QUERY_LIQUID_MASK)) {
        World::s_m2Scene->m_lastHit.m_model = nullptr;

        if (CMap::VectorIntersectTerrain(start, end, dist, flags, nullptr)) {
            s_lastCollisionGUID = WOWGUID{ 0 };

            if (hitInfo && World::s_m2Scene->m_lastHit.m_model) {
                CMap::SetHitTestDebug(hitInfo, static_cast<CMapBaseObj*>(World::s_m2Scene->m_lastHitOwner));
            }

            hit = true;
        }
    }

    if (!hit) {
        return false;
    }

    if (hitPoint) {
        float t = *dist;
        C3Vector point;
        point.x = (end->x - start->x) * t + start->x;
        point.y = (end->y - start->y) * t + start->y;
        point.z = (end->z - start->z) * t + start->z;
        *hitPoint = point;
    }

    return true;
}

// VERIFY: 7a1bc3ec manual
// OFFSET: 0x007A0490
int32_t CMap::QueryAreaIdTerrain(const C3Vector* pos) {
    float x = -(pos->x - HALF_WORLD_SIZE);
    float y = -(pos->y - HALF_WORLD_SIZE) * OO_COORD_TO_CHUNK;
    int32_t cellY = CMath::fint_(y - 0.5f);
    x = x * OO_COORD_TO_CHUNK;
    int32_t cellX = CMath::fint_(x - 0.5f);
    CMapArea* area = CMap::GetAreaForChunk(cellY, cellX);

    if (!area || area->asyncObject) {
        return 0;
    }

    CMapChunk* chunk = area->GetChunk(cellY, cellX);

    if (!chunk) {
        return 0;
    }

    return chunk->m_areaId;
}

// VERIFY: 0a8e296f manual
// OFFSET: 0x00782560
bool CMap::QueryAreaId(CMapEntity* entity, uint32_t& areaId) {
    if (CMap::bDungeon) {
        const MapRec* mapRec = g_mapDB.GetRecord(s_mapId);
        STORM_ASSERT(mapRec);
        int32_t areaTableId = mapRec->m_areaTableID;

        if (!areaTableId) {
            return false;
        }

        areaId = areaTableId;
        return true;
    }

    if (!entity) {
        return false;
    }

    STORM_ASSERT(entity->GetType() & CMapBaseObj::Type_Entity);
    CMapBaseObjLink* parentLink = entity->m_parentLinkList.Head();

    if (parentLink) {
        if (parentLink->ref->GetType() & CMapBaseObj::Type_Chunk) {
            areaId = static_cast<CMapChunk*>(parentLink->ref)->m_areaId;
            return true;
        }

        CMapObjDef* mapObjDef;

        while (true) {
            STORM_ASSERT(parentLink->ref->GetType() & CMapBaseObj::Type_MapObjDefGroup);
            CMapBaseObj* ref = parentLink->ref;
            mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT)) {
                break;
            }

            parentLink = entity->m_parentLinkList.Next(parentLink);

            if (!parentLink) {
                areaId = CMap::QueryAreaIdTerrain(&entity->m_position);
                return true;
            }
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(parentLink->ref);
        STORM_ASSERT(mapObjDefGroup);
        CMapObjGroup* group = mapObjDef->mapObj->GetGroup(mapObjDefGroup->groupNum, 0);
        WMOAreaTableRec* rec = GetWMOAreaRec(mapObjDef->mapObj->m_header->m_wmoID, mapObjDef->m_nameSet, group->m_wmoGroupId);

        if (rec && rec->m_areaTableID) {
            areaId = rec->m_areaTableID;
            return true;
        }
    }

    areaId = CMap::QueryAreaIdTerrain(&entity->m_position);
    return true;
}

// VERIFY: 0728f3ec auto
// OFFSET: 0x009905C0
const LiquidTypeRec* GetLiquidTypeRecForArea(uint32_t areaId, uint16_t liquidType) {
    if (!liquidType) {
        return nullptr;
    }

    if (areaId && liquidType < NUM_BASIC_LIQUID_TYPES) {
        uint32_t index = (liquidType - 1) & 3;
        AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

        if (area) {
            if (!area->m_liquidTypeID[index] && area->m_parentAreaID) {
                area = g_areaTableDB.GetRecord(area->m_parentAreaID);
            }

            if (area && area->m_liquidTypeID[index]) {
                return g_liquidTypeDB.GetRecord(area->m_liquidTypeID[index]);
            }
        }
    }

    return g_liquidTypeDB.GetRecord(liquidType);
}

// VERIFY: 44408b24 auto
// OFFSET: 0x00990530
static int32_t CompareWMOAreaRec(const void* a, const void* b) {
    const WMOAreaTableRec* ra = static_cast<const WMOAreaTableRec*>(a);
    const WMOAreaTableRec* rb = static_cast<const WMOAreaTableRec*>(b);
    int32_t result = ra->m_wmoid - rb->m_wmoid;

    if (!result) {
        result = ra->m_nameSetID - rb->m_nameSetID;

        if (!result) {
            return ra->m_wmogroupID - rb->m_wmogroupID;
        }
    }

    return result;
}

// VERIFY: 5bad450c auto
// OFFSET: 0x00990560
WMOAreaTableRec* GetWMOAreaRec(int32_t wmoId, int32_t nameSet, int32_t groupId) {
    WMOAreaTableRec key;
    key.m_nameSetID = nameSet;
    key.m_wmoid = wmoId;
    key.m_wmogroupID = groupId;
    return static_cast<WMOAreaTableRec*>(bsearch(&key, g_wmoAreaTableDB.GetNumRecords() <= 0 ? nullptr : g_wmoAreaTableDB.m_records, g_wmoAreaTableDB.GetNumRecords(), sizeof(WMOAreaTableRec), CompareWMOAreaRec));
}

// VERIFY: ff49ec1c manual
// OFFSET: 0x007A3D50
bool CMap::GetChunkLiquidFacets(CMapChunk* chunk, CWFrustum* frustum, CiRect* rect, CChunkLiquid* liquid, World::FacetData* facets) {
    World::TriData::ResetPools();
    World::TriData triData;

    if (!liquid->GetTris(&triData, *frustum, *rect)) {
        return false;
    }

    return World::TriDataToFacetData(&triData, facets, WOWGUID{ 0 }) != 0;
}

// VERIFY: 784e1e63 manual
// OFFSET: 0x007A5330
bool CMap::GetChunkFacets(int32_t chunkX, int32_t chunkY, CiRect* subRect, CFrustum* frustum, World::FacetData* facetData, uint32_t flags) {
    STORM_ASSERT(facetData);

    uint32_t first = facetData->facets.Count();
    CMapArea* area = CMap::GetAreaForChunk(chunkX, chunkY);

    if (!area || area->asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->GetChunk(chunkX, chunkY);

    if (!chunk) {
        return false;
    }

    CiRect rect;
    rect.minY = subRect->minY - 8 * chunkY;
    rect.minX = subRect->minX - 8 * chunkX;
    rect.maxY = subRect->maxY - 8 * chunkY;
    rect.maxX = subRect->maxX - 8 * chunkX;

    if (rect.minY < 0) {
        rect.minY = 0;
    }

    if (rect.minX < 0) {
        rect.minX = 0;
    }

    if (rect.maxY >= 8) {
        rect.maxY = 7;
    }

    if (rect.maxX >= 8) {
        rect.maxX = 7;
    }

    CWFrustum localFrustum(*frustum);
    localFrustum.Translate(C3Vector(-chunk->m_topLeftCoords.x, -chunk->m_topLeftCoords.y, -chunk->m_topLeftCoords.z));

    if (flags & WORLD_QUERY_TERRAIN) {
        chunk->Intersect(&rect, &localFrustum, &facetData->facets);
    }

    if (flags & WORLD_QUERY_LIQUID_MASK) {
        bool walkableOnly = (flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID);

        for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
            if (!walkableOnly || (g_liquidTypeDB.GetRecord(liquid->m_type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
                CMap::GetChunkLiquidFacets(chunk, &localFrustum, &rect, liquid, facetData);
            }
        }
    }

    if (flags & WORLD_QUERY_M2_MASK) {
        CAaBox box = CAaBox::Bounding(frustum->corners, 8);

        for (CMapBaseObjLink* link = chunk->doodadDefLinkList.Head(); link; link = chunk->doodadDefLinkList.Next(link)) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);
            STORM_ASSERT(doodadDef);

            if ((doodadDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) || !(doodadDef->m_flags & MAPOBJ_FLAG_PREPARED) || doodadDef->m_queryTag == CMap::s_queryTag) {
                continue;
            }

            CM2Model* model = doodadDef->model;

            if (model && box.Intersects(doodadDef->m_collideExt)) {
                model->GetCollisionFacets(box, doodadDef->m_mat, facetData->facets);
                doodadDef->m_queryTag = CMap::s_queryTag;
            }
        }
    }

    return first != facetData->facets.Count();
}

// VERIFY: 97cd8735 manual
// OFFSET: 0x007A4EE0
bool CMap::GetMapObjFacets(CFrustum* frustum, World::FacetData* facets, uint32_t flags, uint32_t* status) {
    bool result = false;

    for (CMapObjDef* def = CMap::mapObjDefHash.m_fulllist.Head(); def; def = CMap::mapObjDefHash.m_fulllist.Next(def)) {
        if ((def->m_flags & MAPOBJ_FLAG_NO_HITTEST) || !frustum->Cull(def->m_bbox)) {
            continue;
        }

        C3Vector corners[8];

        for (uint32_t i = 0; i < 8; i++) {
            corners[i] = frustum->corners[i] * def->m_invMat;
        }

        CWFrustum localFrustum(corners);
        CMapObj* owner = def->mapObj;

        if (owner) {
            World::TriData::ResetPools();
            World::TriData triData;
            result |= owner->GetTris(localFrustum, flags, &triData, def);
            World::TriDataToFacetData(&triData, facets, WOWGUID{ 0 });

            if (status) {
                *status |= World::TriData::statusFlags;
            }
        }
    }

    return result;
}

// VERIFY: 041a89f8 manual
// OFFSET: 0x007A5DD0
bool CMap::QueryFacets(CFrustum* frustum, World::FacetData* facetData, uint32_t flags, uint32_t* count) {
    STORM_ASSERT(facetData);

    CMap::s_queryTag++;
    facetData->facets.SetCount(0);
    CAaBox box = CAaBox::Bounding(frustum->corners, 8);
    float topX = -(box.t.x - HALF_WORLD_SIZE);
    float topY = -(box.t.y - HALF_WORLD_SIZE);
    float bottomX = -(box.b.x - HALF_WORLD_SIZE);
    float bottomY = -(box.b.y - HALF_WORLD_SIZE);
    // 12213: "tLocation.minx >= 0.0f && tLocation.miny >= 0.0f"
    STORM_ASSERT(topY >= 0.0f && topX >= 0.0f);
    // 12213: "tLocation.maxx < ((64*16)*((150.0f/36.0f)*8)) && tLocation.maxy < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(bottomY < 2.0f * HALF_WORLD_SIZE && bottomX < 2.0f * HALF_WORLD_SIZE);

    CiRect rect;
    rect.maxX = CMath::fint_(bottomY * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.maxY = CMath::fint_(bottomX * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.minX = CMath::fint_(topY * OO_COORD_TO_SUBCHUNK - 0.5f);
    rect.minY = CMath::fint_(topX * OO_COORD_TO_SUBCHUNK - 0.5f);

    for (int32_t y = rect.minY >> 3; y <= rect.maxY >> 3; y++) {
        for (int32_t x = rect.minX >> 3; x <= rect.maxX >> 3; x++) {
            CMap::GetChunkFacets(x, y, &rect, frustum, facetData, flags);
        }
    }

    CMap::GetMapObjFacets(frustum, facetData, flags, count);
    return facetData->facets.Count() != 0;
}

// VERIFY: 10d86f1c auto
// OFFSET: 0x0077F8B0
int32_t CMap::QueryFlightBounds(const C3Vector& pos, float* height) {
    return CMap::GetFlightBounds(pos, height, 0);
}

// VERIFY: 680c26b6 manual
// OFFSET: 0x007A0530
bool CMap::QueryGroundTypeTerrain(C3Vector* pos, uint32_t* groundType) {
    float y = -(pos->y - HALF_WORLD_SIZE);
    float x = -(pos->x - HALF_WORLD_SIZE);

    if (y < 0.0f || x < 0.0f || y >= 34133.332f || x >= 34133.332f) {
        return false;
    }

    float cellYf = x * OO_COORD_TO_SUBCHUNK;
    float cellXf = y * OO_COORD_TO_SUBCHUNK;
    int32_t cellY = CMath::fint_(cellYf - 0.5f);
    int32_t cellX = CMath::fint_(cellXf - 0.5f);

    CMapArea* area = CMap::GetLoadedArea((cellX >> MAP_CELL_TO_AREA_SHIFT) & MAP_AREA_INDEX_MASK, (cellY >> MAP_CELL_TO_AREA_SHIFT) & MAP_AREA_INDEX_MASK);

    if (!area) {
        return false;
    }

    CMapChunk* chunk = area->GetChunkForCell(cellX, cellY);

    if (!chunk) {
        return false;
    }

    int32_t col = cellX & MAP_CELL_INDEX_MASK;
    int32_t row = cellY & MAP_CELL_INDEX_MASK;

    if (chunk->m_header->m_holes & CMap::s_holeMask[4 * (row >> 1) + (col >> 1)]) {
        return false;
    }

    uint32_t layer = (CMap::s_2bitSplatMask[col] & chunk->m_predTex[row]) >> CMap::s_2bitSplatShft[col];
    GroundEffectTextureRec* rec = g_groundEffectTextureDB.GetRecord(chunk->m_layers[layer].m_effectId);

    if (!rec) {
        return false;
    }

    *groundType = rec->m_sound;
    return true;
}

// VERIFY: ddd4e04d manual
// OFFSET: 0x0079D2B0
void CMap::QueryLiquidSounds(C3Vector* pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results) {
    float mx = -(pos->y - HALF_WORLD_SIZE);
    float my = -(pos->x - HALF_WORLD_SIZE);
    STORM_ASSERT(mx >= 0.0f && my >= 0.0f);
    // 12213: "mx < ((64*16)*((150.0f/36.0f)*8)) && my < ((64*16)*((150.0f/36.0f)*8))"
    STORM_ASSERT(mx < 2.0f * HALF_WORLD_SIZE && my < 2.0f * HALF_WORLD_SIZE);

    int32_t cellY = CMath::fint_(mx * OO_COORD_TO_SUBCHUNK - 0.5f);
    int32_t cellX = CMath::fint_(my * OO_COORD_TO_SUBCHUNK - 0.5f);

    CAaSphere sphere;
    sphere.c.x = pos->x;
    sphere.c.y = pos->y;
    sphere.c.z = 0.0f;
    sphere.r = radius;

    int32_t areaRow = (cellX >> MAP_CELL_TO_AREA_SHIFT) & MAP_AREA_INDEX_MASK;
    int32_t areaCol = (cellY >> MAP_CELL_TO_AREA_SHIFT) & MAP_AREA_INDEX_MASK;
    int32_t minRow = areaRow - 1 > 0 ? areaRow - 1 : 0;
    int32_t maxRow = areaRow + 1 >= MAP_AREAS_PER_AXIS - 1 ? MAP_AREAS_PER_AXIS - 1 : areaRow + 1;
    int32_t minCol = areaCol - 1 > 0 ? areaCol - 1 : 0;
    int32_t maxCol = areaCol + 1 >= MAP_AREAS_PER_AXIS - 1 ? MAP_AREAS_PER_AXIS - 1 : areaCol + 1;

    for (int32_t row = minRow; row <= maxRow; row++) {
        for (int32_t col = minCol; col <= maxCol; col++) {
            CMapArea* area = CMap::GetArea(col, row);

            if (!area || area->asyncObject) {
                continue;
            }

            CAaBox box;
            box.b.x = area->m_topLeft2.x - MD_AREA_POLY_SIZE;
            box.b.y = area->m_topLeft2.y - MD_AREA_POLY_SIZE;
            box.b.z = 0.0f;
            box.t.x = area->m_topLeft2.x;
            box.t.y = area->m_topLeft2.y;
            box.t.z = 0.0f;

            if (NTempest::Intersect2d(box, sphere, NTempest::SOLID_BOTH)) {
                area->QueryLiquidSounds(pos, radius, results);
            }
        }
    }
}

// STATIC: 0x00AEDF58
static const uint32_t s_flightTriangles[8][3] = {
    { 3, 0, 4 },
    { 0, 1, 4 },
    { 1, 2, 4 },
    { 2, 5, 4 },
    { 5, 8, 4 },
    { 8, 7, 4 },
    { 7, 6, 4 },
    { 6, 3, 4 },
};

// STATIC: 0x00AEDFB8
static const float s_flightVertices[9][2] = {
    { 0.0f, 0.0f },
    { 0.0f, -266.66666f },
    { 0.0f, -MD_AREA_POLY_SIZE },
    { -266.66666f, 0.0f },
    { -266.66666f, -266.66666f },
    { -266.66666f, -MD_AREA_POLY_SIZE },
    { -MD_AREA_POLY_SIZE, 0.0f },
    { -MD_AREA_POLY_SIZE, -266.66666f },
    { -MD_AREA_POLY_SIZE, -MD_AREA_POLY_SIZE },
};

// VERIFY: b2919fa1 manual
// OFFSET: 0x007AD700
int32_t CMap::GetFlightBounds(const C3Vector& pos, float* height, int32_t which) {
    if (CMap::bDungeon) {
        return 0;
    }

    int32_t layer = which == 1 ? 9 : 0;
    float x = -(pos.x - HALF_WORLD_SIZE);
    float y = -(pos.y - HALF_WORLD_SIZE) * 0.00375f;
    int32_t cellY = CMath::fint_(y - 0.5f);
    x = x * 0.00375f;
    int32_t cellX = CMath::fint_(x - 0.5f);

    CMapArea* area = CMap::GetArea(cellY >> 1, cellX >> 1);

    if (!area || area->asyncObject || !area->m_flyingBbox) {
        return 0;
    }

    int16_t* bounds = area->m_flyingBbox;
    float dx = pos.x - area->m_topLeft2.x;
    float dy = pos.y - area->m_topLeft2.y;
    uint32_t triangle;

    if (cellY & 1) {
        if (cellX & 1) {
            triangle = dy < dx ? 4 : 5;
        } else {
            triangle = dy < -MD_AREA_POLY_SIZE - dx ? 3 : 2;
        }
    } else {
        if (cellX & 1) {
            triangle = dy < -MD_AREA_POLY_SIZE - dx ? 6 : 7;
        } else {
            triangle = dy < dx ? 1 : 0;
        }
    }

    uint32_t a = s_flightTriangles[triangle][0];
    uint32_t b = s_flightTriangles[triangle][1];
    uint32_t c = s_flightTriangles[triangle][2];

    float ax = s_flightVertices[a][0];
    float ay = s_flightVertices[a][1];
    float az = static_cast<float>(bounds[layer + a]);
    float bx = s_flightVertices[b][0];
    float by = s_flightVertices[b][1];
    float bz = static_cast<float>(bounds[layer + b]);
    float cx = s_flightVertices[c][0];
    float cy = s_flightVertices[c][1];
    float cz = static_cast<float>(bounds[layer + c]);

    float e1x = bx - ax;
    float e1y = by - ay;
    float e1z = bz - az;
    float e2x = cx - ax;
    float e2y = cy - ay;
    float e2z = cz - az;

    C3Vector normal(e1y * e2z - e1z * e2y, e1z * e2x - e2z * e1x, e1x * e2y - e1y * e2x);
    normal.Normalize();

    *height = (normal.y * dy - (ax * normal.x + ay * normal.y + az * normal.z) + normal.x * dx) * (-1.0f / normal.z);
    return 1;
}

// VERIFY: aa1eecbc auto
// OFFSET: 0x007AD940
void CMap::QueryDoodadDefHitTests(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, const C3Vector& start, const C3Vector& end) {
    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if (doodadDef->m_queryTag == CMap::s_queryTag || doodadDef->m_collisionGuid) {
            continue;
        }

        if ((doodadDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) || !(doodadDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
            continue;
        }

        CM2Model* model = doodadDef->model;

        if (!model) {
            continue;
        }

        if (start.x > doodadDef->m_bbox.t.x || start.x < doodadDef->m_bbox.b.x || start.y > doodadDef->m_bbox.t.y || start.y < doodadDef->m_bbox.b.y || end.z > doodadDef->m_bbox.t.z || start.z < doodadDef->m_bbox.b.z) {
            continue;
        }

        STORM_ASSERT(model->m_scene->IsHitTesting()); // 12213: "m_scene->IsHitTesting()"

        if (model->m_runtimeFlags & M2_FLAGS::M2_INITIALIZED) {
            if (!model->m_hitTestPrev) {
                model->m_hitTestPrev = &model->m_scene->m_hitList;
                model->m_hitTestNext = model->m_scene->m_hitList;
                model->m_scene->m_hitList = model;

                if (model->m_hitTestNext) {
                    model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
                }
            }

            model->m_hitTestMode = CM2Model::HIT_TEST_COLLISION;
            model->m_hitTestGroup = 0;
            model->m_hitTestOwner = doodadDef;
        }

        doodadDef->m_queryTag = CMap::s_queryTag;
    }
}

// VERIFY: 50db1a05 manual
// OFFSET: 0x007ADA80
void CMap::VectorIntersectMapObjs(CMapChunk* chunk, const C3Vector& start, const C3Vector& end, float* t) {
    float invHeight = 1.0f / (start.z - end.z);

    if (chunk) {
        float hitT = 1.0f;

        for (CMapBaseObjLink* link = chunk->mapObjDefLinkList.Head(); link; link = chunk->mapObjDefLinkList.Next(link)) {
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->owner);

            if (mapObjDef->m_flags & (MAPOBJ_FLAG_NO_HITTEST | MAPOBJ_FLAG_NO_DEPTH_SORT)) {
                continue;
            }

            CMapObj* mapObj = mapObjDef->mapObj;

            if (!mapObj) {
                continue;
            }

            C3Vector localStart = start * mapObjDef->m_invMat;
            C3Vector localEnd = end * mapObjDef->m_invMat;

            if (mapObj->TestBounds(localStart, localEnd)) {
                float normalDot = 0.0f;

                if (mapObj->VectorIntersect(localStart, localEnd, WORLD_QUERY_RENDER_MASK | WORLD_QUERY_LIQUID, 0, &hitT, nullptr, mapObjDef, &normalDot, nullptr) && hitT < *t) {
                    *t = hitT;

                    if (normalDot >= 0.0f) {
                        *t = hitT - ((1.0f - normalDot * invHeight) * invHeight + invHeight);

                        if (*t < 0.0f) {
                            *t = 0.0f;
                        }
                    }
                }
            }

            for (CMapBaseObjLink* groupLink = mapObjDef->groupLinkList.Head(); groupLink; groupLink = mapObjDef->groupLinkList.Next(groupLink)) {
                CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(groupLink->owner);

                if (start.x <= group->aaBox.t.x && start.x >= group->aaBox.b.x && start.y <= group->aaBox.t.y && start.y >= group->aaBox.b.y && end.z <= group->aaBox.t.z && start.z >= group->aaBox.b.z) {
                    CMap::QueryDoodadDefHitTests(&group->doodadDefLinkList, start, end);
                }
            }
        }

        return;
    }

    float hitT = 1.0f;

    for (CMapBaseObjLink* link = CMap::mapObjDefLinkList.Head(); link; link = CMap::mapObjDefLinkList.Next(link)) {
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->owner);

        if (mapObjDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        if (!mapObj) {
            continue;
        }

        C3Vector localStart = start * mapObjDef->m_invMat;
        C3Vector localEnd = end * mapObjDef->m_invMat;

        if (mapObj->TestBounds(localStart, localEnd)) {
            float normalDot = 0.0f;

            if (mapObj->VectorIntersect(const_cast<C3Vector&>(start), const_cast<C3Vector&>(end), WORLD_QUERY_RENDER_MASK | WORLD_QUERY_LIQUID, 0, &hitT, nullptr, mapObjDef, &normalDot, nullptr) && hitT < *t) {
                *t = hitT;

                if (normalDot >= 0.0f) {
                    *t = hitT - ((1.0f - normalDot * invHeight) * invHeight + invHeight);

                    if (*t < 0.0f) {
                        *t = 0.0f;
                    }
                }
            }
        }

        for (CMapBaseObjLink* groupLink = mapObjDef->groupLinkList.Head(); groupLink; groupLink = mapObjDef->groupLinkList.Next(groupLink)) {
            CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(groupLink->owner);

            if (start.x <= group->aaBox.t.x && start.x >= group->aaBox.b.x && start.y <= group->aaBox.t.y && start.y >= group->aaBox.b.y && end.z <= group->aaBox.t.z && start.z >= group->aaBox.b.z) {
                CMap::QueryDoodadDefHitTests(&group->doodadDefLinkList, start, end);
            }
        }
    }
}

// VERIFY: c6ec57f9 manual
// OFFSET: 0x007ADE10
bool CMap::QueryHeight(const C3Vector& position, float range, float* height) {
    if (range < 0.01f) {
        return false;
    }

    CMap::s_queryTag++;
    float bestHeight = -100000.0f;
    bool found = false;
    CMapChunk* chunk = nullptr;

    if (!CMap::bDungeon) {
        float chunkY = -(position.y - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK;
        float chunkX = OO_COORD_TO_SUBCHUNK * -(position.x - HALF_WORLD_SIZE);
        int32_t cellY = CMath::fint_(chunkY - 0.5f);
        int32_t cellX = CMath::fint_(chunkX - 0.5f);
        CMapArea* area = CMap::GetAreaForCell(cellY, cellX);

        if (!area || area->asyncObject) {
            return false;
        }

        chunk = area->GetChunkForCell(cellY, cellX);

        if (chunk) {
            float liquidHeight = 0.0f;
            C2Vector chunkPos(chunkY, chunkX);
            C3Vector pos = position;
            found = CMap::GetHeightTerrain(chunk, pos, cellY, cellX, &bestHeight);

            if (chunk->GetLiquidHeight(chunkPos, &liquidHeight) && liquidHeight > bestHeight) {
                bestHeight = liquidHeight;
            }
        }
    }

    C3Vector top(position.x, position.y, position.z + range);
    C3Vector bottom(position.x, position.y, position.z - range);
    World::s_m2Scene->BeginHitTest();
    float t = 1.0f;
    CMap::VectorIntersectMapObjs(chunk, top, bottom, &t);

    if (chunk) {
        CMap::QueryDoodadDefHitTests(&chunk->doodadDefLinkList, top, bottom);
    }

    World::s_m2Scene->EndHitTestCollisionWorld(top, bottom, t);

    if (t >= 1.0f) {
        if (found) {
            *height = bestHeight;
            return true;
        }

        return false;
    }

    *height = top.z - t * (range + range);

    if (bestHeight > *height) {
        *height = bestHeight;
    }

    return true;
}

// VERIFY: 5c4ccdae manual
// OFFSET: 0x0079FCC0
void CMap::RenderFootprints() {
    if (!(CWorld::enables & Enables::Enable_Footprints)) {
        return;
    }

    uint32_t now = FrameTime::s_curTimeMs;
    uint32_t index = 0;
    CMapFootprint* footprints = &s_footprints[0][0];

    for (uint32_t n = 576; n; n--) {
        CMapFootprint* footprint = &footprints[index];

        if (footprint->m_vertices.Count()) {
            uint32_t elapsed = now - footprint->m_startTime;
            float fade = 1.0f - static_cast<float>(elapsed) * FOOTPRINT_FADEOUT_RATE;

            if (fade >= 0.0f) {
                float alpha = fade * 255.0f;
                int32_t value = CMath::fint_(alpha - 0.5f);
                footprint->m_alpha = value < MAXALPHA ? CMath::fint_(alpha - 0.5f) : MAXALPHA;
                CMapFootprintTexture& texture = s_footprintTextures.m_data[footprint->m_textureIndex];
                texture.m_footprintIndices.Add(1, &index);
                texture.m_vertexCount += footprint->m_vertices.Count();
            } else {
                footprint->m_vertices.SetCount(0);
            }
        }

        if (++index >= 576) {
            index = 0;
        }
    }

    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->RsSet(GxRs_Texture1, ProjectTex2dGetModFadeTex());
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxRsSet(GxRs_AlphaRef, static_cast<int32_t>(CGxDevice::s_alphaRef[2]));
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_PolygonOffset, CWorldParam::cvar_footstepBias->m_floatValue * 0.0039063096f);

    C44Matrix world;
    world.d0 = -CWorldScene::s_activeWorldView.x;
    world.d1 = -CWorldScene::s_activeWorldView.y;
    world.d2 = -CWorldScene::s_activeWorldView.z;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    for (uint32_t t = 0; t < s_footprintTextures.Count(); t++) {
        CMapFootprintTexture& texture = s_footprintTextures.m_data[t];

        if (texture.m_vertexCount) {
            CGxTex* gxTex = TextureGetGxTex(texture.m_texture, 0, nullptr);

            if (gxTex) {
                CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPNCT2), texture.m_vertexCount);
                CGxVertexPNCT2* vertices = reinterpret_cast<CGxVertexPNCT2*>(g_theGxDevicePtr->BufLock(vertexBuf));
                CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, 2, texture.m_vertexCount);
                uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
                uint16_t next = 0;

                for (uint32_t k = 0; k < texture.m_footprintIndices.Count(); k++) {
                    CMapFootprint* footprint = &footprints[texture.m_footprintIndices.m_data[k]];
                    CImVector color;
                    color.b = 0xFF;
                    color.g = 0xFF;
                    color.r = 0xFF;
                    color.a = static_cast<uint8_t>(footprint->m_alpha);

                    for (uint32_t v = 0; v < footprint->m_vertices.Count(); v++) {
                        CGxVertexPT0T1& src = footprint->m_vertices.m_data[v];
                        vertices->p.x = src.m_position.x;
                        vertices->p.y = src.m_position.y;
                        vertices->p.z = src.m_position.z;
                        vertices->n.x = 0.0f;
                        vertices->n.y = 0.0f;
                        vertices->n.z = 1.0f;
                        vertices->c = color;
                        vertices->tc[0].x = src.m_texture0.x;
                        vertices->tc[0].y = src.m_texture0.y;
                        vertices->tc[1].x = src.m_texture1.x;
                        vertices->tc[1].y = src.m_texture1.y;
                        *indices = next++;
                        vertices++;
                        indices++;
                    }
                }

                g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
                vertexBuf->m_hasData = 1;
                g_theGxDevicePtr->BufUnlock(indexBuf, 0);
                indexBuf->m_hasData = 1;
                GxPrimVertexPtr(vertexBuf, GxVBF_PNCT2);
                g_theGxDevicePtr->PrimIndexPtr(indexBuf);
                g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);

                CGxBatch batch;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = 0;
                batch.m_count = texture.m_vertexCount;
                batch.m_minIndex = 0;
                batch.m_maxIndex = static_cast<uint16_t>(texture.m_vertexCount - 1);
                g_theGxDevicePtr->Draw(&batch, 1);
            }
        }

        texture.m_footprintIndices.m_count = 0;
        texture.m_vertexCount = 0;
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// VERIFY: 331b297f manual
// OFFSET: 0x007C3830
void CMap::Unload() {
    CMap::PurgeMaps();
    CMap::s_lowDetail.Unload();

    for (CMapBaseObjLink* link = CMap::mapObjDefLinkList.Head(); link;) {
        CMapBaseObjLink* next = CMap::mapObjDefLinkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->owner);
        CMap::FreeBaseObjLink(link);
        CMap::PurgeMapObjDef(mapObjDef);
        link = next;
    }

    for (CMapBaseObjLink* link = CMap::doodadDefLinkList.Head(); link;) {
        CMapBaseObjLink* next = CMap::doodadDefLinkList.Next(link);
        CMapBaseObj* owner = link->owner;
        CMap::FreeBaseObjLink(link);

        if (!(owner->GetType() & CMapBaseObj::Type_UserCreated)) {
            CMapDoodadDef::Purge(static_cast<CMapDoodadDef*>(owner));
        }

        link = next;
    }

    CDestructibleProxy* proxy = CMap::destructibleProxies.Head();

    while (!CMap::destructibleProxies.IsEnd(proxy)) {
        CDestructibleProxy* next = CMap::destructibleProxies.RawNext(proxy);

        if (proxy->m_uniqueId) {
            CMap::destructibleProxies.UnlinkNode(proxy);
            CMap::destructibleProxies.DeleteNode(proxy);
        }

        proxy = next;
    }

    CMap::destructibleProxies.Clear();

    STORM_ASSERT(mapChunkUpdateList.Head() == nullptr); // 12213: "mapChunkUpdateList.Head() == __null"
    STORM_ASSERT(mapRenderChunkUpdateList.Head() == nullptr); // 12213: "mapRenderChunkUpdateList.Head() == __null"
    STORM_ASSERT(mapChunkLiquidUpdateList.Head() == nullptr); // 12213: "mapChunkLiquidUpdateList.Head() == __null"
    STORM_ASSERT(mapDetailDoodadUpdateList.Head() == nullptr); // 12213: "mapDetailDoodadUpdateList.Head() == __null"
    STORM_ASSERT(doodadDefLinkList.Head() == nullptr); // 12213: "doodadDefLinkList.Head() == __null"
    STORM_ASSERT(mapObjDefLinkList.Head() == nullptr); // 12213: "mapObjDefLinkList.Head() == __null"
    STORM_ASSERT(doodadDefHash.Head() == nullptr); // 12213: "doodadDefHash.Head() == __null"
    STORM_ASSERT(mapObjDefHash.Head() == nullptr); // 12213: "mapObjDefHash.Head() == __null"

    CMapObj::ClearCache(1);
    World::s_m2Scene->PurgeModelCache();
    CMap::ClearAllFootprints();
    CDetailDoodad::FreeGxBufMat();
    CMapRenderChunk::Destroy();
    CMapObj::DestroyPools();
    TextureGxCacheFlush();
    g_theGxDevicePtr->DeviceEvictResources();
    DayNight::UnloadMap();
    CMap::DestroyLight(CMap::s_mapLight);
    CMap::FreeAllChunkLiquids();
    CMap::bActive = 0;
    CMap::bDungeon = 0;
    CMap::enableChunkBatching = 0;
    char path[260];
    SStrCopy(path, CMap::wdtFilename, STORM_MAX_STR);
    char* ext = strrchr(path, '.');
    STORM_ASSERT(ext);
    SStrCopy(ext, ".tex", STORM_MAX_STR);
    TextureUnloadBlob(path);
}

// VERIFY: 6b4c3b7f auto
// OFFSET: 0x0079D460
void CMap::CreateWaterRipple(const C3Vector& pos, float facing, float size, float duration, float width, float growth, int32_t type, int32_t player) {
    if (CWorld::s_waterRipples && !World::s_waterLOD) {
        float alpha = 1.0f;

        if (width < 0.16666667f) {
            alpha = width * 6.0f;
        }

        WaterRipple::Create(pos, facing, size, alpha, duration, growth, type, player);
    }
}

// VERIFY: b6f5c768 auto
// OFFSET: 0x007B4A50
void CMap::SnapBaseObjToSubChunk(CMapBaseObj* baseObj, const C3Vector* position, float facing) {
    C44Matrix rotation = C44Matrix::RotationAroundZ(facing);
    CAaBox bounds(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));

    if (baseObj->GetType() & CMapBaseObj::Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->mapObj->GetBounds(&bounds);
    } else {
        bounds = static_cast<CMapStaticEntity*>(baseObj)->model->GetBoundingBox();
    }

    float centerX = (bounds.t.x + bounds.b.x) * 0.5f;
    float centerY = (bounds.t.y + bounds.b.y) * 0.5f;
    float centerZ = (bounds.t.z + bounds.b.z) * 0.5f;
    bounds.t.x = bounds.t.x - centerX;
    bounds.t.y = bounds.t.y - centerY;
    bounds.t.z = bounds.t.z - centerZ;
    bounds.b.x = bounds.b.x - centerX;
    bounds.b.y = bounds.b.y - centerY;
    bounds.b.z = bounds.b.z - centerZ;
    CAaBox rotated(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
    CWorldMath::TransformAABox(rotation, bounds, rotated);
    C3Vector* snapped = const_cast<C3Vector*>(position);
    float x = snapped->x + rotated.t.x;
    float y = snapped->y + rotated.t.y;
    int32_t cellX = CMath::fint_((x + SMOLTILE_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);
    int32_t cellY = CMath::fint_((y + SMOLTILE_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);
    snapped->x = snapped->x - (x - cellX * SMOLTILE_SIZE);
    snapped->y = snapped->y - (y - SMOLTILE_SIZE * cellY);
}

// VERIFY: 153db93c manual
// OFFSET: 0x007B58C0
void CMap::UpdateDoodadDef(CMapDoodadDef* doodadDef, const C3Vector* position, float facing) {
    doodadDef->m_position = *position;
    doodadDef->m_scale = 1.0f;
    doodadDef->m_mat = C44Matrix();
    doodadDef->m_mat.d0 = position->x;
    doodadDef->m_mat.d1 = position->y;
    doodadDef->m_mat.d2 = position->z;
    doodadDef->m_mat.RotateAroundZ(facing);
    CMap::UpdateDoodadDefBounds(doodadDef);
}

// VERIFY: 05946402 manual
// OFFSET: 0x007B5870
void CMap::UpdateDoodadDefTransform(CMapDoodadDef* doodadDef, const C44Matrix& matrix) {
    doodadDef->m_position.x = matrix.d0;
    doodadDef->m_position.y = matrix.d1;
    doodadDef->m_position.z = matrix.d2;
    doodadDef->m_scale = sqrtf(matrix.a2 * matrix.a2 + matrix.a1 * matrix.a1 + matrix.a0 * matrix.a0);
    doodadDef->m_mat = matrix;
    CMap::UpdateDoodadDefBounds(doodadDef);
}

// VERIFY: 7fce3415 manual
// OFFSET: 0x0079F760
void CMap::BuildFootprintMatrix(C44Matrix& matrix, const float* size, int32_t left, float facing) {
    if (left) {
        matrix.Scale(C3Vector(-1.0f, 1.0f, 1.0f));
    }

    if (size[1] <= size[0]) {
        if (size[0] > size[1]) {
            matrix.Scale(C3Vector(size[0] / size[1], 1.0f, 1.0f));
        }
    } else {
        matrix.Scale(C3Vector(1.0f, size[1] / size[0], 1.0f));
    }

    matrix.RotateAroundZ(-facing);
}

// VERIFY: a2657405 manual
// OFFSET: 0x0079FA70
void CMap::NewFootprint(int32_t textureId, const float* size, const C3Vector* position, float facing, int32_t left, int32_t terrainType, int32_t activePlayer) {
    if (static_cast<uint32_t>(textureId) >= s_footprintTextures.Count()) {
        return;
    }

    TerrainTypeRec* rec = g_terrainTypeDB.GetRecord(terrainType);

    if (!rec || !(rec->m_flags & TERRAIN_TYPE_FLAG_FOOTPRINTS)) {
        return;
    }

    CAaBox box;
    box.b = C3Vector(0.0f, 0.0f, 0.0f);
    box.t = C3Vector(0.0f, 0.0f, 0.0f);
    CWorldMath::MakeAaBox(box, *position, size);

    if (!World::GetFacets(&box, &s_footprintFacets, WORLD_QUERY_RENDER_MASK, nullptr)) {
        return;
    }

    CMapFootprint* footprint;

    if (activePlayer) {
        footprint = &s_footprints[0][0] + s_nextPlayerFootprint++;

        if (s_nextPlayerFootprint >= 64) {
            s_nextPlayerFootprint = 0;
        }
    } else {
        footprint = &s_footprints[0][0] + s_nextFootprint++;

        if (s_nextFootprint >= 576) {
            s_nextFootprint = 64;
        }
    }

    footprint->m_link.Unlink();
    footprint->m_position = *position;
    footprint->m_startTime = FrameTime::s_curTimeMs;
    footprint->m_textureIndex = textureId;
    C44Matrix texMatrix;
    C44Matrix fadeMatrix;
    C44Matrix rotation;
    CMap::BuildFootprintMatrix(rotation, size, left, facing);
    ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, &rotation, 0.5f, 1);
    footprint->m_vertices.SetCount(3 * s_footprintFacets.facets.m_count);
    CGxVertexPT0T1* vertex = footprint->m_vertices.m_data;

    for (uint32_t i = 0; i < s_footprintFacets.facets.m_count; i++) {
        CFacet& facet = s_footprintFacets.facets.m_data[i];

        for (uint32_t j = 0; j < 3; j++) {
            vertex->m_position = facet.v[j];
            vertex->m_texture0 = C2Vector(facet.v[j] * texMatrix);
            vertex->m_texture1 = C2Vector(facet.v[j] * fadeMatrix);
            vertex++;
        }
    }
}

// VERIFY: 599297b5 manual
// OFFSET: 0x007A6260
bool CMap::GetTriSubchunk(CMapChunk* chunk, const CiRect* rect, const CAaBox* box) {
    STORM_ASSERT(s_subVertexIndexFlat[4] == 18); // 12213: "m_subVertexIndex[2][2] == 18"

    int32_t base = TERRAIN_CHUNK_VERTEX_ROW_STRIDE * rect->minY + rect->minX;
    C3Vector* vertices = &CMapChunk::vertexList[base];
    float* height = &chunk->m_height[base];
    int32_t rowSkip = TERRAIN_CHUNK_VERTEX_ROW_STRIDE - (rect->maxX - rect->minX + 1);
    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;

    for (int32_t y = rect->minY; y <= rect->maxY; y++, vertices += rowSkip, height += rowSkip) {
        for (int32_t x = rect->minX; x <= rect->maxX; x++, vertices++, height++) {
            if (chunk->m_header->m_holes & CMap::s_holeMask[4 * (y >> 1) + (x >> 1)]) {
                continue;
            }

            uint8_t outcodes[20];

            for (uint32_t i = 0; i < 5; i++) {
                int32_t index = s_subVertexIndexFlat[i];
                vertices[index].z = height[index];
                outcodes[index] = static_cast<uint8_t>(CWorldMath::ComputeAaBoxOutcode(const_cast<CAaBox*>(box), &vertices[index]));
            }

            uint16_t vertexIndex = static_cast<uint16_t>(vertices - CMapChunk::vertexList);

            for (uint32_t tri = 0; tri < 12; tri += 3) {
                const int32_t* triangle = &s_subVertexTriangles[tri];

                if (outcodes[triangle[0]] & outcodes[triangle[1]] & outcodes[triangle[2]]) {
                    continue;
                }

                if (!found) {
                    batch = World::TriData::AllocBatch();

                    if (batch) {
                        batch->m_def = reinterpret_cast<CMapObjDef*>(chunk);
                        C44Matrix* matrix;

                        if (World::TriData::matrixPoolCount + 1 < 32) {
                            matrix = &World::TriData::matrixPool[World::TriData::matrixPoolCount++];
                        } else {
                            World::TriData::statusFlags |= WORLD_TRI_QUERY_STATUS::TRI_QUERY_STATUS_OVERFLOW;
                            matrix = nullptr;
                        }

                        matrix->a0 = 1.0f;
                        matrix->a1 = 0.0f;
                        matrix->a2 = 0.0f;
                        matrix->a3 = 0.0f;
                        matrix->b0 = 0.0f;
                        matrix->b1 = 1.0f;
                        matrix->b2 = 0.0f;
                        matrix->b3 = 0.0f;
                        matrix->c0 = 0.0f;
                        matrix->c1 = 0.0f;
                        matrix->c2 = 1.0f;
                        matrix->c3 = 0.0f;
                        matrix->d0 = chunk->m_topLeftCoords.x;
                        matrix->d1 = chunk->m_topLeftCoords.y;
                        matrix->d2 = chunk->m_topLeftCoords.z;
                        matrix->d3 = 1.0f;
                        batch->m_matrix = matrix;
                        batch->vertices = CMapChunk::vertexList;
                        batch->heights = chunk->m_height;
                        batch->m_normalList = nullptr;
                        indices = World::TriData::AllocIndices(12 * (rect->maxX - rect->minX + 1) * (rect->maxY - rect->minY + 1));
                        batch->vertexIndices = indices;
                    }
                }

                found = true;

                if (!batch || !batch->vertexIndices || !batch->vertices) {
                    continue;
                }

                for (uint32_t i = 0; i < 3; i++) {
                    uint16_t index = vertexIndex + static_cast<uint16_t>(triangle[i]);
                    *indices++ = index;

                    if (batch->m_minVertexIndex >= index) {
                        batch->m_minVertexIndex = index;
                    }

                    if (batch->m_maxVertexIndex <= index) {
                        batch->m_maxVertexIndex = index;
                    }
                }

                batch->indexCount += 3;
                batch->m_faceCount++;
            }
        }
    }

    return found;
}

// VERIFY: c11b2d43 manual
// OFFSET: 0x007A6630
bool CMap::GetTrisChunk(int32_t chunkX, int32_t chunkY, const CiRect& cellRect, const CAaBox& box, World::TriData* triData, uint32_t flags) {
    CMapArea* area = CMap::GetAreaForChunk(chunkX, chunkY);

    if (!area || area->asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->GetChunk(chunkX, chunkY);

    if (!chunk) {
        return false;
    }

    CAaBox localBox = box;
    localBox.b.x = localBox.b.x - chunk->m_topLeftCoords.x;
    localBox.b.y = localBox.b.y - chunk->m_topLeftCoords.y;
    localBox.b.z = localBox.b.z - chunk->m_topLeftCoords.z;
    localBox.t.x = localBox.t.x - chunk->m_topLeftCoords.x;
    localBox.t.y = localBox.t.y - chunk->m_topLeftCoords.y;
    localBox.t.z = localBox.t.z - chunk->m_topLeftCoords.z;
    bool result = false;
    static CiRect s_chunkRect(0, 0, 7, 7);
    CiRect rect;
    rect.minY = cellRect.minY - 8 * chunkY;
    rect.minX = cellRect.minX - 8 * chunkX;
    rect.maxY = cellRect.maxY - 8 * chunkY;
    rect.maxX = cellRect.maxX - 8 * chunkX;
    rect &= s_chunkRect;

    if (flags & WORLD_QUERY_TERRAIN) {
        result = CMap::GetTriSubchunk(chunk, &rect, &localBox);
    }

    if (flags & WORLD_QUERY_LIQUID_MASK) {
        bool walkableOnly = (flags & WORLD_QUERY_WATER_WALK_LIQUID) && !(flags & WORLD_QUERY_LIQUID);

        for (CChunkLiquid* liquid = chunk->liquidsList.Head(); liquid; liquid = chunk->liquidsList.Next(liquid)) {
            if (walkableOnly && !(g_liquidTypeDB.GetRecord(liquid->m_type)->m_flags & LIQUID_TYPE_FLAG_WATER_WALKING)) {
                continue;
            }

            result |= liquid->GetTris(triData, localBox, rect);
        }
    }

    return result;
}

// VERIFY: bd067831 manual
// OFFSET: 0x007A6830
bool CMap::GetTrisTerrain(const CAaBox& box, World::TriData* triData, uint32_t flags) {
    // 12213: "tLocation.minx >= 0.0f && tLocation.miny >= 0.0f"
    STORM_ASSERT(-(box.t.y - HALF_WORLD_SIZE) >= 0.0f && -(box.t.x - HALF_WORLD_SIZE) >= 0.0f);
    // 12213: "tLocation.maxy < ((64*16)*((150.0f/36.0f)*8)) && tLocation.maxy < ((64*16)*((150.0f/36.0f)*8))" (maxy twice in 12213)
    STORM_ASSERT(-(box.b.x - HALF_WORLD_SIZE) < 2.0f * HALF_WORLD_SIZE);

    bool result = false;
    CiRect cellRect;
    cellRect.minY = CMath::fint_(-(box.t.x - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);
    cellRect.minX = CMath::fint_(-(box.t.y - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);
    cellRect.maxY = CMath::fint_(-(box.b.x - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);
    cellRect.maxX = CMath::fint_(-(box.b.y - HALF_WORLD_SIZE) * OO_COORD_TO_SUBCHUNK - 0.5f);

    for (int32_t chunkY = cellRect.minY >> 3; chunkY <= cellRect.maxY >> 3; chunkY++) {
        for (int32_t chunkX = cellRect.minX >> 3; chunkX <= cellRect.maxX >> 3; chunkX++) {
            result |= CMap::GetTrisChunk(chunkX, chunkY, cellRect, box, triData, flags);
        }
    }

    return result;
}

// VERIFY: 24d73f6d manual
// OFFSET: 0x007A6940
bool CMap::QueryTrisMapObjDefs(const CAaBox& box, World::TriData* triData, uint32_t flags) {
    CAaBox localBox;
    localBox.b = C3Vector(0.0f, 0.0f, 0.0f);
    localBox.t = C3Vector(0.0f, 0.0f, 0.0f);
    C3Vector center((box.b.x + box.t.x) * 0.5f, (box.t.y + box.b.y) * 0.5f, 0.5f * (box.t.z + box.b.z));
    CAaBox extents;
    extents.b = box.b - center;
    extents.t = box.t - center;
    bool result = false;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        if (!mapObj) {
            continue;
        }

        C3Vector localCenter = center * mapObjDef->m_invMat;
        C33Matrix rotation(mapObjDef->m_invMat);
        CWorldMath::TransformAABox(rotation, extents, localBox);
        localBox.b += localCenter;
        localBox.t += localCenter;

        if (mapObj->TestBounds(localBox)) {
            result |= mapObj->GetTris(localBox, flags, triData, mapObjDef);
        }
    }

    return result;
}

// VERIFY: 87b59ea2 auto
// OFFSET: 0x007A6AF0
bool CMap::QueryTris(const CAaBox& box, World::TriData* triData, uint32_t flags) {
    bool result = false;

    if (flags & (WORLD_QUERY_WMO_MASK | WORLD_QUERY_LIQUID_MASK)) {
        result = CMap::QueryTrisMapObjDefs(box, triData, flags);
    }

    if (flags & (WORLD_QUERY_TERRAIN | WORLD_QUERY_LIQUID_MASK)) {
        return CMap::GetTrisTerrain(box, triData, flags) | result;
    }

    return result;
}

// VERIFY: 0e38b513 manual
// OFFSET: 0x007A2AA0
uint32_t CMap::QueryIntersectingModels(CM2Model** doodads, uint32_t maxDoodadsToReturn, const CAaBox& box, uint32_t flags) {
    if (!maxDoodadsToReturn) {
        return 0;
    }

    uint32_t numFound = 0;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if ((mapObjDef->m_flags & MAPOBJ_FLAG_NO_HITTEST) || !mapObjDef->m_bbox.Intersects(box)) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->mapObj;

        for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->owner);

            if (!mapObj->IsGroupLoaded(defGroup->groupNum) || !defGroup->aaBox.Intersects(box)) {
                continue;
            }

            for (CMapBaseObjLink* doodadLink = defGroup->doodadDefLinkList.Head(); doodadLink; doodadLink = defGroup->doodadDefLinkList.Next(doodadLink)) {
                CMapStaticEntity* doodadDef = static_cast<CMapStaticEntity*>(doodadLink->owner);
                CM2Model* model = doodadDef->model;

                if (!model) {
                    continue;
                }

                uint32_t doodadFlags = doodadDef->m_flags;

                if (!(doodadFlags & MAPOBJ_FLAG_PREPARED) || !doodadDef->m_bbox.Intersects(box)) {
                    continue;
                }

                uint32_t match = (flags & WORLD_QUERY_RENDER_MASK) != 0;

                if (flags & WORLD_QUERY_PROJ_TEX_DOODADS) {
                    match |= doodadFlags & MAPOBJ_FLAG_PROJ_TEX;
                }

                if (flags & WORLD_QUERY_COLLISION_MASK) {
                    match |= (doodadFlags & MAPOBJ_FLAG_NO_HITTEST) == 0;
                }

                if (match) {
                    STORM_ASSERT(numFound < maxDoodadsToReturn);
                    doodads[numFound++] = model;

                    if (numFound == maxDoodadsToReturn) {
                        return numFound;
                    }
                }
            }
        }
    }

    return numFound;
}

// VERIFY: d3ebe146 manual
// OFFSET: 0x007B5740
void CMap::UpdateDoodadDefBounds(CMapDoodadDef* doodadDef) {
    STORM_ASSERT(doodadDef);

    CM2Model* model = doodadDef->model;
    doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags | MAPOBJ_FLAG_UNPLACED);

    if (model) {
        if (model->IsLoaded(0, 0)) {
            doodadDef->UpdateBounds();
            doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags | MAPOBJ_FLAG_PREPARED);
        } else {
            doodadDef->m_bbox = doodadDef->model->m_shared->m_boundingBox;
            doodadDef->m_bbox.b.x = doodadDef->m_position.x + doodadDef->m_bbox.b.x;
            doodadDef->m_bbox.b.y = doodadDef->m_bbox.b.y + doodadDef->m_position.y;
            doodadDef->m_bbox.b.z = doodadDef->m_position.z + doodadDef->m_bbox.b.z;
            doodadDef->m_bbox.t.x = doodadDef->m_position.x + doodadDef->m_bbox.t.x;
            doodadDef->m_bbox.t.y = doodadDef->m_bbox.t.y + doodadDef->m_position.y;
            doodadDef->m_bbox.t.z = doodadDef->m_position.z + doodadDef->m_bbox.t.z;
            doodadDef->m_sphere.c = doodadDef->m_position;
            float dz = doodadDef->m_bbox.t.z - doodadDef->m_bbox.b.z;
            float dy = doodadDef->m_bbox.t.y - doodadDef->m_bbox.b.y;
            float dx = doodadDef->m_bbox.t.x - doodadDef->m_bbox.b.x;
            doodadDef->m_sphere.r = sqrtf(dx * dx + (dy * dy + dz * dz));
        }

        CM2Model* current = doodadDef->model;
        current->SetWorldTransform(doodadDef->m_mat);
    }

    doodadDef->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_INSIDE;
    doodadDef->m_flags = static_cast<MapObjFlag>((doodadDef->m_flags & ~(MAPOBJ_FLAG_UNPLACED | MAPOBJ_FLAG_INTERIOR | MAPOBJ_FLAG_EXTERIOR)) | MAPOBJ_FLAG_UNPLACED);
    CMap::LinkStaticEntity(doodadDef);
    doodadDef->ExtendChunkBounds();
    // 12213: "doodadDef->flagExternal"
    STORM_ASSERT(doodadDef->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_EXTERNAL);
    CMap::doodadDefList.LinkToTail(doodadDef);
}
