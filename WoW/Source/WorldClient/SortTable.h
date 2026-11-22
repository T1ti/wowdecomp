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

// The terrain render list CWorldScene::RenderChunksZoneDebug draws (debugZones); lists 0..19 are
// 4 * layer count + shader variant, filled by CWorldScene::CullChunks.
static const uint32_t ZONE_DEBUG_RENDER_LIST = 20;

class CSortTable {
    public:
    /* 0000 */ CSortEntry m_table [64];
    /* 1B00 */ C4Vector m_bucketPlanes [64];
    /* 1F00 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > m_pendingExteriorGroupList;
    /* 1F0C */ TSList<CDetailDoodadInst, TSGetExplicitLink<CDetailDoodadInst> > m_detailDoodadInstList;
    /* 1F18 */ TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> m_renderChunkList [24];
    /* 2038 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > m_mapObjDefGroup;
    /* 2044 */ TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > m_interiorLiquidGroupList;
    /* 2050 */ TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > m_visibleEntityList;
    /* 205C */ TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > m_culledEntityList;
    /* 2068 */ TSList<CWorldOccluder, TSGetExplicitLink<CWorldOccluder> > m_visibleOccluderList;
    /* 2074 */ TSList<CMapAreaLow, TSGetExplicitLink<CMapAreaLow> > m_horizonChunkList;
    /* 2080 */ TSList<CMapObjDef, TSGetExplicitLink<CMapObjDef> > m_horizonMapObjList;

    CSortTable();
    void Clear();
    void BuildPlanes(const C3Vector& dir, const C3Vector& origin);
};

#endif
