#include "WoW/Source/WorldClient/MapObjWrite.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstring>
#include "WoW/Common/AaBsp.h"
#include "WoW/Common/Types.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Os/OsFile.h"

// Tool features: the WMO writer (design in MapObjWrite.h). The root file and the mandatory group chunks are
// implemented from the reader; the optional group chunks the reader transforms on load are TODO.

static int32_t WriteBufferToFile(const char* filename, const TSGrowableArray<uint8_t>& buffer, CStatus* status) {
    HOSFILE file = OsCreateFile(filename, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        if (status) {
            status->Add(STATUS_ERROR, "Unable to create %s", filename);
        }

        return 0;
    }

    uint32_t written = 0;
    int32_t result = OsWriteFile(file, buffer.m_data, buffer.m_count, &written) && written == buffer.m_count;
    OsCloseFile(file);

    if (!result && status) {
        status->Add(STATUS_ERROR, "Unable to write %s", filename);
    }

    return result;
}

CIffWriter::CIffWriter(TSGrowableArray<uint8_t>& out)
    : m_out(out) {
}

void CIffWriter::Write(const void* data, uint32_t size) {
    uint32_t offset = this->m_out.m_count;
    this->m_out.SetCount(offset + size);

    if (size) {
        if (data) {
            memcpy(&this->m_out.m_data[offset], data, size);
        } else {
            memset(&this->m_out.m_data[offset], 0, size);
        }
    }
}

uint32_t CIffWriter::BeginChunk(uint32_t token) {
    uint32_t offset = this->m_out.m_count;
    uint32_t header[2] = { token, 0 };
    this->Write(header, sizeof(header));
    return offset;
}

void CIffWriter::EndChunk(uint32_t chunkOffset) {
    uint32_t size = this->m_out.m_count - chunkOffset - 8;
    memcpy(&this->m_out.m_data[chunkOffset + 4], &size, sizeof(size));
}

void CIffWriter::WriteChunk(uint32_t token, const void* data, uint32_t size) {
    uint32_t chunk = this->BeginChunk(token);
    this->Write(data, size);
    this->EndChunk(chunk);
}

// Root file. Chunk order and element sizes are CMapObj::Load's; every chunk is written even when empty, because the
// reader walks them by position, not by tag (only the trailing MCVP is optional and checked by tag).
int32_t MapObjRootToBin(const CMapObj& mapObj, TSGrowableArray<uint8_t>& out, CStatus* status) {
    if (!mapObj.m_header) {
        if (status) {
            status->Add(STATUS_ERROR, "MapObjRootToBin: map object is not loaded");
        }

        return 0;
    }

    out.SetCount(0);
    CIffWriter iff(out);

    iff.WriteChunk(WMOTOKEN_MVER, &WMO_FILE_VERSION, sizeof(WMO_FILE_VERSION));
    iff.WriteChunk(WMOTOKEN_MOHD, mapObj.m_header, sizeof(SMOHeader));
    iff.WriteChunk(WMOTOKEN_MOTX, mapObj.m_textureNameList, mapObj.m_texturesSize);
    iff.WriteChunk(WMOTOKEN_MOMT, mapObj.materialList, mapObj.materialCount * sizeof(SMOMaterial));
    iff.WriteChunk(WMOTOKEN_MOGN, mapObj.m_groupNameList, mapObj.m_groupNameSize);
    iff.WriteChunk(WMOTOKEN_MOGI, mapObj.m_groupInfo, mapObj.m_groupInfoCount * sizeof(SMOGroupInfo));

    // Load drops an empty skybox name to nullptr (and clears F_USE_SKYBOX in MOGI); write a padded empty string then.
    if (mapObj.m_skybox) {
        uint32_t length = static_cast<uint32_t>(strlen(mapObj.m_skybox)) + 1;
        uint32_t chunk = iff.BeginChunk(WMOTOKEN_MOSB);
        iff.Write(mapObj.m_skybox, length);
        iff.Write(nullptr, (4 - (length & 3)) & 3);
        iff.EndChunk(chunk);
    } else {
        uint32_t empty = 0;
        iff.WriteChunk(WMOTOKEN_MOSB, &empty, sizeof(empty));
    }

    // Portal planes with a NaN distance were replaced by Load (normal +Z, d 800000); they are written as loaded.
    iff.WriteChunk(WMOTOKEN_MOPV, mapObj.m_portalVertexList, mapObj.m_planeVertCount * sizeof(C3Vector));
    iff.WriteChunk(WMOTOKEN_MOPT, mapObj.m_portalList, mapObj.m_portalsCount * sizeof(SMOPortal));
    iff.WriteChunk(WMOTOKEN_MOPR, mapObj.m_portalRefList, mapObj.m_portalRefCount * sizeof(SMOPortalRef));
    iff.WriteChunk(WMOTOKEN_MOVV, mapObj.m_visBlockVertList, mapObj.m_visBlockVertCount * sizeof(C3Vector));
    iff.WriteChunk(WMOTOKEN_MOVB, mapObj.m_visBlockList, mapObj.m_visBlockCount * sizeof(SMOVisibleBlock));
    iff.WriteChunk(WMOTOKEN_MOLT, mapObj.m_lightList, mapObj.m_ligtsCount * sizeof(SMOLight));
    iff.WriteChunk(WMOTOKEN_MODS, mapObj.m_doodadSetList, mapObj.m_doodadSetCount * sizeof(SMODoodadSet));
    iff.WriteChunk(WMOTOKEN_MODN, mapObj.m_doodadNameList, mapObj.m_doodadNameSize);
    iff.WriteChunk(WMOTOKEN_MODD, mapObj.m_doodadDefList, mapObj.m_doodadDefCount * sizeof(SMODoodadDef));
    iff.WriteChunk(WMOTOKEN_MFOG, mapObj.fogList, mapObj.fogCount * sizeof(SMOFog));

    if (mapObj.m_convexVolumePlanes && mapObj.m_convexVolumePlaneCount) {
        iff.WriteChunk(WMOTOKEN_MCVP, mapObj.m_convexVolumePlanes, mapObj.m_convexVolumePlaneCount * sizeof(C4Plane));
    }

    return 1;
}

