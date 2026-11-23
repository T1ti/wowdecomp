#ifndef WOWEDIT_SOURCE_APP_WOWEDITSLIDER_H
#define WOWEDIT_SOURCE_APP_WOWEDITSLIDER_H

// WowEdit application: a slider for the palettes (port; see App/WowEditApp.h).
//
// OsGui reports a dialog's control events to the dialog callback, but the 0.5.3 notification table maps no trackbar
// code, so this trackbar reports its scroll codes itself, as a change event (subType 2). The last scroll code is kept
// so a palette can wait for the thumb to be let go before an expensive change (WOWEDITSLIDER_THUMBTRACK while it is
// dragged).

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Os/Gui.h"

static const int32_t WOWEDITSLIDER_EVENT_CHANGED = 2;
static const int32_t WOWEDITSLIDER_THUMBTRACK = 5; // TB_THUMBTRACK

class CWowEditSlider : public COsSlider {
    public:
    CWowEditSlider(COsDialog* dialog, int16_t id) : COsSlider(dialog, id) {
        this->m_lastCode = 0;
    }

    int32_t OnScroll(int32_t code) override {
        this->m_lastCode = code;
        this->SendEvent(WOWEDITSLIDER_EVENT_CHANGED, 0);
        return 1;
    }

    int32_t m_lastCode;
};

#endif

#endif
