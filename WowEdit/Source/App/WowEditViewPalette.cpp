#include "WowEdit/Source/App/WowEditViewPalette.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditViewToggles.h"

// See WowEditViewPalette.h.

// Control ids (OsGui reserves ids below 10): the game view button, then one per toggle.
static const int16_t ID_GAME_VIEW = 10;
static const int16_t ID_FIRST_TOGGLE = 20;

// OsGui control events (COsDialog callback subType).
static const int32_t EVENT_CLICK = 0;
static const int32_t EVENT_CHANGED = 2;
static const int32_t EVENT_CLOSE = 6;
static const int32_t EVENT_ACTIVATE = 18;

static const int32_t COLUMNS = 4;
static const int32_t BUTTON_WIDTH = 92;
static const int32_t BUTTON_HEIGHT = 24;
static const int32_t SPACING = 4;
static const int32_t MARGIN = 8;

CWowEditViewPalette::CWowEditViewPalette(void* parentWindow) {
    this->m_refreshing = false;

    this->m_dialog = new COsDialog(parentWindow, 8);
    this->m_dialog->SetTitle("View");
    this->m_dialog->SetCallback(CWowEditViewPalette::OnEvent, this);

    COsButton* gameView = new COsButton(this->m_dialog, ID_GAME_VIEW, 0);
    gameView->SetFont(0);
    gameView->SetText("Game View");
    gameView->SetTooltip("Toggle view settings to match in game view");
    gameView->SetPosition(MARGIN, MARGIN);
    gameView->SetSize(COLUMNS * BUTTON_WIDTH + (COLUMNS - 1) * SPACING, BUTTON_HEIGHT);

    uint32_t count;
    const WowEditViewToggle* toggles = WowEditGetViewToggles(&count);
    int32_t top = MARGIN + BUTTON_HEIGHT + SPACING * 2;

    for (uint32_t i = 0; i < count; i++) {
        COsCheckbox* button = new COsCheckbox(this->m_dialog, static_cast<int16_t>(ID_FIRST_TOGGLE + i));
        HWND hwnd = static_cast<HWND>(button->m_handle);
        SetWindowLongPtrA(hwnd, GWL_STYLE, GetWindowLongPtrA(hwnd, GWL_STYLE) | BS_PUSHLIKE);

        button->SetFont(0);
        button->SetText(toggles[i].label);
        button->SetTooltip(toggles[i].tooltip);
        // After SetText, which sizes a checkbox to its text.
        button->SetPosition(MARGIN + static_cast<int32_t>(i % COLUMNS) * (BUTTON_WIDTH + SPACING), top + static_cast<int32_t>(i / COLUMNS) * (BUTTON_HEIGHT + SPACING));
        button->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
        *this->m_buttons.New() = button;
    }

    int32_t rows = static_cast<int32_t>((count + COLUMNS - 1) / COLUMNS);
    int32_t width = MARGIN * 2 + COLUMNS * BUTTON_WIDTH + (COLUMNS - 1) * SPACING;
    int32_t height = top + rows * (BUTTON_HEIGHT + SPACING) + MARGIN;

    // Outer size: the client area plus the frame and caption.
    this->m_dialog->SetSize(width + 16, height + 40);

    // Along the bottom left of the main window, like WoWEdit's bottom toolbar.
    CiRect parentRect = OsGuiGetWindowRect(parentWindow, 1);
    this->m_dialog->SetPosition(parentRect.minX + 16, parentRect.maxY - height - 56);

    this->Refresh();
}

CWowEditViewPalette::~CWowEditViewPalette() {
    delete this->m_dialog;
}

void CWowEditViewPalette::Show(bool show) {
    if (show) {
        this->Refresh();
    }

    this->m_dialog->Show(show);
}

bool CWowEditViewPalette::IsShowing() {
    return this->m_dialog->IsShowing() != 0;
}

void CWowEditViewPalette::Refresh() {
    uint32_t count;
    const WowEditViewToggle* toggles = WowEditGetViewToggles(&count);

    this->m_refreshing = true;

    for (uint32_t i = 0; i < count && i < this->m_buttons.Count(); i++) {
        this->m_buttons[i]->SetValue(toggles[i].get() ? 1 : 0);
    }

    this->m_refreshing = false;
}

void CWowEditViewPalette::OnEvent(const OsGuiCallbackParams& params) {
    static_cast<CWowEditViewPalette*>(params.user)->HandleEvent(params.type, params.subType);
}

void CWowEditViewPalette::HandleEvent(int32_t controlId, int32_t event) {
    if (controlId == -1) {
        if (event == EVENT_CLOSE) {
            this->m_dialog->Show(0);
        } else if (event == EVENT_ACTIVATE) {
            this->Refresh();
        }

        return;
    }

    if (this->m_refreshing) {
        return;
    }

    if (controlId == ID_GAME_VIEW && event == EVENT_CLICK) {
        WowEditMatchGameView();
        this->Refresh();
        return;
    }

    uint32_t count;
    const WowEditViewToggle* toggles = WowEditGetViewToggles(&count);
    int32_t index = controlId - ID_FIRST_TOGGLE;

    if (event == EVENT_CHANGED && index >= 0 && static_cast<uint32_t>(index) < count) {
        // A three-state checkbox cycles through "mixed"; the switch is on or off.
        bool on = !toggles[index].get();
        toggles[index].set(on);
        this->m_refreshing = true;
        this->m_buttons[index]->SetValue(on ? 1 : 0);
        this->m_refreshing = false;
    }
}

#endif
