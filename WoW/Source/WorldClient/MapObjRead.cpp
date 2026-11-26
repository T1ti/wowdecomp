#include "MapObjRead.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "WoW/Common/AaBsp.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include <cmath>
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"

// STATIC: 0x00ADFC4C
TSList<CMapObj, TSGetExplicitLink<CMapObj> > CMapObj::s_asyncLoadQueue = TSExplicitList<CMapObj, offsetof(CMapObj, m_asyncLoadLink)>();

// STATIC: 0x00ADFC58
TSList<CMapObjGroup, TSGetExplicitLink<CMapObjGroup> > CMapObj::s_loadedGroups = TSExplicitList<CMapObjGroup, offsetof(CMapObjGroup, m_asyncLoadLink)>();

// VERIFY: 477d94a9 manual
// OFFSET: 0x007D72D0
void CMapObj::CreateMaterials() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(this->materialCount); i++) {
        this->materialList[i].Textures()[SMOMaterial::MAPID_DIFFUSE] = nullptr;
        this->materialList[i].Textures()[SMOMaterial::MAPID_ENV] = nullptr;
    }
}

// VERIFY: 12bdf795 manual
// OFFSET: 0x007D7380
void CMapObjGroup::FixColorVertexAlpha() {
    uint32_t firstExterior = this->transBatchCount ? this->m_batchList[this->transBatchCount - 1].m_vertexEnd + 1 : 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_colorVertexListSize); i++) {
        CImVector& color = this->cvertexList[i];

        if (i >= firstExterior) {
            uint32_t alpha = color.a;
            uint32_t r = (color.r + ((alpha * color.r) >> 6)) >> 1;
            uint32_t g = (color.g + ((alpha * color.g) >> 6)) >> 1;
            uint32_t b = (color.b + ((alpha * color.b) >> 6)) >> 1;
            color.r = r > 0xFF ? 0xFF : r;
            color.g = g > 0xFF ? 0xFF : g;
            color.b = b > 0xFF ? 0xFF : b;
            color.a = 0xFF;
        } else {
            color.r >>= 1;
            color.g >>= 1;
            color.b >>= 1;
        }
    }
}

// VERIFY: cd3ffadf manual
// OFFSET: 0x007D7470
void CMapObj::Load() {
#if defined(WOWDECOMP_ASSERTIONS)
    const auto* pIffChunk = static_cast<const SIffChunk*>(this->m_pWmoData);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MVER); // 12213: "pIffChunk->token == 'MVER'"
    const auto* version = reinterpret_cast<const uint32_t*>(&pIffChunk->m_data);
    STORM_ASSERT(*version == 0x0011);
