#ifndef WOWEDIT_SOURCE_APP_WOWEDITVIEWTOGGLES_H
#define WOWEDIT_SOURCE_APP_WOWEDITVIEWTOGGLES_H

// WowEdit application: the view toggles (port; see App/WowEditApp.h).
//
// After the bottom toolbar of Blizzard's WoWEdit (wowdev.wiki/WoWEdit, from screenshots of the internal editor):
// "Toggle view settings to match in game view", "Turn Grids on/off", "Turn Distance Fog on/off", "Turn Doodads
// on/off", "Show / Hide WMO groups", "Turn on/off display of Water", "View Sky", "Toggle Wireframe Mode", "Turn on/off
// terrain", "View impathable polygons based on angle", "View Detail Doodads", ... Each is a command (View menu) and
// a button of the View palette (CWowEditViewPalette), made from the 3.3.5 world's own switches (CWorld::enables, the
// sky and fog states) and the WowEdit library's overlays. Toolbar buttons with no 3.3.5 counterpart (area share
// mappings, texture counts, vertex colors, point lights, baked shadows, sea level, death plane, pathing, ...) are
// left out until there is something to show.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

struct WowEditViewToggle {
    int32_t command; // WOWEDITCMD_VIEW_*
    const char* label; // button text
    const char* tooltip; // the WoWEdit tooltip it stands for, or a description
    bool (*get)();
    void (*set)(bool on);
};

const WowEditViewToggle* WowEditGetViewToggles(uint32_t* count);
const WowEditViewToggle* WowEditFindViewToggle(int32_t command);

// From the world start-up: remembers the game's view settings for "match in game view".
void WowEditViewTogglesInitialize();
// "Toggle view settings to match in game view": the game's switches back, every editor overlay off.
void WowEditMatchGameView();

#endif

#endif
