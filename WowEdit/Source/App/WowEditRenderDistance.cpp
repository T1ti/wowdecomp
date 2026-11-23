#include "WowEdit/Source/App/WowEditRenderDistance.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdio>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WowEdit/Source/App/WowEditSettings.h"

// See WowEditRenderDistance.h.

static const WowEditDistanceInfo s_distances[NUM_WOWEDITDISTANCES] = {
    { "ViewDistance", "farclip", "View distance: %.0f yards", 550.0f / 3.0f, 3200.0f, 2400.0f, 1.0f },
    // CWorldParam::EnvironmentDetailCallback: 0.5 to 1.5.
    { "DoodadDistance", "environmentDetail", "Doodad and WMO distance: %.2fx", 0.5f, 1.5f, 1.5f, 100.0f },
    // CWorldParam::GroundEffectDistCallback: 0 to 140 yards.
    { "GroundClutter", "groundEffectDist", "Ground clutter: %.0f yards", 0.0f, 140.0f, 70.0f, 1.0f },
    // CWorldParam::HorizonFarClipScaleCallback: 3 to 6.
    { "HorizonDistance", "horizonFarclipScale", "Horizon (with fog): %.1fx the view distance", 3.0f, 6.0f, 4.0f, 10.0f },
};

static float s_values[NUM_WOWEDITDISTANCES];

const WowEditDistanceInfo& WowEditGetDistanceInfo(int32_t distance) {
    return s_distances[distance];
}

float WowEditGetDistance(int32_t distance) {
    return s_values[distance];
}

void WowEditSetDistance(int32_t distance, float value) {
    const WowEditDistanceInfo& info = s_distances[distance];
    value = value < info.min ? info.min : (value > info.max ? info.max : value);
    s_values[distance] = value;

    CVar* cvar = CVar::Lookup(info.cvar);

    if (!cvar) {
        return;
    }

    char text[32];
    snprintf(text, sizeof(text), "%f", value);
    cvar->Set(text, true, false, false, true);
}

void WowEditRenderDistanceInitialize() {
    for (int32_t i = 0; i < NUM_WOWEDITDISTANCES; i++) {
        WowEditSetDistance(i, WowEditSettingsGetFloat(s_distances[i].key, s_distances[i].defaultValue));
    }
}

void WowEditRenderDistanceSave() {
    for (int32_t i = 0; i < NUM_WOWEDITDISTANCES; i++) {
        WowEditSettingsSetFloat(s_distances[i].key, s_values[i]);
    }
}

#endif
