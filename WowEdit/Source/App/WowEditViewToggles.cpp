#include "WowEdit/Source/App/WowEditViewToggles.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditViewToggles.h.

// CWorld::enables as the game left it after start-up (cvars applied).
static uint32_t s_gameEnables;
static EShadowMode s_gameShadowMode;
static EShadowMode s_shadowModeBeforeToggle;
static bool s_fog;

// ---- CWorld::enables switches

template <uint32_t FLAG>
static bool GetEnable() {
    return (CWorld::enables & FLAG) != 0;
}

template <uint32_t FLAG>
static void SetEnable(bool on) {
    if (on) {
        CWorld::enables |= FLAG;
    } else {
        CWorld::enables &= ~FLAG;
    }
}

static bool GetShadows() {
    return GetEnable<Enables::Enable_Shadow>() || CShadowCache::s_shadowMode != SHADOW_LOWEST;
}

static void ApplyShadows(bool baked, EShadowMode dynamicMode) {
    const bool refreshTerrain = baked != GetEnable<Enables::Enable_Shadow>();
    SetEnable<Enables::Enable_Shadow>(baked);
    if (CShadowCache::s_shadowMode != dynamicMode) {
        CShadowCache::SetShadowMode(dynamicMode);
    }

    if (refreshTerrain) {
        // The terrain shader texture packs the baked shadow mask with the layer weights.
        for (CMapChunk* chunk = CMap::chunkList.Head(); chunk; chunk = CMap::chunkList.Next(chunk)) {
            if (chunk->m_renderChunk) {
                chunk->m_renderChunk->m_layerFlags |= CMapRenderChunk::RENDER_FLAGS::FLAG_LAYER_TEXTURES_DIRTY;
            }
        }
    }
}

static void SetShadows(bool on) {
    if (!on) {
        s_shadowModeBeforeToggle = CShadowCache::s_shadowMode;
    }

    ApplyShadows(on, on ? s_shadowModeBeforeToggle : SHADOW_LOWEST);
}

// ---- Other switches

static bool GetFog() {
    return s_fog;
}

// The low detail terrain (the map's WDL, drawn past the far clip as the horizon) goes with the fog: it is drawn flat
// and white and the fog is what gives it its color, so without fog it would be a white band where the terrain ends.
static void SetFog(bool on) {
    s_fog = on;
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_Fog, on ? Gx_Enable : Gx_Disable);

    if (on) {
        CWorld::enables |= s_gameEnables & Enables::Enable_LowDetail;
    } else {
        CWorld::enables &= ~Enables::Enable_LowDetail;
    }
}

static bool GetSky() {
    return DayNight::g_dnInfo.m_showSky != 0;
}

static void SetSky(bool on) {
    DayNight::g_dnInfo.m_showSky = on;
}

// The 8.1 woweditgrid permutations as two switches: bit 1 the cell wireframe (permutation 1: every cell with its
// diagonals, the terrain triangles), bit 2 the chunk and map tile lines (permutation 2); both are permutation 3.
static const int32_t GRID_WIREFRAME = 1;
static const int32_t GRID_CHUNKS = 2;

static int32_t GetGridBits() {
    int32_t grid = WowEditTerrainGetGrid();
    return grid > 0 ? grid : 0;
}

static void SetGridBit(int32_t bit, bool on) {
    int32_t grid = on ? GetGridBits() | bit : GetGridBits() & ~bit;
    WowEditTerrainSetGrid(grid ? grid : WOWEDITGRID_OFF);

    if (grid) {
        WowEditPrepareTerrainOverlays();
    }
}

static bool GetGrid() {
    return (GetGridBits() & GRID_CHUNKS) != 0;
}

static void SetGrid(bool on) {
    SetGridBit(GRID_CHUNKS, on);
}

static bool GetWireframe() {
    return (GetGridBits() & GRID_WIREFRAME) != 0;
}

static void SetWireframe(bool on) {
    SetGridBit(GRID_WIREFRAME, on);
}

static bool GetSlopes() {
    return WowEditTerrainGetSlopes();
}

static void SetSlopes(bool on) {
    WowEditTerrainSetSlopes(on);

    if (on) {
        WowEditPrepareTerrainOverlays();
    }
}

static bool GetAreas() {
    return WowEditTerrainGetOverlay() == WOWEDITOVERLAY_AREAS;
}

static void SetAreas(bool on) {
    WowEditTerrainSetOverlay(on ? WOWEDITOVERLAY_AREAS : WOWEDITOVERLAY_OFF);

    if (on) {
        WowEditPrepareTerrainOverlays();
    }
}

static bool GetMarkers() {
    return WowEditGetMarkers();
}

