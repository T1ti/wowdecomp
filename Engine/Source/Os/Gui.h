#ifndef ENGINE_SOURCE_OS_GUI_H
#define ENGINE_SOURCE_OS_GUI_H

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"

enum : int32_t {
    OSGUI_MESSAGE_STYLE_OK = 0,
    OSGUI_MESSAGE_STYLE_OKCANCEL = 1,
    OSGUI_MESSAGE_STYLE_YESNO = 2,
    OSGUI_MESSAGE_STYLE_YESNOCANCEL = 3,
};

enum : int32_t {
    OSGUI_MESSAGE_RESULT_YESOK = 0,
    OSGUI_MESSAGE_RESULT_NO = 1,
    OSGUI_MESSAGE_RESULT_CANCEL = 2,
};

enum : int32_t {
    OSGUI_KEY_CONTROL = 0,
    OSGUI_KEY_SHIFT = 1,
    OSGUI_KEY_ALT = 2,
    OSGUI_NUM_MOD_KEYS = 3,
};

enum : int32_t {
    OSGUI_WINDOW_TYPE_GX = 0,
    OSGUI_WINDOW_TYPE_ACTIVE = 1,
    OSGUI_WINDOW_TYPE_FRONT = 2,
    NUM_OSGUI_WINDOW_TYPES = 3,
};

enum : uint32_t {
    OSGUI_FILTER_LETTERS = 0x1,
    OSGUI_FILTER_DIGITS = 0x2,
    OSGUI_FILTER_SIGN = 0x4,
    OSGUI_FILTER_DECIMAL_POINT = 0x8,
    OSGUI_FILTER_SPACE = 0x10,
    OSGUI_FILTER_UNDERSCORE = 0x20,
    OSGUI_FILTER_PRINTABLE_ASCII = 0x40,
};

typedef void* OsGuiHandle;
typedef void* OsGuiTreeItem;

class COsDialog;
class COsMenu;
class COsTreeView;

struct OsGuiCallbackParams {
    /* 0000 */ int32_t type;
    /* 0004 */ int32_t subType;
    /* 0008 */ int32_t code;
    /* 000C */ void* user;
};

typedef void (*OsGuiCallback)(const OsGuiCallbackParams& params);

struct OsGuiCallbackInfo {
    /* 0000 */ OsGuiCallback function; // Preserve the current callback calling convention.
    /* 0004 */ void* userParam;
};

struct OsGuiTVDDInfo {
    /* 0000 */ COsTreeView* treeView;
    /* 0004 */ int32_t action;
    /* 0008 */ OsGuiTreeItem dragItem;
    /* 000C */ OsGuiTreeItem targItem;
    /* 0010 */ int32_t targX;
    /* 0014 */ int32_t targY;
};

struct OsGuiTreeItemParams {
    /* 0000 */ int32_t used;
    /* 0004 */ CImVector color;
    /* 0008 */ void* user;
};

#ifdef WOWDECOMP_TOOL_FEATURES
// Tool features: OsGui types from the 0.5.3 OsGui.cpp (layouts from the 0.5.3 type information).
struct OsGuiMenuHotkey {
    /* 0000 */ int32_t keyID;
    /* 0004 */ int32_t modKeyID;
};

struct OsGuiTVSelectionInfo {
    /* 0000 */ int32_t numSelected;
    /* 0004 */ OsGuiTreeItem firstSelection;
    /* 0008 */ uint32_t flags;
};

// Tree view callbacks; 0.5.3 declares them __fastcall, kept here with the default convention like OsGuiCallback.
typedef void (*OsGuiTVEnumFunc)(COsTreeView* treeView, OsGuiTreeItem item, void* param);
typedef int32_t (*OsGuiTVDragDropFunc)(const OsGuiTVDDInfo& info, void* param);
typedef int32_t (*OsGuiTVCanEditFunc)(OsGuiTreeItem item, void* param);
typedef void (*OsGuiTVExpandFunc)(OsGuiTreeItem item, void* param);
#endif

class COsControl {
    public:
    // m_type; named from the Win32 window class at that index in the control
    // class name table (0x00B1D350). Values whose class is shared by several
    // types (BUTTON, STATIC) are not listed.
    enum TYPE : int32_t {
        // 0.5.3 OsGui constructors pass the remaining values: COsButton 0, COsImageButton 1, COsStaticText 2,
        // COsStaticImage 3, COsCheckbox 7, COsTextButton 11, COsStaticBox 12, COsRadioButton 14, COsDivider 19.
        TYPE_BUTTON = 0,
        TYPE_IMAGE_BUTTON = 1,
        TYPE_STATIC_TEXT = 2,
        TYPE_STATIC_IMAGE = 3,
        TYPE_EDIT_BOX = 4, // EDIT
        TYPE_COMBO_BOX = 5, // COMBOBOX
        TYPE_LIST_BOX = 6, // LISTBOX
        TYPE_CHECKBOX = 7,
        TYPE_PROGRESS_BAR = 8, // msctls_progress32
        TYPE_TRACK_BAR = 9, // msctls_trackbar32
        TYPE_TREE_VIEW = 10, // SysTreeView32
        TYPE_TEXT_BUTTON = 11,
        TYPE_STATIC_BOX = 12,
        TYPE_UP_DOWN = 13, // msctls_updown32
        TYPE_RADIO_BUTTON = 14,
        TYPE_TAB_CONTROL = 15, // SysTabControl32
        TYPE_LIST_VIEW = 16, // SysListView32
        TYPE_TOOLBAR = 17, // ToolbarWindow32
        TYPE_SCROLL_BAR = 18, // SCROLLBAR
        TYPE_DIVIDER = 19,
    };

