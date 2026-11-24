#include "WowEdit/Source/App/WowEditMapDialog.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdio>
#include "Engine/Source/Os/Gui.h"
#include "WowEdit/Source/App/WowEditMapSession.h"
#include "WowEdit/Source/App/WowEditSession.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

// See WowEditMapDialog.h.

enum : int16_t {
    ID_LIST = 10,
    ID_OPEN = 11,
    ID_CANCEL = 12,
};

// OsGui control events (COsDialog callback subType).
static const int32_t EVENT_CLICK = 0;
static const int32_t EVENT_DOUBLE_CLICK = 1;
static const int32_t EVENT_CLOSE = 6;

CWowEditMapDialog::CWowEditMapDialog(void* parentWindow, CWowEditSession& session) : m_session(session) {
    // Flag 8: the close box. Not modal: the world keeps drawing behind it.
    this->m_dialog = new COsDialog(parentWindow, 8);
    this->m_dialog->SetTitle("Open Map");
    this->m_dialog->SetCallback(CWowEditMapDialog::OnEvent, this);

    const int32_t width = 420;
    const int32_t listHeight = 360;

    this->m_list = new COsListBox(this->m_dialog, ID_LIST, 0);
    this->m_list->SetFont(0);
    this->m_list->SetPosition(8, 8);
    this->m_list->SetSize(width, listHeight);

    for (uint32_t i = 0; i < CWowEditMapSession::GetMapCount(); i++) {
        const WowEditMapInfo map = CWowEditMapSession::GetMapInfo(i);
        char text[256];
        snprintf(text, sizeof(text), "%4d   %s   (%s)", map.id, map.name, map.directory);
        this->m_list->InsertItem(text, -1);
        *this->m_mapIds.New() = map.id;
    }

    COsButton* open = new COsButton(this->m_dialog, ID_OPEN, 0);
    open->SetFont(0);
    open->SetText("Open");
    open->SetPosition(width - 168, listHeight + 16);
    open->SetSize(80, 24);
    open->SetDefaultButton();

    COsButton* cancel = new COsButton(this->m_dialog, ID_CANCEL, 0);
    cancel->SetFont(0);
    cancel->SetText("Cancel");
    cancel->SetPosition(width - 80, listHeight + 16);
    cancel->SetSize(80, 24);

    this->m_dialog->SetSize(width + 32, listHeight + 88);

    CiRect parentRect = OsGuiGetWindowRect(parentWindow, 1);
    this->m_dialog->SetPosition((parentRect.minX + parentRect.maxX) / 2 - (width + 32) / 2, parentRect.minY + 60);
}

CWowEditMapDialog::~CWowEditMapDialog() {
    delete this->m_dialog;
}

void CWowEditMapDialog::Open() {
    int32_t current = WowEditWorldGetMapId();

    for (uint32_t i = 0; i < this->m_mapIds.Count(); i++) {
        if (this->m_mapIds[i] == current) {
            this->m_list->SetValue(static_cast<int32_t>(i));
            break;
        }
    }

    this->m_dialog->Show(1);
    this->m_dialog->BringToFront();
}

void CWowEditMapDialog::LoadSelected() {
    int32_t row = this->m_list->GetValue();

    if (row < 0 || static_cast<uint32_t>(row) >= this->m_mapIds.Count()) {
        return;
    }

    if (this->m_session.AreaEdits().HasUnsavedChanges() || this->m_session.AreaEdits().HasPendingActions()) {
        OsGuiMessageBox(this->m_dialog->m_handle, OSGUI_MESSAGE_STYLE_OK,
            "Save the current map's area edits before opening another map.", "Open Map");
        return;
    }

    this->m_dialog->Show(0);
    if (!this->m_session.OpenMap(this->m_mapIds[row], nullptr)) {
        this->m_dialog->Show(1);
        OsGuiMessageBox(this->m_dialog->m_handle, OSGUI_MESSAGE_STYLE_OK,
            "The selected map could not be opened from the configured game data. See the console for details.",
            "Open Map");
    }
}

void CWowEditMapDialog::OnEvent(const OsGuiCallbackParams& params) {
    static_cast<CWowEditMapDialog*>(params.user)->HandleEvent(params.type, params.subType);
}

void CWowEditMapDialog::HandleEvent(int32_t controlId, int32_t event) {
    if ((controlId == -1 && event == EVENT_CLOSE) || (controlId == ID_CANCEL && event == EVENT_CLICK)) {
        this->m_dialog->Show(0);
    } else if ((controlId == ID_OPEN && event == EVENT_CLICK) || (controlId == ID_LIST && event == EVENT_DOUBLE_CLICK)) {
        this->LoadSelected();
    }
}

#endif
