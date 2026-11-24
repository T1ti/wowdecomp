#include "WowEdit/Source/App/WowEditApp.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Input.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditDistancePalette.h"
#include "WowEdit/Source/App/WowEditMapDialog.h"
#include "WowEdit/Source/App/WowEditMenus.h"
#include "WowEdit/Source/App/WowEditSession.h"
#include "WowEdit/Source/App/WowEditToolPalette.h"
#include "WowEdit/Source/App/WowEditViewPalette.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

// See WowEditApp.h.

static const char* const WOWEDIT_WINDOW_TITLE = "World of Warcraft Editor";

static CWowEditApp* s_app;
// Posted to the Gx window to attach outside whatever message revealed it (creation, a format change...).
static UINT s_attachMessage;
// The Gx window's own procedure (CGxDeviceD3d::WindowProcD3d...), under the cursor subclass.
static WNDPROC s_gxWindowProc;
static HWND s_subclassedWindow;

struct WowEditAppUi {
    COsMenuBar* bar;
    COsMenu* menus[WOWEDITMENU_LAST];
    CWowEditMapDialog* mapDialog;
    CWowEditToolPalette* toolPalette;
    CWowEditViewPalette* viewPalette;
    CWowEditDistancePalette* distancePalette;
};

void CWowEditApp::OpenShell(CWowEditSession& session) {
    s_attachMessage = RegisterWindowMessageA("WowEditAttach");
    s_app = new CWowEditApp(session);
    EventSetConfirmCloseCallback(ConfirmClose, s_app);
    OsSetWindowProc(CWowEditApp::WindowProc);

    if (void* window = OsGuiGetWindow(OSGUI_WINDOW_TYPE_GX); window && s_attachMessage) {
        s_app->m_attachPending = PostMessageA(static_cast<HWND>(window), s_attachMessage, 0, 0) != 0;
    }
}

void CWowEditApp::CloseShell() {
    EventSetConfirmCloseCallback(nullptr, nullptr);
    s_app->Detach();
    OsSetWindowProc(nullptr);
    OsGuiSetMenuCommandCallback(nullptr, nullptr);
    OsGuiDestroy();
    OsGuiSetApplicationInfo(nullptr);
    delete s_app;
    s_app = nullptr;
}

// ---- Cursor