    /* 0004 */ uint32_t m_flags;
    /* 0008 */ COsDialog* m_dialog;
    /* 000C */ int16_t m_id;
    /* 0010 */ int32_t m_type; // TYPE
    /* 0014 */ OsGuiHandle m_handle;
    /* 0018 */ OsGuiCallback m_callback;
    /* 001C */ void* m_callbackParam;
    /* 0020 */ COsMenu* m_contextMenu;
    /* 0024 */ int32_t m_contextMenuEnabled;
    /* 0028 */ int32_t m_redrawLevel;

    virtual ~COsControl();
    virtual void OnDestroy();
    virtual int32_t OnEvent(int32_t itemId, int32_t notifyCode, int32_t code);
    virtual int32_t OnDraw(void* dc, uint32_t state, CiRect& rect);
    virtual void* OnSetColors(void* dc);
    virtual int32_t OnReturn();
    virtual int32_t OnEscape();
    virtual int32_t OnMouseDown();
    virtual int32_t OnMouseUp();
    virtual void OnMouseMove(int32_t x, int32_t y);
    virtual void OnSizeChange();
    virtual void OnTextChange();
    virtual int32_t OnNotify(int32_t code, void* data);
    virtual int32_t OnCommand(int32_t code);
    virtual int32_t OnScroll(int32_t code);
    virtual int32_t OnMouseWheel(int32_t delta);
    virtual int32_t OnContextMenu(int32_t x, int32_t y);
    virtual int32_t IsHandleFromControl(OsGuiHandle handle);
    virtual int32_t CanDoClipboardAction(int32_t action);
    virtual int32_t DoClipboardAction(int32_t action);
    virtual void SetValue(int32_t value);
    virtual int32_t GetValue();

    int32_t SendEvent(int32_t event, int32_t code);

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    COsControl(COsDialog* dialog, int32_t type, int16_t id, uint32_t flags); // 0.5.3 00454C70
    COsControl(void* window, int32_t type, int16_t id, uint32_t flags); // 0.5.3 00454CE0
    void Initialize(void*, int32_t, int16_t, uint32_t); // 0.5.3 00454D20
    void Enable(int32_t); // 0.5.3 004553B0
    void GetPosition(int32_t*, int32_t*, int32_t); // 0.5.3 00455410
    void GetSize(int32_t*, int32_t*); // 0.5.3 004554D0
    void GetText(char*, int32_t); // 0.5.3 00455290
    int32_t GetTextLength(); // 0.5.3 004552B0
    void GetTextSize(const char*, int32_t*, int32_t*); // 0.5.3 004552C0
    void GetTextSize(int32_t*, int32_t*); // 0.5.3 00455340
    int32_t HasInputFocus(); // 0.5.3 00455250
    int32_t IsEnabled(); // 0.5.3 004553D0
    int32_t IsShowing(); // 0.5.3 004553A0
    void LoseInputFocus(); // 0.5.3 00455230
    void Refresh(int32_t); // 0.5.3 004550F0
    void SetCallback(OsGuiCallback, void*); // 0.5.3 00455110
    void SetContextMenu(COsMenu*); // 0.5.3 004555C0
    void SetFont(int32_t); // 0.5.3 004551C0
    void SetInputFocus(); // 0.5.3 00455200
    void SetPosition(int32_t, int32_t); // 0.5.3 004553E0
    void SetRedraw(int32_t); // 0.5.3 004550B0
    void SetSize(int32_t, int32_t); // 0.5.3 004554A0
    void SetText(const char*); // 0.5.3 00455270
    void SetTooltip(const char*); // 0.5.3 00455510
    void Show(int32_t); // 0.5.3 00455380
#endif
};

class COsEditBox : public COsControl {
    public:
    /* 002C */ int32_t m_filtersEnabled;
    /* 0030 */ uint32_t m_filters;
    /* 0034 */ int32_t m_selSize;

    BOOL IsCharacterAllowed(char c);

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    void Initialize(); // 0.5.3 00455D90
    COsEditBox(COsDialog*, int16_t, uint32_t); // 0.5.3 00455D00
    COsEditBox(void*, int16_t, uint32_t); // 0.5.3 00455D60
    int32_t CanDoClipboardAction(int32_t) override; // 0.5.3 00455FE0
    int32_t DoClipboardAction(int32_t) override; // 0.5.3 00456070
    void EnableFilters(int32_t); // 0.5.3 00455EC0
    int32_t GetSelectionSize(); // 0.5.3 00455E90
    int32_t OnReturn() override; // 0.5.3 00455E40
    void SelectAll(); // 0.5.3 00455E70
    void SetFilter(uint32_t, int32_t); // 0.5.3 00455ED0
    void SetTextLimit(int32_t); // 0.5.3 00455E50
    void UpdateSelection(); // 0.5.3 00455E20
#endif
};

