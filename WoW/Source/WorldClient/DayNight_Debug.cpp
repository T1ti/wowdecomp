#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "SkyShow".
// 0.5.3 reference: ConsoleCommand_SkyShow (0x006BB4F0): "SkyShow 1" sets s_dnInfo.showSky, anything else clears it.
// 3.3.5 keeps the flag as DNInfo::m_showSky, which DayNight::RenderSky and RenderGlares test (SetOverrideFog saves and
// overrides it while a fog override is on, and ClearOverrideFog puts the saved value back).
static int32_t ConsoleCommand_SkyShow(const char* command, const char* arguments) {
    if (arguments && SStrToInt(arguments)) {
        ConsoleWrite("Sky enabled", DEFAULT_COLOR);
        DayNight::g_dnInfo.m_showSky = 1;
    } else {
        ConsoleWrite("The sky is falling.", DEFAULT_COLOR);
        DayNight::g_dnInfo.m_showSky = 0;
    }

    return 1;
}

// Debug feature: console command "SkyCloudLayers".
// 0.5.3 reference: ConsoleCommand_SkyCloudLayers (0x006BB420): 0 or 1 cloud layers (s_clouds.m_nLayers), anything else
// refused. 3.3.5 has a single cloud layer and no layer count; its DNClouds::m_enabled is the same switch (Update,
// UpdateTextureAnimation, GetDensity and Render skip the clouds without it), so the port sets that.
static int32_t ConsoleCommand_SkyCloudLayers(const char* command, const char* arguments) {
    int32_t layers = arguments && *arguments ? SStrToInt(arguments) : -1;

    if (layers < 0 || layers > 1) {
        ConsoleWrite("CloudLayers must be in the range [0, 1]", DEFAULT_COLOR);
        return 0;
    }

    DayNight::g_clouds.m_enabled = layers;
    ConsoleWrite("CloudLayers set", DEFAULT_COLOR);
    return 1;
}

// Debug feature: console command "SkyCloudDensity".
// 0.5.3 reference: ConsoleCommand_SkyCloudDensity (0x006BB2E0): a density clamped to 0..1 into
// DNClouds::OverrideDensitySharpness, which set m_densityOverride (0 gives the density back to the sky data) and
// recomputed m_density. 3.3.5 keeps m_densityOverride, read by DNClouds::UpdateTextureAnimation each frame, which
// recomputes m_density from it.
static int32_t ConsoleCommand_SkyCloudDensity(const char* command, const char* arguments) {
    if (!arguments || !*arguments) {
        ConsoleWrite("SkyCloudDensity set from data", DEFAULT_COLOR);
        return 0;
    }

    float density = SStrToFloat(arguments);
    density = density < 0.0f ? 0.0f : (density > 1.0f ? 1.0f : density);
    DayNight::g_clouds.m_densityOverride = density;

    if (density == 0.0f) {
        ConsoleWrite("SkyCloudDensity set from data", DEFAULT_COLOR);
    } else {
        ConsoleWriteA("SkyCloudDensity set from override to %f", DEFAULT_COLOR, density);
    }

    return 0;
}

// Debug feature: registers the 0.5.3 DayNightInitialize sky commands missing from 3.3.5.
void DayNightDebugRegisterConsoleCommands() {
    ConsoleCommandRegister("SkyShow", ConsoleCommand_SkyShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("SkyCloudLayers", ConsoleCommand_SkyCloudLayers, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("SkyCloudDensity", ConsoleCommand_SkyCloudDensity, CATEGORY_GRAPHICS, nullptr);
}

#endif
