#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJWRITE_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJWRITE_H

// Tool features: the WMO (map object) writer, the inverse of MapObjRead.cpp.
//
// No client ships a WMO writer: 0.5.3, 0.9.1, 3.3.x and 4.1 all only read WMOs, and the exporter lived in the art
// tools. What the clients do keep is the exporter's tree builder (0.5.3 CAaBsp::Create, restored in
// WoW/Common/AaBspBuild.cpp) and the exact chunk order the reader expects (CMapObj::Load, CMapObjGroup::
// CreateDataPointers / CreateOptionalDataPointers), which this writer mirrors chunk for chunk. It follows the same
// layering as the 0.5.3 model writer (MDLFileWrite -> ModelDataToBin -> per-section writers): MapObjWriteRoot/Group
// write a file, MapObjRootToBin/GroupToBin build the image, one Write<Chunk> per chunk.
//
// Input is a CMapObj / CMapObjGroup as the client holds it after loading, so a loaded WMO can be written back. A few
// load-time edits are not reversible and are noted where they apply (F_USE_SKYBOX cleared without a skybox, MOCV alpha
// fix, NaN portal planes replaced, triangle strip batches folded into MOBA).

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "MapObjRead.h"
#include "Engine/Source/Storm/H/stpl.h"

class C3Vector;
class CAaBsp;
class CMapObj;
class CMapObjGroup;
class CStatus;

// Appends IFF chunks (token, size, data) to a buffer. Chunks may nest (MOGP wraps the whole group body): BeginChunk
// returns the chunk's offset and EndChunk patches its size.
class CIffWriter {
    public:
    TSGrowableArray<uint8_t>& m_out;

    explicit CIffWriter(TSGrowableArray<uint8_t>& out);

    uint32_t BeginChunk(uint32_t token);
    void EndChunk(uint32_t chunkOffset);
    void Write(const void* data, uint32_t size);
    void WriteChunk(uint32_t token, const void* data, uint32_t size);
};

// Public entry points. Return nonzero on success; failures are added to status.
int32_t MapObjWriteRoot(const char* filename, const CMapObj& mapObj, CStatus* status);
int32_t MapObjWriteGroup(const char* filename, const CMapObjGroup& group, CStatus* status);

// In-memory images.
int32_t MapObjRootToBin(const CMapObj& mapObj, TSGrowableArray<uint8_t>& out, CStatus* status);
int32_t MapObjGroupToBin(const CMapObjGroup& group, TSGrowableArray<uint8_t>& out, CStatus* status);

// Builds a group's collision tree (MOBN/MOBR) over its collidable triangles with the restored CAaBsp builder.
void MapObjBuildGroupBsp(CAaBsp& bsp, C3Vector* vertices, uint32_t nVertices, uint16_t* faceVertexIndices, uint32_t nFaceVertexIndices);

#endif

#endif
