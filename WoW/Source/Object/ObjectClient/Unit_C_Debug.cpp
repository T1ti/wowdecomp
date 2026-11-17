#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

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
void UnitCombatDebugLogEnable(int32_t enable) {
}

// 0.5.3 reference: UnitDebugCombatLogOnEnable (0x0061CD50).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void UnitDebugCombatLogOnEnable(int32_t enable) {
}

// Debug feature: unit pathing display.
// 0.5.3 reference: CGUnit_C::RenderDebugPathing (0x005FD3E0).
// 0.5.3 called it from CGObject_C::PreAnimate when DebugTargetPath was set.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CGUnit_C::RenderDebugPathing() {
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
// Flipped boundsPresent and called ModelShowBoundingSphere / ModelHideBounds on the active player's model; see CM2Model::ShowBoundingSphere / HideBounds.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void CGPlayer_C::TogglePlayerBounds() {
}

static CVar* s_cvDebugTargetPath;
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
