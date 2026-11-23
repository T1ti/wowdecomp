#ifndef WOWEDIT_SOURCE_APP_WOWEDITRENDERDISTANCE_H
#define WOWEDIT_SOURCE_APP_WOWEDITRENDERDISTANCE_H

// WowEdit application: render distances (port; see App/WowEditApp.h).
//
// The client's draw distances are graphics cvars; the editor sets them from WowEdit.ini (WowEditSettings.h) and the
// Render Distance palette (WowEditDistancePalette.h), saving them back at exit:
//   ViewDistance     farclip: the far clip plane, in yards, 183.3 to 1583.3. It is also how far the world loads:
//                    World::PrepareAreaOfInterest loads the map tiles and chunks 1.25 to 2 times this far around the
//                    camera (by the view frustum), and the terrain chunk buffers are sized for it. The client caps it
//                    at 791.7 on the old world maps (below the first expansion's) unless farClipOverride is 1, which
//                    the editor sets, so the full 1583.3 is open everywhere. Default: the maximum.
//   DoodadDistance   environmentDetail: the scale of the doodad and WMO fade distances, 0.5 to 1.5. Default 1.5.
//   GroundClutter    groundEffectDist: the detail doodads (grass), in yards, 0 to 140. Default 70, the game's.
//   HorizonDistance  horizonFarclipScale: how far the low detail terrain (WDL) reaches, times the view distance,
//                    3 to 6; drawn only with the fog on (WowEditViewToggles.h). Default 4, the game's.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

enum WOWEDITDISTANCE : int32_t {
    WOWEDITDISTANCE_VIEW = 0,
    WOWEDITDISTANCE_DOODADS = 1,
    WOWEDITDISTANCE_GROUND_CLUTTER = 2,
    WOWEDITDISTANCE_HORIZON = 3,
    NUM_WOWEDITDISTANCES = 4,
};

struct WowEditDistanceInfo {
    const char* key; // in WowEdit.ini
    const char* cvar;
    const char* label; // printf format of the value, for the palette
    float min;
    float max;
    float defaultValue;
    float sliderScale; // slider steps per unit
};

const WowEditDistanceInfo& WowEditGetDistanceInfo(int32_t distance);
float WowEditGetDistance(int32_t distance);
// Clamps and applies the value to its cvar.
void WowEditSetDistance(int32_t distance, float value);

// Before the first map: farClipOverride, then every distance from WowEdit.ini.
void WowEditRenderDistanceInitialize();
// At exit: into WowEdit.ini.
void WowEditRenderDistanceSave();

#endif

#endif
