#if defined(WOWDECOMP_TOOL_FEATURES)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/Gui.h"

// Tool features: the OsGui Win32 control toolkit.
//
// Source: the 0.5.3 client (WoWClient.exe 3368) links ENGINE\Source\Os\W32\OsGui.cpp whole: menus and menu bars
// with hotkeys, modal dialogs, tooltips, and the COsControl family (buttons, edit boxes, combo boxes, list and tree
// views with drag and drop and in-place editing, sliders, spin buttons, toolbars, dividers...). Nothing in the game
// calls the dialog, menu bar, toolbar or tree view code; it is the interface layer of the in-house tools (model
// viewer, world editor) that linked the same engine. The 3.3.5 release build compiled it out and kept only what the
// game window uses (Win32/Win64 Gui.cpp: OsGuiGetWindow, OsGuiProcessMessage, OsGuiMessageBox, COsDialog message
// routing, the edit-box character filters...). The class layouts and the control class name table (0x00B1D350:
// ToolbarWindow32, SysTreeView32, SysListView32, msctls_*) survive in 3.3.5, so the toolkit fits back unchanged.
//
// The 0.9.1 Mac build only keeps the file name (OsGui.cpp) and 4.1 has none of it.
//
// Every function below is a shell: the signature comes from the 0.5.3 symbol, the comment names the 0.5.3 address
// to restore it from. The 0.5.3 code is plain Win32 (CreateWindowEx, SendMessage, TrackPopupMenu, ImageList_*), so
// it ports to Win64 as is, apart from GWL_USERDATA/LONG casts becoming GWLP_USERDATA/LONG_PTR.

// TOOL: 0.5.3 OsGui.cpp 0x0045A960 (OsGuiWindowProc)
LRESULT OsGuiWindowProc(OsGuiHandle window, uint32_t message, WPARAM wparam, LPARAM lparam) {
    // TODO: restore from 0.5.3 0x0045A960: the window procedure of every OsGui dialog and window (control
    // notifications, owner draw, colors, menu commands, idle timer, mouse tracking), then DefWindowProc.
    return DefWindowProcA(static_cast<HWND>(window), message, wparam, lparam);
}

// ===== Free functions =====

