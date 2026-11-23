#include "WowEdit/Source/App/WowEditWorldView.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/TextureCache.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"

// See WowEditWorldView.h.

static CSimpleTop* s_top;
static CGWorldFrame* s_worldFrame;
static HLAYER s_layer;
static int32_t s_mapId = -1;
static int32_t s_timeOfDay = 12 * 60;

// Camera state: yaw around Z from +X (north), pitch up from the horizon, in radians.
static float s_yaw;
static float s_pitch;
static float s_speed = 30.0f; // yards per second
static bool s_looking;
// Movement keys for an AZERTY keyboard (the keys arrive as the layout names them, not by position).
static bool s_azerty;
// Drop the camera onto the ground once the terrain under it has loaded (a map opened without a position).
static bool s_dropToGround;

// Movement keys held.
enum MOVE_KEY {
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_FAST,
    NUM_MOVE_KEYS,
};

static bool s_moveKeys[NUM_MOVE_KEYS];

// Northshire Abbey, the default start on map 0.
static const C3Vector NORTHSHIRE(-8949.95f, -132.49f, 103.53f);
static const float NORTHSHIRE_YAW = 0.0f;
static const float LOOK_SPEED = 0.004f; // radians per relative mouse unit
static const float MAX_PITCH = 1.55f;
static const float DROP_HEIGHT = 40.0f; // yards above the ground after a drop

static CGCamera* Camera() {
    return s_worldFrame ? s_worldFrame->m_camera : nullptr;
}

static C3Vector ForwardVector() {
    float c = cosf(s_pitch);
    return C3Vector(c * cosf(s_yaw), c * sinf(s_yaw), sinf(s_pitch));
}

static void ApplyFacing() {
    CGCamera* camera = Camera();

    if (camera) {
        camera->SetFacing(ForwardVector(), C3Vector(0.0f, 0.0f, 1.0f));
    }
}

// ---- Input

// QWERTY: W A S D to move, E up, Q down. AZERTY, the same places: Z Q S D to move, E up, A down.
static int32_t MoveKeyFor(KEY key) {
    if (s_azerty) {
        switch (key) {
            case KEY_Z:
                return MOVE_FORWARD;
            case KEY_Q:
                return MOVE_LEFT;
            case KEY_A:
                return MOVE_DOWN;
            case KEY_W:
                return -1;
            default:
                break;
        }
    }

    switch (key) {
        case KEY_W:
        case KEY_UP:
            return MOVE_FORWARD;
        case KEY_S:
        case KEY_DOWN:
            return MOVE_BACK;
        case KEY_A:
        case KEY_LEFT:
            return MOVE_LEFT;
        case KEY_D:
        case KEY_RIGHT:
            return MOVE_RIGHT;
        case KEY_E:
            return MOVE_UP;
        case KEY_Q:
            return MOVE_DOWN;
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            return MOVE_FAST;
        default:
            return -1;
    }
}

static int32_t OnKeyDown(const void* data, void* param) {
    int32_t move = MoveKeyFor(static_cast<const EVENT_DATA_KEY*>(data)->m_key);

    if (move >= 0) {
        s_moveKeys[move] = true;
        return 0;
    }

    return 1;
}

static int32_t OnKeyUp(const void* data, void* param) {
    int32_t move = MoveKeyFor(static_cast<const EVENT_DATA_KEY*>(data)->m_key);

    if (move >= 0) {
        s_moveKeys[move] = false;
        return 0;
    }

    return 1;
}

static void StopLooking() {
    if (s_looking) {
        s_looking = false;
        EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
        g_theGxDevicePtr->CursorSetVisible(1);
    }
}

// Losing the focus (a palette, another program) loses the key-up events too.
static int32_t OnFocus(const void* data, void* param) {
    if (!static_cast<const EVENT_DATA_FOCUS*>(data)->m_focus) {
        for (bool& held : s_moveKeys) {
            held = false;
        }

        StopLooking();
    }

    return 1;
}

static int32_t OnMouseDown(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouse = static_cast<const EVENT_DATA_MOUSE*>(data);

    if (mouse->m_button == MOUSE_BUTTON_RIGHT) {
        s_looking = true;
        EventSetMouseMode(MOUSE_MODE_RELATIVE, MOUSE_BUTTON_RIGHT);
        // As the game turning its camera (InputControl): no cursor while the mouse turns the view.
        g_theGxDevicePtr->CursorSetVisible(0);
        return 0;
    }

    return 1;
}

static int32_t OnMouseUp(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouse = static_cast<const EVENT_DATA_MOUSE*>(data);

    if (mouse->m_button == MOUSE_BUTTON_RIGHT) {
        StopLooking();
        return 0;
    }

    return 1;
}