class COsTreeView : public COsControl {
    public:
    /* 002C */ void* m_images;
    /* 0030 */ TSGrowableArray<int32_t> m_unusedImageIDs;
    /* 0040 */ int32_t m_dragging;
    /* 0044 */ OsGuiTVDDInfo m_dragInfo;
    /* 005C */ void* m_dragImage;
    /* 0060 */ int32_t (*m_dragHandler)(const OsGuiTVDDInfo& info, void* param);
    /* 0064 */ void* m_dragHandlerParam;
    /* 0068 */ int32_t (*m_canEditFunc)(OsGuiTreeItem item, void* param);
    /* 006C */ void* m_canEditParam;
    /* 0070 */ void (*m_expandFunc)(OsGuiTreeItem item, void* param);
    /* 0074 */ void* m_expandParam;
    /* 0078 */ int32_t m_textLimit;
    /* 007C */ int32_t m_filtersEnabled;
    /* 0080 */ uint32_t m_filters;
    /* 0084 */ TSGrowableArray<OsGuiTreeItemParams> m_itemParams;

    OsGuiHandle GetEditControl();
    BOOL IsCharacterAllowed(char c);

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    void CreateDragImage(void*); // 0.5.3 00457DC0
    void DestroyDragImage(); // 0.5.3 00457DF0
    void* FindItemUnderCursor(); // 0.5.3 004579E0
    int32_t FindUnusedParams(); // 0.5.3 004584D0
    OsGuiTreeItemParams* GetParams(void*); // 0.5.3 004586B0
    void InitParams(void*); // 0.5.3 00458550
    void InitializeTreeView(); // 0.5.3 00456D40
    void OnDeleteItem(void*); // 0.5.3 00458360
    int32_t RunDragHandler(); // 0.5.3 00457F20
    COsTreeView(COsDialog*, int16_t, uint32_t); // 0.5.3 00456C30
    COsTreeView(void*, int16_t, uint32_t); // 0.5.3 00456CD0
    void ClearItems(); // 0.5.3 00456EE0
    void DeleteItem(void*); // 0.5.3 00456F00
    void EditItem(void*); // 0.5.3 004578C0
    void EnableDragDrop(int32_t); // 0.5.3 00457D50
    void EnableFilters(int32_t); // 0.5.3 00458290
    void EnsureItemVisible(void*); // 0.5.3 00457690
    void EnumerateAllItems(OsGuiTVEnumFunc, void*); // 0.5.3 00457200
    void EnumerateItems(void*, OsGuiTVEnumFunc, void*); // 0.5.3 004571A0
    void ExpandItem(void*, int32_t); // 0.5.3 00457600
    void* GetFirstVisibleItem(); // 0.5.3 00457EB0
    void* GetItemChild(void*, int32_t); // 0.5.3 00457150
    CImVector GetItemColor(void*); // 0.5.3 004570C0
    int32_t GetItemNumChildren(void*); // 0.5.3 00457100
    void* GetItemParam(void*); // 0.5.3 00457030
    void* GetItemParent(void*); // 0.5.3 004570E0
    CiRect GetItemRect(void*); // 0.5.3 00457E10
    void GetItemText(void*, char*, int32_t); // 0.5.3 00456FD0
    void* GetSelectedItem(); // 0.5.3 004576B0
    void GetSelectionInfo(OsGuiTVSelectionInfo*); // 0.5.3 004577B0
    void* InsertItem(void*, void*, const char*); // 0.5.3 00456F20
    int32_t IsHandleFromControl(void*) override; // 0.5.3 00457D20
    int32_t IsItemExpanded(void*); // 0.5.3 00457630
    int32_t IsItemSelected(void*); // 0.5.3 00457770
    void OnBeginDrag(void*, int32_t, int32_t); // 0.5.3 00457F40
    int32_t OnBeginEdit(void*); // 0.5.3 00458200
    void OnEndDrag(); // 0.5.3 004581A0
    int32_t OnEndEdit(void*, const char*); // 0.5.3 00458250
    int32_t OnEscape() override; // 0.5.3 00457AA0
    void OnExpandedItem(void*); // 0.5.3 00457670
    int32_t OnMouseDown() override; // 0.5.3 004578F0
    void OnMouseMove(int32_t, int32_t) override; // 0.5.3 00458040
    int32_t OnMouseUp() override; // 0.5.3 00457930
    int32_t OnNotify(int32_t, void*) override; // 0.5.3 00457AD0
    int32_t OnReturn() override; // 0.5.3 00457A60
    void OnSizeChange() override; // 0.5.3 00457660
    void RefreshItem(void*); // 0.5.3 00457E70
    void ResetItemColor(void*); // 0.5.3 00457090
    void SelectAll(int32_t); // 0.5.3 00457880
    void SelectItem(void*, int32_t); // 0.5.3 004576F0
    void SetBackgroundColor(const CImVector&); // 0.5.3 00456EB0
    void SetCanEditFunction(OsGuiTVCanEditFunc, void*); // 0.5.3 00458320
    void SetDragDropHandler(OsGuiTVDragDropFunc, void*); // 0.5.3 00457DA0
    void SetDropTarget(void*); // 0.5.3 00458130
    void SetExpandFunction(OsGuiTVExpandFunc, void*); // 0.5.3 00458340
    void SetFilter(uint32_t, int32_t); // 0.5.3 004582A0
    void SetFirstVisibleItem(void*); // 0.5.3 00457ED0
    void SetInsertionMark(void*, int32_t); // 0.5.3 00458160
    void SetItemColor(void*, const CImVector&); // 0.5.3 00457050
    void SetItemImage(void*, int32_t, int32_t, void*); // 0.5.3 00457240
    void SetItemParam(void*, void*); // 0.5.3 00457010
    void SetItemText(void*, const char*); // 0.5.3 00456F90
    void SetTextLimit(int32_t); // 0.5.3 00458280
    ~COsTreeView() override; // 0.5.3 00456E50
#endif
};