#endif

    SIffChunk* chunk = reinterpret_cast<SIffChunk*>(static_cast<uint8_t*>(this->m_pWmoData) + 12);
    this->m_header = reinterpret_cast<SMOHeader*>(&chunk->m_data);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_textureNameList = reinterpret_cast<char*>(&chunk->m_data);
    this->m_texturesSize = chunk->m_size;

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->materialList = reinterpret_cast<SMOMaterial*>(&chunk->m_data);
    this->materialCount = chunk->m_size / sizeof(SMOMaterial);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_groupNameList = reinterpret_cast<char*>(&chunk->m_data);
    this->m_groupNameSize = chunk->m_size;

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_groupInfo = reinterpret_cast<SMOGroupInfo*>(&chunk->m_data);
    this->m_groupInfoCount = chunk->m_size / sizeof(SMOGroupInfo);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_skybox = reinterpret_cast<char*>(&chunk->m_data);

    if (!this->m_skybox[0]) {
        this->m_skybox = nullptr;
    }

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);

    if (!this->m_skybox) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_groupInfoCount); i++) {
            this->m_groupInfo[i].m_flags &= ~CMapObjGroup::MOGP_FLAGS::F_USE_SKYBOX;
        }
    }

    this->m_portalVertexList = reinterpret_cast<C3Vector*>(&chunk->m_data);
    this->m_planeVertCount = chunk->m_size / sizeof(C3Vector);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_portalList = reinterpret_cast<SMOPortal*>(&chunk->m_data);
    this->m_portalsCount = chunk->m_size / sizeof(SMOPortal);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);

    for (int32_t i = 0; i < this->m_portalsCount; i++) {
        if (std::isnan(static_cast<double>(this->m_portalList[i].m_plane.d))) {
            this->m_portalList[i].m_plane.n.x = 0.0f;
            this->m_portalList[i].m_plane.n.y = 0.0f;
            this->m_portalList[i].m_plane.n.z = 1.0f;
            this->m_portalList[i].m_plane.d = 800000.0f;
        }
    }

    this->m_portalRefList = reinterpret_cast<SMOPortalRef*>(&chunk->m_data);
    this->m_portalRefCount = chunk->m_size / sizeof(SMOPortalRef);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_visBlockVertList = reinterpret_cast<C3Vector*>(&chunk->m_data);
    this->m_visBlockVertCount = chunk->m_size / sizeof(C3Vector);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_visBlockList = reinterpret_cast<SMOVisibleBlock*>(&chunk->m_data);
    this->m_visBlockCount = chunk->m_size / sizeof(SMOVisibleBlock);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_lightList = reinterpret_cast<SMOLight*>(&chunk->m_data);
    this->m_ligtsCount = chunk->m_size / sizeof(SMOLight);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_doodadSetList = reinterpret_cast<SMODoodadSet*>(&chunk->m_data);
    this->m_doodadSetCount = chunk->m_size / sizeof(SMODoodadSet);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_doodadNameList = reinterpret_cast<char*>(&chunk->m_data);
    this->m_doodadNameSize = chunk->m_size;

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->m_doodadDefList = reinterpret_cast<SMODoodadDef*>(&chunk->m_data);
    this->m_doodadDefCount = chunk->m_size / sizeof(SMODoodadDef);

    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);
    this->fogList = reinterpret_cast<SMOFog*>(&chunk->m_data);
    this->fogCount = chunk->m_size / sizeof(SMOFog);

    uint8_t* end = static_cast<uint8_t*>(this->m_pWmoData) + this->m_wmoFileSize;
    chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&chunk->m_data) + chunk->m_size);

    if (reinterpret_cast<uint8_t*>(chunk) < end && chunk->token == SIffChunk::TOKEN_MCVP) {
        this->m_convexVolumePlanes = reinterpret_cast<C4Plane*>(&chunk->m_data);
        this->m_convexVolumePlaneCount = chunk->m_size / sizeof(C4Plane);
    }
}

// VERIFY: 4e3ee2fd auto
// OFFSET: 0x007D7710
void CMapObj::CreateMaterial(uint8_t materialId) {
    STORM_ASSERT(materialId < this->materialCount); // 12213: "materialId < materialCount"

    SMOMaterial& material = this->materialList[materialId];

    if (material.Textures()[SMOMaterial::MAPID_DIFFUSE]) {
        return;
    }

    const char* texture1 = &this->m_textureNameList[material.m_texture1];
    const char* texture2 = &this->m_textureNameList[material.m_texture2];

    if (!*texture1) {
        texture1 = "createcrappygreentexture.blp";
    }

    if (!CShaderEffect::s_enableShaders) {
        texture2 = "";
    }

    switch (material.m_shader) {
        case CMapObj::SHADER_TYPE::ENVIRONMENT:
        case CMapObj::SHADER_TYPE::ENVIRONMENT_METAL:
        case CMapObj::SHADER_TYPE::COMPOSITE:
            if (*texture2) {
                break;
            }

            material.m_shader = CMapObj::SHADER_TYPE::OPAQUE_EFFECT;
            texture2 = nullptr;
            break;

        case CMapObj::SHADER_TYPE::DIFFUSE:
        case CMapObj::SHADER_TYPE::SPECULAR:
        case CMapObj::SHADER_TYPE::METAL:
        case CMapObj::SHADER_TYPE::OPAQUE_EFFECT:
            texture2 = nullptr;
            break;

        default:
            STORM_ASSERT(0);
            break;
    }

    material.Textures()[SMOMaterial::MAPID_DIFFUSE] = CMap::LoadTexture(texture1);

    if (texture2) {
        material.Textures()[SMOMaterial::MAPID_ENV] = CMap::LoadTexture(texture2);
    } else {
        material.Textures()[SMOMaterial::MAPID_ENV] = nullptr;
    }
}

