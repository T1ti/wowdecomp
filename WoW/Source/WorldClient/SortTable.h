#ifndef WOW_SOURCE_WORLDCLIENT_SORTTABLE_H
#include "WoW/Source/WorldClient/CWorldOcclusion.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#define WOW_SOURCE_WORLDCLIENT_SORTTABLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"

class CDetailDoodadInst;

// The visRenderChunkList row CWorldScene::RenderChunksZoneDebug draws (debugZones); rows 0..4 are
// the layer count and columns the shader variant, filled by CWorldScene::CullChunks.
static const uint32_t ZONE_DEBUG_RENDER_LIST = 5;

class CSortTable {
    public:
    /* 0000 */ CSortEntry m_table [64];
    /* 1B00 */ C4Vector m_bucketPlanes [64];
    /* 1F00 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > moveableMapObjDefGroupList;
    /* 1F0C */ TSList<CDetailDoodadInst, TSGetExplicitLink<CDetailDoodadInst> > visDetailDoodadList;
    /* 1F18 */ TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> visRenderChunkList [6][4];
    /* 2038 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > visMapObjDefGroupList;
    /* 2044 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > visMapObjDefGroupLiquidList;
    /* 2050 */ TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > visEntityList;
    /* 205C */ TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > nonVisEntityList;
    /* 2068 */ TSList<CWorldOccluder, TSGetExplicitLink<CWorldOccluder> > visOccluderList;
    /* 2074 */ TSList<CMapAreaLow, TSGetExplicitLink<CMapAreaLow> > visLowDetailAreaList;
    /* 2080 */ TSList<CMapObjDef, TSGetExplicitLink<CMapObjDef> > visLowDetailMapObjDefList;

    CSortTable();
    void Clear();
#if defined(WOWDECOMP_ASSERTIONS)
    void AssertEmpty();
#endif
    void BuildPlanes(const C3Vector& dir, const C3Vector& origin);
};

#endif
