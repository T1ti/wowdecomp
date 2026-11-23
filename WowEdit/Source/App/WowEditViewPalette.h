#ifndef WOWEDIT_SOURCE_APP_WOWEDITVIEWPALETTE_H
#define WOWEDIT_SOURCE_APP_WOWEDITVIEWPALETTE_H

// WowEdit application: the View palette (port; see App/WowEditApp.h).
//
// WoWEdit's bottom toolbar of view toggles (WowEditViewToggles.h) as a floating COsDialog: one toggle button per
// switch (OsGui checkboxes drawn as push buttons, BS_PUSHLIKE), each with the WoWEdit tooltip it stands for, and a
// "Game View" button for "Toggle view settings to match in game view". The 0.5.3 OsGui toolbar control wants image
// lists for its buttons, and the WoWEdit icons are not known, so the buttons carry their names.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class COsCheckbox;
class COsDialog;
struct OsGuiCallbackParams;

class CWowEditViewPalette {
    public:
    explicit CWowEditViewPalette(void* parentWindow);
    ~CWowEditViewPalette();

    void Show(bool show);
    bool IsShowing();
    // Reads the switches back (they may have changed from the menu or the console).
    void Refresh();

    private:
    static void OnEvent(const OsGuiCallbackParams& params);
    void HandleEvent(int32_t controlId, int32_t event);

    COsDialog* m_dialog;
    TSGrowableArray<COsCheckbox*> m_buttons; // one per WowEditGetViewToggles entry
    bool m_refreshing;
};

#endif

#endif