// VERIFY: 03cc8920 manual
// OFFSET: 0x007D78C0
CMapObjGroup* CMapObj::AttenTransVerts(CMapObjGroup* group) {
    group->m_runtimeFlags |= CMapObjGroup::RUNTIME_FLAGS::TRANS_VERTEX_ATTENUATION_HANDLED;

    if (!group->transBatchCount) {
        return group;
    }

    uint32_t vertexEnd = group->m_batchList[group->transBatchCount - 1].m_vertexEnd;

    for (uint32_t i = 0; i <= vertexEnd; i++) {
        C3Vector& vert = group->m_vertexList[i];
        CImVector& color = group->cvertexList[i];
        float atten = 0.0f;

        if (group->m_portalCount) {
            uint32_t p;

            for (p = 0; p < static_cast<uint32_t>(group->m_portalCount); p++) {
                SMOPortalRef& ref = this->m_portalRefList[p + group->m_portalStart];
                SMOPortal& portal = this->m_portalList[ref.m_portalIndex];
                C3Vector onPlane = vert;
                float side = portal.m_plane.n.z * vert.z + portal.m_plane.n.y * vert.y + portal.m_plane.n.x * vert.x + portal.m_plane.d;

                if (side > 0.001f) {
                    C3Vector end(vert.x - portal.m_plane.n.x, vert.y - portal.m_plane.n.y, vert.z - portal.m_plane.n.z);
                    NTempest::Intersect(C3Ray::FromStartEnd(vert, end, false), portal.m_plane, nullptr, &onPlane, 0.01f);
                } else if (side < -0.001f) {
                    C3Vector end(portal.m_plane.n.x + vert.x, portal.m_plane.n.y + vert.y, portal.m_plane.n.z + vert.z);
                    NTempest::Intersect(C3Ray::FromStartEnd(vert, end, false), portal.m_plane, nullptr, &onPlane, 0.01f);
                }

                C3Vector* portalVerts = &this->m_portalVertexList[portal.m_startVertex];
                float dist;

                if (NTempest::Intersect(onPlane, portalVerts, portal.m_count, portal.m_plane.n.MajorAxis())) {
                    dist = portal.m_plane.n.z * vert.z + portal.m_plane.n.y * vert.y + portal.m_plane.n.x * vert.x + portal.m_plane.d;

                    if (ref.m_side != 1) {
                        dist = dist * -1.0f;
                    }
                } else {
                    dist = NTempest::DistanceFromPolygonEdge(vert, portalVerts, portal.m_count);
                }

                if (this->m_groupInfo[ref.m_groupIndex].m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR_CLASSIFICATION_MASK) {
                    if (dist < 0.0f) {
                        dist = 0.0f;
                    }

                    float weight = 1.0f - dist * 0.15f;

                    if (weight > 0.001f) {
                        atten = weight + atten;
                    }
                } else if (dist > -1.0f && dist < 1.0f) {
                    atten = 0.0f;
                    break;
                }
            }

            if (p == static_cast<uint32_t>(group->m_portalCount)) {
                if (atten <= 0.001f) {
                    atten = 0.0f;
                } else if (atten > 1.0f) {
                    atten = 1.0f;
                }
            }
        }

        color.r = static_cast<uint8_t>(atten * (127.0f - color.r) + color.r);
        color.g = static_cast<uint8_t>(color.g + (127.0f - color.g) * atten);
        color.b = static_cast<uint8_t>(color.b + (127.0f - color.b) * atten);
        color.a = static_cast<uint8_t>(atten * 255.0f);
    }

    return group;
}

