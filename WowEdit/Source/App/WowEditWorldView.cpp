#include "WowEdit/Source/App/WowEditWorldView.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cmath>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/TextureCache.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include "WoW/Source/WorldClient/WorldEditHooks.h"
#include "WowEdit/Source/App/WowEditAreaEdits.h"
#include "WowEdit/Source/App/WowEditMapSession.h"
#include "WowEdit/Source/App/WowEditRenderDistance.h"
#include "WowEdit/Source/WowEdit.h"

// See WowEditWorldView.h.

static WorldEditView s_view;
static bool s_hasFocus = true;
static HLAYER s_layer;
static CWowEditMapSession s_map;
static CWowEditAreaEdits* s_areaEdits;
static int32_t s_timeOfDay = 12 * 60;

// Camera state: yaw around Z from +X (north), pitch up from the horizon, in radians.
static float s_yaw;
static float s_pitch;
static float s_speed = 30.0f; // yards per second
static bool s_looking;
static void (*s_setViewportCursor)(bool looking);
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
    return s_view.m_camera;
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
        if (s_setViewportCursor) {
            s_setViewportCursor(s_looking);
        }
    }
}

// Losing the focus (a palette, another program) loses the key-up events too.
static int32_t OnFocus(const void* data, void* param) {
    s_hasFocus = static_cast<const EVENT_DATA_FOCUS*>(data)->m_focus != 0;
    if (!s_hasFocus) {
        s_areaEdits->CancelPicks();
        for (bool& held : s_moveKeys) {
            held = false;
        }

        StopLooking();
    }

    return 1;
}

