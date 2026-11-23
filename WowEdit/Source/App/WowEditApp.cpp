#include "WowEdit/Source/App/WowEditApp.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/ClientTool.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditModule.h"
#include "WowEdit/Source/App/WowEditRenderDistance.h"
#include "WowEdit/Source/App/WowEditSettings.h"
#include "WowEdit/Source/App/WowEditUndo.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

// See WowEditApp.h.

static const char* const WOWEDIT_WINDOW_TITLE = "World of Warcraft Editor";

static bool s_enabled;
static CWowEditApp* s_app;
// Posted to the Gx window to attach outside whatever message revealed it (creation, a format change...).
static UINT s_attachMessage;
// The Gx window's own procedure (CGxDeviceD3d::WindowProcD3d...), under the cursor subclass.
static WNDPROC s_gxWindowProc;
static HWND s_subclassedWindow;

struct WowEditAppMenus {
    COsMenuBar* bar;
    COsMenu* menus[WOWEDITMENU_LAST];
};

// The client runs as a tool (WoW/Source/ClientTool.h): the editor's world view instead of the game.
static void InitializeWorld() {
    const WowEditSettings& settings = WowEditGetSettings();
    WowEditWorldInitialize();
    WowEditWorldSetAzerty(settings.m_azerty);
    WowEditViewTogglesInitialize();
    WowEditRenderDistanceInitialize();

    C3Vector start(settings.m_start[0], settings.m_start[1], settings.m_start[2]);

    if (!WowEditWorldLoadMap(settings.m_mapId, settings.m_hasStart ? &start : nullptr) && settings.m_mapId != 0) {
        WowEditWorldLoadMap(0, nullptr);
    }
}

static void ShutdownWorld() {
    // The next run starts where this one ended.
    if (WowEditWorldGetMapId() >= 0) {
        float position[3];
        WowEditWorldGetCameraPosition(position);
        WowEditSaveStart(WowEditWorldGetMapId(), position);
    }

    WowEditRenderDistanceSave();

    WowEditWorldShutdown();
}

static CLIENT_TOOL s_clientTool;

void WowEditAppEnable() {
    s_enabled = true;

    // Before the client starts: the game folder comes from WowEdit.ini.
    WowEditLoadSettings();
    s_clientTool.m_basePath = WowEditGetSettings().m_basePath;
    s_clientTool.m_configFile = WOWEDIT_CONFIG_FILE;
    s_clientTool.m_initialize = InitializeWorld;
    s_clientTool.m_shutdown = ShutdownWorld;
    ClientSetTool(&s_clientTool);
}

bool WowEditAppEnabled() {
    return s_enabled;
}

void WowEditAppStartup() {
    if (!s_enabled || s_app) {
        return;
    }

    s_attachMessage = RegisterWindowMessageA("WowEditAttach");
    s_app = new CWowEditApp();
    OsSetWindowProc(CWowEditApp::WindowProc);
}

// ---- Cursor

void WowEditAppUpdateCursor() {
    SetCursor(WowEditWorldIsLooking() ? nullptr : LoadCursorA(nullptr, IDC_ARROW));
}

// The Gx window procedure answers WM_SETCURSOR over its client area itself, before OsWindowProc and the chained
// procedure: it clears the Windows cursor and shows the device's hardware cursor, the game's cursor image, which the
// game interface loads (CursorInitialize) and the editor does not. An editor wants the Windows cursor, so the window
// is subclassed for that one message.
static LRESULT CALLBACK CursorWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_SETCURSOR && LOWORD(lparam) == HTCLIENT) {
        WowEditAppUpdateCursor();
        return TRUE;
    }

    return CallWindowProcA(s_gxWindowProc, hwnd, message, wparam, lparam);
}

static void SubclassCursor(HWND hwnd) {
    if (s_subclassedWindow == hwnd) {
        return;
    }

    s_gxWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CursorWindowProc)));
    s_subclassedWindow = hwnd;
}

static void UnsubclassCursor(HWND hwnd) {
    if (s_subclassedWindow != hwnd) {
        return;
    }

    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_gxWindowProc));
    s_subclassedWindow = nullptr;
}

// ---- CWowEditApp

CWowEditApp* CWowEditApp::Get() {
    return s_app;
}

CWowEditApp::CWowEditApp() {
    this->m_window = nullptr;
    this->m_attachPending = false;
    this->m_menus = nullptr;
    this->m_undoer = new CWowEditUndoer();
    this->m_commander = new CWowEditCommander(this->m_undoer);

    this->m_commander->AddModule(WowEditCreateAppModule());
    this->m_commander->AddModule(WowEditCreateViewModule());
    this->m_commander->AddModule(WowEditCreateTerrainModule());
}

CWowEditCommander* CWowEditApp::GetCommander() {
    return this->m_commander;
}

CWowEditUndoer* CWowEditApp::GetUndoer() {
    return this->m_undoer;
}

void* CWowEditApp::GetMainWindow() {
    return this->m_window;
}