// VERIFY: 5789fe57 manual
// OFFSET: 0x007D7C30
void CMapObjGroup::CreateOptionalDataPointers(SIffChunk* dataChunk) {
    SIffChunk* pIffChunk = dataChunk;
    uint32_t flags = this->m_flags;

    if (flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIGHT_REFS) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOLR); // 12213: "pIffChunk->token=='MOLR'"
        this->m_lightRefList = reinterpret_cast<uint16_t*>(&pIffChunk->m_data);
        this->m_lightRefListCount = pIffChunk->m_size >> 1;
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    }

    if (flags & CMapObjGroup::MOGP_FLAGS::F_HAS_DOODAD_REFS) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MODR); // 12213: "pIffChunk->token=='MODR'"
        this->m_doodadRefList = reinterpret_cast<uint16_t*>(&pIffChunk->m_data);
        this->m_doodadRefListCount = pIffChunk->m_size >> 1;
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    }

    if (flags & CMapObjGroup::MOGP_FLAGS::F_HAS_BSP_TREE) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOBN); // 12213: "pIffChunk->token=='MOBN'"
        uint32_t nodeSize = pIffChunk->m_size;
        CAaBspNode* nodes = reinterpret_cast<CAaBspNode*>(&pIffChunk->m_data);
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + nodeSize);
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOBR); // 12213: "pIffChunk->token=='MOBR'"
        uint32_t faceSize = pIffChunk->m_size;
        uint16_t* faces = reinterpret_cast<uint16_t*>(&pIffChunk->m_data);
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + faceSize);
        this->m_aaBsp.Set(nodes, nodeSize >> 4, faces, faceSize >> 1, this->m_bbox);
    }

    if (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MPBV); // 12213: "pIffChunk->token=='MPBV'"
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MPBP); // 12213: "pIffChunk->token=='MPBP'"
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MPBI); // 12213: "pIffChunk->token=='MPBI'"
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MPBG); // 12213: "pIffChunk->token=='MPBG'"
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    }

    if (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_VERTEX_COLORS) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOCV); // 12213: "pIffChunk->token=='MOCV'"
        CMapObj* parent = this->m_parent;
        this->cvertexList = reinterpret_cast<CImVector*>(&pIffChunk->m_data);
        this->m_colorVertexListSize = pIffChunk->m_size >> 2;
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);

        if (!(parent->m_header->m_flags & SMOHeader::FLAGS::NO_VERTEX_ALPHA_FIX)) {
            this->FixColorVertexAlpha();
        }
    }

    if (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIQUID) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MLIQ); // 12213: "pIffChunk->token == 'MLIQ'"
        uint32_t* header = reinterpret_cast<uint32_t*>(&pIffChunk->m_data);
        this->m_liquidVerts.x = header[0];
        this->m_liquidVerts.y = header[1];
        this->liquidTiles.x = header[2];
        this->liquidTiles.y = header[3];
        this->m_liquidCorner = *reinterpret_cast<C3Vector*>(&header[4]);

        uint16_t materialId = *reinterpret_cast<uint16_t*>(&header[7]);
        uint8_t* data = reinterpret_cast<uint8_t*>(&header[7]) + 2;
        this->m_liquidVertexList = reinterpret_cast<SMOLVert*>(data);
        data += 8 * this->m_liquidVerts.x * this->m_liquidVerts.y;
        this->m_liquidTileList = reinterpret_cast<SMOLTile*>(data);
        data += this->liquidTiles.x * this->liquidTiles.y;
        pIffChunk = reinterpret_cast<SIffChunk*>(data);
        this->m_liquidMaterialId = materialId;

        if (!this->m_liquidType) {
            this->m_liquidType = this->GetLiquidType(this->FindLiquidType());
        }

        CMapObjGroup::AllocVertArray(&this->m_liquidVertArray, this->m_liquidVerts.x * this->m_liquidVerts.y);
        this->GenLiquidVerts();
    }

    if (this->m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_MORI_MORB) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MORI); // 12213: "pIffChunk->token == 'MORI'"
        this->m_stripIndices = reinterpret_cast<uint16_t*>(&pIffChunk->m_data);
        this->m_stripIndexCount = pIffChunk->m_size >> 1;