static int32_t OnMouseMove(const void* data, void* param) {
    // The brush cursor (WowEditTerrain) reads the mouse position the way it does in the game, from the world frame's
    // CSimpleTop, whose own handlers are off here.
    if (s_top) {
        s_top->m_mousePosition = *static_cast<const EVENT_DATA_MOUSE*>(data);
    }

    return 1;
}

static int32_t OnMouseMoveRelative(const void* data, void* param) {
    if (!s_looking) {
        return 1;
    }

    const EVENT_DATA_MOUSE* mouse = static_cast<const EVENT_DATA_MOUSE*>(data);
    s_yaw -= mouse->m_x * LOOK_SPEED;
    s_pitch -= mouse->m_y * LOOK_SPEED;
    s_pitch = s_pitch > MAX_PITCH ? MAX_PITCH : (s_pitch < -MAX_PITCH ? -MAX_PITCH : s_pitch);
    ApplyFacing();
    return 0;
}

static int32_t OnMouseWheel(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouse = static_cast<const EVENT_DATA_MOUSE*>(data);
    s_speed *= mouse->m_wheelDistance > 0 ? 1.25f : 0.8f;
    s_speed = s_speed < 2.0f ? 2.0f : (s_speed > 2000.0f ? 2000.0f : s_speed);
    ConsolePrintf("WowEdit: camera speed %.0f yards per second", s_speed);
    return 0;
}

// ---- Update and render

static void MoveCamera(float elapsedSec) {
    CGCamera* camera = Camera();

    if (!camera) {
        return;
    }

    C3Vector forward = ForwardVector();
    C3Vector right(sinf(s_yaw), -cosf(s_yaw), 0.0f);
    C3Vector move(0.0f, 0.0f, 0.0f);

    if (s_moveKeys[MOVE_FORWARD]) {
        move += forward;
    }

    if (s_moveKeys[MOVE_BACK]) {
        move -= forward;
    }

    if (s_moveKeys[MOVE_RIGHT]) {
        move += right;
    }

    if (s_moveKeys[MOVE_LEFT]) {
        move -= right;
    }

    if (s_moveKeys[MOVE_UP]) {
        move.z += 1.0f;
    }

    if (s_moveKeys[MOVE_DOWN]) {
        move.z -= 1.0f;
    }

    float step = s_speed * elapsedSec * (s_moveKeys[MOVE_FAST] ? 4.0f : 1.0f);
    camera->m_position += move * step;
}

// A map opened without a position: once the terrain under the camera has loaded, set the camera above it.
static void DropToGround() {
    CGCamera* camera = Camera();
    C3Vector start(camera->m_position.x, camera->m_position.y, 5000.0f);
    C3Vector end(camera->m_position.x, camera->m_position.y, -5000.0f);
    C3Vector hit;
    float distance = 1.0f;

    if (World::Intersect(start, end, &hit, &distance, WORLD_QUERY_TERRAIN, nullptr)) {
        camera->m_position.z = hit.z + DROP_HEIGHT;
        s_dropToGround = false;
    }
}

// CGWorldFrame::UpdateDayNight without the game objects: the camera is the light reference, and the time of day is
// the editor's (the game takes it from the server).
static void UpdateDayNight(float elapsedSec) {
    CGCamera* camera = Camera();
    DayNight::DNInfo* info = DayNight::GetInfo();
    info->m_farClip = camera->m_clipRange.m_h;
    info->m_deltaSec = elapsedSec;
    info->m_timeSec = OsGetAsyncTimeMs() * 0.001f;
    info->m_cameraPos = camera->m_position;
    info->m_cameraDir = ForwardVector();
    info->m_lightRefPos = camera->m_position;
    info->m_playerPos = camera->m_position;
    info->m_timeOfDay = s_timeOfDay;
    info->m_dayProgression = s_timeOfDay * (1.0f / 1440.0f);
    info->m_day = 0.0f;

    MapRec* map = g_mapDB.GetRecord(s_mapId);

    if (map && map->m_timeOfDayOverride != -1) {
        float progression = map->m_timeOfDayOverride * (1.0f / 1440.0f);

        if (progression >= 0.0f && progression <= 1.0f) {
            info->m_timeOfDay = 0;
            info->m_dayProgression = progression;
        }
    }

    info->m_weatherIntensity = World::s_weather ? World::s_weather->m_densityBlend * World::s_weather->m_fogIntensity : 0.0f;
}

// The cursor segment the editor overlays use (WorldEditHooks.h), which CGWorldFrame::OnLayerUpdate sets in the game:
// any segment of non-zero length while the cursor is over the view and the camera is not being turned (the brush
// builds its own ray from the mouse position).
static void UpdateCursorSegment() {
    if (s_looking || !s_top) {
        WorldEditSetCursorSegment(nullptr, nullptr);
        return;
    }

    const C3Vector& position = Camera()->m_position;
    C3Vector end = position + ForwardVector();
    WorldEditSetCursorSegment(&position, &end);
}

