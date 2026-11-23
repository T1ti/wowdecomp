#include "WowEdit/Source/App/WowEditViewToggles.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditViewToggles.h.

// CWorld::enables as the game left it after start-up (cvars applied).
static uint32_t s_gameEnables;
static bool s_fog = true;

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

// ---- Other switches

static bool GetFog() {
    return s_fog;
}

static void SetFog(bool on) {
    s_fog = on;
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_Fog, on ? Gx_Enable : Gx_Disable);
}

static bool GetSky() {
    return DayNight::g_dnInfo.m_showSky != 0;
}

static void SetSky(bool on) {
    DayNight::g_dnInfo.m_showSky = on;
}

static bool GetGrid() {
    return WowEditTerrainGetGrid() != WOWEDITGRID_OFF;
}

static void SetGrid(bool on) {
    // As "wowEditGrid" with no argument: permutation 3, chunk, tile and cell lines.
    WowEditTerrainSetGrid(on ? 3 : WOWEDITGRID_OFF);

    if (on) {
        WowEditPrepareTerrainOverlays();
    }
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
    { WOWEDITCMD_VIEW_GRID, "Grid", "Turn Grids on/off", GetGrid, SetGrid },
    { WOWEDITCMD_VIEW_AREAS, "Area IDs", "Each terrain chunk in a color of its area ID", GetAreas, SetAreas },
    { WOWEDITCMD_VIEW_SLOPES, "Impassable", "View impathable polygons based on angle", GetSlopes, SetSlopes },
    { WOWEDITCMD_VIEW_TERRAIN, "Terrain", "Turn on/off terrain", GetEnable<Enables::Enable_Chunks>, SetEnable<Enables::Enable_Chunks> },
    { WOWEDITCMD_VIEW_SHADOWS, "Shadows", "Turn the display of Shadows on/off (terrain shadow maps)", GetEnable<Enables::Enable_Shadow>, SetEnable<Enables::Enable_Shadow> },
    { WOWEDITCMD_VIEW_FOG, "Fog", "Turn Distance Fog on/off", GetFog, SetFog },
    { WOWEDITCMD_VIEW_DOODADS, "Doodads", "Turn Doodads on/off", GetEnable<Enables::Enable_Doodads>, SetEnable<Enables::Enable_Doodads> },
    { WOWEDITCMD_VIEW_DETAIL_DOODADS, "Detail Doodads", "View Detail Doodads", GetEnable<Enables::Enable_DetailDoodads>, SetEnable<Enables::Enable_DetailDoodads> },
    { WOWEDITCMD_VIEW_WMOS, "WMOs", "Show / Hide WMO groups", GetEnable<Enables::Enable_MapObjs>, SetEnable<Enables::Enable_MapObjs> },
    { WOWEDITCMD_VIEW_WATER, "Water", "Turn on/off display of Water", GetEnable<Enables::Enable_Water>, SetEnable<Enables::Enable_Water> },
    { WOWEDITCMD_VIEW_SKY, "Sky", "View Sky", GetSky, SetSky },
    { WOWEDITCMD_VIEW_PARTICULATES, "Particulates", "Underwater particulates", GetEnable<Enables::Enable_Particulates>, SetEnable<Enables::Enable_Particulates> },
    { WOWEDITCMD_VIEW_WIREFRAME, "Wireframe", "Toggle Wireframe Mode", GetEnable<Enables::Enable_Wireframe>, SetEnable<Enables::Enable_Wireframe> },
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
    // The game turns fog on as it enters the world (ClientInitializeGame).
    SetFog(true);
}

void WowEditMatchGameView() {
    CWorld::enables = s_gameEnables;
    SetFog(true);
    SetSky(true);
    SetGrid(false);
    SetSlopes(false);
    SetAreas(false);
    SetMarkers(false);
    WowEditTerrainSetBrush(0.0f, 0.0f);
}

#endif