#if defined(WOWDECOMP_ASSERTIONS)
        const auto* stripBatchChunk = reinterpret_cast<const SIffChunk*>(reinterpret_cast<const uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
        STORM_ASSERT(stripBatchChunk->token == WMOTOKEN_MORB); // 12213: "pIffChunk->token == 'MORB'"
#endif
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size + 8);
        this->m_stripBatches = reinterpret_cast<uint8_t*>(pIffChunk);

        if (CMap::s_enableTriangleStrips) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_batchListCount); i++) {
                this->m_batchList[i].m_indexStart = *reinterpret_cast<uint32_t*>(&this->m_stripBatches[8 * i]);
                this->m_batchList[i].count = *reinterpret_cast<uint16_t*>(&this->m_stripBatches[8 * i + 4]);
            }
        }
    }

    flags = this->m_flags;

    if (flags & CMapObjGroup::MOGP_FLAGS::F_HAS_SECOND_TEXTURE_COORDS) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOTV); // 12213: "pIffChunk->token=='MOTV'"
        this->m_textureVertexList2 = reinterpret_cast<C2Vector*>(&pIffChunk->m_data);
        this->m_textureVertexList2Count = pIffChunk->m_size >> 3;
        pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    }

    if (flags & CMapObjGroup::MOGP_FLAGS::F_HAS_EXTRA_COLOR_VERTICES) {
        STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOCV); // 12213: "pIffChunk->token=='MOCV'"
        this->m_colorVertexListExtra = reinterpret_cast<CImVector*>(&pIffChunk->m_data);
        this->m_colorVertexListExtraSize = pIffChunk->m_size >> 2;
    }
}

// VERIFY: 182a9a50 manual
// OFFSET: 0x007D7F50
void CMapObjGroup::CreateDataPointers(SIffChunk* dataChunk) {
    SIffChunk* pIffChunk = dataChunk;
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOPY); // 12213: "pIffChunk->token=='MOPY'"
    this->m_polyList = reinterpret_cast<SMOPoly*>(&pIffChunk->m_data);
    this->m_polyListSize = pIffChunk->m_size >> 1;

    pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOVI); // 12213: "pIffChunk->token == 'MOVI'"
    this->m_indices = reinterpret_cast<uint16_t*>(&pIffChunk->m_data);
    this->m_indicesCount = pIffChunk->m_size >> 1;

    pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOVT); // 12213: "pIffChunk->token=='MOVT'"
    this->m_vertexList = reinterpret_cast<C3Vector*>(&pIffChunk->m_data);
    this->m_vertexListCount = pIffChunk->m_size / sizeof(C3Vector);

    pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MONR); // 12213: "pIffChunk->token=='MONR'"
    this->m_normalList = reinterpret_cast<C3Vector*>(&pIffChunk->m_data);
    this->m_normalListCount = pIffChunk->m_size / sizeof(C3Vector);

    pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOTV); // 12213: "pIffChunk->token=='MOTV'"
    this->m_textureVertexList = reinterpret_cast<C2Vector*>(&pIffChunk->m_data);
    this->m_textureVertexListCount = pIffChunk->m_size >> 3;

    pIffChunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size);
    STORM_ASSERT(pIffChunk->token == WMOTOKEN_MOBA); // 12213: "pIffChunk->token=='MOBA'"
    this->m_batchList = reinterpret_cast<SMOBatch*>(&pIffChunk->m_data);
    this->m_batchListCount = pIffChunk->m_size / sizeof(SMOBatch);

    this->CreateOptionalDataPointers(reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(&pIffChunk->m_data) + pIffChunk->m_size));
}

