#include "WowEdit/Source/App/WowEditSettings.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// See WowEditSettings.h.

static WowEditSettings s_settings;

static const char* const SECTION = "WowEdit";

static bool DirectoryExists(const char* path) {
    DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// The setting may name the Data folder or the game folder; the client wants the game folder (it opens Data\*.MPQ
// relative to it), with a trailing backslash.
static void ResolveBasePath(const char* setting, char* basePath, size_t size) {
    basePath[0] = '\0';

    if (!setting[0]) {
        return;
    }

    char path[260];
    snprintf(path, sizeof(path), "%s", setting);

    size_t length = strlen(path);

    while (length && (path[length - 1] == '\\' || path[length - 1] == '/')) {
        path[--length] = '\0';
    }

    char dataPath[300];
    snprintf(dataPath, sizeof(dataPath), "%s\\Data", path);

    if (!DirectoryExists(dataPath)) {
        // Not the game folder: the Data folder itself, so its parent.
        char* slash = strrchr(path, '\\');
        char* forward = strrchr(path, '/');

        if (forward > slash) {
            slash = forward;
        }

        if (slash) {
            *slash = '\0';
        }
    }

    snprintf(basePath, size, "%s\\", path);
}

static void WriteDefaults(const char* iniPath) {
    FILE* file = fopen(iniPath, "w");

    if (!file) {
        return;
    }

    fputs("[WowEdit]\n"
          "; The game's Data folder (holding the MPQs), or the folder holding it.\n"
          "; Empty: the folder of WowEdit64.exe, as the game does.\n"
          "DataPath=\n"
          "; The map opened at start (Map.dbc ID: 0 Eastern Kingdoms, 1 Kalimdor, 530 Outland, 571 Northrend).\n"
          "MapId=0\n"
          "; The camera position at start; empty for a default (Northshire on map 0, the middle of the terrain on others).\n"
          "StartX=\n"
          "StartY=\n"
          "StartZ=\n"
          "; The movement keys: QWERTY (W A S D) or AZERTY (Z Q S D). Empty: from the keyboard layout.\n"
          "Keyboard=\n"
          "; Render distances (Window > Render Distance); empty for the editor's defaults.\n"
          "; ViewDistance: base view distance, 183 to 3200 yards (default 2400). Looking down adds camera altitude.\n"
          "; DoodadDistance: doodad and WMO fade distance scale, 0.5 to 1.5 (default 1.5).\n"
          "; GroundClutter: grass distance, 0 to 140 yards (default 70).\n"
          "; HorizonDistance: WDL horizon reach with the fog on, 3 to 6 times the view distance (default 4).\n"
          "ViewDistance=\n"
          "DoodadDistance=\n"
          "GroundClutter=\n"
          "HorizonDistance=\n",
        file);
    fclose(file);
}

// The editor's cvar file (WTF\WowEdit.wtf), so it keeps its own video settings apart from the game's
// Config.wtf. On the first run: a 1600 x 900 window, since the menu bar and the palettes need a windowed device.
void WowEditEnsureDefaultConfig() {
    // Startup has selected the working directory, including any command-line data path.
    CreateDirectoryA("WTF", nullptr);
    char path[300];
    snprintf(path, sizeof(path), "WTF\\%s", WOWEDIT_CONFIG_FILE);

    if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) {
        return;
    }

    FILE* file = fopen(path, "w");

    if (file) {
        fputs("SET gxWindow \"1\"\nSET gxMaximize \"0\"\nSET gxResolution \"1600x900\"\n", file);
        fclose(file);
    }
}

// The keyboard layout at start puts the A key where QWERTY has Q (scan code 0x10): an AZERTY keyboard.
static bool LayoutIsAzerty() {
    return MapVirtualKeyA('A', MAPVK_VK_TO_VSC) == 0x10;
}

void WowEditLoadSettings() {
    char exePath[260];
    GetModuleFileNameA(nullptr, exePath, sizeof(exePath));
    char* slash = strrchr(exePath, '\\');

    if (slash) {
        slash[1] = '\0';
    }

    snprintf(s_settings.m_iniPath, sizeof(s_settings.m_iniPath), "%sWowEdit.ini", exePath);

    if (GetFileAttributesA(s_settings.m_iniPath) == INVALID_FILE_ATTRIBUTES) {
        WriteDefaults(s_settings.m_iniPath);
    }

    char value[260];
    GetPrivateProfileStringA(SECTION, "DataPath", "", value, sizeof(value), s_settings.m_iniPath);
    ResolveBasePath(value, s_settings.m_basePath, sizeof(s_settings.m_basePath));

    s_settings.m_mapId = GetPrivateProfileIntA(SECTION, "MapId", 0, s_settings.m_iniPath);

    const char* const keys[3] = { "StartX", "StartY", "StartZ" };
    s_settings.m_hasStart = true;

    for (int32_t i = 0; i < 3; i++) {
        GetPrivateProfileStringA(SECTION, keys[i], "", value, sizeof(value), s_settings.m_iniPath);

        if (!value[0]) {
            s_settings.m_hasStart = false;
            break;
        }

        s_settings.m_start[i] = static_cast<float>(atof(value));
    }

    GetPrivateProfileStringA(SECTION, "Keyboard", "", value, sizeof(value), s_settings.m_iniPath);
    s_settings.m_azerty = value[0] ? _stricmp(value, "AZERTY") == 0 : LayoutIsAzerty();
}

const WowEditSettings& WowEditGetSettings() {
    return s_settings;
}

void WowEditSaveStart(int32_t mapId, const float* position) {
    if (!s_settings.m_iniPath[0]) {
        return;
    }

    char value[64];
    snprintf(value, sizeof(value), "%d", mapId);
    WritePrivateProfileStringA(SECTION, "MapId", value, s_settings.m_iniPath);

    const char* const keys[3] = { "StartX", "StartY", "StartZ" };

    for (int32_t i = 0; i < 3; i++) {
        snprintf(value, sizeof(value), "%.2f", position[i]);
        WritePrivateProfileStringA(SECTION, keys[i], value, s_settings.m_iniPath);
    }
}

float WowEditSettingsGetFloat(const char* key, float fallback) {
    char value[64];
    GetPrivateProfileStringA(SECTION, key, "", value, sizeof(value), s_settings.m_iniPath);
    return value[0] ? static_cast<float>(atof(value)) : fallback;
}

void WowEditSettingsSetFloat(const char* key, float value) {
    if (!s_settings.m_iniPath[0]) {
        return;
    }

    char text[64];
    snprintf(text, sizeof(text), "%.2f", value);
    WritePrivateProfileStringA(SECTION, key, text, s_settings.m_iniPath);
}

void WowEditSaveKeyboard(bool azerty) {
    s_settings.m_azerty = azerty;

    if (s_settings.m_iniPath[0]) {
        WritePrivateProfileStringA(SECTION, "Keyboard", azerty ? "AZERTY" : "QWERTY", s_settings.m_iniPath);
    }
}

#endif
