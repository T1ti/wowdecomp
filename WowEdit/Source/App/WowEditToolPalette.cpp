#include "WowEdit/Source/App/WowEditToolPalette.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdio>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditSlider.h"
#include "WowEdit/Source/WowEdit.h"
#include "WowEdit/Source/WowEditTerrain.h"

// See WowEditToolPalette.h.

const float CWowEditToolPalette::DEFAULT_RADIUS = 10.0f;
const float CWowEditToolPalette::DEFAULT_INNER = 0.5f;

// The brush size while the brush is off, so turning it back on keeps it.
static float s_radius = CWowEditToolPalette::DEFAULT_RADIUS;
static float s_inner = CWowEditToolPalette::DEFAULT_INNER;

static const int32_t RADIUS_MIN = 1; // yards
static const int32_t RADIUS_MAX = 100;

// Control ids (OsGui reserves ids below 10).
enum : int16_t {
    ID_BRUSH_ON = 10,
    ID_RADIUS_TEXT = 11,
    ID_RADIUS = 12,
    ID_INNER_TEXT = 13,
    ID_INNER = 14,
};

// OsGui control events (COsDialog callback subType).
static const int32_t EVENT_CHANGED = 2;
static const int32_t EVENT_CLOSE = 6;
static const int32_t EVENT_ACTIVATE = 18;

CWowEditToolPalette::CWowEditToolPalette(void* parentWindow) {
    this->m_refreshing = false;

    // Flag 8: a system menu (the close box). Not modal (flag 4), so the world stays usable.
    this->m_dialog = new COsDialog(parentWindow, 8);
    this->m_dialog->SetTitle("Tool Palette");
    this->m_dialog->SetCallback(CWowEditToolPalette::OnEvent, this);

    const int32_t left = 10;
    const int32_t width = 190;
    int32_t y = 10;

    this->m_brushOn = new COsCheckbox(this->m_dialog, ID_BRUSH_ON);
    this->m_brushOn->SetFont(0);
    this->m_brushOn->SetText("Terrain brush");
    this->m_brushOn->SetPosition(left, y);
    y += 26;

    this->m_radiusText = new COsStaticText(this->m_dialog, ID_RADIUS_TEXT, 0);
    this->m_radiusText->SetFont(0);
    this->m_radiusText->SetPosition(left, y);
    this->m_radiusText->SetSize(width, 16);
    y += 18;

    this->m_radius = new CWowEditSlider(this->m_dialog, ID_RADIUS);
    this->m_radius->SetPosition(left - 4, y);
    this->m_radius->SetSize(width + 8, 26);
    this->m_radius->SetMinValue(RADIUS_MIN);
    this->m_radius->SetMaxValue(RADIUS_MAX);
    y += 34;

    this->m_innerText = new COsStaticText(this->m_dialog, ID_INNER_TEXT, 0);
    this->m_innerText->SetFont(0);
    this->m_innerText->SetPosition(left, y);
    this->m_innerText->SetSize(width, 16);
    y += 18;

    this->m_inner = new CWowEditSlider(this->m_dialog, ID_INNER);
    this->m_inner->SetPosition(left - 4, y);
    this->m_inner->SetSize(width + 8, 26);
    this->m_inner->SetMinValue(0);
    this->m_inner->SetMaxValue(100);
    y += 34;

    // Outer size: the controls plus the frame and caption.
    this->m_dialog->SetSize(left + width + 26, y + 40);

    CiRect parentRect = OsGuiGetWindowRect(parentWindow, 1);
    this->m_dialog->SetPosition(parentRect.minX + 16, parentRect.minY + 16);

    this->Refresh();
}

CWowEditToolPalette::~CWowEditToolPalette() {
    // The dialog deletes its controls.
    delete this->m_dialog;
}

void CWowEditToolPalette::Show(bool show) {
    if (show) {
        this->Refresh();
    }

    this->m_dialog->Show(show);
}

bool CWowEditToolPalette::IsShowing() {
    return this->m_dialog->IsShowing() != 0;
}

void CWowEditToolPalette::Refresh() {
    float radius = WowEditTerrainGetBrush();

    if (radius > 0.0f) {
        s_radius = radius;
        s_inner = WowEditTerrainGetBrushInner() / radius;
    }

    this->m_refreshing = true;
    this->m_brushOn->SetValue(radius > 0.0f);
    this->m_radius->SetValue(static_cast<int32_t>(s_radius + 0.5f));
    this->m_inner->SetValue(static_cast<int32_t>(s_inner * 100.0f + 0.5f));
    this->m_refreshing = false;

    this->UpdateTexts();
}

void CWowEditToolPalette::UpdateTexts() {
    char text[64];

    snprintf(text, sizeof(text), "Radius: %.0f yards", s_radius);
    this->m_radiusText->SetText(text);

    snprintf(text, sizeof(text), "Inner circle: %.0f%% of the radius", s_inner * 100.0f);
    this->m_innerText->SetText(text);
}

void CWowEditToolPalette::ToggleBrush() {
    if (WowEditTerrainGetBrush() > 0.0f) {
        WowEditTerrainSetBrush(0.0f, 0.0f);
    } else {
        WowEditTerrainSetBrush(s_radius, s_radius * s_inner);
        WowEditPrepareTerrainOverlays();
    }
}

void CWowEditToolPalette::ApplyBrush() {
    if (this->m_brushOn->GetValue()) {
        bool wasOn = WowEditTerrainGetBrush() > 0.0f;
        WowEditTerrainSetBrush(s_radius, s_radius * s_inner);

        if (!wasOn) {
            WowEditPrepareTerrainOverlays();
        }
    } else {
        WowEditTerrainSetBrush(0.0f, 0.0f);
    }
}

void CWowEditToolPalette::OnEvent(const OsGuiCallbackParams& params) {
    static_cast<CWowEditToolPalette*>(params.user)->HandleEvent(params.type, params.subType);
}

void CWowEditToolPalette::HandleEvent(int32_t controlId, int32_t event) {
    if (controlId == -1 && event == EVENT_CLOSE) {
        // The close box hides the palette; Window > Tool Palette brings it back.
        this->m_dialog->Show(0);
        return;
    }

    if (controlId == -1 && event == EVENT_ACTIVATE) {
        // The brush may have changed from the console or the menu meanwhile.
        this->Refresh();
        return;
    }

    if (this->m_refreshing || event != EVENT_CHANGED) {
        return;
    }

    switch (controlId) {
        case ID_BRUSH_ON:
            this->ApplyBrush();
            break;

        case ID_RADIUS:
            s_radius = static_cast<float>(this->m_radius->GetValue());
            this->UpdateTexts();
            this->ApplyBrush();
            break;

        case ID_INNER:
            s_inner = static_cast<float>(this->m_inner->GetValue()) / 100.0f;
            this->UpdateTexts();
            this->ApplyBrush();
            break;

        default:
            break;
    }
}

#endif