// VERIFY: aea1ef5e manual
// OFFSET: 0x007D8050
void CMapObj::PostloadCallback(void* arg) {
    CMapObj* mapObj = static_cast<CMapObj*>(arg);

    STORM_ASSERT(mapObj);

    AsyncFileReadDestroyObject(mapObj->m_asyncObject);
    mapObj->m_asyncObject = nullptr;
    mapObj->m_asyncLoadLink.Unlink();
    mapObj->Load();
    mapObj->CreateMaterials();
    mapObj->m_argbColor = mapObj->m_header->m_ambColor;
    mapObj->m_bbox = mapObj->m_header->m_boundingBox;
    STORM_ASSERT(mapObj->m_mapObjGroupCount == 0); // 12213: "groupPtrList.Count() == 0"
    mapObj->m_mapObjGroupCount = mapObj->m_groupInfoCount;

    for (uint32_t n = 0; n < static_cast<uint32_t>(mapObj->m_groupInfoCount); n++) {
        mapObj->groupPtrList[n] = CMap::AllocMapObjGroup();
        STORM_ASSERT(mapObj->groupPtrList[n]); // 12213: "groupPtrList[n]"
    }

    mapObj->m_isGroupLoaded = 1;
}

// VERIFY: 737e6a62 manual
// OFFSET: 0x007D80C0
bool CMapObj::Read(char* fileName) {
    SStrCopy(this->m_wmoName, fileName, STORM_MAX_STR);

    SFileImpl* file = nullptr;

    if (CMap::SafeOpen(fileName, &file)) {
        SStrCopy(this->m_wmoName, fileName, STORM_MAX_STR);
    } else {
        CMap::SafeOpen("world\\wmo\\Dungeon\\test\\missingwmo.wmo", &file);
        SStrCopy(this->m_wmoName, "world\\wmo\\Dungeon\\test\\missingwmo.wmo", STORM_MAX_STR);
    }

    this->m_wmoFileSize = SFileGetFileSize(file, nullptr);
    this->m_pWmoData = SMemAlloc(this->m_wmoFileSize, __FILE__, __LINE__, 0);

    CAsyncObject* asyncObject = AsyncFileReadAllocObject();
    STORM_ASSERT(asyncObject);
    this->m_asyncObject = asyncObject;
    this->m_asyncObject->file = file;
    this->m_asyncObject->userArg = this;
    this->m_asyncObject->buffer = this->m_pWmoData;
    this->m_asyncObject->size = this->m_wmoFileSize;
    this->m_asyncObject->userPostloadCallback = CMapObj::PostloadCallback;
    this->m_asyncObject->priority = 124;
    AsyncFileReadObject(this->m_asyncObject, 0);

    CMapObj::s_asyncLoadQueue.LinkToTail(this);

    return true;
}

// VERIFY: a3a005c2 manual
// OFFSET: 0x007D81C0
void CMapObjGroup::HandleAntiportal() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_polyListSize); i++) {
        C3Vector* verts[3];
        verts[0] = &this->m_vertexList[this->m_indices[3 * i]];
        verts[1] = &this->m_vertexList[this->m_indices[3 * i + 1]];
        verts[2] = &this->m_vertexList[this->m_indices[3 * i + 2]];

        float height = (verts[1]->z + verts[2]->z + verts[0]->z) * 0.33333334f;
        uint32_t above[3];
        uint32_t count = 0;

        if (height < verts[0]->z) {
            above[count++] = 0;
        }

        if (height < verts[1]->z) {
            above[count++] = 1;
        }

        if (height < verts[2]->z) {
            above[count++] = 2;
        }

        if (count >= 2) {
            CMapObjOccluder* occluder = CMapObj::AllocOccluder();
            occluder->m_start = *verts[above[0]];
            occluder->m_end = *verts[above[1]];
            this->m_occluderList.LinkToTail(occluder);
        }
    }

    this->m_extBatchCount = 0;
    this->intBatchCount = 0;
}

