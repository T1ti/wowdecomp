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
//   WowEdit64.exe (Tools/WowEditApp)    the client executable plus WowEditAppMain.cpp, which calls
//                                       WowEditAppEnable before main; the plain client never enables the app.
//   WowEditWorldView                    the world drawn without the game, the free camera, map loading.
//   CWowEditApp (this file)             attaches the editor to the Gx window: OsGui setup, the window procedure
//                                       hook, the menu bar, the palettes. The Gx window is the main window, as
//                                       OsGui expects (OSGUI_WINDOW_TYPE_GX); the 3D view is the game's world frame.
//   CWowEditCommander (WowEditCommands.h)  the command table (menu, item text, hotkey) and dispatch to modules.
//   CWowEditModule (WowEditModule.h)    one editor each (view, terrain, ...): runs its commands, reports their
//                                       menu state, owns its palettes.
//   CWowEditUndoer (WowEditUndo.h)      the undo and redo stacks; a command that changes data returns a
//                                       CWowEditUndoable.
//   CWowEditToolPalette (WowEditToolPalette.h)  the floating terrain tool palette (brush), a COsDialog.
//
// Hookup with the client, which knows nothing of the app:
//   - WowEditRegisterConsoleCommands (from ClientRegisterConsoleCommands) calls WowEditAppStartup, which sets
//     OsSetWindowProc: OsWindowProc, the Gx window procedure, hands the messages it does not keep to the app.
//   - The app attaches on the first message to the window OsGui knows as the Gx window (ConsoleDevice sets it with
//     OsGuiSetGxWindow after creating the device), and again if the device makes a new window.
//   - OsInputGet already passes every message to OsGuiProcessMessage (menu hotkeys, dialog keyboard navigation).
//   - Menu commands arrive as WM_COMMAND through OsGuiWindowProc and the OsGui menu command callback.
//
// The client runs as a tool (WoW/Source/ClientTool.h): WowEditAppEnable reads WowEdit.ini (WowEditSettings.h: the
// game folder, the start map and position) and hands the client its tool description, so no login screen, network,
// interface or game objects are started; the editor draws the world itself around a free camera
// (WowEditWorldView.h), with WoWEdit's view toggles (WowEditViewToggles.h) and a map list (WowEditMapDialog.h).
// The editor keeps its own cvars, a windowed device by default: a fullscreen device would cover the menu bar.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class CWowEditCommander;
class CWowEditUndoer;

// From the WowEdit executable, before main: this run is the editor.
void WowEditAppEnable();
bool WowEditAppEnabled();

// From WowEditRegisterConsoleCommands: hooks the Gx window procedure when the app is enabled.
void WowEditAppStartup();

class CWowEditApp {
    public:
    static CWowEditApp* Get(); // nullptr until the app starts

    CWowEditCommander* GetCommander();
    CWowEditUndoer* GetUndoer();
    void* GetMainWindow(); // the Gx window the editor is attached to, or nullptr

    // Window procedure chained after OsWindowProc (see OsSetWindowProc).
    static intptr_t WindowProc(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam);

    private:
    friend void WowEditAppStartup();
    CWowEditApp();

    void Attach(void* window);
    void Detach();
    void UpdateMenus();

    static void OnMenuCommand(const struct OsGuiCallbackParams& params);

    void* m_window;
    bool m_attachPending;
    struct WowEditAppMenus* m_menus;
    CWowEditCommander* m_commander;
    CWowEditUndoer* m_undoer;
};

#endif

#endif
