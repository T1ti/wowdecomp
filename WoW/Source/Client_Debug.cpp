#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Net/Types.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "fog".
// 0.5.3 reference: CCommand_ToggleFog (0x00402FB0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleFog(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "DepthTest".
// 0.5.3 reference: CCommand_ToggleDepthTesting (0x00402FF0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleDepthTesting(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "DepthSet".
// 0.5.3 reference: CCommand_ToggleDepthSetting (0x00403030).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleDepthSetting(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "culling".
// 0.5.3 reference: CCommand_ToggleCulling (0x00403070).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleCulling(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "dblbuffer".
// 0.5.3 reference: CCommand_ToggleDblBuffer (0x004030B0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ToggleDblBuffer(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "resxy".
// 0.5.3 reference: CCommand_SetResolutionXY (0x004030F0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_SetResolutionXY(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "resmode".
// 0.5.3 reference: CCommand_SetResolutionMode (0x00403100).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_SetResolutionMode(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "bitdepth".
// 0.5.3 reference: CCommand_SetColorDepth (0x00403110).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_SetColorDepth(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "3dapi".
// 0.5.3 reference: CCommand_SetAPI (0x00403120).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
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
// Toggled the bounding sphere on the selected model; see CM2Model::ShowBoundingSphere / HideBounds.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_ShowBounds(const char* command, const char* arguments) {
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
    ConsoleCommandUnregister("drawlog");
    ConsoleCommandUnregister("animlog");
    ConsoleCommandUnregister("motionBlend");
    ConsoleCommandUnregister("mem");
    ConsoleCommandUnregister("forceanim");
    ConsoleCommandUnregister("resetanim");
    ConsoleCommandUnregister("dirwtf");
}

#endif
