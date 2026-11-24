#ifndef WOWEDIT_SOURCE_APP_WOWEDITTOOLPALETTE_H
#define WOWEDIT_SOURCE_APP_WOWEDITTOOLPALETTE_H

// WowEdit application: the terrain tool palette (port; see App/WowEditApp.h).
//
// A floating COsDialog owned by the Gx window, after the War3 World Editor's tool palette (CToolPalette: brush shape
// and size beside the tool buttons). For now it holds the brush cursor of the WowEdit library
// (WowEditTerrainSetBrush): on/off, radius in yards, inner (falloff) circle as a
// share of the radius. Tool buttons (raise, lower, flatten, smooth, paint) come with the first editing tool.
//
// OsGui reports a dialog's control events to the dialog callback (COsDialog::SetCallback): type is the control id,
// subType the event (2 for a checkbox click, 6 for the close box). The 0.5.3 notification table maps no trackbar
// code, so the sliders here report their scroll codes themselves (CWowEditSlider).

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class COsCheckbox;
class COsDialog;
class COsStaticText;
class CWowEditSlider;
struct OsGuiCallbackParams;

class CWowEditToolPalette {
    public:
    explicit CWowEditToolPalette(void* parentWindow);
    ~CWowEditToolPalette();

    void Show(bool show);
    bool IsShowing();
    // Reads the state back after a command changes the brush.
    void Refresh();

    private:
    static void OnEvent(const OsGuiCallbackParams& params);
    void HandleEvent(int32_t controlId, int32_t event);
    void UpdateTexts();

    COsDialog* m_dialog;
    COsCheckbox* m_brushOn;
    COsStaticText* m_radiusText;
    CWowEditSlider* m_radius;
    COsStaticText* m_innerText;
    CWowEditSlider* m_inner;
    bool m_refreshing;
};

#endif

#endif