// CGWorldFrame::OnWorldUpdate, the world part.
static void UpdateWorld(const CRect& window, float elapsedSec) {
    CGCamera* camera = Camera();
    camera->m_clipRange.m_l = World::s_nearClip;
    camera->m_clipRange.m_h = World::farClip;
    camera->SetupWorldProjection(window);
    UpdateDayNight(elapsedSec);

    C3Vector position = camera->m_position;
    C3Vector target = position + ForwardVector();
    CWorld::Update(&position, &target, &position);
}

// CGWorldFrame::OnWorldRender, the world part: no units, name plates, spells or chat bubbles.
static void RenderWorld(float elapsedSec) {
    CGCamera* camera = Camera();

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Multisample, 1);

    if (CWorld::enables & Enables::Enable_Wireframe) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, Gx_Disable);
    }

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, CWorld::s_unk_ADEEE4);

    TextureCacheResetLoadCount();
    CShaderEffect::UpdateProjMatrix();
    CWorld::Render(camera->m_position, elapsedSec);

    CM2Scene* scene = World::s_m2Scene;

    if (scene) {
        scene->SetFloat18(-CWorld::s_unk_CD7758);
        scene->m_passMask = CWorld::m_M2PassMask;
        scene->Draw(M2PASS_0);

        GxuSortDrawList(GxuCat_0, GxuSort_VtxStream, *World::GetCamPos());
        GxuFlushDrawList(GxuCat_0, *World::GetCamPos());
        CWorldScene::RenderDetailDoodads();

        if (World::SceneCamLiquidStatus(nullptr)) {
            CWorldScene::RenderBarriers(elapsedSec);
            scene->Draw(M2PASS_1);
            CWorldScene::RenderWeather();
            CWorldScene::RenderLiquidPass();
            scene->Draw(M2PASS_2);
        } else {
            scene->Draw(M2PASS_2);
            CWorldScene::RenderLiquidPass();
            CWorldScene::RenderWeather();
            CWorldScene::RenderBarriers(elapsedSec);
            scene->Draw(M2PASS_1);
        }

        CWorldScene::RenderParticulate();
        GxuSortDrawList(GxuCat_1, GxuSort_B2F, *World::GetCamPos());
        GxuFlushDrawList(GxuCat_1, *World::GetCamPos());
        DayNight::RenderGlares();

        // The editor overlays (WorldEditHooks.h) and the Gxu debug-draw queue, as CGWorldFrame::OnWorldRender draws them.
        WorldEditRenderOverlays();
        WorldEditRenderTerrainPassLate();
        GxuDebugDrawRender(CWorldScene::s_activeWorldView);
    }

    if (CWorld::enables & Enables::Enable_Wireframe) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, Gx_Enable);
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// The screen layer: the whole frame, as CGWorldFrame::RenderWorld does it from its frame batch.
static void PaintWorld(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    CImVector black;
    black.b = 0x00;
    black.g = 0x00;
    black.r = 0x00;
    black.a = 0xFF;
    GxSceneClear(GxBufferBit_Color | GxBufferBit_Depth, black);

    CGCamera* camera = Camera();

    if (s_mapId < 0 || !camera) {
        return;
    }

    CRect window = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(window);

    if (window.maxX - window.minX <= 0.0f || window.maxY - window.minY <= 0.0f) {
        return;
    }

    // The world frame covers the screen: the brush cursor maps the mouse through its viewport.
    s_worldFrame->m_viewportDDC.minX = 0.0f;
    s_worldFrame->m_viewportDDC.minY = 0.0f;
    s_worldFrame->m_viewportDDC.maxX = NDCToDDCWidth(1.0f);
    s_worldFrame->m_viewportDDC.maxY = NDCToDDCHeight(1.0f);
    s_worldFrame->m_viewportNDC = { 0.0f, 0.0f, 1.0f, 1.0f };

    MoveCamera(elapsedSec);

    if (s_dropToGround) {
        DropToGround();
    }

    UpdateCursorSegment();

    C44Matrix projection = g_theGxDevicePtr->m_projection;
    C44Matrix view;
    GxXformView(view);

    UpdateWorld(window, elapsedSec);
    RenderWorld(elapsedSec);

    g_theGxDevicePtr->XformSetProjection(projection);
    g_theGxDevicePtr->XformSetView(view);
    CShaderEffect::UpdateProjMatrix();
}

// ---- Set-up

