#include "WowEdit/Source/App/WowEditModule.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditApp.h"
#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditMapDialog.h"
#include "WowEdit/Source/App/WowEditToolPalette.h"
#include "WowEdit/Source/App/WowEditViewPalette.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"
#include "WowEdit/Source/App/WowEditWorldView.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditModule.h.

// ---- File and Help

class CWowEditAppModule : public CWowEditModule {
    public:
    CWowEditAppModule() {
        this->m_mapDialog = nullptr;
    }

    ~CWowEditAppModule() override {
        this->OnDetach();
    }

    const char* GetName() override {
        return "App";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        void* window = CWowEditApp::Get()->GetMainWindow();

        switch (command) {
            case WOWEDITCMD_OPEN_MAP:
                if (this->m_mapDialog) {
                    this->m_mapDialog->Open();
                }

                return true;

            case WOWEDITCMD_EXIT:
                // As the close box: OsWindowProc queues OS_INPUT_CLOSE and the client shuts down the usual way.
                PostMessageA(static_cast<HWND>(window), WM_SYSCOMMAND, SC_CLOSE, 0);
                return true;

            case WOWEDITCMD_ABOUT:
                OsGuiMessageBox(window, OSGUI_MESSAGE_STYLE_OK,
                    "World of Warcraft Editor\n\n"
                    "A reconstruction of Blizzard's world editor on the 3.3.5 client engine: the world drawn on its own, "
                    "its window built with OsGui, the engine's tool interface layer.\n\n"
                    "Camera: hold the right mouse button to look, W A S D to move, E and Q up and down, Shift faster, "
                    "the mouse wheel for the speed.",
                    "About WowEdit");
                return true;

            default:
                return false;
        }
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        switch (command) {
            case WOWEDITCMD_OPEN_MAP:
                *enabled = this->m_mapDialog != nullptr;
                return true;

            case WOWEDITCMD_EXIT:
            case WOWEDITCMD_ABOUT:
                return true;

            default:
                return false;
        }
    }

    void OnAttach(void* mainWindow) override {
        this->m_mapDialog = new CWowEditMapDialog(mainWindow);
    }

    void OnDetach() override {
        delete this->m_mapDialog;
        this->m_mapDialog = nullptr;
    }

    private:
    CWowEditMapDialog* m_mapDialog;
};

// ---- View: WoWEdit's view toggles, the time of day, the View palette

class CWowEditViewModule : public CWowEditModule {
    public:
    CWowEditViewModule() {
        this->m_palette = nullptr;
    }

    ~CWowEditViewModule() override {
        this->OnDetach();
    }

    const char* GetName() override {
        return "View";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        int32_t time = TimeForCommand(command);

        if (time >= 0) {
            WowEditWorldSetTimeOfDay(time);
            return true;
        }

        if (command == WOWEDITCMD_VIEW_GAME) {
            WowEditMatchGameView();
            this->OnRefresh();
            return true;
        }

        if (command == WOWEDITCMD_WINDOW_VIEW_PALETTE) {
            if (this->m_palette) {
                this->m_palette->Show(!this->m_palette->IsShowing());
            }

            return true;
        }

        const WowEditViewToggle* toggle = WowEditFindViewToggle(command);

        if (!toggle) {
            return false;
        }

        toggle->set(!toggle->get());
        this->OnRefresh();
        return true;
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        int32_t time = TimeForCommand(command);

        if (time >= 0) {
            *checked = WowEditWorldGetTimeOfDay() == time;
            return true;
        }

        if (command == WOWEDITCMD_VIEW_GAME) {
            return true;
        }

        if (command == WOWEDITCMD_WINDOW_VIEW_PALETTE) {
            *enabled = this->m_palette != nullptr;
            *checked = this->m_palette && this->m_palette->IsShowing();
            return true;
        }

        const WowEditViewToggle* toggle = WowEditFindViewToggle(command);

        if (!toggle) {
            return false;
        }

        *checked = toggle->get();
        return true;
    }

    void OnAttach(void* mainWindow) override {
        this->m_palette = new CWowEditViewPalette(mainWindow);
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
    static int32_t TimeForCommand(int32_t command) {
        switch (command) {
            case WOWEDITCMD_TIME_DAWN:
                return 6 * 60;
            case WOWEDITCMD_TIME_NOON:
                return 12 * 60;
            case WOWEDITCMD_TIME_DUSK:
                return 19 * 60;
            case WOWEDITCMD_TIME_MIDNIGHT:
                return 0;
            default:
                return -1;
        }
    }

    CWowEditViewPalette* m_palette;
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