static int32_t OnMouseDown(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouse = static_cast<const EVENT_DATA_MOUSE*>(data);

    if (mouse->m_button == MOUSE_BUTTON_LEFT && s_hasFocus && !s_looking && s_areaEdits->IsPainting()) {
        s_areaEdits->QueuePick(mouse->m_x, mouse->m_y, (mouse->m_metaKeyState & META_KEY_CONTROL) != 0);
        return 0;
    }

    if (mouse->m_button == MOUSE_BUTTON_RIGHT) {
        s_areaEdits->CancelPicks();
        s_looking = true;
        EventSetMouseMode(MOUSE_MODE_RELATIVE, MOUSE_BUTTON_RIGHT);
        // As the game turning its camera (InputControl): no cursor while the mouse turns the view.
        if (s_setViewportCursor) {
            s_setViewportCursor(s_looking);
        }
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

static bool FindGroundBelow(const C3Vector& position, C3Vector& hit) {
    C3Vector start(position.x, position.y, 5000.0f);
    C3Vector end(position.x, position.y, -5000.0f);
    float distance = 1.0f;
    return World::Intersect(start, end, &hit, &distance, WORLD_QUERY_TERRAIN, nullptr) != 0;
}

// A map opened without a position: once the terrain under the camera has loaded, set the camera above it.
static void DropToGround() {
    CGCamera* camera = Camera();
    C3Vector hit;

    if (FindGroundBelow(camera->m_position, hit)) {
        camera->m_position.z = hit.z + DROP_HEIGHT;
        s_dropToGround = false;
    }
}

// CGWorldFrame::UpdateDayNight without the game objects: the camera is the light reference, and the time of day is
// the editor's (the game takes it from the server).
static void UpdateDayNight(const C3Vector& forward, float elapsedSec) {
    CGCamera* camera = Camera();
    DayNight::DNInfo* info = DayNight::GetInfo();
    info->m_farClip = camera->m_clipRange.m_h;
    info->m_deltaSec = elapsedSec;
    info->m_timeSec = OsGetAsyncTimeMs() * 0.001f;
    info->m_cameraPos = camera->m_position;
    info->m_cameraDir = forward;
    info->m_lightRefPos = camera->m_position;
    info->m_playerPos = camera->m_position;
    info->m_timeOfDay = s_timeOfDay;
    info->m_dayProgression = s_timeOfDay * (1.0f / 1440.0f);
    info->m_day = 0.0f;

    const MapRec* map = s_map.GetMap();

    if (map && map->m_timeOfDayOverride != -1) {
        float progression = map->m_timeOfDayOverride * (1.0f / 1440.0f);

        if (progression >= 0.0f && progression <= 1.0f) {
            info->m_timeOfDay = 0;
            info->m_dayProgression = progression;
        }
    }

    info->m_weatherIntensity = World::s_weather ? World::s_weather->m_densityBlend * World::s_weather->m_fogIntensity : 0.0f;
}

bool WowEditWorldGetBrushRay(C3Vector& start, C3Vector& end) {
    if (!s_hasFocus || s_looking || !s_map.GetMap()) {
        return false;
    }

    const float width = NDCToDDCWidth(1.0f);
    const float height = NDCToDDCHeight(1.0f);
    if (width <= 0.0f || height <= 0.0f) {
        return false;
    }

    float x = -1.0f;
    float y = -1.0f;
    EventInputGetMousePosition(&x, &y);
    x /= width;
    y /= height;
    if (!(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f)) {
        return false;
    }

    // Terrain passes use the same camera-relative projection as the world this frame.
    CameraGetLineSegment(x, y, &start, &end);
    start += CWorldScene::s_activeWorldView;
    end += CWorldScene::s_activeWorldView;
    return true;
}

static WorldStreamingView UpdateViewInputs(const C3Vector& forward) {
    CGCamera& camera = *Camera();
    const C3Vector& position = camera.m_position;
    const float base = World::farClip;
    const float farZ = camera.m_clipRange.m_h;
    WorldStreamingView streaming = { &camera, base, position.z - base };
    s_view.m_horizonFarClip = World::s_horizonFarClipScale * base + fmaxf(farZ - base, 0.0f);
    s_view.m_terrainBounds.minX = position.x - base;
    s_view.m_terrainBounds.minY = position.y - base;
    s_view.m_terrainBounds.maxX = position.x + base;
    s_view.m_terrainBounds.maxY = position.y + base;

    const C3Vector right = camera.Right();
    const C3Vector up = camera.Up();
    const float halfHeight = tanf(camera.FOV() * 0.3f);
    const float halfWidth = halfHeight * camera.m_aspect;
    // At unit depth the corner rays bound the whole frustum horizontally. Start the WDL behind
    // the detailed pass before those rays can leave the horizontal terrain square.
    const float reachX = fabsf(forward.x) + halfWidth * fabsf(right.x) + halfHeight * fabsf(up.x);
    const float reachY = fabsf(forward.y) + halfWidth * fabsf(right.y) + halfHeight * fabsf(up.y);
    const float edgeDepth = base / fmaxf(reachX, reachY);
    s_view.m_horizonNearClip = fmaxf(camera.m_clipRange.m_l, fminf(farZ, edgeDepth) - 50.0f);

    if (farZ > base) {
        // The free camera has no roll. Project the bottom of its far plane onto world Z for WMO queries.
        float bottomZ = position.z + farZ * (forward.z - halfHeight * fabsf(up.z));
        streaming.m_objectMinZ = fminf(streaming.m_objectMinZ, bottomZ);
    }

    // Budget for the whole draw square and a paired chunk at each edge.
    uint32_t radius = 1 + static_cast<uint32_t>(base * OO_COORD_TO_CHUNK);
    uint32_t diameter = 2 * radius + 4;
    s_view.m_terrainBufferCount = diameter * diameter;
    // The horizontal occlusion buffer is inactive at steep pitches.
    s_view.m_useTerrainOcclusion = fabsf(forward.z) <= 0.9f;
    return streaming;
}

// CGWorldFrame::OnWorldUpdate, the world part.
static void UpdateWorld(const CRect& window, float elapsedSec) {
    CGCamera* camera = Camera();
    C3Vector position = camera->m_position;
    C3Vector forward = ForwardVector();
    camera->m_clipRange.m_l = World::s_nearClip;
    camera->m_clipRange.m_h = World::farClip;

    if (forward.z < 0.0f) {
        const C2Vector xy(position.x, position.y);
        const CMapChunk* chunk = World::IsValidPosition(position, 0.0f) ? CMap::GetChunkAtPosition(&xy) : nullptr;
        float ground = chunk ? chunk->m_bbox2.b.z : 0.0f;
        // Clip distances are along the view axis. Keep the base range beyond the depth spent reaching the ground.
        float groundDepth = fmaxf(position.z - ground, 0.0f) * -forward.z;
        camera->m_clipRange.m_h += groundDepth;
        // Recover depth precision for terrain and overlays when the camera is well above the ground.
        camera->m_clipRange.m_l = fmaxf(World::s_nearClip, fminf(groundDepth * 0.002f, 4.0f));
    }

    camera->SetupWorldProjection(window);
    s_areaEdits->ApplyPending(position);
    const WorldStreamingView streaming = UpdateViewInputs(forward);
    UpdateDayNight(forward, elapsedSec);

    C3Vector target = position + forward;
    CWorld::Update(&position, &target, &position, &streaming);
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
    CWorldScene::Render(camera->m_position, elapsedSec, camera);

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

        if (WowEditGetMarkers()) {
            C3Vector ground;
            if (FindGroundBelow(camera->m_position, ground)) {
                WowEditDrawSampleMarkers(ground, s_yaw);
            }
        }

        if (s_areaEdits->IsPainting()) {
            const int32_t selectedArea = s_areaEdits->SelectedArea();
            char status[112];
            if (selectedArea) {
                SStrPrintf(status, sizeof(status), "Area %d: Click to paint a chunk. Ctrl-click to sample.", selectedArea);
            } else {
                SStrCopy(status, "Area paint: Ctrl-click terrain to sample an area.", sizeof(status));
            }
            GxuDebugDrawText(status, 0.02f, 0.14f, CImVector(0xFF, 0xFF, 0xFF, 0xFF));
        }
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

    if (!s_map.GetMap() || !camera) {
        return;
    }

    CRect window = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(window);

    if (window.maxX - window.minX <= 0.0f || window.maxY - window.minY <= 0.0f) {
        return;
    }

    MoveCamera(elapsedSec);

    if (s_dropToGround) {
        DropToGround();
    }

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

void WowEditWorldInitialize(CWowEditAreaEdits& areaEdits, void (*setViewportCursor)(bool looking)) {
    s_areaEdits = &areaEdits;
    s_setViewportCursor = setViewportCursor;

    // CGCamera reads the camera cvars (cameraView...) normally registered by the game interface.
    CameraRegisterCVars();

    // The camera constructor also reads "autoInteract" (CGCamera::UpdateAutoInteractState), a game interface cvar.
    if (!g_cvAutoInteract) {
        g_cvAutoInteract = CVar::Register("autoInteract", "Toggles auto-move to interact target", CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", nullptr, CATEGORY_GAME, false, nullptr, false);
    }

    s_view.m_camera = new (__FILE__, __LINE__) CGCamera();
    const WowEditDistanceInfo& viewDistance = WowEditGetDistanceInfo(WOWEDITDISTANCE_VIEW);
    s_view.m_minFarClip = viewDistance.min;
    s_view.m_maxFarClip = viewDistance.max;
    s_view.m_allowPendingMapAssets = true;
    WorldEditSetView(&s_view);

    // Under the console (6, 7) and the system messages.
    ScrnLayerCreate(nullptr, 2.0f, SCRN_LAYER_FULLSCREEN_VIEWPORT, nullptr, PaintWorld, &s_layer);

    EventRegisterEx(EVENT_ID_KEYDOWN, OnKeyDown, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_KEYUP, OnKeyUp, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_FOCUS, OnFocus, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, OnMouseDown, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEUP, OnMouseUp, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, OnMouseMoveRelative, nullptr, EVENT_PRIORITY_NORMAL);
    EventRegisterEx(EVENT_ID_MOUSEWHEEL, OnMouseWheel, nullptr, EVENT_PRIORITY_NORMAL);
}

void WowEditWorldShutdown() {
    StopLooking();
    s_setViewportCursor = nullptr;
    s_areaEdits = nullptr;

    s_map.Close();

    WorldEditSetView(nullptr);
    delete s_view.m_camera;
    s_view.m_camera = nullptr;
}

int32_t WowEditWorldGetAreaId(const CMapChunk& chunk) {
    return s_areaEdits->AreaId(chunk);
}

bool WowEditWorldLoadMap(int32_t mapId, const C3Vector* position) {
    CGCamera* camera = Camera();
    if (!camera) {
        return false;
    }

    const MapRec* map = CWowEditMapSession::FindAvailableMap(mapId);
    if (!map) {
        return false;
    }

    s_map.Close();

    C3Vector start = position ? *position : (mapId == 0 ? NORTHSHIRE : C3Vector(0.0f, 0.0f, 300.0f));
    camera->m_position = start;

    if (!position) {
        s_yaw = mapId == 0 ? NORTHSHIRE_YAW : 0.0f;
        s_pitch = -0.25f;
    }

    ApplyFacing();
    camera->m_clipRange.m_l = World::s_nearClip;
    camera->m_clipRange.m_h = World::farClip;
    const WorldStreamingView streaming = UpdateViewInputs(ForwardVector());
    s_map.Open(*map, start, streaming);
    s_dropToGround = false;

    // No position on a map other than Azeroth: the middle of the terrain the map's WDT lists, then the ground.
    if (!position && mapId != 0) {
        int32_t count = 0;
        float sumX = 0.0f;
        float sumY = 0.0f;

        for (int32_t areaY = 0; areaY < MAP_AREAS_PER_AXIS; areaY++) {
            for (int32_t areaX = 0; areaX < MAP_AREAS_PER_AXIS; areaX++) {
                if (CMap::m_areaInfo[CMap::GetAreaIndex(areaX, areaY)].m_flags & SMAreaInfo::FLAG_EXISTS) {
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
    const MapRec* map = s_map.GetMap();
    return map ? map->m_id : -1;
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

bool WowEditWorldIsLooking() {
    return s_looking;
}

#endif
