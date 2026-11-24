#ifndef WOWEDIT_SOURCE_APP_WOWEDITSETTINGS_H
#define WOWEDIT_SOURCE_APP_WOWEDITSETTINGS_H

// WowEdit application: settings (port; see App/WowEditApp.h).
//
// WowEdit.ini, next to WowEdit64.exe, written with the defaults on the first run:
//   [WowEdit]
//   DataPath=C:\Games\WoW\Data   the game's Data folder (holding the MPQs), or the folder holding it; empty for
//                                the folder of the executable, as the game does
//   MapId=0                      the map opened at start (Map.dbc ID)
//   StartX=, StartY=, StartZ=    the camera position on it; empty for a default (Northshire on map 0, the middle
//                                of the map's terrain on the others)
//   ViewDistance=, DoodadDistance=, GroundClutter=, HorizonDistance=
//                                the render distances (WowEditRenderDistance.h); empty for the editor's defaults
//   Keyboard=AZERTY              the movement keys: QWERTY (W A S D) or AZERTY (Z Q S D); empty for the layout
//                                Windows has at start (View > AZERTY Movement Keys sets it)
//
// The editor keeps its cvars (video settings...) in WTF\WowEdit.wtf in the game folder, apart from the game's
// Config.wtf; the first run writes it with a 1600 x 900 window.
//
// Read before the client starts (before Storm), so it uses only Win32 and the C runtime.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

static const char* const WOWEDIT_CONFIG_FILE = "WowEdit.wtf";

struct WowEditSettings {
    char m_iniPath[260];
    char m_basePath[260]; // the game folder (holding Data\), with a trailing backslash; empty for the exe folder
    int32_t m_mapId;
    bool m_hasStart;
    float m_start[3];
    bool m_azerty;
};

// Reads WowEdit.ini (writing it first if missing).
void WowEditLoadSettings();
void WowEditEnsureDefaultConfig();
const WowEditSettings& WowEditGetSettings();
// Saves the map and camera position, for the next run.
void WowEditSaveStart(int32_t mapId, const float* position);
// Saves the movement keys.
void WowEditSaveKeyboard(bool azerty);
// Other numbers in the [WowEdit] section (the render distances, WowEditRenderDistance.h); empty or missing gives
// the fallback.
float WowEditSettingsGetFloat(const char* key, float fallback);
void WowEditSettingsSetFloat(const char* key, float value);

#endif

#endif