class COsTabControl : public COsControl {
    public:
    int32_t GetNumItems();
    int32_t OnControlTab();

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    int32_t GetValue() override; // 0.5.3 00458D80
    void InsertItem(const char*, int32_t); // 0.5.3 00458DA0
    void SetValue(int32_t) override; // 0.5.3 00458D60
#endif
};

class COsDialog {
    public:
    /* 0004 */ OsGuiHandle m_handle;
    /* 0008 */ void* m_tooltips;
    /* 000C */ OsGuiCallbackInfo m_callbackInfo;
    /* 0014 */ TSGrowableArray<COsControl*> m_controls;
    /* 0024 */ COsControl* m_cancelButton;
    /* 0028 */ int32_t m_trackMouse;
    /* 002C */ int32_t m_mouseInside;
    /* 0030 */ BOOL m_needNewTrack;
    /* 0034 */ int32_t m_tooltipsEnabled;
    /* 0038 */ int32_t m_contextMenuEnabled;
    /* 003C */ COsMenu* m_contextMenu;
    /* 0040 */ uint32_t m_flags;
    /* 0044 */ TSGrowableArray<OsGuiHandle> m_disabledWindows;
    /* 0054 */ C2iVector m_minSize;

    virtual void SetSize(int32_t width, int32_t height);

    COsControl* FindControl(OsGuiHandle handle);
    OsGuiHandle GetParentWindow();
    BOOL HasFlag(uint32_t flag);
    BOOL IsInFront();
    int32_t OnCancel();
    int32_t OnControlTab();
    int32_t OnEvent(int32_t itemId, int32_t notifyCode, int32_t code);
    int32_t ProcessMessage(void* message);

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    void ApplyModality(int32_t); // 0.5.3 00453D80
    int32_t FindControl(COsControl*); // 0.5.3 00454130
    void AddControl(COsControl*); // 0.5.3 00454090
    void BringToFront(); // 0.5.3 004545C0
    COsDialog(void*, uint32_t); // 0.5.3 00453190
    int32_t CanDoClipboardAction(int32_t); // 0.5.3 00454C10
    void CheckEvents(); // 0.5.3 004544D0
    void DeleteControl(COsControl*); // 0.5.3 004541A0
    void DetachControl(COsControl*); // 0.5.3 004541E0
    int32_t DoClipboardAction(int32_t); // 0.5.3 00454C40
    void EnableTooltips(int32_t); // 0.5.3 00454060
    int32_t GetMinSize(int32_t*, int32_t*); // 0.5.3 004547B0
    void GetPosition(int32_t*, int32_t*, int32_t); // 0.5.3 004546D0
    void GetSize(int32_t*, int32_t*, int32_t); // 0.5.3 00454750
    void* GetTooltips(); // 0.5.3 00453FC0
    int32_t IsEnabled(); // 0.5.3 00454640
    int32_t IsMouseInside(); // 0.5.3 00454540
    int32_t IsShowing(); // 0.5.3 00454630
    int32_t OnAccept(); // 0.5.3 00454810
    int32_t OnContextMenu(int32_t, int32_t); // 0.5.3 00454B90
    int32_t OnMouseDown(); // 0.5.3 00454860
    int32_t OnMouseLeave(); // 0.5.3 00454920
    int32_t OnMouseMove(int32_t, int32_t); // 0.5.3 00454960
    int32_t OnMouseUp(); // 0.5.3 004548C0
    void Refresh(int32_t); // 0.5.3 00454670
    void SetCallback(OsGuiCallback, void*); // 0.5.3 004545A0
    void SetContextMenu(COsMenu*); // 0.5.3 00454B30
    void SetInputFocus(); // 0.5.3 00454600
    void SetMinSize(int32_t, int32_t); // 0.5.3 00454790
    void SetPosition(int32_t, int32_t); // 0.5.3 004546A0
    void SetRedraw(int32_t); // 0.5.3 00454650
    void SetTitle(const char*); // 0.5.3 004547F0
    void SetTrackMouse(int32_t); // 0.5.3 00454510
    void Show(int32_t); // 0.5.3 00454610
    ~COsDialog(); // 0.5.3 00453B40
#endif
};

