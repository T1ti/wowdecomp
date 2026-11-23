#include "WowEdit/Source/App/WowEditModule.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditApp.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditToolPalette.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditModule.h.

// ---- File and Help

class CWowEditAppModule : public CWowEditModule {
    public:
    const char* GetName() override {
        return "App";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        void* window = CWowEditApp::Get()->GetMainWindow();

        switch (command) {
            case WOWEDITCMD_EXIT:
                // As the close box: OsWindowProc queues OS_INPUT_CLOSE and the client shuts down the usual way.
                PostMessageA(static_cast<HWND>(window), WM_SYSCOMMAND, SC_CLOSE, 0);
                return true;

            case WOWEDITCMD_ABOUT:
                OsGuiMessageBox(window, OSGUI_MESSAGE_STYLE_OK,
                    "World of Warcraft Editor\n\n"
                    "A reconstruction of Blizzard's world editor on the 3.3.5 client engine, its window built with "
                    "OsGui, the engine's tool interface layer.",
                    "About WowEdit");
                return true;

            default:
                return false;
        }
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        return command == WOWEDITCMD_EXIT || command == WOWEDITCMD_ABOUT;
    }
};

// ---- View: the terrain overlays and markers

class CWowEditViewModule : public CWowEditModule {
    public:
    const char* GetName() override {
        return "View";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        bool on;

        switch (command) {
            case WOWEDITCMD_VIEW_GRID:
                // As "wowEditGrid" with no argument: permutation 3, the one with every line.
                on = WowEditTerrainGetGrid() == WOWEDITGRID_OFF;
                WowEditTerrainSetGrid(on ? 3 : WOWEDITGRID_OFF);
                break;

            case WOWEDITCMD_VIEW_SLOPES:
                on = !WowEditTerrainGetSlopes();
                WowEditTerrainSetSlopes(on);
                break;

            case WOWEDITCMD_VIEW_AREAS:
                on = WowEditTerrainGetOverlay() != WOWEDITOVERLAY_AREAS;
                WowEditTerrainSetOverlay(on ? WOWEDITOVERLAY_AREAS : WOWEDITOVERLAY_OFF);
                break;

            case WOWEDITCMD_VIEW_MARKERS:
                WowEditSetMarkers(!WowEditGetMarkers());
                return true;

            default:
                return false;
        }

        if (on) {
            WowEditPrepareTerrainOverlays();
        }

        return true;
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        switch (command) {
            case WOWEDITCMD_VIEW_GRID:
                *checked = WowEditTerrainGetGrid() != WOWEDITGRID_OFF;
                return true;

            case WOWEDITCMD_VIEW_SLOPES:
                *checked = WowEditTerrainGetSlopes();
                return true;

            case WOWEDITCMD_VIEW_AREAS:
                *checked = WowEditTerrainGetOverlay() == WOWEDITOVERLAY_AREAS;
                return true;

            case WOWEDITCMD_VIEW_MARKERS:
                *checked = WowEditGetMarkers();
                return true;

            default:
                return false;
        }
    }
};

// ---- Terrain tools

class CWowEditTerrainModule : public CWowEditModule {
    public:
    CWowEditTerrainModule() {
        this->m_palette = nullptr;
    }

    ~CWowEditTerrainModule() override {
        this->OnDetach();
    }

    const char* GetName() override {
        return "Terrain";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        switch (command) {
            case WOWEDITCMD_TOOL_BRUSH:
                CWowEditToolPalette::ToggleBrush();

                if (this->m_palette) {
                    this->m_palette->Refresh();
                }

                return true;

            case WOWEDITCMD_WINDOW_TOOL_PALETTE:
                if (this->m_palette) {
                    this->m_palette->Show(!this->m_palette->IsShowing());
                }

                return true;

            default:
                return false;
        }
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        switch (command) {
            case WOWEDITCMD_TOOL_BRUSH:
                *checked = WowEditTerrainGetBrush() > 0.0f;
                return true;

            case WOWEDITCMD_WINDOW_TOOL_PALETTE:
                *enabled = this->m_palette != nullptr;
                *checked = this->m_palette && this->m_palette->IsShowing();
                return true;

            default:
                return false;
        }
    }

    void OnAttach(void* mainWindow) override {
        this->m_palette = new CWowEditToolPalette(mainWindow);
        this->m_palette->Show(true);
    }

    void OnDetach() override {
        delete this->m_palette;
        this->m_palette = nullptr;
    }

    void OnRefresh() override {
        if (this->m_palette) {
            this->m_palette->Refresh();
        }
    }

    private:
    CWowEditToolPalette* m_palette;
};

CWowEditModule* WowEditCreateAppModule() {
    return new CWowEditAppModule();
}

CWowEditModule* WowEditCreateViewModule() {
    return new CWowEditViewModule();
}

CWowEditModule* WowEditCreateTerrainModule() {
    return new CWowEditTerrainModule();
}

#endif