// Group file: MVER, then one MOGP chunk holding SMOGroupHeader followed by the group sub-chunks.
int32_t MapObjGroupToBin(const CMapObjGroup& group, TSGrowableArray<uint8_t>& out, CStatus* status) {
    const CMapObj* parent = group.m_parent;

    if (!parent || !group.m_polyList) {
        if (status) {
            status->Add(STATUS_ERROR, "MapObjGroupToBin: group is not loaded");
        }

        return 0;
    }

    out.SetCount(0);
    CIffWriter iff(out);
    iff.WriteChunk(WMOTOKEN_MVER, &WMO_FILE_VERSION, sizeof(WMO_FILE_VERSION));

    uint32_t mogp = iff.BeginChunk(WMOTOKEN_MOGP);

    // SMOGroupHeader, rebuilt from what the reader kept (CMapObjGroup::Create copies these fields out).
    SMOGroupHeader header = {};
    header.m_groupName = group.m_groupName ? static_cast<uint32_t>(group.m_groupName - parent->m_groupNameList) : 0;
    header.m_descriptiveGroupName = 0; // TODO: not kept by the reader; take it from the root MOGN when rebuilding.
    header.flags = group.m_flags; // F_USE_SKYBOX may have been cleared at load when the root has no skybox.
    header.m_boundingBox = group.m_bbox;
    header.m_portalStart = static_cast<uint16_t>(group.m_portalStart);
    header.m_portalCount = static_cast<uint16_t>(group.m_portalCount);
    header.m_transBatchCount = group.transBatchCount;
    header.m_intBatchCount = group.intBatchCount;
    header.m_extBatchCount = group.m_extBatchCount;
    memcpy(header.m_fogIds, group.m_fogs, sizeof(header.m_fogIds));
    header.m_groupLiquid = 0; // TODO: the reader keeps the resolved liquid type, not the raw MOGP value.
    header.m_uniqueID = static_cast<uint32_t>(group.m_wmoGroupId);
    iff.Write(&header, sizeof(header));

    // Mandatory chunks, CreateDataPointers order.
    iff.WriteChunk(WMOTOKEN_MOPY, group.m_polyList, group.m_polyListSize * 2);
    iff.WriteChunk(WMOTOKEN_MOVI, group.m_indices, group.m_indicesCount * sizeof(uint16_t));
    iff.WriteChunk(WMOTOKEN_MOVT, group.m_vertexList, group.m_vertexListCount * sizeof(C3Vector));
    iff.WriteChunk(WMOTOKEN_MONR, group.m_normalList, group.m_normalListCount * sizeof(C3Vector));
    iff.WriteChunk(WMOTOKEN_MOTV, group.m_textureVertexList, group.m_textureVertexListCount * sizeof(C2Vector));
    iff.WriteChunk(WMOTOKEN_MOBA, group.m_batchList, group.m_batchListCount * sizeof(SMOBatch));

    // Optional chunks, CreateOptionalDataPointers order, each present when its MOGP flag is set.
    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIGHT_REFS) {
        iff.WriteChunk(WMOTOKEN_MOLR, group.m_lightRefList, group.m_lightRefListCount * sizeof(uint16_t));
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_DOODAD_REFS) {
        iff.WriteChunk(WMOTOKEN_MODR, group.m_doodadRefList, group.m_doodadRefListCount * sizeof(uint16_t));
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_BSP_TREE) {
        // The loaded tree (CAaBsp::Set over MOBN/MOBR). A group edited in a tool rebuilds it first with
        // MapObjBuildGroupBsp.
        iff.WriteChunk(WMOTOKEN_MOBN, group.m_aaBsp.m_nodes, group.m_aaBsp.m_nNodes * sizeof(CAaBspNode));
        iff.WriteChunk(WMOTOKEN_MOBR, group.m_aaBsp.m_nodeFaceIndices, group.m_aaBsp.m_nNodeFaceIndices * sizeof(uint16_t));
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400) {
        // TODO: the reader skips four chunks for this flag without keeping them; they cannot be written back.
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_VERTEX_COLORS) {
        // TODO: MOCV. Unless the root has SMOHeader::NO_VERTEX_ALPHA_FIX the reader rewrites the colors in place
        // (FixColorVertexAlpha), so writing cvertexList as is does not give back the file values.
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_LIQUID) {
        // TODO: MLIQ: header (vertex and tile counts, m_liquidCorner, material id) + SMOLVert grid + SMOLTile flags.
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_MORI_MORB) {
        // TODO: MORI strip indices + MORB strip batches. With CMap::s_enableTriangleStrips the reader copies the strip
        // ranges into MOBA, so MOBA above then holds strip ranges, not the file's list ranges.
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_SECOND_TEXTURE_COORDS) {
        iff.WriteChunk(WMOTOKEN_MOTV, group.m_textureVertexList2, group.m_textureVertexList2Count * sizeof(C2Vector));
    }

    if (group.m_flags & CMapObjGroup::MOGP_FLAGS::F_HAS_EXTRA_COLOR_VERTICES) {
        iff.WriteChunk(WMOTOKEN_MOCV, group.m_colorVertexListExtra, group.m_colorVertexListExtraSize * sizeof(CImVector));
    }

    iff.EndChunk(mogp);
    return 1;
}

int32_t MapObjWriteRoot(const char* filename, const CMapObj& mapObj, CStatus* status) {
    TSGrowableArray<uint8_t> buffer;
    return MapObjRootToBin(mapObj, buffer, status) && WriteBufferToFile(filename, buffer, status);
}

int32_t MapObjWriteGroup(const char* filename, const CMapObjGroup& group, CStatus* status) {
    TSGrowableArray<uint8_t> buffer;
    return MapObjGroupToBin(group, buffer, status) && WriteBufferToFile(filename, buffer, status);
}

// The exporter's use of the 0.5.3 builder: a tree over the group's triangles. MOBR stores face numbers (index into
// MOPY, triangle i = MOVI[3i..3i+2]) and CAaBsp::Create numbers the faces it is given 0..n-1, so pass the whole MOVI
// list. Whether the exporter left non-collidable faces out of the leaves is unknown (the queries filter by SMOPoly
// flags anyway).
void MapObjBuildGroupBsp(CAaBsp& bsp, C3Vector* vertices, uint32_t nVertices, uint16_t* faceVertexIndices, uint32_t nFaceVertexIndices) {
    bsp.Clear();
    bsp.Create(vertices, nVertices, faceVertexIndices, nFaceVertexIndices);
}

#endif
