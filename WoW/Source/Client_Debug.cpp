#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cstdio>
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "light".
// 0.5.3 reference: CCommand_ToggleLighting (0x00402F80). Takes 0 or 1.
static int32_t CCommand_ToggleLighting(const char* command, const char* arguments) {
    int32_t enable = SStrToInt(arguments);
    GxMasterEnableSet(GxMasterEnable_Lighting, enable);
    ConsoleWrite(enable ? "Lighting enabled" : "Lighting disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "fog".
// 0.5.3 reference: CCommand_ToggleFog (0x00402FB0).
// Takes 0 or 1.
static int32_t CCommand_ToggleFog(const char* command, const char* arguments) {
    int32_t enable = SStrToInt(arguments);
    GxMasterEnableSet(GxMasterEnable_Fog, enable);
    ConsoleWrite(enable ? "Fog enabled" : "Fog disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "DepthTest".
// 0.5.3 reference: CCommand_ToggleDepthTesting (0x00402FF0).
static int32_t CCommand_ToggleDepthTesting(const char* command, const char* arguments) {
    int32_t enable = !GxMasterEnable(GxMasterEnable_DepthTest);
    GxMasterEnableSet(GxMasterEnable_DepthTest, enable);
    ConsoleWrite(enable ? "Depth Testing enabled" : "Depth Testing disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "DepthSet".
// 0.5.3 reference: CCommand_ToggleDepthSetting (0x00403030).
static int32_t CCommand_ToggleDepthSetting(const char* command, const char* arguments) {
    int32_t enable = !GxMasterEnable(GxMasterEnable_DepthWrite);
    GxMasterEnableSet(GxMasterEnable_DepthWrite, enable);
    ConsoleWrite(enable ? "Depth Writing enabled" : "Depth Writing disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "culling".
// 0.5.3 reference: CCommand_ToggleCulling (0x00403070).
static int32_t CCommand_ToggleCulling(const char* command, const char* arguments) {
    int32_t enable = !GxMasterEnable(GxMasterEnable_Culling);
    GxMasterEnableSet(GxMasterEnable_Culling, enable);
    ConsoleWrite(enable ? "Back Face Culling enabled" : "Back Face Culling disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "dblbuffer".
// 0.5.3 reference: CCommand_ToggleDblBuffer (0x004030B0).
static int32_t CCommand_ToggleDblBuffer(const char* command, const char* arguments) {
    int32_t enable = !GxMasterEnable(GxMasterEnable_DoubleBuffering);
    GxMasterEnableSet(GxMasterEnable_DoubleBuffering, enable);
    ConsoleWrite(enable ? "Double Buffering enabled" : "DoubleBuffering disabled", DEFAULT_COLOR);

    return 1;
}

// Debug feature: console command "resxy".
// 0.5.3 reference: CCommand_SetResolutionXY (0x004030F0).
// Already an empty stub in 0.5.3 (the gx* CVars replaced it), so it stays empty.
static int32_t CCommand_SetResolutionXY(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "resmode".
// 0.5.3 reference: CCommand_SetResolutionMode (0x00403100).
// Already an empty stub in 0.5.3 (the gx* CVars replaced it), so it stays empty.
static int32_t CCommand_SetResolutionMode(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "bitdepth".
// 0.5.3 reference: CCommand_SetColorDepth (0x00403110).
// Already an empty stub in 0.5.3 (the gx* CVars replaced it), so it stays empty.
static int32_t CCommand_SetColorDepth(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "3dapi".
// 0.5.3 reference: CCommand_SetAPI (0x00403120).
// Already an empty stub in 0.5.3 (the gx* CVars replaced it), so it stays empty.
static int32_t CCommand_SetAPI(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "loc".
// 0.5.3 reference: CCommand_Loc (0x00404200).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_Loc(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "dloc".
// 0.5.3 reference: CCommand_DLoc (0x00404320).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_DLoc(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "tloc".
// 0.5.3 reference: CCommand_TargetLoc (0x00404380).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_TargetLoc(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "facing".
// 0.5.3 reference: CCommand_Facing (0x004046A0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_Facing(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "dfacing".
// 0.5.3 reference: CCommand_DFacing (0x00404740).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_DFacing(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "showbounds".
// 0.5.3 reference: CCommand_ShowBounds (0x00404190).
// Toggles the bounding sphere on the tracked object's model (CGGameUI::m_currentObjectTrack, the object under the
// cursor), or the active player's when nothing is tracked; see CM2Model::ShowBoundingSphere / HideBounds.
// Port extension: "showbounds box" toggles the bounding box instead (0.5.3 had ModelShowBoundingBox but no command).
static int32_t CCommand_ShowBounds(const char* command, const char* arguments) {
    CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPE_OBJECT, __FILE__, __LINE__);

    if (!object) {
        object = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_OBJECT, __FILE__, __LINE__);
    }

    CM2Model* model = object ? object->GetObjectModel() : nullptr;

    if (!model) {
        return 0;
    }

    bool box = arguments && !SStrCmpI(arguments, "box", STORM_MAX_STR);

    if (box ? model->IsShowingBoundingBox() : model->IsShowingBoundingSphere()) {
        model->HideBounds();
    } else if (box) {
        model->ShowBoundingBox();
    } else {
        model->ShowBoundingSphere();
    }

    return 1;
}

// Debug feature: console command "showhittest".
// Not in 0.5.3 (which had ModelShowHitTestGeometry but no command); registered by 0.8 and 0.9.1 in
// InstallGameConsoleCommands next to "showbounds" (0.9.1 Mac symbols, handler in ClientCommands.cpp). Like
// showbounds, it acts on the tracked object's model (CGGameUI::m_currentObjectTrack), or the active player's when
// nothing is tracked, and toggles ModelShowHitTestGeometry / ModelHideHitTestGeometry; see
// CM2Model::ShowHitTestGeometry for what 3.3.5 hit-tests.
static int32_t CCommand_ShowHitTest(const char* command, const char* arguments) {
    CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPE_OBJECT, __FILE__, __LINE__);

    if (!object) {
        object = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_OBJECT, __FILE__, __LINE__);
    }

    CM2Model* model = object ? object->GetObjectModel() : nullptr;

    if (!model) {
        return 0;
    }

    if (model->IsShowingHitTestGeometry()) {
        model->HideHitTestGeometry();
    } else {
        model->ShowHitTestGeometry();
    }

    return 1;
}

// Debug feature: console command "showunselectable" (port: the command name is the port's; 0.5.3 had
// ModelShowUnselectable and nothing calling it). Like showhittest, on the tracked object's model or the player's:
// toggles the display of the batches cursor picking skips (CM2Model::ShowUnselectable), in yellow, or in the color
// given as "red green blue" (0..255).
static int32_t CCommand_ShowUnselectable(const char* command, const char* arguments) {
    CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPE_OBJECT, __FILE__, __LINE__);

    if (!object) {
        object = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPE_OBJECT, __FILE__, __LINE__);
    }

    CM2Model* model = object ? object->GetObjectModel() : nullptr;

    if (!model) {
        return 0;
    }

    int32_t red = 255;
    int32_t green = 255;
    int32_t blue = 0;
    bool colorGiven = arguments && *arguments && sscanf(arguments, "%d %d %d", &red, &green, &blue) == 3;

    if (model->IsShowingUnselectable() && !colorGiven) {
        model->HideUnselectable();
    } else {
        model->ShowUnselectable(static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue));
    }

    return 1;
}

// Debug feature: console command "drawlog".
// 0.5.3 reference: CCommand_DrawLog (0x00404070).
// Called ModelRenderSceneLogToggle("RenderLog.txt"); see CM2Model::RenderSceneLogToggle.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_DrawLog(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "animlog".
// 0.5.3 reference: CCommand_AnimLog (0x004040A0).
// Called ModelAnimateLogToggle("AnimLog.txt"); see CM2Model::AnimateLogToggle.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_AnimLog(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "motionBlend".
// 0.5.3 reference: CCommand_ToggleAnimBlending (0x00404100).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleAnimBlending(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "mem".
// 0.5.3 reference: CCommand_Mem (0x00406B10).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_Mem(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "forceanim".
// 0.5.3 reference: CCommand_ForceMonsterAnim (0x005E14B0).
// Registered by 0.5.3 PlayerClientInitialize.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ForceMonsterAnim(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "resetanim".
// 0.5.3 reference: CCommand_ResetMonsterAnim (0x005E1500).
// Registered by 0.5.3 PlayerClientInitialize.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ResetMonsterAnim(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "dirwtf".
// 0.5.3 reference: ConsoleCommand_DirWtf (0x00630490).
// Registered by 0.5.3 ConsoleCommandInitialize.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ConsoleCommand_DirWtf(const char* command, const char* arguments) {
    return 1;
}

static CVar* s_cvDebugTargetInfo;
static CVar* s_cvShowGUIDs;
static CVar* s_cvErrorFileLog;

// 0.5.3 reference: DebugTargetInfoCallback (0x00402E20).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static bool DebugTargetInfoCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

// 0.5.3 reference: DebugShowGUIDsCallback (0x00402E50).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static bool DebugShowGUIDsCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

// Debug feature: server debug message handler, not registered (0.5.3 opcodes do not map to 3.3.5).
// 0.5.3 reference: MovementLoggingHandler (0x00401730).
// Registered with ClientServices_SetMessageHandler by 0.5.3 ClientInitializeGame.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t MovementLoggingHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return 0;
}

// Debug feature: server debug message handler, not registered (0.5.3 opcodes do not map to 3.3.5).
// 0.5.3 reference: MovementFallLoggingHandler (0x004016A0).
// Registered with ClientServices_SetMessageHandler by 0.5.3 ClientInitializeGame.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t MovementFallLoggingHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return 0;
}

// Debug feature: server debug message handler, not registered (0.5.3 opcodes do not map to 3.3.5).
// 0.5.3 reference: DebugAIStateHandler (0x004015D0).
// Registered with ClientServices_SetMessageHandler by 0.5.3 ClientInitializeGame.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t DebugAIStateHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return 0;
}

// Debug feature: registers the 0.5.3 ClientRegisterConsoleCommands debug commands and CVars.
void ClientDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("light", CCommand_ToggleLighting, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("fog", CCommand_ToggleFog, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("DepthTest", CCommand_ToggleDepthTesting, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("DepthSet", CCommand_ToggleDepthSetting, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("culling", CCommand_ToggleCulling, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("dblbuffer", CCommand_ToggleDblBuffer, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("resxy", CCommand_SetResolutionXY, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("resmode", CCommand_SetResolutionMode, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("bitdepth", CCommand_SetColorDepth, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("3dapi", CCommand_SetAPI, CATEGORY_GRAPHICS, nullptr);
    s_cvDebugTargetInfo = CVar::Register("debugTargetInfo", nullptr, CVAR_FLAGS::CVAR_NONE, "0", DebugTargetInfoCallback, CATEGORY_DEBUG, false, nullptr, false);
    s_cvShowGUIDs = CVar::Register("showGUIDs", nullptr, CVAR_FLAGS::CVAR_NONE, "0", DebugShowGUIDsCallback, CATEGORY_DEBUG, false, nullptr, false);
    // 0.5.3 registered ErrorFileLog from InitializeHandlerPlayer.
    s_cvErrorFileLog = CVar::Register("ErrorFileLog", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
}

// Debug feature: registers the 0.5.3 InstallGameConsoleCommands debug commands.
void GameClientCommandsDebugInstall() {
    ConsoleCommandRegister("loc", CCommand_Loc, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("dloc", CCommand_DLoc, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("tloc", CCommand_TargetLoc, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("facing", CCommand_Facing, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("dfacing", CCommand_DFacing, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("showbounds", CCommand_ShowBounds, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("showhittest", CCommand_ShowHitTest, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("showunselectable", CCommand_ShowUnselectable, CATEGORY_DEBUG, "Shows the target's unselectable geometry [red green blue]");
    ConsoleCommandRegister("drawlog", CCommand_DrawLog, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("animlog", CCommand_AnimLog, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("motionBlend", CCommand_ToggleAnimBlending, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("mem", CCommand_Mem, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("forceanim", CCommand_ForceMonsterAnim, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("resetanim", CCommand_ResetMonsterAnim, CATEGORY_DEBUG, nullptr);
    ConsoleCommandRegister("dirwtf", ConsoleCommand_DirWtf, CATEGORY_DEBUG, nullptr);
}

void GameClientCommandsDebugUninstall() {
    ConsoleCommandUnregister("loc");
    ConsoleCommandUnregister("dloc");
    ConsoleCommandUnregister("tloc");
    ConsoleCommandUnregister("facing");
    ConsoleCommandUnregister("dfacing");
    ConsoleCommandUnregister("showbounds");
    ConsoleCommandUnregister("showhittest");
    ConsoleCommandUnregister("showunselectable");
    ConsoleCommandUnregister("drawlog");
    ConsoleCommandUnregister("animlog");
    ConsoleCommandUnregister("motionBlend");
    ConsoleCommandUnregister("mem");
    ConsoleCommandUnregister("forceanim");
    ConsoleCommandUnregister("resetanim");
    ConsoleCommandUnregister("dirwtf");
}

#endif