static void SetMarkers(bool on) {
    WowEditSetMarkers(on);
}

static const WowEditViewToggle s_toggles[] = {
    { WOWEDITCMD_VIEW_GRID, "Grid", "Chunk and map tile lines (woweditgrid 2)", GetGrid, SetGrid },
    { WOWEDITCMD_VIEW_AREAS, "Area IDs", "Each terrain chunk in a color of its area ID", GetAreas, SetAreas },
    { WOWEDITCMD_VIEW_SLOPES, "Impassable", "View impathable polygons based on angle", GetSlopes, SetSlopes },
    { WOWEDITCMD_VIEW_TERRAIN, "Terrain", "Turn on/off terrain", GetEnable<Enables::Enable_Chunks>, SetEnable<Enables::Enable_Chunks> },
    { WOWEDITCMD_VIEW_SHADOWS, "Shadows", "Show baked terrain shadows and dynamic shadows", GetShadows, SetShadows },
    { WOWEDITCMD_VIEW_FOG, "Fog", "Turn Distance Fog on/off", GetFog, SetFog },
    { WOWEDITCMD_VIEW_DOODADS, "Doodads", "Turn Doodads on/off", GetEnable<Enables::Enable_Doodads>, SetEnable<Enables::Enable_Doodads> },
    { WOWEDITCMD_VIEW_DETAIL_DOODADS, "Detail Doodads", "View Detail Doodads", GetEnable<Enables::Enable_DetailDoodads>, SetEnable<Enables::Enable_DetailDoodads> },
    { WOWEDITCMD_VIEW_WMOS, "WMOs", "Show / Hide WMO groups", GetEnable<Enables::Enable_MapObjs>, SetEnable<Enables::Enable_MapObjs> },
    { WOWEDITCMD_VIEW_WATER, "Water", "Turn on/off display of Water", GetEnable<Enables::Enable_Water>, SetEnable<Enables::Enable_Water> },
    { WOWEDITCMD_VIEW_SKY, "Sky", "View Sky", GetSky, SetSky },
    { WOWEDITCMD_VIEW_PARTICULATES, "Particulates", "Underwater particulates", GetEnable<Enables::Enable_Particulates>, SetEnable<Enables::Enable_Particulates> },
    { WOWEDITCMD_VIEW_WIREFRAME, "Wireframe", "Terrain cells and their diagonals over the textured ground (woweditgrid 1)", GetWireframe, SetWireframe },
    { WOWEDITCMD_VIEW_NORMALS, "Normals", "View Terrain Normals (3.3.5 draws WMO normals)", GetEnable<Enables::Enable_ShowNormals>, SetEnable<Enables::Enable_ShowNormals> },
#ifdef WOWDECOMP_DEBUG_FEATURES
    { WOWEDITCMD_VIEW_COLLISION, "Collision", "Show / Hide WMO Invisible Collision (the showCollision display)", GetEnable<Enables::Enable_Collision>, SetEnable<Enables::Enable_Collision> },
#endif
    { WOWEDITCMD_VIEW_MARKERS, "Markers", "Sample editor markers (spawn points, paths)", GetMarkers, SetMarkers },
};

const WowEditViewToggle* WowEditGetViewToggles(uint32_t* count) {
    *count = sizeof(s_toggles) / sizeof(s_toggles[0]);
    return s_toggles;
}

const WowEditViewToggle* WowEditFindViewToggle(int32_t command) {
    for (const WowEditViewToggle& toggle : s_toggles) {
        if (toggle.command == command) {
            return &toggle;
        }
    }

    return nullptr;
}

void WowEditViewTogglesInitialize() {
    s_gameEnables = static_cast<uint32_t>(CWorld::enables);
    s_gameShadowMode = CShadowCache::s_shadowMode;
    s_shadowModeBeforeToggle = s_gameShadowMode;
    // The editor starts without the distance fog (and so without the WDL horizon), to see the terrain as it is;
    // Fog or Game View brings both back as the game draws them.
    SetFog(false);
}

void WowEditMatchGameView() {
    s_shadowModeBeforeToggle = s_gameShadowMode;
    ApplyShadows((s_gameEnables & Enables::Enable_Shadow) != 0, s_gameShadowMode);
    CWorld::enables = s_gameEnables;
    SetFog(true);
    SetSky(true);
    SetGrid(false);
    SetWireframe(false);
    SetSlopes(false);
    SetAreas(false);
    SetMarkers(false);
    WowEditBrushSettings brush = WowEditTerrainGetBrush();
    brush.enabled = false;
    WowEditTerrainSetBrush(brush);
}

#endif