// The Gx window procedure answers WM_SETCURSOR over its client area itself, before OsWindowProc and the chained
// procedure: it clears the Windows cursor and shows the device's hardware cursor, the game's cursor image, which the
// game interface loads (CursorInitialize) and the editor does not. An editor wants the Windows cursor, so the window
// is subclassed for that one message.
static LRESULT CALLBACK CursorWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_SETCURSOR && LOWORD(lparam) == HTCLIENT) {
        SetCursor(WowEditWorldIsLooking() ? nullptr : LoadCursorA(nullptr, IDC_ARROW));
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

void CWowEditApp::SetViewportCursor(bool looking) {
    SetCursor(looking ? nullptr : LoadCursorA(nullptr, IDC_ARROW));
}

CWowEditApp::CWowEditApp(CWowEditSession& session) : m_session(session) {
    this->m_window = nullptr;
    this->m_attachPending = false;
    this->m_ui = nullptr;
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

        if (s_attachMessage && !app->m_attachPending && window == gxWindow && window != app->m_window) {
            app->m_attachPending = PostMessageA(static_cast<HWND>(window), s_attachMessage, 0, 0) != 0;
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
    this->m_ui = new WowEditAppUi();
    this->m_ui->bar = new COsMenuBar(window);
    this->m_ui->menus[0] = nullptr;

    TSGrowableArray<COsMenu*> menus;

    for (uint8_t id = 1; id < WOWEDITMENU_LAST; id++) {
        COsMenu* menu = new COsMenu(id, WowEditGetMenuTitle(id));
        this->m_ui->menus[id] = menu;
        *menus.New() = menu;
    }

    uint32_t count;
    const WowEditCommandData* table = WowEditGetCommandTable(&count);
    int32_t positions[WOWEDITMENU_LAST] = {};

    for (uint32_t i = 0; i < count; i++) {
        const WowEditCommandData& data = table[i];
        COsMenu* menu = this->m_ui->menus[data.menu];
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

    this->m_ui->mapDialog = new CWowEditMapDialog(window, this->m_session);
    this->m_ui->viewPalette = new CWowEditViewPalette(window);
    this->m_ui->viewPalette->Show(true);
    // Hidden until Window > Render Distance.
    this->m_ui->distancePalette = new CWowEditDistancePalette(window);
    this->m_ui->toolPalette = new CWowEditToolPalette(window);
    this->m_ui->toolPalette->Show(true);

    this->m_ui->bar->Set(menus);
    OsGuiSetWindowTitle(window, WOWEDIT_WINDOW_TITLE);
    this->UpdateMenus();
}

void CWowEditApp::Detach() {
    if (!this->m_window) {
        return;
    }

    delete this->m_ui->mapDialog;
    delete this->m_ui->viewPalette;
    delete this->m_ui->distancePalette;
    delete this->m_ui->toolPalette;

    // The bar destroys its menus' handles with its own; the COsMenu objects go after it.
    SetMenu(static_cast<HWND>(this->m_window), nullptr);
    delete this->m_ui->bar;

    for (uint8_t id = 1; id < WOWEDITMENU_LAST; id++) {
        delete this->m_ui->menus[id];
    }

    delete this->m_ui;
    this->m_ui = nullptr;
    UnsubclassCursor(static_cast<HWND>(this->m_window));
    this->m_window = nullptr;
}

// Before a menu opens: each item's enabled and checked state from its module.
void CWowEditApp::UpdateMenus() {
    if (!this->m_ui) {
        return;
    }

    this->RefreshPalettes();

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
        this->GetMenuCommandStatus(data.command, &enabled, &checked);

        COsMenu* menu = this->m_ui->menus[data.menu];
        menu->EnableItem(position, enabled);
        menu->CheckItem(position, checked);
    }
}

void CWowEditApp::RefreshPalettes() {
    this->m_ui->viewPalette->Refresh();
    this->m_ui->distancePalette->Refresh();
    this->m_ui->toolPalette->Refresh();
}

void CWowEditApp::GetMenuCommandStatus(int32_t command, bool* enabled, bool* checked) {
    *enabled = true;
    *checked = false;

    switch (command) {
        case WOWEDITCMD_OPEN_MAP:
        case WOWEDITCMD_EXIT:
        case WOWEDITCMD_ABOUT:
            return;

        case WOWEDITCMD_SAVE_AREA_EDITS:
            *enabled = this->m_session.AreaEdits().HasUnsavedChanges();
            return;

        case WOWEDITCMD_WINDOW_VIEW_PALETTE:
            *checked = this->m_ui->viewPalette->IsShowing();
            return;

        case WOWEDITCMD_WINDOW_DISTANCE_PALETTE:
            *checked = this->m_ui->distancePalette->IsShowing();
            return;

        case WOWEDITCMD_WINDOW_TOOL_PALETTE:
            *checked = this->m_ui->toolPalette->IsShowing();
            return;

        default:
            this->m_session.GetCommandStatus(command, enabled, checked);
            return;
    }
}

void CWowEditApp::DoMenuCommand(int32_t command) {
    if (!this->m_ui) {
        return;
    }

    switch (command) {
        case WOWEDITCMD_OPEN_MAP:
            this->m_ui->mapDialog->Open();
            return;

        case WOWEDITCMD_SAVE_AREA_EDITS:
            if (!this->m_session.AreaEdits().Save()) {
                OsGuiMessageBox(this->m_window, OSGUI_MESSAGE_STYLE_OK,
                    "Area edits could not be saved. Check the console for details and try again.", "Save Area Edits");
            }
            return;

        case WOWEDITCMD_EXIT:
            // As the close box: OsWindowProc queues OS_INPUT_CLOSE.
            PostMessageA(static_cast<HWND>(this->m_window), WM_SYSCOMMAND, SC_CLOSE, 0);
            return;

        case WOWEDITCMD_ABOUT:
            OsGuiMessageBox(this->m_window, OSGUI_MESSAGE_STYLE_OK,
                "World of Warcraft Editor\n\n"
                "A reconstruction of Blizzard's world editor on the 3.3.5 client engine: the world drawn on its own, "
                "its window built with OsGui, the engine's tool interface layer.\n\n"
                "Camera: hold the right mouse button to look, W A S D to move, E and Q up and down, Shift faster, "
                "the mouse wheel for the speed.",
                "About WowEdit");
            return;

        case WOWEDITCMD_WINDOW_VIEW_PALETTE:
            this->m_ui->viewPalette->Show(!this->m_ui->viewPalette->IsShowing());
            return;

        case WOWEDITCMD_WINDOW_DISTANCE_PALETTE:
            this->m_ui->distancePalette->Show(!this->m_ui->distancePalette->IsShowing());
            return;

        case WOWEDITCMD_WINDOW_TOOL_PALETTE:
            this->m_ui->toolPalette->Show(!this->m_ui->toolPalette->IsShowing());
            return;

        default:
            if (this->m_session.DoCommand(command)) {
                this->RefreshPalettes();
            }
            return;
    }
}

int32_t CWowEditApp::ConfirmClose(void* param) {
    auto* app = static_cast<CWowEditApp*>(param);
    CWowEditAreaEdits& edits = app->m_session.AreaEdits();
    if (edits.HasPendingActions()) {
        return 0;
    }
    if (!edits.HasUnsavedChanges()) {
        return 1;
    }

    const int32_t choice = OsGuiMessageBox(app->m_window, OSGUI_MESSAGE_STYLE_YESNOCANCEL,
        "Save the current map's area edits before closing?", "World of Warcraft Editor");
    if (choice == OSGUI_MESSAGE_RESULT_YESOK) {
        if (edits.Save()) {
            return 1;
        }

        OsGuiMessageBox(app->m_window, OSGUI_MESSAGE_STYLE_OK,
            "Area edits could not be saved. Check the console for details.", "Save Area Edits");
    }
    return choice == OSGUI_MESSAGE_RESULT_NO;
}

// OsGui menu command callback: type is the menu id, subType the item position.
void CWowEditApp::OnMenuCommand(const OsGuiCallbackParams& params) {
    CWowEditApp* app = static_cast<CWowEditApp*>(params.user);
    int32_t command = WowEditFindMenuCommand(static_cast<uint8_t>(params.type), params.subType);
    app->DoMenuCommand(command);
}

#endif