class COsMenuBar {
    public:
    /* 0000 */ TSGrowableArray<COsMenu*> m_menus;
    /* 0010 */ OsGuiHandle m_windowHandle;
    /* 0014 */ void* m_menuBarHandle;
    /* 0018 */ void* m_accelerators;

#ifdef WOWDECOMP_TOOL_FEATURES
    // Tool features: the rest of the 0.5.3 OsGui.cpp interface (stub bodies in Os/GuiToolkit.cpp).
    COsMenuBar(void*); // 0.5.3 00452A90
    void Refresh(); // 0.5.3 00453180
    void Set(TSGrowableArray<COsMenu*>&); // 0.5.3 00452D10
    void UpdateAccelerators(); // 0.5.3 00452E30
    ~COsMenuBar(); // 0.5.3 00452B60
#endif
};

void OsGuiBeep();
OsGuiHandle OsGuiGetWindow(int32_t type);
int32_t OsGuiIsModifierKeyDown(int32_t key);
int32_t OsGuiProcessMessage(void* message);
void OsGuiSetGxWindow(OsGuiHandle window);
int32_t OsGuiMessageBox(OsGuiHandle parentWindow, int32_t style, const char* message, const char* title);
void OsGuiSetWindowTitle(OsGuiHandle window, const char* title);


#ifdef WOWDECOMP_TOOL_FEATURES
// Tool features: the OsGui Win32 control toolkit of the 0.5.3 ENGINE\Source\Os\W32\OsGui.cpp. The 3.3.5
// release build keeps COsControl, COsDialog, COsEditBox, COsTreeView, COsTabControl and COsMenuBar (above) and
// a few of their methods; everything below was compiled out. Layouts are the 0.5.3 ones (32-bit offsets);
// method bodies are stubs in Os/GuiToolkit.cpp, each naming its 0.5.3 address.

class COsMenu {
    public:
    /* 0000 */ uint8_t m_id;
    /* 0004 */ void* m_menuHandle;
    /* 0008 */ char m_title[32];
    /* 0028 */ TSGrowableArray<OsGuiMenuHotkey> m_hotkeys;

    void AddHotkey(int32_t); // 0.5.3 00452310
    void RemoveHotkey(int32_t); // 0.5.3 00452430
    void AddSeparator(int32_t); // 0.5.3 004528D0
    void AddSubMenu(int32_t, const char*, COsMenu*); // 0.5.3 00452870
    void AddTextItem(int32_t, const char*, OsGuiMenuHotkey*); // 0.5.3 00452780
    static void AppendHotkeyText(char*, const OsGuiMenuHotkey&); // 0.5.3 00452550
    COsMenu(); // 0.5.3 00452190
    COsMenu(uint8_t, const char*); // 0.5.3 00452140
    void CheckItem(int32_t, int32_t); // 0.5.3 00452980
    void Clear(); // 0.5.3 004521F0
    void EnableItem(int32_t, int32_t); // 0.5.3 00452920
    int32_t GetHotkey(int32_t, OsGuiMenuHotkey*); // 0.5.3 00452250
    int32_t GetNumItems(); // 0.5.3 00452240
    void RemoveItem(int32_t); // 0.5.3 00452210
    void SetItemText(int32_t, const char*); // 0.5.3 004529E0
    ~COsMenu(); // 0.5.3 004521C0
};

class COsWindow {
    public:
    /* 0004 */ OsGuiHandle m_handle;
    /* 0008 */ C2iVector m_minSize;

    COsWindow(void*); // 0.5.3 0045A840
    void GetMinSize(int32_t*, int32_t*); // 0.5.3 0045A8E0
    virtual void OnResize(); // 0.5.3 0045AF60
    void SetCursor(int32_t); // 0.5.3 0045A900
    void SetIcon(const char*); // 0.5.3 0045A930
    void SetInputFocus(); // 0.5.3 0045A950
    void SetMinSize(int32_t, int32_t); // 0.5.3 0045A8C0
    virtual ~COsWindow(); // 0.5.3 0045A8B0
};

class COsSharedMemory {
    public:
    /* 0000 */ void* m_data;
    /* 0004 */ uint32_t m_size;
    /* 0008 */ uint8_t m_opaqueData[4];

    COsSharedMemory(); // 0.5.3 0045B780
    bool ChangeAccess(int32_t); // 0.5.3 0045B860
    void Destroy(); // 0.5.3 0045B8C0
    bool Initialize(const char*, uint32_t, int32_t); // 0.5.3 0045B7A0
    ~COsSharedMemory(); // 0.5.3 0045B790
};

class COsButton : public COsControl {
    public:
    COsButton(COsDialog*, int16_t, uint32_t); // 0.5.3 00455780
    void SetCancelButton(); // 0.5.3 00455800
    void SetDefaultButton(); // 0.5.3 004557E0
    void SetHighlight(int32_t); // 0.5.3 00455810
};

