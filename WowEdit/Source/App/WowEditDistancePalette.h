#ifndef WOWEDIT_SOURCE_APP_WOWEDITDISTANCEPALETTE_H
#define WOWEDIT_SOURCE_APP_WOWEDITDISTANCEPALETTE_H

// WowEdit application: the Render Distance palette (port; see App/WowEditApp.h).
//
// A floating COsDialog with one slider per render distance (WowEditRenderDistance.h). The view distance is applied
// when its slider is let go: each new far clip rebuilds the terrain chunk buffers and reloads the area of interest.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "WowEdit/Source/App/WowEditRenderDistance.h"

class COsDialog;
class COsStaticText;
class CWowEditSlider;
struct OsGuiCallbackParams;

class CWowEditDistancePalette {
    public:
    explicit CWowEditDistancePalette(void* parentWindow);
    ~CWowEditDistancePalette();

    void Show(bool show);
    bool IsShowing();
    void Refresh();

    private:
    static void OnEvent(const OsGuiCallbackParams& params);
    void HandleEvent(int32_t controlId, int32_t event);
    void UpdateText(int32_t distance, float value);

    COsDialog* m_dialog;
    COsStaticText* m_texts[NUM_WOWEDITDISTANCES];
    CWowEditSlider* m_sliders[NUM_WOWEDITDISTANCES];
    bool m_refreshing;
};

#endif

#endif
