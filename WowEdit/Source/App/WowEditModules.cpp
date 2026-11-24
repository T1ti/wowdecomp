#include "WowEdit/Source/App/WowEditModule.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "WowEdit/Source/App/WowEditCommands.h"
#include "WowEdit/Source/App/WowEditSettings.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"
#include "WowEdit/Source/App/WowEditWorldView.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditModule.h.

// ---- View: WoWEdit's view toggles and time of day

class CWowEditViewModule : public CWowEditModule {
    public:
    const char* GetName() override {
        return "View";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        int32_t time = TimeForCommand(command);

        if (time >= 0) {
            WowEditWorldSetTimeOfDay(time);
            return true;
        }

        if (command == WOWEDITCMD_VIEW_AZERTY) {
            WowEditWorldSetAzerty(!WowEditWorldGetAzerty());
            WowEditSaveKeyboard(WowEditWorldGetAzerty());
            return true;
        }

        if (command == WOWEDITCMD_VIEW_GAME) {
            WowEditMatchGameView();
            return true;
        }

        const WowEditViewToggle* toggle = WowEditFindViewToggle(command);

        if (!toggle) {
            return false;
        }

        toggle->set(!toggle->get());
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

        if (command == WOWEDITCMD_VIEW_AZERTY) {
            *checked = WowEditWorldGetAzerty();
            return true;
        }

        const WowEditViewToggle* toggle = WowEditFindViewToggle(command);

        if (!toggle) {
            return false;
        }

        *checked = toggle->get();
        return true;
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
};

// ---- Terrain tools

class CWowEditTerrainModule : public CWowEditModule {
    public:
    const char* GetName() override {
        return "Terrain";
    }

    bool DoCommand(int32_t command, CWowEditUndoable** undoable) override {
        if (command != WOWEDITCMD_TOOL_BRUSH) {
            return false;
        }

        WowEditBrushSettings brush = WowEditTerrainGetBrush();
        brush.enabled = !brush.enabled;
        WowEditTerrainSetBrush(brush);
        return true;
    }

    bool GetCommandStatus(int32_t command, bool* enabled, bool* checked) override {
        if (command != WOWEDITCMD_TOOL_BRUSH) {
            return false;
        }

        *checked = WowEditTerrainGetBrush().enabled;
        return true;
    }
};

CWowEditModule* WowEditCreateViewModule() {
    return new CWowEditViewModule();
}

CWowEditModule* WowEditCreateTerrainModule() {
    return new CWowEditTerrainModule();
}

#endif