// TOOL: 0.5.3 OsGui.cpp 0x0045A7E0
CImVector OsGuiGetColor(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045A7E0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A640
CiRect OsGuiGetScreenBounds() {
    // TODO: restore from 0.5.3 0x0045A640.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A290
CiRect OsGuiGetWindowRect(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A290.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A4E0
CiRect OsGuiGetWindowRestoredRect(void* a1) {
    // TODO: restore from 0.5.3 0x0045A4E0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A390
int32_t OsGuiWindowEnabled(void* a1) {
    // TODO: restore from 0.5.3 0x0045A390.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A5B0
int32_t OsGuiWindowIsCursorInside(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A5B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A440
int32_t OsGuiWindowMaximized(void* a1) {
    // TODO: restore from 0.5.3 0x0045A440.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A4B0
int32_t OsGuiWindowMinimized(void* a1) {
    // TODO: restore from 0.5.3 0x0045A4B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A340
void OsGuiBringWindowToFront(void* a1) {
    // TODO: restore from 0.5.3 0x0045A340.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451E60
void OsGuiDestroy() {
    // TODO: restore from 0.5.3 0x00451E60.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452130
void OsGuiEnableMenuHotkeys(int32_t a1) {
    // TODO: restore from 0.5.3 0x00452130.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452090
void OsGuiEnableTooltips(int32_t a1) {
    // TODO: restore from 0.5.3 0x00452090.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A380
void OsGuiEnableWindow(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A380.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A1E0
void OsGuiGetCursorPosition(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x0045A1E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A7C0
void OsGuiGetHotkeyText(const OsGuiMenuHotkey& a1, char* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x0045A7C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451E10
void OsGuiInitialize() {
    // TODO: restore from 0.5.3 0x00451E10.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A400
void OsGuiMaximizeWindow(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A400.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451DF0
void OsGuiMenuSelect(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00451DF0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A470
void OsGuiMinimizeWindow(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A470.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451E90
void OsGuiSetApplicationInfo(void* a1) {
    // TODO: restore from 0.5.3 0x00451E90.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A1B0
void OsGuiSetCursor(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045A1B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451EB0
void OsGuiSetIdleCallback(OsGuiCallback a1, void* a2) {
    // TODO: restore from 0.5.3 0x00451EB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00451EA0
void OsGuiSetMenuCommandCallback(OsGuiCallback a1, void* a2) {
    // TODO: restore from 0.5.3 0x00451EA0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A220
void OsGuiSetWindowIcon(void* a1, const char* a2) {
    // TODO: restore from 0.5.3 0x0045A220.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A310
void OsGuiSetWindowRect(void* a1, const CiRect& a2) {
    // TODO: restore from 0.5.3 0x0045A310.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A540
void OsGuiSetWindowRestoredRect(void* a1, const CiRect& a2) {
    // TODO: restore from 0.5.3 0x0045A540.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A1D0
void OsGuiShowCursor(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045A1D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A350
void OsGuiShowWindow(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A350.
}


// ===== COsTreeView =====

// TOOL: 0.5.3 OsGui.cpp 0x004586B0
OsGuiTreeItemParams* COsTreeView::GetParams(void* a1) {
    // TODO: restore from 0.5.3 0x004586B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004584D0
int32_t COsTreeView::FindUnusedParams() {
    // TODO: restore from 0.5.3 0x004584D0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457F20
int32_t COsTreeView::RunDragHandler() {
    // TODO: restore from 0.5.3 0x00457F20.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457DC0
void COsTreeView::CreateDragImage(void* a1) {
    // TODO: restore from 0.5.3 0x00457DC0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457DF0
void COsTreeView::DestroyDragImage() {
    // TODO: restore from 0.5.3 0x00457DF0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458550
void COsTreeView::InitParams(void* a1) {
    // TODO: restore from 0.5.3 0x00458550.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456D40
void COsTreeView::InitializeTreeView() {
    // TODO: restore from 0.5.3 0x00456D40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458360
void COsTreeView::OnDeleteItem(void* a1) {
    // TODO: restore from 0.5.3 0x00458360.
}

// TOOL: 0.5.3 OsGui.cpp 0x004579E0
void* COsTreeView::FindItemUnderCursor() {
    // TODO: restore from 0.5.3 0x004579E0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456C30
COsTreeView::COsTreeView(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_TREE_VIEW, a2, a3) {
    // TODO: restore from 0.5.3 0x00456C30.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456CD0
COsTreeView::COsTreeView(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_TREE_VIEW, a2, a3) {
    // TODO: restore from 0.5.3 0x00456CD0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004570C0
CImVector COsTreeView::GetItemColor(void* a1) {
    // TODO: restore from 0.5.3 0x004570C0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457E10
CiRect COsTreeView::GetItemRect(void* a1) {
    // TODO: restore from 0.5.3 0x00457E10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457100
int32_t COsTreeView::GetItemNumChildren(void* a1) {
    // TODO: restore from 0.5.3 0x00457100.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457630
int32_t COsTreeView::IsItemExpanded(void* a1) {
    // TODO: restore from 0.5.3 0x00457630.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457770
int32_t COsTreeView::IsItemSelected(void* a1) {
    // TODO: restore from 0.5.3 0x00457770.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458200
int32_t COsTreeView::OnBeginEdit(void* a1) {
    // TODO: restore from 0.5.3 0x00458200.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458250
int32_t COsTreeView::OnEndEdit(void* a1, const char* a2) {
    // TODO: restore from 0.5.3 0x00458250.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456E50
COsTreeView::~COsTreeView() {
    // TODO: restore from 0.5.3 0x00456E50.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457D20
int32_t COsTreeView::IsHandleFromControl(void* a1) {
    // TODO: restore from 0.5.3 0x00457D20.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457AA0
int32_t COsTreeView::OnEscape() {
    // TODO: restore from 0.5.3 0x00457AA0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004578F0
int32_t COsTreeView::OnMouseDown() {
    // TODO: restore from 0.5.3 0x004578F0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457930
int32_t COsTreeView::OnMouseUp() {
    // TODO: restore from 0.5.3 0x00457930.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457AD0
int32_t COsTreeView::OnNotify(int32_t a1, void* a2) {
    // TODO: restore from 0.5.3 0x00457AD0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457A60
int32_t COsTreeView::OnReturn() {
    // TODO: restore from 0.5.3 0x00457A60.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458040
void COsTreeView::OnMouseMove(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00458040.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457660
void COsTreeView::OnSizeChange() {
    // TODO: restore from 0.5.3 0x00457660.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456EE0
void COsTreeView::ClearItems() {
    // TODO: restore from 0.5.3 0x00456EE0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456F00
void COsTreeView::DeleteItem(void* a1) {
    // TODO: restore from 0.5.3 0x00456F00.
}

// TOOL: 0.5.3 OsGui.cpp 0x004578C0
void COsTreeView::EditItem(void* a1) {
    // TODO: restore from 0.5.3 0x004578C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457D50
void COsTreeView::EnableDragDrop(int32_t a1) {
    // TODO: restore from 0.5.3 0x00457D50.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458290
void COsTreeView::EnableFilters(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458290.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457690
void COsTreeView::EnsureItemVisible(void* a1) {
    // TODO: restore from 0.5.3 0x00457690.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457200
void COsTreeView::EnumerateAllItems(OsGuiTVEnumFunc a1, void* a2) {
    // TODO: restore from 0.5.3 0x00457200.
}

// TOOL: 0.5.3 OsGui.cpp 0x004571A0
void COsTreeView::EnumerateItems(void* a1, OsGuiTVEnumFunc a2, void* a3) {
    // TODO: restore from 0.5.3 0x004571A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457600
void COsTreeView::ExpandItem(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00457600.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456FD0
void COsTreeView::GetItemText(void* a1, char* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00456FD0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004577B0
void COsTreeView::GetSelectionInfo(OsGuiTVSelectionInfo* a1) {
    // TODO: restore from 0.5.3 0x004577B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457F40
void COsTreeView::OnBeginDrag(void* a1, int32_t a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00457F40.
}

// TOOL: 0.5.3 OsGui.cpp 0x004581A0
void COsTreeView::OnEndDrag() {
    // TODO: restore from 0.5.3 0x004581A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457670
void COsTreeView::OnExpandedItem(void* a1) {
    // TODO: restore from 0.5.3 0x00457670.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457E70
void COsTreeView::RefreshItem(void* a1) {
    // TODO: restore from 0.5.3 0x00457E70.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457090
void COsTreeView::ResetItemColor(void* a1) {
    // TODO: restore from 0.5.3 0x00457090.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457880
void COsTreeView::SelectAll(int32_t a1) {
    // TODO: restore from 0.5.3 0x00457880.
}

// TOOL: 0.5.3 OsGui.cpp 0x004576F0
void COsTreeView::SelectItem(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004576F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456EB0
void COsTreeView::SetBackgroundColor(const CImVector& a1) {
    // TODO: restore from 0.5.3 0x00456EB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458320
void COsTreeView::SetCanEditFunction(OsGuiTVCanEditFunc a1, void* a2) {
    // TODO: restore from 0.5.3 0x00458320.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457DA0
void COsTreeView::SetDragDropHandler(OsGuiTVDragDropFunc a1, void* a2) {
    // TODO: restore from 0.5.3 0x00457DA0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458130
void COsTreeView::SetDropTarget(void* a1) {
    // TODO: restore from 0.5.3 0x00458130.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458340
void COsTreeView::SetExpandFunction(OsGuiTVExpandFunc a1, void* a2) {
    // TODO: restore from 0.5.3 0x00458340.
}

// TOOL: 0.5.3 OsGui.cpp 0x004582A0
void COsTreeView::SetFilter(uint32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004582A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457ED0
void COsTreeView::SetFirstVisibleItem(void* a1) {
    // TODO: restore from 0.5.3 0x00457ED0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458160
void COsTreeView::SetInsertionMark(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00458160.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457050
void COsTreeView::SetItemColor(void* a1, const CImVector& a2) {
    // TODO: restore from 0.5.3 0x00457050.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457240
void COsTreeView::SetItemImage(void* a1, int32_t a2, int32_t a3, void* a4) {
    // TODO: restore from 0.5.3 0x00457240.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457010
void COsTreeView::SetItemParam(void* a1, void* a2) {
    // TODO: restore from 0.5.3 0x00457010.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456F90
void COsTreeView::SetItemText(void* a1, const char* a2) {
    // TODO: restore from 0.5.3 0x00456F90.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458280
void COsTreeView::SetTextLimit(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458280.
}

// TOOL: 0.5.3 OsGui.cpp 0x00457EB0
void* COsTreeView::GetFirstVisibleItem() {
    // TODO: restore from 0.5.3 0x00457EB0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457150
void* COsTreeView::GetItemChild(void* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00457150.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00457030
void* COsTreeView::GetItemParam(void* a1) {
    // TODO: restore from 0.5.3 0x00457030.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004570E0
void* COsTreeView::GetItemParent(void* a1) {
    // TODO: restore from 0.5.3 0x004570E0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004576B0
void* COsTreeView::GetSelectedItem() {
    // TODO: restore from 0.5.3 0x004576B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456F20
void* COsTreeView::InsertItem(void* a1, void* a2, const char* a3) {
    // TODO: restore from 0.5.3 0x00456F20.
    return {};
}


// ===== COsDialog =====

// TOOL: 0.5.3 OsGui.cpp 0x00454130
int32_t COsDialog::FindControl(COsControl* a1) {
    // TODO: restore from 0.5.3 0x00454130.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00453D80
void COsDialog::ApplyModality(int32_t a1) {
    // TODO: restore from 0.5.3 0x00453D80.
}

// TOOL: 0.5.3 OsGui.cpp 0x00453190
COsDialog::COsDialog(void* a1, uint32_t a2) {
    // TODO: restore from 0.5.3 0x00453190.
}

// TOOL: 0.5.3 OsGui.cpp 0x00453B40
COsDialog::~COsDialog() {
    // TODO: restore from 0.5.3 0x00453B40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454C10
int32_t COsDialog::CanDoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454C10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454C40
int32_t COsDialog::DoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454C40.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004547B0
int32_t COsDialog::GetMinSize(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x004547B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454640
int32_t COsDialog::IsEnabled() {
    // TODO: restore from 0.5.3 0x00454640.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454540
int32_t COsDialog::IsMouseInside() {
    // TODO: restore from 0.5.3 0x00454540.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454630
int32_t COsDialog::IsShowing() {
    // TODO: restore from 0.5.3 0x00454630.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454810
int32_t COsDialog::OnAccept() {
    // TODO: restore from 0.5.3 0x00454810.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454B90
int32_t COsDialog::OnContextMenu(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00454B90.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454860
int32_t COsDialog::OnMouseDown() {
    // TODO: restore from 0.5.3 0x00454860.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454920
int32_t COsDialog::OnMouseLeave() {
    // TODO: restore from 0.5.3 0x00454920.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454960
int32_t COsDialog::OnMouseMove(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00454960.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004548C0
int32_t COsDialog::OnMouseUp() {
    // TODO: restore from 0.5.3 0x004548C0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00454720
void COsDialog::SetSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00454720.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454090
void COsDialog::AddControl(COsControl* a1) {
    // TODO: restore from 0.5.3 0x00454090.
}

// TOOL: 0.5.3 OsGui.cpp 0x004545C0
void COsDialog::BringToFront() {
    // TODO: restore from 0.5.3 0x004545C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004544D0
void COsDialog::CheckEvents() {
    // TODO: restore from 0.5.3 0x004544D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004541A0
void COsDialog::DeleteControl(COsControl* a1) {
    // TODO: restore from 0.5.3 0x004541A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004541E0
void COsDialog::DetachControl(COsControl* a1) {
    // TODO: restore from 0.5.3 0x004541E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454060
void COsDialog::EnableTooltips(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454060.
}

// TOOL: 0.5.3 OsGui.cpp 0x004546D0
void COsDialog::GetPosition(int32_t* a1, int32_t* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x004546D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454750
void COsDialog::GetSize(int32_t* a1, int32_t* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00454750.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454670
void COsDialog::Refresh(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454670.
}

// TOOL: 0.5.3 OsGui.cpp 0x004545A0
void COsDialog::SetCallback(OsGuiCallback a1, void* a2) {
    // TODO: restore from 0.5.3 0x004545A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454B30
void COsDialog::SetContextMenu(COsMenu* a1) {
    // TODO: restore from 0.5.3 0x00454B30.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454600
void COsDialog::SetInputFocus() {
    // TODO: restore from 0.5.3 0x00454600.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454790
void COsDialog::SetMinSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00454790.
}

// TOOL: 0.5.3 OsGui.cpp 0x004546A0
void COsDialog::SetPosition(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004546A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454650
void COsDialog::SetRedraw(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454650.
}

// TOOL: 0.5.3 OsGui.cpp 0x004547F0
void COsDialog::SetTitle(const char* a1) {
    // TODO: restore from 0.5.3 0x004547F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454510
void COsDialog::SetTrackMouse(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454510.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454610
void COsDialog::Show(int32_t a1) {
    // TODO: restore from 0.5.3 0x00454610.
}

// TOOL: 0.5.3 OsGui.cpp 0x00453FC0
void* COsDialog::GetTooltips() {
    // TODO: restore from 0.5.3 0x00453FC0.
    return {};
}


// ===== COsControl =====

// TOOL: 0.5.3 OsGui.cpp 0x00454D20
void COsControl::Initialize(void* a1, int32_t a2, int16_t a3, uint32_t a4) {
    // TODO: restore from 0.5.3 0x00454D20.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454C70
COsControl::COsControl(COsDialog* a1, int32_t a2, int16_t a3, uint32_t a4) {
    this->m_flags = a4;
    this->m_dialog = nullptr;
    this->m_id = a3;
    this->m_type = a2;
    this->m_handle = nullptr;
    this->m_callback = nullptr;
    this->m_callbackParam = nullptr;
    this->m_contextMenu = nullptr;
    this->m_contextMenuEnabled = 0;
    this->m_redrawLevel = 0;
    // TODO: restore from 0.5.3 0x00454C70.
}

// TOOL: 0.5.3 OsGui.cpp 0x00454CE0
COsControl::COsControl(void* a1, int32_t a2, int16_t a3, uint32_t a4) {
    this->m_flags = a4;
    this->m_dialog = nullptr;
    this->m_id = a3;
    this->m_type = a2;
    this->m_handle = nullptr;
    this->m_callback = nullptr;
    this->m_callbackParam = nullptr;
    this->m_contextMenu = nullptr;
    this->m_contextMenuEnabled = 0;
    this->m_redrawLevel = 0;
    // TODO: restore from 0.5.3 0x00454CE0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004552B0
int32_t COsControl::GetTextLength() {
    // TODO: restore from 0.5.3 0x004552B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455250
int32_t COsControl::HasInputFocus() {
    // TODO: restore from 0.5.3 0x00455250.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004553D0
int32_t COsControl::IsEnabled() {
    // TODO: restore from 0.5.3 0x004553D0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004553A0
int32_t COsControl::IsShowing() {
    // TODO: restore from 0.5.3 0x004553A0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455060
COsControl::~COsControl() {
    // TODO: restore from 0.5.3 0x00455060.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF20
int32_t COsControl::CanDoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045AF20.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF30
int32_t COsControl::DoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045AF30.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF50
int32_t COsControl::GetValue() {
    // TODO: restore from 0.5.3 0x0045AF50.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455690
int32_t COsControl::IsHandleFromControl(void* a1) {
    // TODO: restore from 0.5.3 0x00455690.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455720
int32_t COsControl::OnCommand(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455720.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455620
int32_t COsControl::OnContextMenu(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00455620.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AE80
int32_t COsControl::OnDraw(void* a1, uint32_t a2, CiRect& a3) {
    // TODO: restore from 0.5.3 0x0045AE80.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AEB0
int32_t COsControl::OnEscape() {
    // TODO: restore from 0.5.3 0x0045AEB0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455130
int32_t COsControl::OnEvent(int32_t a1, int32_t a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00455130.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AEC0
int32_t COsControl::OnMouseDown() {
    // TODO: restore from 0.5.3 0x0045AEC0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AED0
int32_t COsControl::OnMouseUp() {
    // TODO: restore from 0.5.3 0x0045AED0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF10
int32_t COsControl::OnMouseWheel(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045AF10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004556B0
int32_t COsControl::OnNotify(int32_t a1, void* a2) {
    // TODO: restore from 0.5.3 0x004556B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AEA0
int32_t COsControl::OnReturn() {
    // TODO: restore from 0.5.3 0x0045AEA0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455750
int32_t COsControl::OnScroll(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455750.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AE70
void COsControl::OnDestroy() {
    // TODO: restore from 0.5.3 0x0045AE70.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AEE0
void COsControl::OnMouseMove(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045AEE0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AEF0
void COsControl::OnSizeChange() {
    // TODO: restore from 0.5.3 0x0045AEF0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF00
void COsControl::OnTextChange() {
    // TODO: restore from 0.5.3 0x0045AF00.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF40
void COsControl::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045AF40.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AE90
void* COsControl::OnSetColors(void* a1) {
    // TODO: restore from 0.5.3 0x0045AE90.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004553B0
void COsControl::Enable(int32_t a1) {
    // TODO: restore from 0.5.3 0x004553B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455410
void COsControl::GetPosition(int32_t* a1, int32_t* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00455410.
}

// TOOL: 0.5.3 OsGui.cpp 0x004554D0
void COsControl::GetSize(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x004554D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455290
void COsControl::GetText(char* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00455290.
}

// TOOL: 0.5.3 OsGui.cpp 0x004552C0
void COsControl::GetTextSize(const char* a1, int32_t* a2, int32_t* a3) {
    // TODO: restore from 0.5.3 0x004552C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455340
void COsControl::GetTextSize(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x00455340.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455230
void COsControl::LoseInputFocus() {
    // TODO: restore from 0.5.3 0x00455230.
}

// TOOL: 0.5.3 OsGui.cpp 0x004550F0
void COsControl::Refresh(int32_t a1) {
    // TODO: restore from 0.5.3 0x004550F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455110
void COsControl::SetCallback(OsGuiCallback a1, void* a2) {
    // TODO: restore from 0.5.3 0x00455110.
}

// TOOL: 0.5.3 OsGui.cpp 0x004555C0
void COsControl::SetContextMenu(COsMenu* a1) {
    // TODO: restore from 0.5.3 0x004555C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004551C0
void COsControl::SetFont(int32_t a1) {
    // TODO: restore from 0.5.3 0x004551C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455200
void COsControl::SetInputFocus() {
    // TODO: restore from 0.5.3 0x00455200.
}

// TOOL: 0.5.3 OsGui.cpp 0x004553E0
void COsControl::SetPosition(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004553E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004550B0
void COsControl::SetRedraw(int32_t a1) {
    // TODO: restore from 0.5.3 0x004550B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004554A0
void COsControl::SetSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004554A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455270
void COsControl::SetText(const char* a1) {
    // TODO: restore from 0.5.3 0x00455270.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455510
void COsControl::SetTooltip(const char* a1) {
    // TODO: restore from 0.5.3 0x00455510.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455380
void COsControl::Show(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455380.
}


// ===== COsDivider =====

// TOOL: 0.5.3 OsGui.cpp 0x00459D90
void COsDivider::Initialize() {
    // TODO: restore from 0.5.3 0x00459D90.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459F00
void COsDivider::UpdateCursor() {
    // TODO: restore from 0.5.3 0x00459F00.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459CD0
COsDivider::COsDivider(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_DIVIDER, a2, a3) {
    // TODO: restore from 0.5.3 0x00459CD0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459D40
COsDivider::COsDivider(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_DIVIDER, a2, a3) {
    // TODO: restore from 0.5.3 0x00459D40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459D80
COsDivider::~COsDivider() {
    // TODO: restore from 0.5.3 0x00459D80.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459F20
void COsDivider::OnDivMouseDown() {
    // TODO: restore from 0.5.3 0x00459F20.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A0F0
void COsDivider::OnDivMouseLeave() {
    // TODO: restore from 0.5.3 0x0045A0F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459FB0
void COsDivider::OnDivMouseMove(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00459FB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A0C0
void COsDivider::OnDivMouseUp() {
    // TODO: restore from 0.5.3 0x0045A0C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A100
void COsDivider::SetPositionRange(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A100.
}


// ===== COsEditBox =====

// TOOL: 0.5.3 OsGui.cpp 0x00455D90
void COsEditBox::Initialize() {
    // TODO: restore from 0.5.3 0x00455D90.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455D00
COsEditBox::COsEditBox(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_EDIT_BOX, a2, a3) {
    // TODO: restore from 0.5.3 0x00455D00.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455D60
COsEditBox::COsEditBox(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_EDIT_BOX, a2, a3) {
    // TODO: restore from 0.5.3 0x00455D60.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455E90
int32_t COsEditBox::GetSelectionSize() {
    // TODO: restore from 0.5.3 0x00455E90.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455FE0
int32_t COsEditBox::CanDoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455FE0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456070
int32_t COsEditBox::DoClipboardAction(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456070.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455E40
int32_t COsEditBox::OnReturn() {
    // TODO: restore from 0.5.3 0x00455E40.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455EC0
void COsEditBox::EnableFilters(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455EC0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455E70
void COsEditBox::SelectAll() {
    // TODO: restore from 0.5.3 0x00455E70.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455ED0
void COsEditBox::SetFilter(uint32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00455ED0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455E50
void COsEditBox::SetTextLimit(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455E50.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455E20
void COsEditBox::UpdateSelection() {
    // TODO: restore from 0.5.3 0x00455E20.
}


// ===== COsMenu =====

// TOOL: 0.5.3 OsGui.cpp 0x00452310
void COsMenu::AddHotkey(int32_t a1) {
    // TODO: restore from 0.5.3 0x00452310.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452430
void COsMenu::RemoveHotkey(int32_t a1) {
    // TODO: restore from 0.5.3 0x00452430.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452190
COsMenu::COsMenu() {
    // TODO: restore from 0.5.3 0x00452190.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452140
COsMenu::COsMenu(uint8_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x00452140.
}

// TOOL: 0.5.3 OsGui.cpp 0x004521C0
COsMenu::~COsMenu() {
    // TODO: restore from 0.5.3 0x004521C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452250
int32_t COsMenu::GetHotkey(int32_t a1, OsGuiMenuHotkey* a2) {
    // TODO: restore from 0.5.3 0x00452250.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00452240
int32_t COsMenu::GetNumItems() {
    // TODO: restore from 0.5.3 0x00452240.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00452550
void COsMenu::AppendHotkeyText(char* a1, const OsGuiMenuHotkey& a2) {
    // TODO: restore from 0.5.3 0x00452550.
}

// TOOL: 0.5.3 OsGui.cpp 0x004528D0
void COsMenu::AddSeparator(int32_t a1) {
    // TODO: restore from 0.5.3 0x004528D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452870
void COsMenu::AddSubMenu(int32_t a1, const char* a2, COsMenu* a3) {
    // TODO: restore from 0.5.3 0x00452870.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452780
void COsMenu::AddTextItem(int32_t a1, const char* a2, OsGuiMenuHotkey* a3) {
    // TODO: restore from 0.5.3 0x00452780.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452980
void COsMenu::CheckItem(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00452980.
}

// TOOL: 0.5.3 OsGui.cpp 0x004521F0
void COsMenu::Clear() {
    // TODO: restore from 0.5.3 0x004521F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452920
void COsMenu::EnableItem(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00452920.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452210
void COsMenu::RemoveItem(int32_t a1) {
    // TODO: restore from 0.5.3 0x00452210.
}

// TOOL: 0.5.3 OsGui.cpp 0x004529E0
void COsMenu::SetItemText(int32_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x004529E0.
}


// ===== COsPopupMenu =====

// TOOL: 0.5.3 OsGui.cpp 0x00456320
void COsPopupMenu::AdjustHeight() {
    // TODO: restore from 0.5.3 0x00456320.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456140
COsPopupMenu::COsPopupMenu(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_COMBO_BOX, a2, 0) {
    // TODO: restore from 0.5.3 0x00456140.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456300
int32_t COsPopupMenu::GetItemHeight() {
    // TODO: restore from 0.5.3 0x00456300.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456260
int32_t COsPopupMenu::GetNumItems() {
    // TODO: restore from 0.5.3 0x00456260.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004561C0
COsPopupMenu::~COsPopupMenu() {
    // TODO: restore from 0.5.3 0x004561C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456220
int32_t COsPopupMenu::GetValue() {
    // TODO: restore from 0.5.3 0x00456220.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456200
void COsPopupMenu::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456200.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456240
void COsPopupMenu::ClearItems() {
    // TODO: restore from 0.5.3 0x00456240.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456390
void COsPopupMenu::DeleteItem(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456390.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456280
void COsPopupMenu::InsertItem(const char* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00456280.
}

// TOOL: 0.5.3 OsGui.cpp 0x004562D0
void COsPopupMenu::SetItemHeight(int32_t a1) {
    // TODO: restore from 0.5.3 0x004562D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004563C0
void COsPopupMenu::SetItemText(int32_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x004563C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456370
void COsPopupMenu::SetMaxHeight(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456370.
}

// TOOL: 0.5.3 OsGui.cpp 0x004561D0
void COsPopupMenu::SetSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004561D0.
}


// ===== COsScrollBar =====

// TOOL: 0.5.3 OsGui.cpp 0x004599C0
void COsScrollBar::Initialize() {
    // TODO: restore from 0.5.3 0x004599C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459A40
void COsScrollBar::UpdateRangeValues() {
    // TODO: restore from 0.5.3 0x00459A40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459930
COsScrollBar::COsScrollBar(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_SCROLL_BAR, a2, a3) {
    // TODO: restore from 0.5.3 0x00459930.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459990
COsScrollBar::COsScrollBar(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_SCROLL_BAR, a2, a3) {
    // TODO: restore from 0.5.3 0x00459990.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459AF0
int32_t COsScrollBar::GetValue() {
    // TODO: restore from 0.5.3 0x00459AF0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459C70
int32_t COsScrollBar::OnMouseWheel(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459C70.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459B10
int32_t COsScrollBar::OnScroll(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459B10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459AA0
void COsScrollBar::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459AA0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459A00
void COsScrollBar::SetPageSize(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459A00.
}

// TOOL: 0.5.3 OsGui.cpp 0x004599E0
void COsScrollBar::SetRange(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004599E0.
}


// ===== COsSpinButton =====

// TOOL: 0.5.3 OsGui.cpp 0x00458BD0
void COsSpinButton::Initialize() {
    // TODO: restore from 0.5.3 0x00458BD0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458B40
COsSpinButton::COsSpinButton(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_UP_DOWN, a2, a3) {
    // TODO: restore from 0.5.3 0x00458B40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458BA0
COsSpinButton::COsSpinButton(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_UP_DOWN, a2, a3) {
    // TODO: restore from 0.5.3 0x00458BA0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458C90
int32_t COsSpinButton::GetValue() {
    // TODO: restore from 0.5.3 0x00458C90.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458C70
void COsSpinButton::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458C70.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458C40
void COsSpinButton::OnSpinMouseUp() {
    // TODO: restore from 0.5.3 0x00458C40.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458C50
void COsSpinButton::SetValueRange(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00458C50.
}


// ===== COsStaticText =====

// TOOL: 0.5.3 OsGui.cpp 0x00455AE0
void COsStaticText::Initialize() {
    // TODO: restore from 0.5.3 0x00455AE0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455A30
COsStaticText::COsStaticText(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_STATIC_TEXT, a2, a3) {
    // TODO: restore from 0.5.3 0x00455A30.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455AA0
COsStaticText::COsStaticText(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_STATIC_TEXT, a2, a3) {
    // TODO: restore from 0.5.3 0x00455AA0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455B70
void* COsStaticText::OnSetColors(void* a1) {
    // TODO: restore from 0.5.3 0x00455B70.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00455AF0
void COsStaticText::SetJustification(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455AF0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455B40
void COsStaticText::SetTextColor(const CImVector& a1) {
    // TODO: restore from 0.5.3 0x00455B40.
}


// ===== COsToolBar =====

// TOOL: 0.5.3 OsGui.cpp 0x00459560
void COsToolBar::InitializeToolBar() {
    // TODO: restore from 0.5.3 0x00459560.
}

// TOOL: 0.5.3 OsGui.cpp 0x004594B0
COsToolBar::COsToolBar(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_TOOLBAR, a2, a3) {
    // TODO: restore from 0.5.3 0x004594B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459510
COsToolBar::COsToolBar(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_TOOLBAR, a2, a3) {
    // TODO: restore from 0.5.3 0x00459510.
}

// TOOL: 0.5.3 OsGui.cpp 0x004596E0
int32_t COsToolBar::GetNumButtons() {
    // TODO: restore from 0.5.3 0x004596E0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459540
COsToolBar::~COsToolBar() {
    // TODO: restore from 0.5.3 0x00459540.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459910
int32_t COsToolBar::OnCommand(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459910.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459620
void COsToolBar::AddButton(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459620.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459670
void COsToolBar::AddSeparator(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459670.
}

// TOOL: 0.5.3 OsGui.cpp 0x004598B0
void COsToolBar::CheckButton(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004598B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459600
void COsToolBar::Clear() {
    // TODO: restore from 0.5.3 0x00459600.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459850
void COsToolBar::EnableButton(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00459850.
}

// TOOL: 0.5.3 OsGui.cpp 0x004595D0
void COsToolBar::GetButtonSize(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x004595D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459810
void COsToolBar::GetButtonText(int32_t a1, char* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x00459810.
}

// TOOL: 0.5.3 OsGui.cpp 0x004596C0
void COsToolBar::RemoveButton(int32_t a1) {
    // TODO: restore from 0.5.3 0x004596C0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459700
void COsToolBar::SetButtonImage(int32_t a1, int32_t a2, int32_t a3, void* a4) {
    // TODO: restore from 0.5.3 0x00459700.
}

// TOOL: 0.5.3 OsGui.cpp 0x004595A0
void COsToolBar::SetButtonSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x004595A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004597D0
void COsToolBar::SetButtonText(int32_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x004597D0.
}


// ===== COsButton =====

// TOOL: 0.5.3 OsGui.cpp 0x00455780
COsButton::COsButton(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_BUTTON, a2, a3) {
    // TODO: restore from 0.5.3 0x00455780.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455800
void COsButton::SetCancelButton() {
    // TODO: restore from 0.5.3 0x00455800.
}

// TOOL: 0.5.3 OsGui.cpp 0x004557E0
void COsButton::SetDefaultButton() {
    // TODO: restore from 0.5.3 0x004557E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455810
void COsButton::SetHighlight(int32_t a1) {
    // TODO: restore from 0.5.3 0x00455810.
}


// ===== COsCheckbox =====

// TOOL: 0.5.3 OsGui.cpp 0x00456860
COsCheckbox::COsCheckbox(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_CHECKBOX, a2, 0) {
    // TODO: restore from 0.5.3 0x00456860.
}

// TOOL: 0.5.3 OsGui.cpp 0x004568D0
COsCheckbox::COsCheckbox(void* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_CHECKBOX, a2, 0) {
    // TODO: restore from 0.5.3 0x004568D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004569A0
int32_t COsCheckbox::HasValue() {
    // TODO: restore from 0.5.3 0x004569A0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456960
int32_t COsCheckbox::GetValue() {
    // TODO: restore from 0.5.3 0x00456960.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004569C0
int32_t COsCheckbox::OnEvent(int32_t a1, int32_t a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x004569C0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456A60
void COsCheckbox::OnSizeChange() {
    // TODO: restore from 0.5.3 0x00456A60.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456A00
void COsCheckbox::OnTextChange() {
    // TODO: restore from 0.5.3 0x00456A00.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456930
void COsCheckbox::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456930.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456980
void COsCheckbox::ClearValue() {
    // TODO: restore from 0.5.3 0x00456980.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456910
void COsCheckbox::SetMaxWidth(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456910.
}


// ===== COsImageButton =====

// TOOL: 0.5.3 OsGui.cpp 0x00455850
COsImageButton::COsImageButton(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_IMAGE_BUTTON, a2, a3) {
    // TODO: restore from 0.5.3 0x00455850.
}

// TOOL: 0.5.3 OsGui.cpp 0x004558B0
COsImageButton::COsImageButton(void* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_IMAGE_BUTTON, a2, a3) {
    // TODO: restore from 0.5.3 0x004558B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455A10
int32_t COsImageButton::IsPushed() {
    // TODO: restore from 0.5.3 0x00455A10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004558E0
COsImageButton::~COsImageButton() {
    // TODO: restore from 0.5.3 0x004558E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004558F0
void COsImageButton::OnDestroy() {
    // TODO: restore from 0.5.3 0x004558F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x004559D0
void COsImageButton::SetHighlight(int32_t a1) {
    // TODO: restore from 0.5.3 0x004559D0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455910
void COsImageButton::SetImage(int32_t a1, int32_t a2, void* a3) {
    // TODO: restore from 0.5.3 0x00455910.
}


// ===== COsListBox =====

// TOOL: 0.5.3 OsGui.cpp 0x004563F0
COsListBox::COsListBox(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_LIST_BOX, a2, a3) {
    // TODO: restore from 0.5.3 0x004563F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456770
int32_t COsListBox::GetItemHeight() {
    // TODO: restore from 0.5.3 0x00456770.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004566D0
int32_t COsListBox::GetItemTextLength(int32_t a1) {
    // TODO: restore from 0.5.3 0x004566D0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456630
int32_t COsListBox::GetNumItems() {
    // TODO: restore from 0.5.3 0x00456630.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456550
int32_t COsListBox::IsItemSelected(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456550.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456450
COsListBox::~COsListBox() {
    // TODO: restore from 0.5.3 0x00456450.
}

// TOOL: 0.5.3 OsGui.cpp 0x004564B0
int32_t COsListBox::GetValue() {
    // TODO: restore from 0.5.3 0x004564B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456790
int32_t COsListBox::OnContextMenu(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00456790.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456850
int32_t COsListBox::OnReturn() {
    // TODO: restore from 0.5.3 0x00456850.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456460
void COsListBox::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456460.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456610
void COsListBox::ClearItems() {
    // TODO: restore from 0.5.3 0x00456610.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456670
void COsListBox::DeleteItem(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456670.
}

// TOOL: 0.5.3 OsGui.cpp 0x004566F0
void COsListBox::GetItemText(int32_t a1, char* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x004566F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456650
void COsListBox::InsertItem(const char* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00456650.
}

// TOOL: 0.5.3 OsGui.cpp 0x004565A0
void COsListBox::SelectAll(int32_t a1) {
    // TODO: restore from 0.5.3 0x004565A0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456500
void COsListBox::SelectItem(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00456500.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456750
void COsListBox::SetItemHeight(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456750.
}

// TOOL: 0.5.3 OsGui.cpp 0x004566A0
void COsListBox::SetItemText(int32_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x004566A0.
}


// ===== COsListView =====

// TOOL: 0.5.3 OsGui.cpp 0x00458E60
COsListView::COsListView(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_LIST_VIEW, a2, a3) {
    // TODO: restore from 0.5.3 0x00458E60.
}

// TOOL: 0.5.3 OsGui.cpp 0x004590B0
CImVector COsListView::GetRowColor(int32_t a1) {
    // TODO: restore from 0.5.3 0x004590B0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459190
int32_t COsListView::GetColumnWidth(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459190.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458FB0
int32_t COsListView::GetNumColumns() {
    // TODO: restore from 0.5.3 0x00458FB0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459050
int32_t COsListView::GetNumRows() {
    // TODO: restore from 0.5.3 0x00459050.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458F10
COsListView::~COsListView() {
    // TODO: restore from 0.5.3 0x00458F10.
}

// TOOL: 0.5.3 OsGui.cpp 0x004592F0
int32_t COsListView::GetValue() {
    // TODO: restore from 0.5.3 0x004592F0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459380
int32_t COsListView::OnNotify(int32_t a1, void* a2) {
    // TODO: restore from 0.5.3 0x00459380.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x004594A0
int32_t COsListView::OnReturn() {
    // TODO: restore from 0.5.3 0x004594A0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00459340
void COsListView::OnSizeChange() {
    // TODO: restore from 0.5.3 0x00459340.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459290
void COsListView::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459290.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459030
void COsListView::ClearRows() {
    // TODO: restore from 0.5.3 0x00459030.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458F80
void COsListView::DeleteColumn(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458F80.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459010
void COsListView::DeleteRow(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459010.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459270
void COsListView::EnsureRowVisible(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459270.
}

// TOOL: 0.5.3 OsGui.cpp 0x004591E0
void COsListView::GetColumnTitle(int32_t a1, char* a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x004591E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459130
void COsListView::GetItemText(int32_t a1, int32_t a2, char* a3, int32_t a4) {
    // TODO: restore from 0.5.3 0x00459130.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458F20
void COsListView::InsertColumn(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458F20.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458FC0
void COsListView::InsertRow(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458FC0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459360
void COsListView::OnColumnClick(int32_t a1) {
    // TODO: restore from 0.5.3 0x00459360.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459350
void COsListView::OnSelectionChange() {
    // TODO: restore from 0.5.3 0x00459350.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459220
void COsListView::SetColumnJustification(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00459220.
}

// TOOL: 0.5.3 OsGui.cpp 0x004591B0
void COsListView::SetColumnTitle(int32_t a1, const char* a2) {
    // TODO: restore from 0.5.3 0x004591B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459170
void COsListView::SetColumnWidth(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00459170.
}

// TOOL: 0.5.3 OsGui.cpp 0x004590F0
void COsListView::SetItemText(int32_t a1, int32_t a2, const char* a3) {
    // TODO: restore from 0.5.3 0x004590F0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00459070
void COsListView::SetRowColor(int32_t a1, const CImVector& a2) {
    // TODO: restore from 0.5.3 0x00459070.
}


// ===== COsMenuBar =====

// TOOL: 0.5.3 OsGui.cpp 0x00452A90
COsMenuBar::COsMenuBar(void* a1) {
    // TODO: restore from 0.5.3 0x00452A90.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452B60
COsMenuBar::~COsMenuBar() {
    // TODO: restore from 0.5.3 0x00452B60.
}

// TOOL: 0.5.3 OsGui.cpp 0x00453180
void COsMenuBar::Refresh() {
    // TODO: restore from 0.5.3 0x00453180.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452D10
void COsMenuBar::Set(TSGrowableArray<COsMenu*>& a1) {
    // TODO: restore from 0.5.3 0x00452D10.
}

// TOOL: 0.5.3 OsGui.cpp 0x00452E30
void COsMenuBar::UpdateAccelerators() {
    // TODO: restore from 0.5.3 0x00452E30.
}


// ===== COsProgressBar =====

// TOOL: 0.5.3 OsGui.cpp 0x00456AB0
COsProgressBar::COsProgressBar(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_PROGRESS_BAR, a2, 0) {
    // TODO: restore from 0.5.3 0x00456AB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456B30
int32_t COsProgressBar::GetValue() {
    // TODO: restore from 0.5.3 0x00456B30.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456B10
void COsProgressBar::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456B10.
}


// ===== COsRadioButton =====

// TOOL: 0.5.3 OsGui.cpp 0x00458CB0
COsRadioButton::COsRadioButton(COsDialog* a1, int16_t a2, uint32_t a3) : COsControl(a1, COsControl::TYPE_RADIO_BUTTON, a2, a3) {
    // TODO: restore from 0.5.3 0x00458CB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458D40
int32_t COsRadioButton::GetValue() {
    // TODO: restore from 0.5.3 0x00458D40.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458D10
void COsRadioButton::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458D10.
}


// ===== COsSharedMemory =====

// TOOL: 0.5.3 OsGui.cpp 0x0045B780
COsSharedMemory::COsSharedMemory() {
    // TODO: restore from 0.5.3 0x0045B780.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045B790
COsSharedMemory::~COsSharedMemory() {
    // TODO: restore from 0.5.3 0x0045B790.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045B860
bool COsSharedMemory::ChangeAccess(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045B860.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045B7A0
bool COsSharedMemory::Initialize(const char* a1, uint32_t a2, int32_t a3) {
    // TODO: restore from 0.5.3 0x0045B7A0.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x0045B8C0
void COsSharedMemory::Destroy() {
    // TODO: restore from 0.5.3 0x0045B8C0.
}


// ===== COsSlider =====

// TOOL: 0.5.3 OsGui.cpp 0x00456B50
COsSlider::COsSlider(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_TRACK_BAR, a2, 0) {
    // TODO: restore from 0.5.3 0x00456B50.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456C10
int32_t COsSlider::GetValue() {
    // TODO: restore from 0.5.3 0x00456C10.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00456BF0
void COsSlider::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456BF0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456BD0
void COsSlider::SetMaxValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456BD0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00456BB0
void COsSlider::SetMinValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00456BB0.
}


// ===== COsStaticImage =====

// TOOL: 0.5.3 OsGui.cpp 0x00455BC0
COsStaticImage::COsStaticImage(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_STATIC_IMAGE, a2, 0) {
    // TODO: restore from 0.5.3 0x00455BC0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455C20
COsStaticImage::~COsStaticImage() {
    // TODO: restore from 0.5.3 0x00455C20.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455C30
void COsStaticImage::OnDestroy() {
    // TODO: restore from 0.5.3 0x00455C30.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455CB0
void COsStaticImage::ClearImage() {
    // TODO: restore from 0.5.3 0x00455CB0.
}

// TOOL: 0.5.3 OsGui.cpp 0x00455C50
void COsStaticImage::SetImage(int32_t a1, int32_t a2, void* a3) {
    // TODO: restore from 0.5.3 0x00455C50.
}


// ===== COsTextButton =====

// TOOL: 0.5.3 OsGui.cpp 0x00458770
COsTextButton::COsTextButton(COsDialog* a1, int16_t a2) : COsControl(a1, COsControl::TYPE_TEXT_BUTTON, a2, 0) {
    // TODO: restore from 0.5.3 0x00458770.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458800
int32_t COsTextButton::OnDraw(void* a1, uint32_t a2, CiRect& a3) {
    // TODO: restore from 0.5.3 0x00458800.
    return {};
}


// ===== COsWindow =====

// TOOL: 0.5.3 OsGui.cpp 0x0045A840
COsWindow::COsWindow(void* a1) {
    // TODO: restore from 0.5.3 0x0045A840.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A8B0
COsWindow::~COsWindow() {
    // TODO: restore from 0.5.3 0x0045A8B0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045AF60
void COsWindow::OnResize() {
    // TODO: restore from 0.5.3 0x0045AF60.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A8E0
void COsWindow::GetMinSize(int32_t* a1, int32_t* a2) {
    // TODO: restore from 0.5.3 0x0045A8E0.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A900
void COsWindow::SetCursor(int32_t a1) {
    // TODO: restore from 0.5.3 0x0045A900.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A930
void COsWindow::SetIcon(const char* a1) {
    // TODO: restore from 0.5.3 0x0045A930.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A950
void COsWindow::SetInputFocus() {
    // TODO: restore from 0.5.3 0x0045A950.
}

// TOOL: 0.5.3 OsGui.cpp 0x0045A8C0
void COsWindow::SetMinSize(int32_t a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x0045A8C0.
}


// ===== COsStaticBox =====

// TOOL: 0.5.3 OsGui.cpp 0x00458A40
int32_t COsStaticBox::OnDraw(void* a1, uint32_t a2, CiRect& a3) {
    // TODO: restore from 0.5.3 0x00458A40.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458970
void COsStaticBox::AddTransparentRect(const CiRect& a1) {
    // TODO: restore from 0.5.3 0x00458970.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458960
void COsStaticBox::ClearTransparentRects() {
    // TODO: restore from 0.5.3 0x00458960.
}


// ===== COsTabControl =====

// TOOL: 0.5.3 OsGui.cpp 0x00458D80
int32_t COsTabControl::GetValue() {
    // TODO: restore from 0.5.3 0x00458D80.
    return {};
}

// TOOL: 0.5.3 OsGui.cpp 0x00458D60
void COsTabControl::SetValue(int32_t a1) {
    // TODO: restore from 0.5.3 0x00458D60.
}

// TOOL: 0.5.3 OsGui.cpp 0x00458DA0
void COsTabControl::InsertItem(const char* a1, int32_t a2) {
    // TODO: restore from 0.5.3 0x00458DA0.
}

#endif
