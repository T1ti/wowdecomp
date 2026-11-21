#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Ui/GameUI.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

static CVar* s_cvDebugTargetPath;

// Debug feature: console command "CombatDebugForceActionOn".
// 0.5.3 reference: CCommand_ForceActionSet (0x005E17C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionSet(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOff".
// 0.5.3 reference: CCommand_ForceActionUnset (0x005E1920).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionUnset(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOtherOn".
// 0.5.3 reference: CCommand_ForceActionOnOtherSet (0x005E1970).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionOnOtherSet(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugForceActionOtherOff".
// 0.5.3 reference: CCommand_ForceActionOnOtherUnset (0x005E19C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionOnOtherUnset(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "CombatDebugShowFlags".
// 0.5.3 reference: CCommand_ForceActionShowFlags (0x005E1A00).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceActionShowFlags(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "DumpDeathHoldLogs".
// 0.5.3 reference: CCommand_DumpDeathHoldLogs (0x005E1530).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_DumpDeathHoldLogs(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "playercombatlogdebug".
// 0.5.3 reference: DebugCombatLogHandler (0x0061D670).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t DebugCombatLogHandler(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: combat force-action flags display.
// 0.5.3 reference: ShowForceActionFlags (0x005DE6F0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static void ShowForceActionFlags() {
}

// Debug feature: combat debug log.
// 0.5.3 reference: UnitCombatDebugLogEnable (0x0061D730).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void UnitCombatDebugLogEnable(BOOL enable) {
}

// 0.5.3 reference: UnitDebugCombatLogOnEnable (0x0061CD50).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void UnitDebugCombatLogOnEnable(BOOL enable) {
}

// Debug feature: unit pathing display.
// 0.5.3 reference: CGUnit_C::RenderDebugPathing (0x005FD3E0).
//
// 0.5.3 drew the path the server sent for debugging (m_debugPathPoints, m_numDebugPathNodes, from a debug opcode): a
// green solid sphere of radius 0.5 (ModelCreateSolidSphere) at each path node and a white line strip through the
// remaining points, while the unit followed a spline; otherwise a green ground square at the server location
// (m_serverLoc) and red squares at the path points (ProjectTex2d).
//
// 3.3.5 has neither the debug opcode nor m_serverLoc, but every unit following a server path keeps it as its movement
// spline (CMoveSpline, the points of SMSG_MONSTER_MOVE). So this draws that spline the 0.5.3 way: green spheres at the
// spline's points and a white line strip through them, plus a yellow line from the unit to the spline's final
// destination. Spline points of a unit on a transport are transport-local and are moved to world space with the
// transport matrix. There is nothing to draw while the unit has no active spline (the 0.5.3 server-location branch).
void CGUnit_C::RenderDebugPathing() {
    CMoveSpline* spline = this->m_movementData.m_spline;

    if (!spline || (spline->m_flags & SPLINEFLAG_DONE)) {
        return;
    }

    C3Spline_CatmullRom& path = spline->m_spline;
    uint32_t count = path.m_pointCount;

    if (!count) {
        return;
    }

    C44Matrix transport;
    bool onTransport = this->m_movementData.m_transportGuid && MovementGetTransportMtxSafe(this->m_movementData.m_transportGuid, &transport);

    CImVector green(0x00, 0xFF, 0x00, 0xFF);
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    CImVector yellow(0x00, 0xFF, 0xFF, 0xFF);
    C3Vector previous;

    for (uint32_t i = 0; i < count; i++) {
        C3Vector point = path.Point(i);

        if (onTransport) {
            point = point * transport;
        }

        C44Matrix sphere;
        sphere.a0 = 0.5f;
        sphere.b1 = 0.5f;
        sphere.c2 = 0.5f;
        sphere.d0 = point.x;
        sphere.d1 = point.y;
        sphere.d2 = point.z;
        GxuDebugDrawShape(GXUDEBUGSHAPE_SPHERE, sphere, green, GXUDEBUGSTYLE_SOLID);

        if (i) {
            GxuDebugDrawLine(previous, point, white, GXUDEBUGSTYLE_SOLID);
        }

        previous = point;
    }

    C3Vector destination = spline->m_finalDestination;

    if (onTransport) {
        destination = destination * transport;
    }

    GxuDebugDrawLine(this->GetPosition(), destination, yellow, GXUDEBUGSTYLE_SOLID);
}

// Port: the 0.5.3 CGObject_C::PreAnimate condition, called from CGUnit_C::PreAnimate. With DebugTargetPath set, 0.5.3
// drew the path of the locked target, and of the active player (when not on a vehicle-like state it also checked).
void CGUnit_C::DebugPathingPreAnimate() {
    if (!s_cvDebugTargetPath || !s_cvDebugTargetPath->m_intValue) {
        return;
    }

    WOWGUID guid = this->m_obj->m_guid;

    if (guid == CGGameUI::m_lockedTarget || guid == ClntObjMgrGetActivePlayer()) {
        this->RenderDebugPathing();
    }
}

// Debug feature: combat hit-roll debug.
// 0.5.3 reference: CGUnit_C::SetDebugHitRolls (0x00622A40).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CGUnit_C::SetDebugHitRolls(const ATTACKROUNDINFO& info) {
}

// 0.5.3 reference: CGUnit_C::GetDebugStateInfo (0x005FC400).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CGUnit_C::GetDebugStateInfo(ATTACKROUNDINFO* info) {
    return 0;
}

// Debug feature: game object debug status.
// 0.5.3 reference: CGGameObject_C::GetDebugStatus (0x005EF930).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
const char* CGGameObject_C::GetDebugStatus() const {
    return nullptr;
}

// Debug feature: player bounding sphere display.
// 0.5.3 reference: CGPlayer_C::TogglePlayerBounds (0x005DCE60).
// Flips boundsPresent and shows or hides the bounding sphere on the active player's model (CM2Model::ShowBoundingSphere
// / HideBounds, drawn through the Gxu debug-draw module).
static bool s_boundsPresent; // 0.5.3 boundsPresent

void CGPlayer_C::TogglePlayerBounds() {
    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_PLAYER, __FILE__, __LINE__);
    CM2Model* model = player ? player->GetObjectModel() : nullptr;

    if (!model) {
        return;
    }

    bool wasPresent = s_boundsPresent;
    s_boundsPresent = !s_boundsPresent;

    if (wasPresent) {
        model->HideBounds();
    } else {
        model->ShowBoundingSphere();
    }
}

static CVar* s_cvShowEffectsStandalone;

// Debug feature: registers the 0.5.3 PlayerClientInitialize / UnitCombatLogInitialize debug commands and the
// CGObject_C / UnitEffects debug CVars.
void UnitDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("CombatDebugForceActionOn", CCommand_ForceActionSet, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOff", CCommand_ForceActionUnset, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOtherOn", CCommand_ForceActionOnOtherSet, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugForceActionOtherOff", CCommand_ForceActionOnOtherUnset, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("CombatDebugShowFlags", CCommand_ForceActionShowFlags, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("DumpDeathHoldLogs", CCommand_DumpDeathHoldLogs, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("playercombatlogdebug", DebugCombatLogHandler, CATEGORY_DEBUG, nullptr);
    // 0.5.3 registered DebugTargetPath from CGObject_C::Initialize and showEffectsStandalone from UnitEffectsInitialize.
    s_cvDebugTargetPath = CVar::Register("DebugTargetPath", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
    s_cvShowEffectsStandalone = CVar::Register("showEffectsStandalone", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
}

#endif
