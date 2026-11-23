#include "WowEdit/Source/App/WowEditDistancePalette.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdio>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditSlider.h"

// See WowEditDistancePalette.h.

// Control ids (OsGui reserves ids below 10): a text and a slider per distance.
static const int16_t ID_FIRST_TEXT = 10;
static const int16_t ID_FIRST_SLIDER = 20;

// OsGui control events (COsDialog callback subType).
static const int32_t EVENT_CLOSE = 6;
static const int32_t EVENT_ACTIVATE = 18;

static int32_t ToSlider(const WowEditDistanceInfo& info, float value) {
    return static_cast<int32_t>(value * info.sliderScale + 0.5f);
}

CWowEditDistancePalette::CWowEditDistancePalette(void* parentWindow) {
    this->m_refreshing = false;

    // Flag 8: the close box. Not modal.
    this->m_dialog = new COsDialog(parentWindow, 8);
    this->m_dialog->SetTitle("Render Distance");
    this->m_dialog->SetCallback(CWowEditDistancePalette::OnEvent, this);

    const int32_t left = 10;
    const int32_t width = 260;
    int32_t y = 10;

    for (int32_t i = 0; i < NUM_WOWEDITDISTANCES; i++) {
        const WowEditDistanceInfo& info = WowEditGetDistanceInfo(i);

        this->m_texts[i] = new COsStaticText(this->m_dialog, static_cast<int16_t>(ID_FIRST_TEXT + i), 0);
        this->m_texts[i]->SetFont(0);
        this->m_texts[i]->SetPosition(left, y);
        this->m_texts[i]->SetSize(width, 16);
        y += 18;

        this->m_sliders[i] = new CWowEditSlider(this->m_dialog, static_cast<int16_t>(ID_FIRST_SLIDER + i));
        this->m_sliders[i]->SetPosition(left - 4, y);
        this->m_sliders[i]->SetSize(width + 8, 26);
        this->m_sliders[i]->SetMinValue(ToSlider(info, info.min));
        this->m_sliders[i]->SetMaxValue(ToSlider(info, info.max));
        y += 34;
    }

    // Outer size: the controls plus the frame and caption.
    this->m_dialog->SetSize(left + width + 26, y + 40);

    // Top right of the main window.
    CiRect parentRect = OsGuiGetWindowRect(parentWindow, 1);
    this->m_dialog->SetPosition(parentRect.maxX - (left + width + 26) - 16, parentRect.minY + 16);

    this->Refresh();
}

CWowEditDistancePalette::~CWowEditDistancePalette() {
    // The dialog deletes its controls.
    delete this->m_dialog;
}

void CWowEditDistancePalette::Show(bool show) {
    if (show) {
        this->Refresh();
    }

    this->m_dialog->Show(show);
}

bool CWowEditDistancePalette::IsShowing() {
    return this->m_dialog->IsShowing() != 0;
}

void CWowEditDistancePalette::Refresh() {
    this->m_refreshing = true;

    for (int32_t i = 0; i < NUM_WOWEDITDISTANCES; i++) {
        float value = WowEditGetDistance(i);
        this->m_sliders[i]->SetValue(ToSlider(WowEditGetDistanceInfo(i), value));
        this->UpdateText(i, value);
    }

    this->m_refreshing = false;
}

void CWowEditDistancePalette::UpdateText(int32_t distance, float value) {
    char text[96];
    snprintf(text, sizeof(text), WowEditGetDistanceInfo(distance).label, value);
    this->m_texts[distance]->SetText(text);
}

void CWowEditDistancePalette::OnEvent(const OsGuiCallbackParams& params) {
    static_cast<CWowEditDistancePalette*>(params.user)->HandleEvent(params.type, params.subType);
}

void CWowEditDistancePalette::HandleEvent(int32_t controlId, int32_t event) {
    if (controlId == -1) {
        if (event == EVENT_CLOSE) {
            // The close box hides the palette; Window > Render Distance brings it back.
            this->m_dialog->Show(0);
        } else if (event == EVENT_ACTIVATE) {
            this->Refresh();
        }

        return;
    }

    int32_t distance = controlId - ID_FIRST_SLIDER;

    if (this->m_refreshing || event != WOWEDITSLIDER_EVENT_CHANGED || distance < 0 || distance >= NUM_WOWEDITDISTANCES) {
        return;
    }

    CWowEditSlider* slider = this->m_sliders[distance];
    float value = static_cast<float>(slider->GetValue()) / WowEditGetDistanceInfo(distance).sliderScale;
    this->UpdateText(distance, value);

    // The view distance rebuilds the terrain buffers: once the thumb is let go, not at every step of the drag.
    if (distance == WOWEDITDISTANCE_VIEW && slider->m_lastCode == WOWEDITSLIDER_THUMBTRACK) {
        return;
    }

    WowEditSetDistance(distance, value);
}

#endif
