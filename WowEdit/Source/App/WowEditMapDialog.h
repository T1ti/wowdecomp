#ifndef WOWEDIT_SOURCE_APP_WOWEDITMAPDIALOG_H
#define WOWEDIT_SOURCE_APP_WOWEDITMAPDIALOG_H

// WowEdit application: the Open Map dialog (port; see App/WowEditApp.h).
//
// Every map of Map.dbc in a list (ID, name, directory); Open or a double click loads it at its default start
// (WowEditWorldLoadMap). A COsDialog owned by the main window, created on File > Open Map and kept for later.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class COsDialog;
class COsListBox;
class CWowEditSession;
struct OsGuiCallbackParams;

class CWowEditMapDialog {
    public:
    CWowEditMapDialog(void* parentWindow, CWowEditSession& session);
    ~CWowEditMapDialog();

    // Shows the dialog with the current map selected.
    void Open();

    private:
    static void OnEvent(const OsGuiCallbackParams& params);
    void HandleEvent(int32_t controlId, int32_t event);
    void LoadSelected();

    CWowEditSession& m_session;
    COsDialog* m_dialog;
    COsListBox* m_list;
    TSGrowableArray<int32_t> m_mapIds; // per list row
};

#endif

#endif
