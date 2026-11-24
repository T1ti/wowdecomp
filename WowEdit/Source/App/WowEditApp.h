#ifndef WOWEDIT_SOURCE_APP_WOWEDITAPP_H
#define WOWEDIT_SOURCE_APP_WOWEDITAPP_H

// WowEdit application: the world editor as a program of its own, its window chrome built with OsGui (port; no
// client keeps any of it), built only with WOWDECOMP_TOOL_FEATURES.
//
// Blizzard's tools linked the same engine as the game. OsGui (Engine/Source/Os/Gui.h, the 0.5.3 OsGui.cpp ported in
// Os/GuiToolkit.inl) is that engine's Win32 interface layer: menu bars with hotkeys, dialogs, tree views, sliders.
// The game never calls it; the Warcraft III World Editor, built on the same engine, does, and this application
// follows its layout (War3\Tools\WorldEdit\Source: CWorldEditApp, CWorldEditFrame, CCommander, CWEModule,
// CUndoable, CToolPalette):
//
//   WowEdit64.exe (Tools/WowEditApp)    the editor entry point, with startup and shutdown in WowEditAppStartup.cpp.
//   CWowEditSession                     owns the viewport and command/undo state independently of OsGui.
//   WowEditWorldView                    the world drawn without the game, the free camera, map loading.
//   CWowEditApp (this file)             attaches the editor to the Gx window: OsGui setup, the window procedure
//                                       hook, the menu bar, the palettes. The Gx window is the main window, as
//                                       OsGui expects (OSGUI_WINDOW_TYPE_GX); the 3D view belongs to the editor.
//   WowEditMenus                        the OsGui menu table and keyboard shortcuts.
//   CWowEditCommander (WowEditCommands.h)  dispatches editor commands to modules.
//   CWowEditModule (WowEditModule.h)    runs commands and reports state for each editor tool (view, terrain, ...).
//   CWowEditUndoer (WowEditUndo.h)      the undo and redo stacks; a command that changes data returns a
//                                       CWowEditUndoable.
//   CWowEditToolPalette (WowEditToolPalette.h)  the floating terrain tool palette (brush), a COsDialog.
//
// Window integration:
//   - The editor initialization callback calls OsSetWindowProc to receive messages forwarded by the Gx window.
//   - The app posts an attach message to the window registered by ConsoleDevice, and reattaches if the device
//     makes a new window.
//   - OsInputGet already passes every message to OsGuiProcessMessage (menu hotkeys, dialog keyboard navigation).
//   - Menu commands arrive as WM_COMMAND through OsGuiWindowProc and the OsGui menu command callback.
//
// The editor reads the data folder and start position from WowEdit.ini (WowEditSettings.h) and draws a free-camera
// viewport (WowEditWorldView.h), with view toggles (WowEditViewToggles.h) and a map list (WowEditMapDialog.h).
// The editor keeps its own cvars, a windowed device by default: a fullscreen device would cover the menu bar.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class CWowEditSession;

class CWowEditApp {
    public:
    static int32_t Run();
    // Window procedure chained after OsWindowProc (see OsSetWindowProc).
    static intptr_t WindowProc(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam);

    private:
    static int32_t InitializeContext(const void* data, void* param);
    static int32_t DestroyContext(const void* data, void* param);
    static void OpenShell(CWowEditSession& session);
    static void CloseShell();
    static void SetViewportCursor(bool looking);
    explicit CWowEditApp(CWowEditSession& session);

    void Attach(void* window);
    void Detach();
    void UpdateMenus();
    void RefreshPalettes();
    void DoMenuCommand(int32_t command);
    void GetMenuCommandStatus(int32_t command, bool* enabled, bool* checked);

    static int32_t ConfirmClose(void* param);
    static void OnMenuCommand(const struct OsGuiCallbackParams& params);

    void* m_window;
    bool m_attachPending;
    struct WowEditAppUi* m_ui;
    CWowEditSession& m_session;
};

#endif

#endif
