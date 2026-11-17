#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "PLightInfo".
// 0.5.3 reference: CCommand_PLightInfo (0x00548370).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightInfo(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightEnable".
// 0.5.3 reference: CCommand_PLightEnable (0x00548490).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightEnable(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightOmni".
// 0.5.3 reference: CCommand_PLightOmni (0x005484F0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightOmni(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightDirPos".
// 0.5.3 reference: CCommand_PLightDir (0x00548550).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightDir(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightAmbColor".
// 0.5.3 reference: CCommand_PLightAmbColor (0x005485C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightAmbColor(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightDirColor".
// 0.5.3 reference: CCommand_PLightDirColor (0x00548630).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightDirColor(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightAmbIntens".
// 0.5.3 reference: CCommand_PLightAmbIntens (0x005486A0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightAmbIntens(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "PLightDirIntens".
// 0.5.3 reference: CCommand_PLightDirIntens (0x00548700).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CCommand_PLightDirIntens(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: registers the 0.5.3 PortraitInitialize portrait lighting commands.
void PortraitDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("PLightInfo", CCommand_PLightInfo, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightEnable", CCommand_PLightEnable, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightOmni", CCommand_PLightOmni, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightDirPos", CCommand_PLightDir, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightAmbColor", CCommand_PLightAmbColor, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightDirColor", CCommand_PLightDirColor, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightAmbIntens", CCommand_PLightAmbIntens, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("PLightDirIntens", CCommand_PLightDirIntens, CATEGORY_GRAPHICS, nullptr);
}

#endif