// VERIFY: 579978be manual
// OFFSET: 0x007D82E0
void CMapObjGroup::Create() {
#if defined(WOWDECOMP_ASSERTIONS)
    const auto* iffChunk = static_cast<const SIffChunk*>(this->rawData);
    STORM_ASSERT(iffChunk->token == WMOTOKEN_MVER); // 12213: "iffChunk->token == 'MVER'"
    const auto* version = reinterpret_cast<const uint32_t*>(&iffChunk->m_data);
    STORM_ASSERT(*version == 0x0011);
    iffChunk = reinterpret_cast<const SIffChunk*>(version + 1);
    STORM_ASSERT(iffChunk->token == WMOTOKEN_MOGP); // 12213: "iffChunk->token=='MOGP'"
#endif

    SMOGroupHeader* smoGroup = reinterpret_cast<SMOGroupHeader*>(static_cast<uint8_t*>(this->rawData) + 20);
    CMapObj* parent = this->m_parent;
    STORM_ASSERT(parent);
    parent->m_mapObjGroupList.LinkToTail(this);

    this->m_groupName = &parent->m_groupNameList[smoGroup->m_groupName];
    this->m_flags = smoGroup->flags;

    if (!parent->m_skybox) {
        this->m_flags &= ~CMapObjGroup::MOGP_FLAGS::F_USE_SKYBOX;
    }

    this->m_bbox = smoGroup->m_boundingBox;
    this->m_portalStart = smoGroup->m_portalStart;
    this->m_portalCount = smoGroup->m_portalCount;
    this->transBatchCount = smoGroup->m_transBatchCount;
    this->intBatchCount = smoGroup->m_intBatchCount;
    this->m_extBatchCount = smoGroup->m_extBatchCount;
    memcpy(this->m_fogs, smoGroup->m_fogIds, sizeof(this->m_fogs));
    this->m_wmoGroupId = smoGroup->m_uniqueID;

    if (parent->m_header->m_flags & SMOHeader::FLAGS::LIQUID_TYPE_IDS) {
        this->m_liquidType = this->GetLiquidType(smoGroup->m_groupLiquid);
    } else {
        int32_t liquidType = this->GetLiquidType(smoGroup->m_groupLiquid == 15 ? 0 : smoGroup->m_groupLiquid + 1);
        this->m_liquidType = liquidType;
        STORM_ASSERT(!liquidType || !(smoGroup->flags & MOGP_FLAGS::F_HAS_LIQUID)); // 12213: "!liquidType || !(smoGroup->flags&SMOGroup::LIQUIDSURFACE)"
    }

    this->CreateDataPointers(reinterpret_cast<SIffChunk*>(smoGroup + 1));

    CMapObj* mapObj = this->m_parent;
    STORM_ASSERT(mapObj);

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_batchListCount); i++) {
        mapObj->CreateMaterial(this->m_batchList[i].m_texture);
    }

    this->m_runtimeFlags = (this->m_runtimeFlags & ~CMapObjGroup::RUNTIME_FLAGS::TRANS_VERTEX_ATTENUATION_HANDLED) | CMapObjGroup::RUNTIME_FLAGS::LOADED;

    if (mapObj->m_header->m_flags & SMOHeader::FLAGS::NO_VERTEX_ATTENUATION) {
        this->m_runtimeFlags |= CMapObjGroup::RUNTIME_FLAGS::TRANS_VERTEX_ATTENUATION_HANDLED;
    }

    if (!SStrCmp(this->m_groupName, "antiportal", STORM_MAX_STR)) {
        this->HandleAntiportal();
    }

    this->m_runtimeFlags |= CMapObjGroup::RUNTIME_FLAGS::ALL_BATCHES_OPAQUE;

    uint32_t batchCount = this->m_batchListCount;

    if (!batchCount) {
        return;
    }

    this->m_indexMin = 0xFFFFFFFF;
    this->m_indexMax = 0;
    this->m_vertexMin = 0xFFFF;
    this->m_vertexMax = 0;

    for (uint32_t i = 0; i < batchCount; i++) {
        SMOBatch& batch = this->m_batchList[i];

        if (mapObj->materialList[batch.m_texture].m_blendMode) {
            this->m_runtimeFlags &= ~CMapObjGroup::RUNTIME_FLAGS::ALL_BATCHES_OPAQUE;
            break;
        }

        STORM_ASSERT(batch.count > 0);
        uint32_t indexEnd = batch.count + batch.m_indexStart - 1;

        if (this->m_vertexMax < batch.m_vertexEnd) {
            this->m_vertexMax = batch.m_vertexEnd;
        }

        if (this->m_vertexMin > batch.m_vertexStart) {
            this->m_vertexMin = batch.m_vertexStart;
        }

        if (this->m_indexMin > batch.m_indexStart) {
            this->m_indexMin = batch.m_indexStart;
        }

        if (this->m_indexMax < indexEnd) {
            this->m_indexMax = indexEnd;
        }
    }

    for (uint32_t i = 0; i < batchCount; i++) {
        if (mapObj->materialList[this->m_batchList[i].m_texture].m_shader == CMapObj::SHADER_TYPE::COMPOSITE) {
            this->m_runtimeFlags |= CMapObjGroup::RUNTIME_FLAGS::EXTENDED_VERTEX_FORMAT;
            return;
        }
    }
}