intptr_t CWowEditApp::WindowProc(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam) {
    CWowEditApp* app = s_app;

    if (app) {
        void* gxWindow = OsGuiGetWindow(OSGUI_WINDOW_TYPE_GX);

        if (s_attachMessage && message == s_attachMessage) {
            app->m_attachPending = false;

            if (window == gxWindow && window != app->m_window) {
                app->Attach(window);
            }

            return 0;
        }

        if (!app->m_attachPending && window == gxWindow && window != app->m_window) {
            app->m_attachPending = true;
            PostMessageA(static_cast<HWND>(window), s_attachMessage, 0, 0);
        }

        if (window == app->m_window) {
            if (message == WM_INITMENUPOPUP) {
                app->UpdateMenus();
            } else if (message == WM_DESTROY) {
                app->Detach();
            }
        }
    }

    // Menu commands (WM_COMMAND), menu loop idle, control colors; ends with DefWindowProcA.
    return OsGuiWindowProc(window, message, wparam, lparam);
}

void CWowEditApp::Attach(void* window) {
    if (this->m_window) {
        this->Detach();
    }

    static bool s_osGuiInitialized;

    if (!s_osGuiInitialized) {
        // Until OsGuiSetApplicationInfo, OsGuiProcessMessage lets every message through untouched.
        OsGuiSetApplicationInfo(GetModuleHandleA(nullptr));
        OsGuiInitialize();
        OsGuiSetMenuCommandCallback(CWowEditApp::OnMenuCommand, this);
        s_osGuiInitialized = true;
    }

    this->m_window = window;
    SubclassCursor(static_cast<HWND>(window));

    // The menu bar, from the command table: each menu's items in table order, a null text being a separator.
    this->m_menus = new WowEditAppMenus();
    this->m_menus->bar = new COsMenuBar(window);
    this->m_menus->menus[0] = nullptr;

    TSGrowableArray<COsMenu*> menus;

    for (uint8_t id = 1; id < WOWEDITMENU_LAST; id++) {
        COsMenu* menu = new COsMenu(id, WowEditGetMenuTitle(id));
        this->m_menus->menus[id] = menu;
        *menus.New() = menu;
    }

    uint32_t count;
    const WowEditCommandData* table = WowEditGetCommandTable(&count);
    int32_t positions[WOWEDITMENU_LAST] = {};

    for (uint32_t i = 0; i < count; i++) {
        const WowEditCommandData& data = table[i];
        COsMenu* menu = this->m_menus->menus[data.menu];
        int32_t position = positions[data.menu]++;

        if (data.command == WOWEDITCMD_NONE) {
            menu->AddSeparator(position);
            continue;
        }

        OsGuiMenuHotkey hotkey;
        hotkey.keyID = data.key;
        hotkey.modKeyID = data.modifiers;
        menu->AddTextItem(position, data.text, data.key != KEY_NONE ? &hotkey : nullptr);
    }

    this->m_menus->bar->Set(menus);

    OsGuiSetWindowTitle(window, WOWEDIT_WINDOW_TITLE);

    for (uint32_t i = 0; i < this->m_commander->GetNumModules(); i++) {
        this->m_commander->GetModule(i)->OnAttach(window);
    }

    this->UpdateMenus();
}

void CWowEditApp::Detach() {
    if (!this->m_window) {
        return;
    }

    for (uint32_t i = 0; i < this->m_commander->GetNumModules(); i++) {
        this->m_commander->GetModule(i)->OnDetach();
    }

    // The bar destroys its menus' handles with its own; the COsMenu objects go after it.
    SetMenu(static_cast<HWND>(this->m_window), nullptr);
    delete this->m_menus->bar;

    for (uint8_t id = 1; id < WOWEDITMENU_LAST; id++) {
        delete this->m_menus->menus[id];
    }

    delete this->m_menus;
    this->m_menus = nullptr;
    UnsubclassCursor(static_cast<HWND>(this->m_window));
    this->m_window = nullptr;
}

// Before a menu opens: each item's enabled and checked state from its module.
void CWowEditApp::UpdateMenus() {
    if (!this->m_menus) {
        return;
    }

    for (uint32_t i = 0; i < this->m_commander->GetNumModules(); i++) {
        this->m_commander->GetModule(i)->OnRefresh();
    }

    uint32_t count;
    const WowEditCommandData* table = WowEditGetCommandTable(&count);
    int32_t positions[WOWEDITMENU_LAST] = {};

    for (uint32_t i = 0; i < count; i++) {
        const WowEditCommandData& data = table[i];
        int32_t position = positions[data.menu]++;

        if (data.command == WOWEDITCMD_NONE) {
            continue;
        }

        bool enabled;
        bool checked;
        this->m_commander->GetCommandStatus(data.command, &enabled, &checked);

        COsMenu* menu = this->m_menus->menus[data.menu];
        menu->EnableItem(position, enabled);
        menu->CheckItem(position, checked);
    }
}

// OsGui menu command callback: type is the menu id, subType the item position.
void CWowEditApp::OnMenuCommand(const OsGuiCallbackParams& params) {
    CWowEditApp* app = static_cast<CWowEditApp*>(params.user);
    int32_t command = app->m_commander->FindMenuCommand(static_cast<uint8_t>(params.type), params.subType);
    app->m_commander->DoCommand(command);
}

#endif