class COsTextButton : public COsControl {
    public:
    /* 002C */ CImVector m_activeColor;
    /* 0030 */ CImVector m_pushedColor;
    /* 0034 */ CImVector m_greyedColor;
    /* 0038 */ int32_t m_underline;

    COsTextButton(COsDialog*, int16_t); // 0.5.3 00458770
    int32_t OnDraw(void*, uint32_t, CiRect&) override; // 0.5.3 00458800
};

class COsImageButton : public COsControl {
    public:
    COsImageButton(COsDialog*, int16_t, uint32_t); // 0.5.3 00455850
    COsImageButton(void*, int16_t, uint32_t); // 0.5.3 004558B0
    int32_t IsPushed(); // 0.5.3 00455A10
    void OnDestroy() override; // 0.5.3 004558F0
    void SetHighlight(int32_t); // 0.5.3 004559D0
    void SetImage(int32_t, int32_t, void*); // 0.5.3 00455910
    ~COsImageButton() override; // 0.5.3 004558E0
};

class COsRadioButton : public COsControl {
    public:
    COsRadioButton(COsDialog*, int16_t, uint32_t); // 0.5.3 00458CB0
    int32_t GetValue() override; // 0.5.3 00458D40
    void SetValue(int32_t) override; // 0.5.3 00458D10
};

class COsCheckbox : public COsControl {
    public:
    /* 002C */ int32_t m_settingSize;
    /* 0030 */ int32_t m_maxWidth;

    COsCheckbox(COsDialog*, int16_t); // 0.5.3 00456860
    COsCheckbox(void*, int16_t); // 0.5.3 004568D0
    void ClearValue(); // 0.5.3 00456980
    int32_t GetValue() override; // 0.5.3 00456960
    int32_t HasValue(); // 0.5.3 004569A0
    int32_t OnEvent(int32_t, int32_t, int32_t) override; // 0.5.3 004569C0
    void OnSizeChange() override; // 0.5.3 00456A60
    void OnTextChange() override; // 0.5.3 00456A00
    void SetMaxWidth(int32_t); // 0.5.3 00456910
    void SetValue(int32_t) override; // 0.5.3 00456930
};

class COsStaticText : public COsControl {
    public:
    /* 002C */ CImVector m_textColor;

    void Initialize(); // 0.5.3 00455AE0
    COsStaticText(COsDialog*, int16_t, uint32_t); // 0.5.3 00455A30
    COsStaticText(void*, int16_t, uint32_t); // 0.5.3 00455AA0
    void* OnSetColors(void*) override; // 0.5.3 00455B70
    void SetJustification(int32_t); // 0.5.3 00455AF0
    void SetTextColor(const CImVector&); // 0.5.3 00455B40
};

class COsStaticImage : public COsControl {
    public:
    COsStaticImage(COsDialog*, int16_t); // 0.5.3 00455BC0
    void ClearImage(); // 0.5.3 00455CB0
    void OnDestroy() override; // 0.5.3 00455C30
    void SetImage(int32_t, int32_t, void*); // 0.5.3 00455C50
    ~COsStaticImage() override; // 0.5.3 00455C20
};

class COsStaticBox : public COsControl {
    public:
    /* 002C */ TSGrowableArray<CiRect> m_transRects;

    void AddTransparentRect(const CiRect&); // 0.5.3 00458970
    void ClearTransparentRects(); // 0.5.3 00458960
    int32_t OnDraw(void*, uint32_t, CiRect&) override; // 0.5.3 00458A40
};

class COsPopupMenu : public COsControl {
    public:
    /* 002C */ int32_t m_baseHeight;
    /* 0030 */ int32_t m_maxHeight;

    void AdjustHeight(); // 0.5.3 00456320
    COsPopupMenu(COsDialog*, int16_t); // 0.5.3 00456140
    void ClearItems(); // 0.5.3 00456240
    void DeleteItem(int32_t); // 0.5.3 00456390
    int32_t GetItemHeight(); // 0.5.3 00456300
    int32_t GetNumItems(); // 0.5.3 00456260
    int32_t GetValue() override; // 0.5.3 00456220
    void InsertItem(const char*, int32_t); // 0.5.3 00456280
    void SetItemHeight(int32_t); // 0.5.3 004562D0
    void SetItemText(int32_t, const char*); // 0.5.3 004563C0
    void SetMaxHeight(int32_t); // 0.5.3 00456370
    void SetSize(int32_t, int32_t); // 0.5.3 004561D0
    void SetValue(int32_t) override; // 0.5.3 00456200
    ~COsPopupMenu() override; // 0.5.3 004561C0
};