void WowEditWorldInitialize() {
    // The world frame's camera reads the camera cvars (cameraView...), registered by the game interface in the game.
    CameraRegisterCVars();

    // The game's cursor (Interface\Cursor\Point.blp), a Gx hardware cursor: the game interface loads it, and
    // without it the Gx window has no cursor over its client area (the Gx window procedure clears the Windows one).
    CursorInitialize();

    // The camera constructor also reads "autoInteract" (CGCamera::UpdateAutoInteractState), a game interface cvar.
    if (!g_cvAutoInteract) {
        g_cvAutoInteract = CVar::Register("autoInteract", "Toggles auto-move to interact target", CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", nullptr, CATEGORY_GAME, false, nullptr, false);
    }

    // A CSimpleFrame registers with the current CSimpleTop. Neither is shown; the top's input handlers are off so the
    // keys and the mouse come to the editor.
    s_top = new (__FILE__, __LINE__) CSimpleTop();
    s_top->DisableEvents();
    s_worldFrame = static_cast<CGWorldFrame*>(CGWorldFrame::Create(nullptr));

    // Above the (empty, hidden) CSimpleTop layer at 1, under the console (6, 7) and the system messages.
    ScrnLayerCreate(nullptr, 2.0f, SCRN_LAYER_FULLSCREEN_VIEWPORT, nullptr, PaintWorld, &s_layer);

    EventRegisterEx(EVENT_ID_KEYDOWN, OnKeyDown, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_KEYUP, OnKeyUp, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_FOCUS, OnFocus, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, OnMouseDown, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEUP, OnMouseUp, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, OnMouseMove, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, OnMouseMoveRelative, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEWHEEL, OnMouseWheel, nullptr, EVENT_PRIORITY_NORMAL);
}

void WowEditWorldShutdown() {
    StopLooking();
    CursorDestroy();

    if (s_mapId >= 0) {
        World::UnloadMap();
        s_mapId = -1;
    }
}

bool WowEditWorldLoadMap(int32_t mapId, const C3Vector* position) {
    MapRec* map = g_mapDB.GetRecord(mapId);
    CGCamera* camera = Camera();

    if (!map || !camera) {
        ConsolePrintf("WowEdit: no map %d in Map.dbc", mapId);
        return false;
    }

    if (s_mapId >= 0) {
        World::UnloadMap();

        if (World::s_weather) {
            World::s_weather->Reset();
        }
    }

    C3Vector start = position ? *position : (mapId == 0 ? NORTHSHIRE : C3Vector(0.0f, 0.0f, 300.0f));
    camera->m_position = start;

    if (!position) {
        s_yaw = mapId == 0 ? NORTHSHIRE_YAW : 0.0f;
        s_pitch = -0.25f;
    }

    ApplyFacing();
    CWorld::LoadMap(map->m_directory, &start, mapId, 1);
    s_mapId = mapId;
    s_dropToGround = false;

    // No position on a map other than Azeroth: the middle of the terrain the map's WDT lists, then the ground.
    if (!position && mapId != 0) {
        int32_t count = 0;
        float sumX = 0.0f;
        float sumY = 0.0f;

        for (int32_t areaY = 0; areaY < MAP_AREAS_PER_AXIS; areaY++) {
            for (int32_t areaX = 0; areaX < MAP_AREAS_PER_AXIS; areaX++) {
                if (CMap::m_areaInfo[CMap::GetAreaIndex(areaX, areaY)].m_flags & SMAreaInfo::F_HAS_ADT) {
                    sumX += areaX;
                    sumY += areaY;
                    count++;
                }
            }
        }

        if (count) {
            // CMap::PrepareArea: an area's corner is HALF_WORLD_SIZE minus its index times the area size, Y index
            // along world X and X index along world Y.
            const float areaSize = HALF_WORLD_SIZE / 32.0f;
            camera->m_position.x = HALF_WORLD_SIZE - (sumY / count + 0.5f) * areaSize;
            camera->m_position.y = HALF_WORLD_SIZE - (sumX / count + 0.5f) * areaSize;
            camera->m_position.z = 300.0f;
            s_dropToGround = true;
        }
    }

    ConsolePrintf("WowEdit: map %d (%s) at %.1f %.1f %.1f", mapId, map->m_directory, camera->m_position.x, camera->m_position.y, camera->m_position.z);
    return true;
}

int32_t WowEditWorldGetMapId() {
    return s_mapId;
}

void WowEditWorldGetCameraPosition(float* position) {
    CGCamera* camera = Camera();
    position[0] = camera ? camera->m_position.x : 0.0f;
    position[1] = camera ? camera->m_position.y : 0.0f;
    position[2] = camera ? camera->m_position.z : 0.0f;
}

void WowEditWorldSetTimeOfDay(int32_t minutes) {
    s_timeOfDay = ((minutes % 1440) + 1440) % 1440;
}

int32_t WowEditWorldGetTimeOfDay() {
    return s_timeOfDay;
}

void WowEditWorldSetAzerty(bool azerty) {
    s_azerty = azerty;

    for (bool& held : s_moveKeys) {
        held = false;
    }
}

bool WowEditWorldGetAzerty() {
    return s_azerty;
}

#endif
