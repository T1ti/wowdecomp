#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "SkyShow".
// 0.5.3 reference: ConsoleCommand_SkyShow (0x006BB4F0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ConsoleCommand_SkyShow(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "SkyCloudLayers".
// 0.5.3 reference: ConsoleCommand_SkyCloudLayers (0x006BB420).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ConsoleCommand_SkyCloudLayers(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "SkyCloudDensity".
// 0.5.3 reference: ConsoleCommand_SkyCloudDensity (0x006BB2E0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ConsoleCommand_SkyCloudDensity(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: registers the 0.5.3 DayNightInitialize sky commands missing from 3.3.5.
void DayNightDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("SkyShow", ConsoleCommand_SkyShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("SkyCloudLayers", ConsoleCommand_SkyCloudLayers, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("SkyCloudDensity", ConsoleCommand_SkyCloudDensity, CATEGORY_GRAPHICS, nullptr);
}

#endif