class COsListBox : public COsControl {
    public:
    COsListBox(COsDialog*, int16_t, uint32_t); // 0.5.3 004563F0
    void ClearItems(); // 0.5.3 00456610
    void DeleteItem(int32_t); // 0.5.3 00456670
    int32_t GetItemHeight(); // 0.5.3 00456770
    void GetItemText(int32_t, char*, int32_t); // 0.5.3 004566F0
    int32_t GetItemTextLength(int32_t); // 0.5.3 004566D0
    int32_t GetNumItems(); // 0.5.3 00456630
    int32_t GetValue() override; // 0.5.3 004564B0
    void InsertItem(const char*, int32_t); // 0.5.3 00456650
    int32_t IsItemSelected(int32_t); // 0.5.3 00456550
    int32_t OnContextMenu(int32_t, int32_t) override; // 0.5.3 00456790
    int32_t OnReturn() override; // 0.5.3 00456850
    void SelectAll(int32_t); // 0.5.3 004565A0
    void SelectItem(int32_t, int32_t); // 0.5.3 00456500
    void SetItemHeight(int32_t); // 0.5.3 00456750
    void SetItemText(int32_t, const char*); // 0.5.3 004566A0
    void SetValue(int32_t) override; // 0.5.3 00456460
    ~COsListBox() override; // 0.5.3 00456450
};

class COsListView : public COsControl {
    public:
    /* 002C */ int32_t m_numCols;

    COsListView(COsDialog*, int16_t, uint32_t); // 0.5.3 00458E60
    void ClearRows(); // 0.5.3 00459030
    void DeleteColumn(int32_t); // 0.5.3 00458F80
    void DeleteRow(int32_t); // 0.5.3 00459010
    void EnsureRowVisible(int32_t); // 0.5.3 00459270
    void GetColumnTitle(int32_t, char*, int32_t); // 0.5.3 004591E0
    int32_t GetColumnWidth(int32_t); // 0.5.3 00459190
    void GetItemText(int32_t, int32_t, char*, int32_t); // 0.5.3 00459130
    int32_t GetNumColumns(); // 0.5.3 00458FB0
    int32_t GetNumRows(); // 0.5.3 00459050
    CImVector GetRowColor(int32_t); // 0.5.3 004590B0
    int32_t GetValue() override; // 0.5.3 004592F0
    void InsertColumn(int32_t); // 0.5.3 00458F20
    void InsertRow(int32_t); // 0.5.3 00458FC0
    void OnColumnClick(int32_t); // 0.5.3 00459360
    int32_t OnNotify(int32_t, void*) override; // 0.5.3 00459380
    int32_t OnReturn() override; // 0.5.3 004594A0
    void OnSelectionChange(); // 0.5.3 00459350
    void OnSizeChange() override; // 0.5.3 00459340
    void SetColumnJustification(int32_t, int32_t); // 0.5.3 00459220
    void SetColumnTitle(int32_t, const char*); // 0.5.3 004591B0
    void SetColumnWidth(int32_t, int32_t); // 0.5.3 00459170
    void SetItemText(int32_t, int32_t, const char*); // 0.5.3 004590F0
    void SetRowColor(int32_t, const CImVector&); // 0.5.3 00459070
    void SetValue(int32_t) override; // 0.5.3 00459290
    ~COsListView() override; // 0.5.3 00458F10
};

class COsSlider : public COsControl {
    public:
    COsSlider(COsDialog*, int16_t); // 0.5.3 00456B50
    int32_t GetValue() override; // 0.5.3 00456C10
    void SetMaxValue(int32_t); // 0.5.3 00456BD0
    void SetMinValue(int32_t); // 0.5.3 00456BB0
    void SetValue(int32_t) override; // 0.5.3 00456BF0
};

class COsSpinButton : public COsControl {
    public:
    void Initialize(); // 0.5.3 00458BD0
    COsSpinButton(COsDialog*, int16_t, uint32_t); // 0.5.3 00458B40
    COsSpinButton(void*, int16_t, uint32_t); // 0.5.3 00458BA0
    int32_t GetValue() override; // 0.5.3 00458C90
    void OnSpinMouseUp(); // 0.5.3 00458C40
    void SetValue(int32_t) override; // 0.5.3 00458C70
    void SetValueRange(int32_t, int32_t); // 0.5.3 00458C50
};

class COsScrollBar : public COsControl {
    public:
    /* 002C */ int32_t m_realMin;
    /* 0030 */ int32_t m_realMax;
    /* 0034 */ int32_t m_pageSize;

    void Initialize(); // 0.5.3 004599C0
    void UpdateRangeValues(); // 0.5.3 00459A40
    COsScrollBar(COsDialog*, int16_t, uint32_t); // 0.5.3 00459930
    COsScrollBar(void*, int16_t, uint32_t); // 0.5.3 00459990
    int32_t GetValue() override; // 0.5.3 00459AF0
    int32_t OnMouseWheel(int32_t) override; // 0.5.3 00459C70
    int32_t OnScroll(int32_t) override; // 0.5.3 00459B10
    void SetPageSize(int32_t); // 0.5.3 00459A00
    void SetRange(int32_t, int32_t); // 0.5.3 004599E0
    void SetValue(int32_t) override; // 0.5.3 00459AA0
};

class COsProgressBar : public COsControl {
    public:
    COsProgressBar(COsDialog*, int16_t); // 0.5.3 00456AB0
    int32_t GetValue() override; // 0.5.3 00456B30
    void SetValue(int32_t) override; // 0.5.3 00456B10
};

class COsToolBar : public COsControl {
    public:
    /* 002C */ void* m_imageList;