// VERIFY: 2026a068 auto
// OFFSET: 0x007D8570
void CMapObjGroup::AsyncPostloadCallback(void* arg) {
    CMapObjGroup* mapObjGroup = static_cast<CMapObjGroup*>(arg);
    STORM_ASSERT(mapObjGroup);

    AsyncFileReadDestroyObject(mapObjGroup->asyncObject);
    mapObjGroup->asyncObject = nullptr;
    mapObjGroup->m_asyncLoadLink.Unlink();
    mapObjGroup->Create();
}

// VERIFY: 1fdd96d9 manual
// OFFSET: 0x007D85E0
void CMapObj::ReadGroup(uint32_t index, bool preLoad) {
#if defined(WOWDECOMP_ASSERTIONS)
    const SMOGroupInfo* groupInfo = this->m_groupInfo ? &this->m_groupInfo[index] : nullptr;
    STORM_ASSERT(groupInfo);
#endif
    CMapObjGroup* group = this->groupPtrList[index];
    STORM_ASSERT(group);

    char fileName[256];
    strcpy(fileName, this->m_wmoName);
    SStrPrintf(strrchr(fileName, '.'), sizeof(fileName), "_%03d", index);
    strcat(fileName, ".wmo");

    SFileImpl* file = nullptr;
    CMap::SafeOpen(fileName, &file);

    group->m_fileSize = SFileGetFileSize(file, nullptr);
    group->rawData = SMemAlloc(group->m_fileSize, __FILE__, __LINE__, 0);
    group->m_parent = this;

    if (preLoad) {
        CMap::SafeRead(fileName, file, group->rawData, group->m_fileSize);
        group->Create();
        SFileCloseFile(file);
        return;
    }

    CAsyncObject* asyncObject = AsyncFileReadAllocObject();
    STORM_ASSERT(asyncObject);
    asyncObject->file = file;
    asyncObject->buffer = group->rawData;
    asyncObject->size = group->m_fileSize;
    asyncObject->userArg = group;
    asyncObject->userPostloadCallback = CMapObjGroup::AsyncPostloadCallback;
    group->asyncObject = asyncObject;
    AsyncFileReadObject(asyncObject, 0);

    CMapObj::s_loadedGroups.LinkToTail(group);
}
