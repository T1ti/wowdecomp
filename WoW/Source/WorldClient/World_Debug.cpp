#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/WorldClient/World.h"
#include <cstdio>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/Types.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowTerrain (0x00665860).
int32_t CWorld::ConsoleCommand_ShowTerrain(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Chunks) {
        ConsoleWrite("Terrain disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_Chunks;
    } else {
        ConsoleWrite("Terrain enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_Chunks;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowDoodads (0x006658A0).
int32_t CWorld::ConsoleCommand_ShowDoodads(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Doodads) {
        ConsoleWrite("Terrain doodads disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_Doodads;
    } else {
        ConsoleWrite("Terrain doodads enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_Doodads;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowCollision (0x00665930).
// Drawn through CM2Model::SetDebugWorldCollision: collision triangles and face normals on every drawn M2 with collision data.
int32_t CWorld::ConsoleCommand_ShowCollision(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Collision) {
        ConsoleWrite("Terrain doodads collision visuals disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_Collision;
    } else {
        ConsoleWrite("Terrain doodads collision visuals enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_Collision;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowAABoxes (0x006658E0).
// Drawn through CM2Model::SetDebugWorldCollision: the collision box on every drawn M2 with collision data.
int32_t CWorld::ConsoleCommand_ShowAABoxes(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_AABoxes) {
        ConsoleWrite("Terrain doodads AA Box visuals disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_AABoxes;
    } else {
        ConsoleWrite("Terrain doodads AA Box visuals enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_AABoxes;
    }

    return 1;
}

// Port: the 3.3.5 WMO face renderers CMapObj::PrepareUpdate chooses by CMapObj::s_renderMode, which the release build
// keeps but never sets (always DEFAULT_RENDER). Their names here are the port's, after the renderers.
struct MapObjRenderModeName {
    const char* m_name;
    CMapObj::RENDER_MODE m_mode;
    const char* m_description;
};

static const MapObjRenderModeName s_mapObjRenderModes[] = {
    { "collision", CMapObj::RENDER_MODE::COLLISION_FACES, "collision faces, red (RenderGroupCollisionFaces)" },
    { "detail", CMapObj::RENDER_MODE::DETAIL_FACES, "detail faces, blue (RenderGroupDetailFaces)" },
    { "render", CMapObj::RENDER_MODE::RENDER_FACES, "render faces that are not detail faces, green (RenderGroupRenderFaces)" },
    { "transition", CMapObj::RENDER_MODE::TRANSPARENT_FACES, "transition faces that render, in their vertex colors (RenderGroupTransFaces)" },
    { "collidable", CMapObj::RENDER_MODE::COLLIDABLE, "collision faces and non-detail render faces, grey with a dark wireframe (RenderGroupCollidable)" },
    { "normal", CMapObj::RENDER_MODE::DEFAULT_RENDER, "normal rendering" },
};

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjBSP (0x006657C0): toggles Enable_MapObjBSP, which CMapObj::
// PrepareUpdate reads to draw every WMO group with CMapObj::RenderGroupBsp (render faces green, detail faces blue,
// collision faces red over them).
// Port addition: with an argument, draws the faces of one of the 3.3.5 face renderers (s_mapObjRenderModes) over the
// normal rendering (see CMapObj::RenderGroupFaceOverlay); "normal"
// goes back to normal rendering. Selecting one turns the BSP display off, and the BSP display, turned on, takes
// precedence over the mode.
int32_t CWorld::ConsoleCommand_ShowMapObjBSP(const char* command, const char* arguments) {
    if (arguments && *arguments) {
        for (const MapObjRenderModeName& mode : s_mapObjRenderModes) {
            if (!SStrCmpI(arguments, mode.m_name, STORM_MAX_STR)) {
                // The BSP display would take precedence (see CMapObj::PrepareUpdate): a mode replaces it.
                CWorld::enables &= ~Enables::Enable_MapObjBSP;
                CMapObj::s_renderMode = mode.m_mode;
                ConsoleWriteA("WMO render mode: %s.", DEFAULT_COLOR, mode.m_description);

                return 1;
            }
        }

        ConsoleWrite("Usage: showMapObjBSP [collision | detail | render | transition | collidable | normal]", DEFAULT_COLOR);
        return 1;
    }

    if (CWorld::enables & Enables::Enable_MapObjBSP) {
        ConsoleWrite("BSP render disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_MapObjBSP;
    } else {
        ConsoleWrite("BSP render enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_MapObjBSP;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_DebugBSP (0x00665810).
// Has no visible effect: no client reads Enable_DebugBSP. 0.5.3 sets it and nothing tests it; by 0.9.1 the bit had
// become CMap::enableTriangleStrips (read by CMap::Load), and 3.3.5 does not read it at all.
int32_t CWorld::ConsoleCommand_DebugBSP(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_DebugBSP) {
        ConsoleWrite("BSP debug disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_DebugBSP;
    } else {
        ConsoleWrite("BSP debug enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_DebugBSP;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjLight (0x00665AD0).
int32_t CWorld::ConsoleCommand_ShowMapObjLight(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_MapObjLight) {
        ConsoleWrite("MapObj lighting disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_MapObjLight;
    } else {
        ConsoleWrite("MapObj lighting enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_MapObjLight;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjTex (0x00665B50).
int32_t CWorld::ConsoleCommand_ShowMapObjTex(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_MapObjTex) {
        ConsoleWrite("MapObj textures disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_MapObjTex;
    } else {
        ConsoleWrite("MapObj textures enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_MapObjTex;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowCrappyBatches (0x00665B90).
// Has no visible effect: nothing in 0.5.3 reads Enable_CrappyBatches (only this command tests it), nor in 3.3.5.
int32_t CWorld::ConsoleCommand_ShowCrappyBatches(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_CrappyBatches) {
        ConsoleWrite("Crappy batches disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_CrappyBatches;
    } else {
        ConsoleWrite("Crappy batches enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_CrappyBatches;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjs (0x00665BE0).
// 3.3.5 also gates WMO hit tests (CWorld vector intersect) on this bit.
int32_t CWorld::ConsoleCommand_ShowMapObjs(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_MapObjs) {
        ConsoleWrite("Map objects disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_MapObjs;
    } else {
        ConsoleWrite("Map objects enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_MapObjs;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowPortals (0x00665C20).
int32_t CWorld::ConsoleCommand_ShowPortals(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Portals) {
        ConsoleWrite("Portal display disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_Portals;
    } else {
        ConsoleWrite("Portal display enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_Portals;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_PortalVis (0x00665C60).
int32_t CWorld::ConsoleCommand_PortalVis(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_PortalVis) {
        ConsoleWrite("Portal vis disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_PortalVis;
    } else {
        ConsoleWrite("Portal vis enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_PortalVis;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_WaterShow (0x00665CA0).
int32_t CWorld::ConsoleCommand_WaterShow(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Water) {
        ConsoleWrite("Water disabled", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_Water;
    } else {
        ConsoleWrite("Water enabled", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_Water;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowTris (0x00665E20).
// Draws the world in wireframe (CGWorldFrame disables GxMasterEnable_PolygonFill).
int32_t CWorld::ConsoleCommand_ShowTris(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_Wireframe) {
        CWorld::enables &= ~Enables::Enable_Wireframe;
    } else {
        CWorld::enables |= Enables::Enable_Wireframe;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowNormals (0x00665E50).
int32_t CWorld::ConsoleCommand_ShowNormals(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_ShowNormals) {
        CWorld::enables &= ~Enables::Enable_ShowNormals;
    } else {
        CWorld::enables |= Enables::Enable_ShowNormals;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowQuery (0x00665F60).
// Shows collision query facets (CMap::TestQueryAdd, CWorldScene::RenderCollisionDebug).
int32_t CWorld::ConsoleCommand_ShowQuery(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_CollisionDebug) {
        ConsoleWrite("show query disabled.", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_CollisionDebug;
    } else {
        ConsoleWrite("show query enabled.", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_CollisionDebug;
    }

    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_DebugZones (0x00665E80).
// With Enable_ZoneBounds set, the terrain outside the player's zone draws plain white (see ZoneDebugIsInCurrentZone and
// CMapRenderChunk::RenderZoneDebug). 0.5.3 also sent a client message (opcode 10, the on/off byte) asking the server
// for its zone map (server message 11, see ZoneDebugInitialize); that opcode does not map to 3.3.5, so the port
// uses the client's own area data and only the local bit is toggled.
int32_t CWorld::ConsoleCommand_DebugZones(const char* command, const char* arguments) {
    if (CWorld::enables & Enables::Enable_ZoneBounds) {
        ConsoleWrite("Zone boundary visuals disabled", DEFAULT_COLOR);
        CWorld::enables &= ~Enables::Enable_ZoneBounds;
    } else {
        ConsoleWrite("Zone boundary visuals enabled", DEFAULT_COLOR);
        CWorld::enables |= Enables::Enable_ZoneBounds;
    }

    return 1;
}

// Debug feature: normal projection toggle.
// 0.5.3 reference: CWorld::ConsoleCommand_Proj (0x00665DF0).
int32_t CWorld::ConsoleCommand_Proj(const char* command, const char* arguments) {
    GxMasterEnableSet(GxMasterEnable_NormalProjection, !GxMasterEnable(GxMasterEnable_NormalProjection));
    return 1;
}

// Debug feature: detail doodad test (read by CMapChunk detail doodad creation).
// 0.5.3 reference: CWorld::ConsoleCommand_DetailDoodadTest (0x00665FB0).
int32_t CWorld::ConsoleCommand_DetailDoodadTest(const char* command, const char* arguments) {
    if (CWorld::detailDoodadTest) {
        ConsoleWrite("Detail doodad debug test disabled.", DEFAULT_COLOR);
        CWorld::detailDoodadTest = 0;
    } else {
        ConsoleWrite("Detail doodad debug test enabled.", DEFAULT_COLOR);
        CWorld::detailDoodadTest = 1;
    }

    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_GroupOnly (0x00666040).
// 0.5.3 switched CMapObj::maxRLevel, the portal recursion limit CMapObj::RRenderThruPortals tests (level <=
// maxRLevel), between 0, the camera's group only, and CMapObj::DEFAULT_RLEVEL (10). 3.3.5 keeps the limit as
// s_maxPortalDepth (CMapObj::RenderThruPortals returns past it), also 10.
static const uint32_t DEFAULT_PORTAL_DEPTH = 10;

int32_t CWorld::ConsoleCommand_GroupOnly(const char* command, const char* arguments) {
    if (s_maxPortalDepth == DEFAULT_PORTAL_DEPTH) {
        s_maxPortalDepth = 0;
        ConsoleWrite("Rendering only current group", DEFAULT_COLOR);
    } else {
        s_maxPortalDepth = DEFAULT_PORTAL_DEPTH;
        ConsoleWrite("Rendering all visible groups (standard)", DEFAULT_COLOR);
    }

    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_MapObjLightMode (0x00665B10).
// 0.5.3 toggled enables bit 0x400 ("MapObj vertex light enabled." / "MapObj lightmaps enabled."), but nothing in 0.5.3
// reads that bit, and 3.3.5 reuses it as Enable_Footprints. The port keeps the switch and its messages on a flag of
// its own, so the command has no visible effect, as in 0.5.3, and leaves the footprints alone.
static bool s_mapObjVertexLight;

int32_t CWorld::ConsoleCommand_MapObjLightMode(const char* command, const char* arguments) {
    s_mapObjVertexLight = !s_mapObjVertexLight;
    ConsoleWrite(s_mapObjVertexLight ? "MapObj vertex light enabled." : "MapObj lightmaps enabled.", DEFAULT_COLOR);
    return 1;
}

// Debug feature: world console commands, the water settings.
// 0.5.3 references: CWorld::ConsoleCommand_WaterMaxLOD (0x00665CF0), ConsoleCommand_WaterWaves (0x00665D40),
// ConsoleCommand_WaterSpecular (0x00665D60). They read a number into CMapArea::ccWaterMaxLOD (clamped to 0..4),
// ccWaterWaves and ccWaterSpecular, and nothing in 0.5.3 reads those three: they had no effect there either. The
// 3.3.5 liquid system has nothing like them. The port keeps the values, with 0.5.3's clamp, and has no effect.
static int32_t s_waterMaxLOD;
static int32_t s_waterWaves;
static int32_t s_waterSpecular;

int32_t CWorld::ConsoleCommand_WaterMaxLOD(const char* command, const char* arguments) {
    s_waterMaxLOD = SStrToInt(arguments ? arguments : "0");
    s_waterMaxLOD = s_waterMaxLOD < 0 ? 0 : (s_waterMaxLOD > 4 ? 4 : s_waterMaxLOD);
    return 1;
}

int32_t CWorld::ConsoleCommand_WaterWaves(const char* command, const char* arguments) {
    s_waterWaves = SStrToInt(arguments ? arguments : "0");
    return 1;
}

int32_t CWorld::ConsoleCommand_WaterSpecular(const char* command, const char* arguments) {
    s_waterSpecular = SStrToInt(arguments ? arguments : "0");
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_EnumTextures (0x00666150).
// 0.5.3 wrote a <name>_<timestamp>.log with TextureLogTextures and CMap::GetTextureUseage; no 3.3.5 counterparts identified yet.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_EnumTextures(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_EnumTextureGxCache (0x00666210).
// 0.5.3 wrote a <name>_<timestamp>.log with TextureLogGxCache; no 3.3.5 counterpart identified yet.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_EnumTextureGxCache(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: zone debug (debugZones, Enable_ZoneBounds).
// 0.5.3 reference: ZoneDebugInitialize (0x0060F1A0); 0.9.1 ZoneDebugInitialize (0x00423E28).
// 0.5.3 and 0.9.1 cleared a 256 x 256 byte map of zone numbers, one per world prox-map cell, and registered the
// handler of server message 11, which filled it. 3.3.5 has neither the message nor the map: ZoneDebugIsInCurrentZone
// uses the client's own area data instead, so there is nothing to initialize and nothing calls this.
void ZoneDebugInitialize() {
}

// Port: the top-level zone of an area (its last m_parentAreaID ancestor), or 0 for no area.
static int32_t ZoneDebugTopZone(int32_t areaId) {
    // Bounded, in case of a parent loop in the data.
    for (int32_t depth = 0; areaId && depth < 8; depth++) {
        const AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

        if (!area || !area->m_parentAreaID) {
            break;
        }

        areaId = area->m_parentAreaID;
    }

    return areaId;
}

// 0.5.3 reference: ZoneDebugIsInCurrentZone (0x0060F290); 0.9.1 ZoneDebugIsInCurrentZone (0x002B483C).
// 0.9.1 compared the zone numbers of the server map at (x, y) and at the active player's position, and returned true
// without a player. Port: compares the top-level zone of the terrain area at (x, y) (the MCNK area id, through
// CMap::QueryAreaIdTerrain) with that of the player's zone (CGGameUI::m_areaID, from CGGameUI::NewZoneFeedback). It
// returns true when either is unknown (no player zone yet, an unloaded chunk, a chunk without area), so that only the
// terrain known to be in another zone turns white.
bool ZoneDebugIsInCurrentZone(float x, float y) {
    int32_t playerZone = ZoneDebugTopZone(CGGameUI::m_areaID);

    if (!playerZone) {
        return true;
    }

    C3Vector position(x, y, 0.0f);
    int32_t zone = ZoneDebugTopZone(CMap::QueryAreaIdTerrain(&position));
    return !zone || zone == playerZone;
}

// Debug feature: registers the 0.5.3 CWorld::Initialize console commands missing from 3.3.5.
void CWorld::DebugRegisterConsoleCommands() {
    ConsoleCommandRegister("mapObjLightMode", CWorld::ConsoleCommand_MapObjLightMode, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterShow", CWorld::ConsoleCommand_WaterShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showWater", CWorld::ConsoleCommand_WaterShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterMaxLOD", CWorld::ConsoleCommand_WaterMaxLOD, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterWaves", CWorld::ConsoleCommand_WaterWaves, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterSpecular", CWorld::ConsoleCommand_WaterSpecular, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("enumTextures", CWorld::ConsoleCommand_EnumTextures, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("enumTextureGxCache", CWorld::ConsoleCommand_EnumTextureGxCache, CATEGORY_GRAPHICS, nullptr);

    // 0.5.3 compiled these handlers but never registered them, so no command name survives;
    // the names below are made up from the handler names.
    ConsoleCommandRegister("showTerrain", CWorld::ConsoleCommand_ShowTerrain, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showDoodads", CWorld::ConsoleCommand_ShowDoodads, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showCollision", CWorld::ConsoleCommand_ShowCollision, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showAABoxes", CWorld::ConsoleCommand_ShowAABoxes, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showMapObjBSP", CWorld::ConsoleCommand_ShowMapObjBSP, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("debugBSP", CWorld::ConsoleCommand_DebugBSP, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showMapObjLight", CWorld::ConsoleCommand_ShowMapObjLight, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showMapObjTex", CWorld::ConsoleCommand_ShowMapObjTex, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showCrappyBatches", CWorld::ConsoleCommand_ShowCrappyBatches, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showMapObjs", CWorld::ConsoleCommand_ShowMapObjs, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showPortals", CWorld::ConsoleCommand_ShowPortals, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("portalVis", CWorld::ConsoleCommand_PortalVis, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showTris", CWorld::ConsoleCommand_ShowTris, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showNormals", CWorld::ConsoleCommand_ShowNormals, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showQuery", CWorld::ConsoleCommand_ShowQuery, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("debugZones", CWorld::ConsoleCommand_DebugZones, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("proj", CWorld::ConsoleCommand_Proj, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("detailDoodadTest", CWorld::ConsoleCommand_DetailDoodadTest, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("groupOnly", CWorld::ConsoleCommand_GroupOnly, CATEGORY_GRAPHICS, nullptr);
}

#endif