    void InitializeToolBar(); // 0.5.3 00459560
    void AddButton(int32_t); // 0.5.3 00459620
    void AddSeparator(int32_t); // 0.5.3 00459670
    COsToolBar(COsDialog*, int16_t, uint32_t); // 0.5.3 004594B0
    COsToolBar(void*, int16_t, uint32_t); // 0.5.3 00459510
    void CheckButton(int32_t, int32_t); // 0.5.3 004598B0
    void Clear(); // 0.5.3 00459600
    void EnableButton(int32_t, int32_t); // 0.5.3 00459850
    void GetButtonSize(int32_t*, int32_t*); // 0.5.3 004595D0
    void GetButtonText(int32_t, char*, int32_t); // 0.5.3 00459810
    int32_t GetNumButtons(); // 0.5.3 004596E0
    int32_t OnCommand(int32_t) override; // 0.5.3 00459910
    void RemoveButton(int32_t); // 0.5.3 004596C0
    void SetButtonImage(int32_t, int32_t, int32_t, void*); // 0.5.3 00459700
    void SetButtonSize(int32_t, int32_t); // 0.5.3 004595A0
    void SetButtonText(int32_t, const char*); // 0.5.3 004597D0
    ~COsToolBar() override; // 0.5.3 00459540
};

class COsDivider : public COsControl {
    public:
    /* 002C */ int32_t m_maxPos;
    /* 0030 */ int32_t m_minPos;
    /* 0034 */ int32_t m_tracking;
    /* 0038 */ int32_t m_dragging;
    /* 003C */ int32_t m_dragStartMouseX;
    /* 0040 */ int32_t m_dragStartMouseY;
    /* 0044 */ CiRect m_dragStartPos;

    void Initialize(); // 0.5.3 00459D90
    void UpdateCursor(); // 0.5.3 00459F00
    COsDivider(COsDialog*, int16_t, uint32_t); // 0.5.3 00459CD0
    COsDivider(void*, int16_t, uint32_t); // 0.5.3 00459D40
    void OnDivMouseDown(); // 0.5.3 00459F20
    void OnDivMouseLeave(); // 0.5.3 0045A0F0
    void OnDivMouseMove(int32_t, int32_t); // 0.5.3 00459FB0
    void OnDivMouseUp(); // 0.5.3 0045A0C0
    void SetPositionRange(int32_t, int32_t); // 0.5.3 0045A100
    ~COsDivider() override; // 0.5.3 00459D80
};

void OsGuiBringWindowToFront(void*); // 0.5.3 0045A340
void OsGuiDestroy(); // 0.5.3 00451E60
void OsGuiEnableMenuHotkeys(int32_t); // 0.5.3 00452130
void OsGuiEnableTooltips(int32_t); // 0.5.3 00452090
void OsGuiEnableWindow(void*, int32_t); // 0.5.3 0045A380
CImVector OsGuiGetColor(int32_t); // 0.5.3 0045A7E0
void OsGuiGetCursorPosition(int32_t*, int32_t*); // 0.5.3 0045A1E0
void OsGuiGetHotkeyText(const OsGuiMenuHotkey&, char*, int32_t); // 0.5.3 0045A7C0
CiRect OsGuiGetScreenBounds(); // 0.5.3 0045A640
CiRect OsGuiGetWindowRect(void*, int32_t); // 0.5.3 0045A290
CiRect OsGuiGetWindowRestoredRect(void*); // 0.5.3 0045A4E0
void OsGuiInitialize(); // 0.5.3 00451E10
void OsGuiMaximizeWindow(void*, int32_t); // 0.5.3 0045A400
void OsGuiMenuSelect(int32_t, int32_t); // 0.5.3 00451DF0
void OsGuiMinimizeWindow(void*, int32_t); // 0.5.3 0045A470
void OsGuiSetApplicationInfo(void*); // 0.5.3 00451E90
void OsGuiSetCursor(int32_t); // 0.5.3 0045A1B0
void OsGuiSetIdleCallback(OsGuiCallback, void*); // 0.5.3 00451EB0
void OsGuiSetMenuCommandCallback(OsGuiCallback, void*); // 0.5.3 00451EA0
void OsGuiSetWindowIcon(void*, const char*); // 0.5.3 0045A220
void OsGuiSetWindowRect(void*, const CiRect&); // 0.5.3 0045A310
void OsGuiSetWindowRestoredRect(void*, const CiRect&); // 0.5.3 0045A540
void OsGuiShowCursor(int32_t); // 0.5.3 0045A1D0
void OsGuiShowWindow(void*, int32_t); // 0.5.3 0045A350
int32_t OsGuiWindowEnabled(void*); // 0.5.3 0045A390
int32_t OsGuiWindowIsCursorInside(void*, int32_t); // 0.5.3 0045A5B0
int32_t OsGuiWindowMaximized(void*); // 0.5.3 0045A440
int32_t OsGuiWindowMinimized(void*); // 0.5.3 0045A4B0
LRESULT OsGuiWindowProc(OsGuiHandle window, uint32_t message, WPARAM wparam, LPARAM lparam); // 0.5.3 0045A960
#endif

#endif
