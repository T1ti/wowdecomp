// Tool features: the OsGui Win32 control toolkit, ported from the 0.5.3 client.
//
// This file is not compiled on its own: Win32/Gui.cpp and Win64/Gui.cpp include it at their end, inside
// WOWDECOMP_TOOL_FEATURES, so that it shares their file statics (s_appInstance, s_GxDevWindow, s_globalTips,
// s_dialogs, s_menubars, s_menuHotkeysEnabled, sIsCharacterAllowed) the way the single 0.5.3 OsGui.cpp did.
//
// Source: the 0.5.3 client (WoWClient.exe 3368) links ENGINE\Source\Os\W32\OsGui.cpp whole (0x00451D00..0x0045B8F0):
// menus and menu bars with hotkeys, modal dialogs, tooltips, and the COsControl family (buttons, edit boxes, combo
// boxes, list and tree views with drag and drop and in-place editing, sliders, spin buttons, toolbars, dividers...).
// Nothing in the game calls the dialog, menu bar, toolbar or tree view code; it is the interface layer of the
// in-house tools that linked the same engine. The 3.3.5 release build compiled it out and kept only what the game
// window uses (the rest of Gui.cpp). The 3.3.5 class layouts and control class name table (0x00B1D350) are the
// 0.5.3 ones, so the code fits back unchanged.
//
// Every function names its 0.5.3 address. The bodies follow the 0.5.3 code; the port changes are:
//   - Win64: GWL_*/GCL_* become GWLP_*/GCLP_* with Get/SetWindowLongPtr and Get/SetClassLongPtr, DWL_MSGRESULT
//     becomes DWLP_MSGRESULT, control ids go through INT_PTR, and COsSharedMemory keeps its mapping as a pointer.
//   - Message numbers, styles and notification codes are written with their <windows.h>/<commctrl.h> names.
//   - Hotkeys use the 3.3.5 KEY_* numbering (Event/Types.h), which matches 0.5.3 for letters, digits, space and the
//     0x200/0x300 blocks; the 0.5.3 meta keys (0 shift, 1 control, 2 alt), tilde (256), numpad (258..266) and
//     brackets (274, 275) moved in 3.3.5 and are mapped to their 3.3.5 values.
//   - 0.5.3 OsGuiInitialize installed OsGuiWindowProc through OsSetWindowProc; 3.3.5 has no such hook (the window
//     procedure is handed to CGxDevice::DeviceCreate), so a tool's own window procedure must forward to
//     OsGuiWindowProc.
//   - Two 0.5.3 bugs are fixed and marked: ~COsDialog skipped every other control while deleting them, and
//     COsMenu::AddSubMenu inserted its hotkey slot twice.

#include <cstdlib>
#include "Engine/Source/Event/Types.h"

#define OSGUI_ASSERT(expr) ((expr) ? (void)0 : (void)SErrDisplayError(STORM_ERROR_ASSERTION, __FILE__, __LINE__, #expr, 0, 1, 0))

// ===== File statics (0.5.3 addresses) =====

// 0.5.3 0x00CC455C..0x00CC4568 sCallbacks: [0] menu command callback, [1] idle callback.
static OsGuiCallbackInfo s_callbacks[2];

// 0.5.3 0x00CC456C sIdleTimerID: the timer that runs the idle callback while a menu or modal loop is open.
static UINT_PTR s_idleTimerID;

// 0.5.3 0x00835CB0 sMasterTooltipsEnabled.
static int32_t s_masterTooltipsEnabled = 1;

// 0.5.3 0x00835CB8 ControlClassName (3.3.5 keeps the same table at 0x00B1D350), indexed by COsControl::TYPE.
static const char* const s_controlClassName[] = {
    "BUTTON", // TYPE_BUTTON
    "BUTTON", // TYPE_IMAGE_BUTTON
    "STATIC", // TYPE_STATIC_TEXT
    "STATIC", // TYPE_STATIC_IMAGE
    "EDIT", // TYPE_EDIT_BOX
    "COMBOBOX", // TYPE_COMBO_BOX
    "LISTBOX", // TYPE_LIST_BOX
    "BUTTON", // TYPE_CHECKBOX
    "msctls_progress32", // TYPE_PROGRESS_BAR
    "msctls_trackbar32", // TYPE_TRACK_BAR
    "SysTreeView32", // TYPE_TREE_VIEW
    "BUTTON", // TYPE_TEXT_BUTTON
    "STATIC", // TYPE_STATIC_BOX
    "msctls_updown32", // TYPE_UP_DOWN
    "BUTTON", // TYPE_RADIO_BUTTON
    "SysTabControl32", // TYPE_TAB_CONTROL
    "SysListView32", // TYPE_LIST_VIEW
    "ToolbarWindow32", // TYPE_TOOLBAR
    "SCROLLBAR", // TYPE_SCROLL_BAR
    "STATIC", // TYPE_DIVIDER
};

static const uint32_t OSGUI_NUM_CONTROL_TYPES = sizeof(s_controlClassName) / sizeof(s_controlClassName[0]);

// 0.5.3 0x008036C0: window style per control type (WS_CHILD | WS_VISIBLE are added by COsControl::Initialize).
static const uint32_t s_controlStyle[OSGUI_NUM_CONTROL_TYPES] = {
    WS_TABSTOP, // button
    BS_BITMAP, // image button
    SS_NOTIFY, // static text
    SS_NOTIFY | SS_BITMAP, // static image
    WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, // edit box
    WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST, // combo box
    WS_BORDER | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY, // list box
    WS_TABSTOP | BS_AUTO3STATE, // checkbox
    WS_BORDER | PBS_SMOOTH, // progress bar
    TBS_AUTOTICKS, // track bar
    WS_BORDER | WS_TABSTOP | TVS_HASBUTTONS | TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS, // tree view
    BS_OWNERDRAW, // text button
    SS_BLACKFRAME, // static box
    UDS_ARROWKEYS, // spin button
    BS_AUTORADIOBUTTON, // radio button
    0, // tab control
    WS_BORDER | WS_VSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS, // list view
    TBSTYLE_TOOLTIPS | TBSTYLE_LIST, // toolbar
    0, // scroll bar
    WS_THICKFRAME | SS_NOTIFY, // divider
};

// 0.5.3 0x00803718: extended style per control type (all zero in 0.5.3; kept as a table like the original).
static const uint32_t s_controlExStyle[OSGUI_NUM_CONTROL_TYPES] = {};

// 0.5.3 0x0080374C: COsControl::SetFont font per control type (0 ANSI_VAR_FONT, 1 SYSTEM_FONT, 2 ANSI_FIXED_FONT).
static const int32_t s_controlFont[OSGUI_NUM_CONTROL_TYPES] = { 0, 0, 1 };

// 0.5.3 0x008035F8: Win32 notification code -> OsGui event code, per control type (sNCodeToItemCode).
// OsGui events: 0 clicked, 1 double-clicked, 2 value changed, 8 label edited, 13 lost focus, 14 got focus.
struct OsGuiNotifyEntry {
    int32_t ctrlType;
    int32_t winCode;
    int32_t osGuiCode;
};

static const OsGuiNotifyEntry s_notifyTable[] = {
    { COsControl::TYPE_BUTTON, BN_CLICKED, 0 },
    { COsControl::TYPE_IMAGE_BUTTON, BN_CLICKED, 0 },
    { COsControl::TYPE_EDIT_BOX, EN_CHANGE, 2 },
    { COsControl::TYPE_EDIT_BOX, EN_KILLFOCUS, 13 },
    { COsControl::TYPE_COMBO_BOX, CBN_SELCHANGE, 2 },
    { COsControl::TYPE_LIST_BOX, LBN_SELCHANGE, 2 },
    { COsControl::TYPE_LIST_BOX, LBN_DBLCLK, 1 },
    { COsControl::TYPE_CHECKBOX, BN_CLICKED, 2 },
    { COsControl::TYPE_TREE_VIEW, static_cast<int32_t>(TVN_SELCHANGEDA), 2 },
    { COsControl::TYPE_TREE_VIEW, static_cast<int32_t>(NM_DBLCLK), 1 },
    { COsControl::TYPE_TREE_VIEW, static_cast<int32_t>(TVN_ENDLABELEDITA), 8 },
    { COsControl::TYPE_TREE_VIEW, static_cast<int32_t>(NM_SETFOCUS), 14 },
    { COsControl::TYPE_TREE_VIEW, static_cast<int32_t>(NM_KILLFOCUS), 13 },
    { COsControl::TYPE_TEXT_BUTTON, BN_CLICKED, 0 },
    { COsControl::TYPE_UP_DOWN, SB_THUMBPOSITION, 2 },
    { COsControl::TYPE_RADIO_BUTTON, BN_CLICKED, 2 },
    { COsControl::TYPE_TAB_CONTROL, static_cast<int32_t>(TCN_SELCHANGE), 2 },
    { COsControl::TYPE_LIST_VIEW, static_cast<int32_t>(NM_DBLCLK), 1 },
};

// OsGuiTVSelectionInfo::flags (sTVGetSelectInfo).
enum : uint32_t {
    OSGUI_TVSI_SAME_PARENT = 0x1, // every selected item has the parent of the first one
    OSGUI_TVSI_CONTIGUOUS = 0x2, // the selected items follow each other in enumeration order
};

// sTVGetSelectInfo accumulator (0.5.3 OsGuiTVSIResults).
struct OsGuiTVSIResults {
    OsGuiTVSelectionInfo info;
    OsGuiTreeItem lastSelected;
    OsGuiTreeItem lastProcessed;
};

// ===== Static helpers =====

// 0.5.3 0x00452080 sGetOsGuiPointer, untyped: the "OsGuiPointer" window property holds the COsDialog, COsControl or
// COsWindow that owns the window (the typed sGetOsGuiPointer above is the release build's dialog-only copy).
static void* sGetOsGuiObject(void* window) {
    return GetPropA(static_cast<HWND>(window), "OsGuiPointer");
}

// 0.5.3 0x00453340
static void sSetOsGuiPointer(void* window, void* object) {
    SetPropA(static_cast<HWND>(window), "OsGuiPointer", object);
}

// 0.5.3 0x00453D70
static void sRemoveOsGuiPointer(void* window) {
    RemovePropA(static_cast<HWND>(window), "OsGuiPointer");
}

// 0.5.3 0x00453B00
static void sWinRectToCiRect(const RECT& in, CiRect& out) {
    out.minY = in.top;
    out.minX = in.left;
    out.maxY = in.bottom;
    out.maxX = in.right;
}

// 0.5.3 0x00458940
static void sCiRectToWinRect(const CiRect& in, RECT& out) {
    out.left = in.minX;
    out.top = in.minY;
    out.right = in.maxX;
    out.bottom = in.maxY;
}

// CImVector -> COLORREF, as 0.5.3 builds it inline.
static COLORREF sColorRef(const CImVector& color) {
    return RGB(color.r, color.g, color.b);
}

static uint32_t sColorValue(const CImVector& color) {
    return color.b | (color.g << 8) | (color.r << 16) | (static_cast<uint32_t>(color.a) << 24);
}

static CImVector sColorFromValue(uint32_t value) {
    return CImVector(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF);
}

// 0.5.3 0x00452850: menu item ids 1 and 2 are IDOK and IDCANCEL in a dialog, so they are moved out of the way.
static uint32_t sMenuReal2RawID(uint32_t id) {
    if (id == 1) {
        return 0xFABC;
    }

    if (id == 2) {
        return 0xFABD;
    }

    return id;
}

// 0.5.3 0x00453A70
static uint32_t sMenuRaw2RealID(uint32_t id) {
    if (id == 0xFABC) {
        return 1;
    }

    if (id == 0xFABD) {
        return 2;
    }

    return id;
}

// 0.5.3 0x004539A0
static void sDoCallback(uint32_t index, int32_t type, int32_t subType) {
    if (index > 1) {
        SErrDisplayAppFatal("inCB >= 0 && inCB < NUM_OSGUI_CALLBACKS");
    }

    OsGuiCallback function = s_callbacks[index].function;

    if (function) {
        OsGuiCallbackParams params;
        params.type = type;
        params.subType = subType;
        params.code = 0;
        params.user = s_callbacks[index].userParam;
        function(params);
    }
}

// 0.5.3 0x00453A30
static void CALLBACK sIdleTimerProc(HWND window, UINT message, UINT_PTR id, DWORD time) {
    sDoCallback(1, 0, 0);
}

// 0.5.3 0x00453A10
static void sStartIdle() {
    if (!s_idleTimerID) {
        s_idleTimerID = SetTimer(nullptr, 0, 50, sIdleTimerProc);
    }
}

// 0.5.3 0x00453A50
static void sStopIdle() {
    if (s_idleTimerID) {
        KillTimer(nullptr, s_idleTimerID);
        s_idleTimerID = 0;
    }
}

// 0.5.3 0x00452110
static void sEnableGlobalTips(int32_t enable) {
    if (s_globalTips) {
        SendMessageA(s_globalTips, TTM_ACTIVATE, enable, 0);
    }
}

// 0.5.3 0x00453FE0
static HWND sCreateTooltips(HWND parent) {
    HWND tips = CreateWindowExA(0, TOOLTIPS_CLASSA, nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, nullptr, s_appInstance, nullptr);
    SetWindowPos(tips, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SendMessageA(tips, TTM_SETMAXTIPWIDTH, 0, 300);
    SendMessageA(tips, TTM_ACTIVATE, s_masterTooltipsEnabled, 0);
    return tips;
}

// 0.5.3 0x004554A0
static HWND sGetGlobalTips() {
    if (!s_globalTips) {
        s_globalTips = sCreateTooltips(nullptr);
    }

    return s_globalTips;
}

// 0.5.3 0x00453E70: disables a window of this application and remembers it (an EnumWindowsProc in shape).
static BOOL CALLBACK sDisableWindow(HWND window, LPARAM param) {
    TSGrowableArray<OsGuiHandle>* disabled = reinterpret_cast<TSGrowableArray<OsGuiHandle>*>(param);

    if (reinterpret_cast<HINSTANCE>(GetWindowLongPtrA(window, GWLP_HINSTANCE)) == s_appInstance && IsWindowEnabled(window)) {
        EnableWindow(window, FALSE);
        *disabled->New() = window;
    }

    return TRUE;
}

// 0.5.3 0x004555E0
static int32_t sNCodeToItemCode(int32_t code, int32_t ctrlType) {
    for (const OsGuiNotifyEntry& entry : s_notifyTable) {
        if (entry.ctrlType == ctrlType && entry.winCode == code) {
            return entry.osGuiCode;
        }
    }

    return -1;
}

// 0.5.3 0x00453A90: WM_DRAWITEM -> COsControl::OnDraw. State bits: 1 disabled, 2 focused, 4 selected (pushed).
static LRESULT sHandleDrawItem(LPARAM lparam) {
    DRAWITEMSTRUCT* item = reinterpret_cast<DRAWITEMSTRUCT*>(lparam);
    COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(item->hwndItem));

    if (!control) {
        return 0;
    }

    uint32_t state = (item->itemState & ODS_DISABLED) ? 1 : 0;

    if (item->itemState & ODS_FOCUS) {
        state |= 2;
    }

    if (item->itemState & ODS_SELECTED) {
        state |= 4;
    }

    CiRect rect = {};
    sWinRectToCiRect(item->rcItem, rect);
    return control->OnDraw(item->hDC, state, rect);
}

// 0.5.3 0x00453B20: WM_CTLCOLOR* -> COsControl::OnSetColors (the brush, or null for the default colors).
static LRESULT sHandleCtlColor(WPARAM dc, LPARAM window) {
    COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(window)));

    if (!control) {
        return 0;
    }

    return reinterpret_cast<LRESULT>(control->OnSetColors(reinterpret_cast<void*>(dc)));
}

// 0.5.3 0x00455870
static HBITMAP sBitmapFromImageData(int32_t width, int32_t height, const void* data, HDC dc) {
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    return CreateDIBitmap(dc, &info.bmiHeader, CBM_INIT, data, &info, DIB_RGB_COLORS);
}

// 0.5.3 0x00457470: a 1-bit mask, set where the 32-bit image is fully transparent.
static HBITMAP sMaskFromImageData(int32_t width, int32_t height, const void* data, HDC dc) {
    // 0.5.3 rounds the rows to two bytes per eight pixels (DWORD aligned for the 16-pixel images it is used with).
    int32_t rowBytes = 2 * ((width - 1) / 8 + 1);
    uint8_t* bits = static_cast<uint8_t*>(SMemAlloc(rowBytes * height, __FILE__, __LINE__, 0x8));
    const uint8_t* alpha = static_cast<const uint8_t*>(data) + 3;

    for (int32_t y = 0; y < height; y++) {
        uint8_t* row = bits + y * rowBytes;

        for (int32_t x = 0; x < width; x++) {
            if (*alpha == 0) {
                row[x / 8] |= 1 << (7 - x % 8);
            }

            alpha += 4;
        }
    }

    struct {
        BITMAPINFOHEADER header;
        RGBQUAD colors[2];
    } info = {};

    info.header.biSize = sizeof(BITMAPINFOHEADER);
    info.header.biWidth = width;
    info.header.biHeight = -height;
    info.header.biPlanes = 1;
    info.header.biBitCount = 1;
    info.colors[1] = { 0xFF, 0xFF, 0xFF, 0xFF };

    HBITMAP mask = CreateDIBitmap(dc, &info.header, CBM_INIT, bits, reinterpret_cast<BITMAPINFO*>(&info), DIB_RGB_COLORS);
    SMemFree(bits, __FILE__, __LINE__, 0);
    return mask;
}

// 0.5.3 0x00452590: "Ctrl+Shift+Alt+Key". Modifier bits: 1 shift, 2 control, 4 alt.
static void sGetHotkeyText(int32_t keyID, int32_t modKeyID, char* buf, uint32_t bufSize) {
    char modText[20];
    char keyText[50];

    buf[0] = '\0';
    modText[0] = '\0';

    if (modKeyID & 2) {
        SStrPack(modText, "Ctrl+", sizeof(modText));
    }

    if (modKeyID & 1) {
        SStrPack(modText, "Shift+", sizeof(modText));
    }

    if (modKeyID & 4) {
        SStrPack(modText, "Alt+", sizeof(modText));
    }

    keyText[0] = '\0';

    if ((keyID >= KEY_0 && keyID <= KEY_9) || (keyID >= KEY_A && keyID <= KEY_Z)) {
        keyText[0] = static_cast<char>(keyID);
        keyText[1] = '\0';
    } else if (keyID >= KEY_F1 && keyID <= KEY_F12) {
        SStrPrintf(keyText, sizeof(keyText), "F%1d", keyID - KEY_F1 + 1);
    } else {
        const char* name;

        switch (keyID) {
            case KEY_SPACE: name = "Space"; break;
            case KEY_BRACKET_OPEN: name = "["; break;
            case KEY_BRACKET_CLOSE: name = "]"; break;
            case KEY_ESCAPE: name = "Esc"; break;
            case KEY_ENTER: name = "Enter"; break;
            case KEY_BACKSPACE: name = "Backspace"; break;
            case KEY_TAB: name = "Tab"; break;
            case KEY_LEFT: name = "Left"; break;
            case KEY_UP: name = "Up"; break;
            case KEY_RIGHT: name = "Right"; break;
            case KEY_DOWN: name = "Down"; break;
            case KEY_DELETE: name = "Delete"; break;
            default: name = "Unknown"; break;
        }

        SStrCopy(keyText, name, sizeof(keyText));
    }

    if (keyText[0]) {
        SStrPrintf(buf, bufSize, "%s%s", modText, keyText);
    }
}

// 0.5.3 0x00452FF0: KEY_* -> virtual key for the accelerator table.
static int32_t sKeyToVirtKey(int32_t keyID) {
    if ((keyID >= KEY_0 && keyID <= KEY_9) || (keyID >= KEY_A && keyID <= KEY_Z)) {
        return keyID;
    }

    if (keyID >= KEY_F1 && keyID <= KEY_F12) {
        return VK_F1 + (keyID - KEY_F1);
    }

    if (keyID >= KEY_NUMPAD0 && keyID <= KEY_NUMPAD9) {
        return VK_NUMPAD0 + (keyID - KEY_NUMPAD0);
    }

    switch (keyID) {
        case KEY_LSHIFT:
        case KEY_RSHIFT: return VK_SHIFT;
        case KEY_LCONTROL:
        case KEY_RCONTROL: return VK_CONTROL;
        case KEY_LALT:
        case KEY_RALT: return VK_MENU;
        case KEY_SPACE: return VK_SPACE;
        case KEY_TILDE: return VK_OEM_3;
        case KEY_ESCAPE: return VK_ESCAPE;
        case KEY_ENTER: return VK_RETURN;
        case KEY_BACKSPACE: return VK_BACK;
        case KEY_TAB: return VK_TAB;
        case KEY_LEFT: return VK_LEFT;
        case KEY_UP: return VK_UP;
        case KEY_RIGHT: return VK_RIGHT;
        case KEY_DOWN: return VK_DOWN;
        case KEY_INSERT: return VK_INSERT;
        case KEY_DELETE: return VK_DELETE;
        case KEY_HOME: return VK_HOME;
        case KEY_END: return VK_END;
        case KEY_PAGEUP: return VK_PRIOR;
        case KEY_PAGEDOWN: return VK_NEXT;
        case KEY_CAPSLOCK: return VK_CAPITAL;
        case KEY_NUMLOCK: return VK_NUMLOCK;
        case KEY_SCROLLLOCK: return VK_SCROLL;
        case KEY_PAUSE: return VK_PAUSE;
        case KEY_PRINTSCREEN: return VK_SNAPSHOT;
        default: break;
    }

    OSGUI_ASSERT(!"sKeyToVirtKey: unmapped key");
    return -1;
}

// 0.5.3 0x00452FC0
static void sHotkeyToAccel(const OsGuiMenuHotkey& hotkey, ACCEL& accel) {
    accel.fVirt = FVIRTKEY;

    if (hotkey.modKeyID & 2) {
        accel.fVirt = FVIRTKEY | FCONTROL;
    }

    if (hotkey.modKeyID & 1) {
        accel.fVirt |= FSHIFT;
    }

    if (hotkey.modKeyID & 4) {
        accel.fVirt |= FALT;
    }

    accel.key = static_cast<WORD>(sKeyToVirtKey(hotkey.keyID));
}

// 0.5.3 0x0045A120: 0 arrow, 1 wait, 2 west-east resize, 3 north-south resize.
static HCURSOR sWinCursor(int32_t cursor) {
    switch (cursor) {
        case 0: return LoadCursorA(nullptr, IDC_ARROW);
        case 1: return LoadCursorA(nullptr, IDC_WAIT);
        case 2: return LoadCursorA(nullptr, IDC_SIZEWE);
        case 3: return LoadCursorA(nullptr, IDC_SIZENS);
        default: break;
    }

    SErrDisplayAppFatal("inCursor >= 0 && inCursor < NUM_OSGUI_CURSORS");
    return nullptr;
}

// 0.5.3 0x00459B50: SB_* -> sProcessScrollMessage action.
static int32_t sConvertScrollMsg(int32_t code) {
    switch (code) {
        case SB_LINEUP: return 3;
        case SB_LINEDOWN: return 2;
        case SB_PAGEUP: return 5;
        case SB_PAGEDOWN: return 4;
        case SB_THUMBPOSITION: return 0;
        case SB_THUMBTRACK: return 1;
        case SB_TOP: return 7;
        case SB_BOTTOM: return 6;
        default: return -1;
    }
}

// 0.5.3 0x00459BB0: moves the scroll position; returns how far it moved (old - new).
static int32_t sProcessScrollMessage(HWND window, int32_t bar, int32_t action, int32_t increment) {
    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo(window, bar, &info);

    int32_t oldPos = info.nPos;
    int32_t pos = info.nPos;

    switch (action) {
        case 0:
        case 1: pos = info.nTrackPos; break;
        case 2: pos = info.nPos + increment; break;
        case 3: pos = info.nPos - increment; break;
        case 4: pos = info.nPos + info.nPage; break;
        case 5: pos = info.nPos - info.nPage; break;
        case 6: pos = info.nMax; break;
        case 7: pos = info.nMin; break;
        default: break;
    }

    if (pos > static_cast<int32_t>(info.nMax - info.nPage + 1)) {
        pos = info.nMax - info.nPage + 1;
    }

    if (pos < info.nMin) {
        pos = info.nMin;
    }

    if (pos != info.nPos) {
        SetScrollPos(window, bar, pos, TRUE);
    }

    return oldPos - pos;
}

// Calls the window class procedure of a subclassed control (0.5.3: CallWindowProc(GetClassLong(GCL_WNDPROC))).
static LRESULT sCallClassProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    WNDPROC proc = reinterpret_cast<WNDPROC>(GetClassLongPtrA(window, GCLP_WNDPROC));
    return CallWindowProcA(proc, window, message, wparam, lparam);
}

// 0.5.3 0x00455DB0: keeps COsEditBox::m_selSize current on every key and mouse action.
static LRESULT CALLBACK sEditBoxProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    COsEditBox* editBox = static_cast<COsEditBox*>(sGetOsGuiObject(window));

    if (editBox) {
        if ((message >= WM_KEYDOWN && message <= WM_KEYUP) || (message >= WM_MOUSEMOVE && message <= WM_LBUTTONUP)
            || (message >= WM_RBUTTONDOWN && message <= WM_RBUTTONUP)) {
            editBox->UpdateSelection();
        }
    }

    return sCallClassProc(window, message, wparam, lparam);
}

// 0.5.3 0x00456DF0: left clicks go to OnMouseDown/OnMouseUp first (multi-selection and drag and drop).
static LRESULT CALLBACK sTreeViewProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    COsTreeView* treeView = static_cast<COsTreeView*>(sGetOsGuiObject(window));

    if (treeView) {
        int32_t handled = 0;

        if (message == WM_LBUTTONDOWN) {
            handled = treeView->OnMouseDown();
        } else if (message == WM_LBUTTONUP) {
            handled = treeView->OnMouseUp();
        }

        if (handled) {
            return 0;
        }
    }

    return sCallClassProc(window, message, wparam, lparam);
}

// 0.5.3 0x00458BF0
static LRESULT CALLBACK sSpinButtonProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    COsSpinButton* spinButton = static_cast<COsSpinButton*>(sGetOsGuiObject(window));

    if (spinButton && message == WM_LBUTTONUP) {
        spinButton->OnSpinMouseUp();
    }

    return sCallClassProc(window, message, wparam, lparam);
}

// 0.5.3 0x00459DC0
static LRESULT CALLBACK sDividerProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    COsDivider* divider = static_cast<COsDivider*>(sGetOsGuiObject(window));

    if (divider) {
        switch (message) {
            case WM_MOUSEMOVE:
                divider->OnDivMouseMove(static_cast<int16_t>(LOWORD(lparam)), static_cast<int16_t>(HIWORD(lparam)));
                break;

            case WM_LBUTTONDOWN:
                divider->OnDivMouseDown();
                break;

            case WM_LBUTTONUP:
                divider->OnDivMouseUp();
                break;

            case WM_MOUSELEAVE:
                divider->OnDivMouseLeave();
                break;

            default:
                break;
        }
    }

    return sCallClassProc(window, message, wparam, lparam);
}

// 0.5.3 0x004577F0
static void sTVGetSelectInfo(COsTreeView* treeView, OsGuiTreeItem item, void* param) {
    OsGuiTVSIResults* results = static_cast<OsGuiTVSIResults*>(param);

    if (!treeView->IsItemSelected(item)) {
        results->lastProcessed = item;
        return;
    }

    if (++results->info.numSelected == 1) {
        results->info.firstSelection = item;
        results->info.flags |= OSGUI_TVSI_SAME_PARENT | OSGUI_TVSI_CONTIGUOUS;
    } else {
        if ((results->info.flags & OSGUI_TVSI_SAME_PARENT)
            && treeView->GetItemParent(item) != treeView->GetItemParent(results->info.firstSelection)) {
            results->info.flags &= ~OSGUI_TVSI_SAME_PARENT;
        }

        if ((results->info.flags & OSGUI_TVSI_CONTIGUOUS) && results->lastSelected != results->lastProcessed) {
            results->info.flags &= ~OSGUI_TVSI_CONTIGUOUS;
        }
    }

    results->lastSelected = item;
    results->lastProcessed = item;
}

// 0.5.3 0x004578A0
static void sTVSelect(COsTreeView* treeView, OsGuiTreeItem item, void* param) {
    treeView->SelectItem(item, *static_cast<int32_t*>(param));
}

// 0.5.3 0x00453350: the dialog procedure of every COsDialog.
static INT_PTR CALLBACK sDlgProc(HWND dialogWindow, UINT message, WPARAM wparam, LPARAM lparam) {
    COsDialog* dialog = static_cast<COsDialog*>(sGetOsGuiObject(dialogWindow));

    switch (message) {
        case WM_SIZE:
            return dialog ? dialog->OnEvent(-1, 7, 0) : 0;

        case WM_ACTIVATE:
            return dialog && LOWORD(wparam) ? dialog->OnEvent(-1, 18, 0) : 0;

        case WM_CLOSE:
            return dialog ? dialog->OnEvent(-1, 6, 0) : 0;

        case WM_GETMINMAXINFO: {
            if (dialog) {
                int32_t minWidth;
                int32_t minHeight;
                dialog->GetMinSize(&minWidth, &minHeight);

                MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lparam);

                if (minWidth != -1) {
                    info->ptMinTrackSize.x = minWidth;
                }

                if (minHeight != -1) {
                    info->ptMinTrackSize.y = minHeight;
                }
            }

            return 0;
        }

        case WM_DRAWITEM:
            return sHandleDrawItem(lparam);

        case WM_NOTIFY: {
            NMHDR* header = reinterpret_cast<NMHDR*>(lparam);
            COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(header->hwndFrom));
            return control ? control->OnNotify(header->code, header) : 0;
        }

        case WM_CONTEXTMENU: {
            int32_t x = LOWORD(lparam);
            int32_t y = HIWORD(lparam);

            if (reinterpret_cast<HWND>(wparam) == dialogWindow && dialog) {
                return dialog->OnContextMenu(x, y);
            }

            COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(wparam)));
            return control ? control->OnContextMenu(x, y) : 0;
        }

        case WM_INITDIALOG:
            return 1;

        case WM_COMMAND: {
            if (lparam) {
                COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(lparam)));
                return control ? control->OnCommand(static_cast<int32_t>(wparam)) : 0;
            }

            if (wparam == IDOK || wparam == IDCANCEL) {
                if (!dialog) {
                    return 0;
                }

                int32_t accept = wparam == IDOK;
                COsControl* focus = dialog->FindControl(GetFocus());

                if (focus && (accept ? focus->OnReturn() : focus->OnEscape())) {
                    return 1;
                }

                return accept ? dialog->OnAccept() : dialog->OnCancel();
            }

            if (dialog && GetMenu(static_cast<HWND>(dialog->m_handle))) {
                return dialog->OnEvent(-2, sMenuRaw2RealID(LOWORD(wparam)), 0);
            }

            return 0;
        }

        case WM_HSCROLL:
        case WM_VSCROLL: {
            if (!lparam) {
                return 0;
            }

            COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(lparam)));
            return control ? control->OnScroll(static_cast<int32_t>(wparam)) : 0;
        }

        case WM_ENTERIDLE:
            sDoCallback(1, 0, 0);
            return 0;

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            return sHandleCtlColor(wparam, lparam);

        case WM_MOUSEMOVE:
            if (dialog) {
                dialog->OnMouseMove(LOWORD(lparam), HIWORD(lparam));
            }

            return 0;

        case WM_LBUTTONDOWN:
            return dialog ? dialog->OnMouseDown() : 0;

        case WM_LBUTTONUP:
            return dialog ? dialog->OnMouseUp() : 0;

        case WM_MOUSEWHEEL: {
            COsControl* control = dialog ? dialog->FindControl(GetFocus()) : nullptr;

            if (control) {
                control->OnMouseWheel(static_cast<int16_t>(HIWORD(wparam)) / WHEEL_DELTA);
            }

            return 0;
        }

        case WM_ENTERMENULOOP:
        case WM_ENTERSIZEMOVE:
            sStartIdle();
            return 0;

        case WM_EXITMENULOOP:
        case WM_EXITSIZEMOVE:
            sStopIdle();
            return 0;

        case WM_MOUSELEAVE:
            if (dialog) {
                dialog->OnMouseLeave();
            }

            return 0;

        default:
            return 0;
    }
}

// ===== Free functions =====

// 0.5.3 0x00451DF0: posts the menu command (menu id, item) to the active window, as if picked from the menu.
void OsGuiMenuSelect(int32_t menuID, int32_t itemID) {
    WPARAM command = ((menuID & 0xFF) << 8) | (itemID & 0xFF);
    PostMessageA(GetActiveWindow(), WM_COMMAND, command, 0);
}

// 0.5.3 0x00451E10
void OsGuiInitialize() {
    // 0.5.3 first called OsSetWindowProc(OsGuiWindowProc); see the file comment.
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES;

    BOOL success = InitCommonControlsEx(&initCtrls);
    OSGUI_ASSERT(success);
}

// 0.5.3 0x00451E60
void OsGuiDestroy() {
    if (s_globalTips) {
        DestroyWindow(s_globalTips);
        s_globalTips = nullptr;
    }
}

// 0.5.3 0x00451E90
void OsGuiSetApplicationInfo(void* instance) {
    s_appInstance = static_cast<HINSTANCE>(instance);
}

// 0.5.3 0x00451EA0: called with (menu id, item position) when a menu item or its hotkey is picked.
void OsGuiSetMenuCommandCallback(OsGuiCallback callback, void* param) {
    s_callbacks[0].function = callback;
    s_callbacks[0].userParam = param;
}

// 0.5.3 0x00451EB0: called every 50 ms while a menu, a move/size loop or a divider drag blocks the main loop.
void OsGuiSetIdleCallback(OsGuiCallback callback, void* param) {
    s_callbacks[1].function = callback;
    s_callbacks[1].userParam = param;
}

// 0.5.3 0x00452090
void OsGuiEnableTooltips(int32_t enable) {
    if (enable == s_masterTooltipsEnabled) {
        return;
    }

    s_masterTooltipsEnabled = enable;
    sEnableGlobalTips(enable);

    for (uint32_t i = 0; i < s_dialogs.Count(); i++) {
        s_dialogs[i]->EnableTooltips(s_masterTooltipsEnabled);
    }
}

// 0.5.3 0x00452130
void OsGuiEnableMenuHotkeys(int32_t enable) {
    s_menuHotkeysEnabled = enable;
}

// 0.5.3 0x0045A1B0
void OsGuiSetCursor(int32_t cursor) {
    HCURSOR winCursor = sWinCursor(cursor);

    if (winCursor) {
        SetCursor(winCursor);
    }
}

// 0.5.3 0x0045A1D0
void OsGuiShowCursor(int32_t show) {
    ShowCursor(show);
}

// 0.5.3 0x0045A1E0: screen coordinates.
void OsGuiGetCursorPosition(int32_t* x, int32_t* y) {
    POINT point;

    if (GetCursorPos(&point)) {
        *x = point.x;
        *y = point.y;
    }
}

// 0.5.3 0x0045A220: loads the icon resource for both the large and small class icons.
void OsGuiSetWindowIcon(void* window, const char* name) {
    HWND hwnd = static_cast<HWND>(window);

    HANDLE large = LoadImageA(GetModuleHandleA(nullptr), name, IMAGE_ICON, 32, 32, 0);
    HICON oldLarge = reinterpret_cast<HICON>(SetClassLongPtrA(hwnd, GCLP_HICON, reinterpret_cast<LONG_PTR>(large)));

    if (oldLarge) {
        DestroyIcon(oldLarge);
    }

    HANDLE small = LoadImageA(GetModuleHandleA(nullptr), name, IMAGE_ICON, 16, 16, 0);
    HICON oldSmall = reinterpret_cast<HICON>(SetClassLongPtrA(hwnd, GCLP_HICONSM, reinterpret_cast<LONG_PTR>(small)));

    if (oldSmall) {
        DestroyIcon(oldSmall);
    }
}

// 0.5.3 0x0045A290: screen rectangle of the window, or of its client area.
CiRect OsGuiGetWindowRect(void* window, int32_t clientOnly) {
    WINDOWINFO info;
    info.cbSize = sizeof(info);
    GetWindowInfo(static_cast<HWND>(window), &info);

    CiRect rect = {};
    sWinRectToCiRect(clientOnly ? info.rcClient : info.rcWindow, rect);
    return rect;
}

// 0.5.3 0x0045A310: rect in the parent's client coordinates.
void OsGuiSetWindowRect(void* window, const CiRect& rect) {
    SetWindowPos(static_cast<HWND>(window), nullptr, rect.minX, rect.minY, rect.maxX - rect.minX, rect.maxY - rect.minY,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

// 0.5.3 0x0045A340
void OsGuiBringWindowToFront(void* window) {
    BringWindowToTop(static_cast<HWND>(window));
}

// 0.5.3 0x0045A350
void OsGuiShowWindow(void* window, int32_t show) {
    SetWindowPos(static_cast<HWND>(window), nullptr, 0, 0, 0, 0,
                 (show ? SWP_SHOWWINDOW : SWP_HIDEWINDOW) | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
}

// 0.5.3 0x0045A380
void OsGuiEnableWindow(void* window, int32_t enable) {
    EnableWindow(static_cast<HWND>(window), enable);
}

// 0.5.3 0x0045A390
int32_t OsGuiWindowEnabled(void* window) {
    return IsWindowEnabled(static_cast<HWND>(window));
}

// 0.5.3 0x0045A400
void OsGuiMaximizeWindow(void* window, int32_t maximize) {
    HWND hwnd = static_cast<HWND>(window);

    if (IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, maximize ? SW_MAXIMIZE : SW_SHOWNORMAL);
    } else {
        ShowWindow(hwnd, maximize ? SW_MAXIMIZE : SW_HIDE);
    }
}

// 0.5.3 0x0045A440
int32_t OsGuiWindowMaximized(void* window) {
    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(static_cast<HWND>(window), &placement);
    return placement.showCmd == SW_SHOWMAXIMIZED;
}

// 0.5.3 0x0045A470
void OsGuiMinimizeWindow(void* window, int32_t minimize) {
    HWND hwnd = static_cast<HWND>(window);

    if (IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, minimize ? SW_SHOWMINIMIZED : SW_SHOWNORMAL);
    } else {
        ShowWindow(hwnd, minimize ? SW_MINIMIZE : SW_HIDE);
    }
}

// 0.5.3 0x0045A4B0
int32_t OsGuiWindowMinimized(void* window) {
    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(static_cast<HWND>(window), &placement);
    return placement.showCmd == SW_SHOWMINIMIZED;
}

// 0.5.3 0x0045A4E0: the rectangle the window has when neither minimized nor maximized.
CiRect OsGuiGetWindowRestoredRect(void* window) {
    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(static_cast<HWND>(window), &placement);

    CiRect rect = {};
    sWinRectToCiRect(placement.rcNormalPosition, rect);
    return rect;
}

// 0.5.3 0x0045A540
void OsGuiSetWindowRestoredRect(void* window, const CiRect& rect) {
    HWND hwnd = static_cast<HWND>(window);

    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(hwnd, &placement);

    sCiRectToWinRect(rect, placement.rcNormalPosition);
    placement.showCmd = IsWindowVisible(hwnd) ? SW_SHOWNA : SW_HIDE;
    SetWindowPlacement(hwnd, &placement);
}

// 0.5.3 0x0045A5B0
int32_t OsGuiWindowIsCursorInside(void* window, int32_t clientOnly) {
    int32_t x = 0;
    int32_t y = 0;
    OsGuiGetCursorPosition(&x, &y);

    POINT point = { x, y };

    if (WindowFromPoint(point) != window) {
        return 0;
    }

    if (!clientOnly) {
        return 1;
    }

    CiRect rect = OsGuiGetWindowRect(window, 1);
    return x >= rect.minX && x <= rect.maxX && y >= rect.minY && y <= rect.maxY;
}

// 0.5.3 0x0045A640: the work area of the primary monitor.
CiRect OsGuiGetScreenBounds() {
    RECT workArea;
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);

    CiRect rect = {};
    sWinRectToCiRect(workArea, rect);
    return rect;
}

// 0.5.3 0x0045A7C0
void OsGuiGetHotkeyText(const OsGuiMenuHotkey& hotkey, char* buf, int32_t bufSize) {
    sGetHotkeyText(hotkey.keyID, hotkey.modKeyID, buf, bufSize);
}

// 0.5.3 0x0045A7E0: 0 dialog background, 1 window (edit) background.
CImVector OsGuiGetColor(int32_t color) {
    int32_t sysColor;

    if (color == 0) {
        sysColor = COLOR_BTNFACE;
    } else if (color == 1) {
        sysColor = COLOR_WINDOW;
    } else {
        return CImVector(0, 0, 0, 0);
    }

    COLORREF value = GetSysColor(sysColor);
    return CImVector(GetBValue(value), GetGValue(value), GetRValue(value), 0xFF);
}

// 0.5.3 0x0045A960: the window procedure of the tools' top-level windows (see the file comment for the hookup).
LRESULT OsGuiWindowProc(OsGuiHandle window, uint32_t message, WPARAM wparam, LPARAM lparam) {
    HWND hwnd = static_cast<HWND>(window);

    switch (message) {
        case WM_DRAWITEM:
            return sHandleDrawItem(lparam);

        case WM_NOTIFY: {
            NMHDR* header = reinterpret_cast<NMHDR*>(lparam);
            COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(header->hwndFrom));
            return control ? control->OnNotify(header->code, header) : 0;
        }

        case WM_COMMAND:
            if (lparam) {
                COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(lparam)));
                return control ? control->OnCommand(static_cast<int32_t>(wparam)) : 0;
            }

            // Menu (0) or accelerator (1): the id is (menu id << 8) | item position.
            if (HIWORD(wparam) <= 1) {
                uint32_t id = sMenuRaw2RealID(LOWORD(wparam));
                sDoCallback(0, (id >> 8) & 0xFF, id & 0xFF);
            }

            break;

        case WM_HSCROLL:
        case WM_VSCROLL:
            if (lparam) {
                COsControl* control = static_cast<COsControl*>(sGetOsGuiObject(reinterpret_cast<HWND>(lparam)));
                return control ? control->OnScroll(static_cast<int32_t>(wparam)) : 0;
            }

            break;

        case WM_ENTERIDLE:
            sDoCallback(1, 0, 0);
            break;

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            return sHandleCtlColor(wparam, lparam);

        case WM_ENTERMENULOOP:
        case WM_ENTERSIZEMOVE:
            sStartIdle();
            return 0;

        case WM_EXITMENULOOP:
        case WM_EXITSIZEMOVE:
            sStopIdle();
            return 0;

        default:
            break;
    }

    COsWindow* osWindow = static_cast<COsWindow*>(sGetOsGuiObject(hwnd));

    if (osWindow) {
        if (message == WM_SIZE) {
            osWindow->OnResize();
        } else if (message == WM_GETMINMAXINFO) {
            int32_t minWidth;
            int32_t minHeight;
            osWindow->GetMinSize(&minWidth, &minHeight);

            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lparam);

            if (minWidth != -1) {
                info->ptMinTrackSize.x = minWidth;
            }

            if (minHeight != -1) {
                info->ptMinTrackSize.y = minHeight;
            }

            return 0;
        }
    }

    return DefWindowProcA(hwnd, message, wparam, lparam);
}

// ===== COsMenu =====

// 0.5.3 0x00452140: a menu bar menu; its items get the command ids (inID << 8) | position.
COsMenu::COsMenu(uint8_t id, const char* title) {
    SStrCopy(this->m_title, title, sizeof(this->m_title));
    this->m_menuHandle = CreateMenu();
    this->m_id = id;
}

// 0.5.3 0x00452190: a popup (context) menu, id 0xFF.
COsMenu::COsMenu() {
    this->m_title[0] = '\0';
    this->m_id = 0xFF;
    this->m_menuHandle = CreatePopupMenu();
}

// 0.5.3 0x004521C0
COsMenu::~COsMenu() {
    DestroyMenu(static_cast<HMENU>(this->m_menuHandle));
}

// 0.5.3 0x004521F0
void COsMenu::Clear() {
    for (int32_t i = this->GetNumItems(); i > 0; i--) {
        this->RemoveItem(0);
    }
}

// 0.5.3 0x00452210
void COsMenu::RemoveItem(int32_t pos) {
    DeleteMenu(static_cast<HMENU>(this->m_menuHandle), pos, MF_BYPOSITION);
    this->RemoveHotkey(pos);
}

// 0.5.3 0x00452240
int32_t COsMenu::GetNumItems() {
    return GetMenuItemCount(static_cast<HMENU>(this->m_menuHandle));
}

// 0.5.3 0x00452250
int32_t COsMenu::GetHotkey(int32_t pos, OsGuiMenuHotkey* hotkey) {
    OSGUI_ASSERT(pos >= 0 && pos < static_cast<int32_t>(this->m_hotkeys.Count()));

    if (this->m_hotkeys[pos].keyID == -1) {
        return 0;
    }

    *hotkey = this->m_hotkeys[pos];
    return 1;
}

// 0.5.3 0x00452310: inserts an empty hotkey slot, keeping m_hotkeys parallel to the menu items.
void COsMenu::AddHotkey(int32_t pos) {
    uint32_t count = this->m_hotkeys.Count();
    this->m_hotkeys.SetCount(count + 1);

    for (int32_t i = count; i > pos; i--) {
        this->m_hotkeys[i] = this->m_hotkeys[i - 1];
    }

    this->m_hotkeys[pos].keyID = -1;
}

// 0.5.3 0x00452430
void COsMenu::RemoveHotkey(int32_t pos) {
    int32_t count = this->m_hotkeys.Count();

    for (int32_t i = pos; i < count - 1; i++) {
        this->m_hotkeys[i] = this->m_hotkeys[i + 1];
    }

    this->m_hotkeys.SetCount(count - 1);
}

// 0.5.3 0x00452550: appends "\t<hotkey>" to a menu item text (the tab right-aligns it in the menu).
void COsMenu::AppendHotkeyText(char* text, const OsGuiMenuHotkey& hotkey) {
    char keyText[50];
    sGetHotkeyText(hotkey.keyID, hotkey.modKeyID, keyText, sizeof(keyText));
    SStrPack(text, "\t", STORM_MAX_STR);
    SStrPack(text, keyText, STORM_MAX_STR);
}

// 0.5.3 0x00452780
void COsMenu::AddTextItem(int32_t pos, const char* text, OsGuiMenuHotkey* hotkey) {
    char itemText[260];
    SStrCopy(itemText, text, sizeof(itemText));

    if (hotkey) {
        COsMenu::AppendHotkeyText(itemText, *hotkey);
    }

    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_ID | MIIM_TYPE;
    info.fType = MFT_STRING;
    info.dwTypeData = itemText;
    info.wID = sMenuReal2RawID((this->m_id << 8) | pos);
    InsertMenuItemA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    this->AddHotkey(pos);

    if (hotkey) {
        this->m_hotkeys[pos] = *hotkey;
    }
}

// 0.5.3 0x00452870
void COsMenu::AddSubMenu(int32_t pos, const char* title, COsMenu* menu) {
    this->AddTextItem(pos, title, nullptr);

    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;
    info.hSubMenu = static_cast<HMENU>(menu->m_menuHandle);
    SetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    // 0.5.3 called AddHotkey(pos) again here, leaving one hotkey slot too many after every sub menu (the slots of the
    // items after it no longer lined up). AddTextItem already inserted the slot.
}

// 0.5.3 0x004528D0
void COsMenu::AddSeparator(int32_t pos) {
    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_ID | MIIM_TYPE;
    info.fType = MFT_SEPARATOR;
    info.wID = 0xFFFF;
    InsertMenuItemA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    this->AddHotkey(pos);
}

// 0.5.3 0x00452920
void COsMenu::EnableItem(int32_t pos, int32_t enable) {
    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_STATE;
    GetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    if (enable) {
        info.fState &= ~MFS_DISABLED;
    } else {
        info.fState |= MFS_DISABLED;
    }

    SetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);
}

// 0.5.3 0x00452980
void COsMenu::CheckItem(int32_t pos, int32_t check) {
    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_STATE;
    GetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    if (check) {
        info.fState |= MFS_CHECKED;
    } else {
        info.fState &= ~MFS_CHECKED;
    }

    SetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);
}

// 0.5.3 0x004529E0: only touches the menu when the text changes (avoids menu flicker when refreshed every frame).
void COsMenu::SetItemText(int32_t pos, const char* text) {
    char itemText[260];
    SStrCopy(itemText, text, sizeof(itemText));

    OsGuiMenuHotkey hotkey;

    if (this->GetHotkey(pos, &hotkey)) {
        COsMenu::AppendHotkeyText(itemText, hotkey);
    }

    char oldText[260];

    MENUITEMINFOA info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_STRING;
    info.dwTypeData = oldText;
    info.cch = sizeof(oldText);
    GetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);

    if (SStrCmp(oldText, itemText, STORM_MAX_STR)) {
        info.dwTypeData = itemText;
        SetMenuItemInfoA(static_cast<HMENU>(this->m_menuHandle), pos, TRUE, &info);
    }
}

// ===== COsMenuBar =====

// 0.5.3 0x00452A90
COsMenuBar::COsMenuBar(void* window) {
    this->m_windowHandle = window;
    this->m_menuBarHandle = CreateMenu();
    SetMenu(static_cast<HWND>(this->m_windowHandle), static_cast<HMENU>(this->m_menuBarHandle));
    this->m_accelerators = nullptr;

    *s_menubars.New() = this;
}

// 0.5.3 0x00452B60
COsMenuBar::~COsMenuBar() {
    DestroyMenu(static_cast<HMENU>(this->m_menuBarHandle));

    if (this->m_accelerators) {
        DestroyAcceleratorTable(static_cast<HACCEL>(this->m_accelerators));
    }

    int32_t id = -1;

    for (uint32_t i = 0; i < s_menubars.Count(); i++) {
        if (s_menubars[i] == this) {
            id = i;
            break;
        }
    }

    OSGUI_ASSERT(id != -1);

    if (id != -1) {
        uint32_t last = s_menubars.Count() - 1;
        s_menubars[id] = s_menubars[last];
        s_menubars.SetCount(last);
    }
}

// 0.5.3 0x00452D10: the menu bar shows each menu under its title; the menus stay owned by the caller.
void COsMenuBar::Set(TSGrowableArray<COsMenu*>& menus) {
    uint32_t count = menus.Count();
    this->m_menus.Set(count, menus.Ptr());

    for (uint32_t i = 0; i < count; i++) {
        COsMenu* menu = this->m_menus[i];

        MENUITEMINFOA info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_TYPE;
        info.fType = MFT_STRING;
        info.dwTypeData = menu->m_title;
        InsertMenuItemA(static_cast<HMENU>(this->m_menuBarHandle), i, TRUE, &info);

        info.fMask = MIIM_SUBMENU;
        info.hSubMenu = static_cast<HMENU>(menu->m_menuHandle);
        SetMenuItemInfoA(static_cast<HMENU>(this->m_menuBarHandle), i, TRUE, &info);
    }

    this->UpdateAccelerators();
    this->Refresh();
}

// 0.5.3 0x00452E30: rebuilds the accelerator table from the menu hotkeys (OsGuiProcessMessage translates it).
void COsMenuBar::UpdateAccelerators() {
    if (this->m_accelerators) {
        DestroyAcceleratorTable(static_cast<HACCEL>(this->m_accelerators));
        this->m_accelerators = nullptr;
    }

    TSGrowableArray<ACCEL> accels;

    for (uint32_t i = 0; i < this->m_menus.Count(); i++) {
        COsMenu* menu = this->m_menus[i];

        for (int32_t pos = 0; pos < menu->GetNumItems(); pos++) {
            OsGuiMenuHotkey hotkey;

            if (menu->GetHotkey(pos, &hotkey)) {
                ACCEL* accel = accels.New();
                sHotkeyToAccel(hotkey, *accel);
                accel->cmd = static_cast<WORD>((menu->m_id << 8) | pos);
            }
        }
    }

    if (accels.Count()) {
        this->m_accelerators = CreateAcceleratorTableA(accels.Ptr(), accels.Count());
        OSGUI_ASSERT(this->m_accelerators != nullptr);
    }
}

// 0.5.3 0x00453180
void COsMenuBar::Refresh() {
    DrawMenuBar(static_cast<HWND>(this->m_windowHandle));
}

// ===== COsDialog =====

// 0.5.3 0x00453190. Flags: 1 resizable, 2 overlapped window frame, 4 modal (disables the parent), 8 maximize box,
// 0x10 uses the parent's menu hotkeys (OsGuiProcessMessage).
COsDialog::COsDialog(void* parentWindow, uint32_t flags) {
    this->m_callbackInfo.function = nullptr;
    this->m_callbackInfo.userParam = nullptr;
    this->m_tooltips = nullptr;
    this->m_cancelButton = nullptr;
    this->m_trackMouse = 0;
    this->m_mouseInside = 0;
    this->m_needNewTrack = 0;
    this->m_tooltipsEnabled = s_masterTooltipsEnabled;
    this->m_minSize.x = -1;
    this->m_minSize.y = -1;
    this->m_contextMenu = nullptr;
    this->m_contextMenuEnabled = 1;
    this->m_flags = flags;

    // An empty dialog template: no items, menu, class or title; controls are added as child windows afterwards.
    struct {
        DLGTEMPLATE header;
        WORD menu;
        WORD windowClass;
        WORD title;
    } dlgTemplate = {};

    dlgTemplate.header.style = WS_POPUP | WS_CAPTION | DS_3DLOOK;
    dlgTemplate.header.cx = 10;
    dlgTemplate.header.cy = 10;

    if (flags & 2) {
        dlgTemplate.header.style = WS_OVERLAPPEDWINDOW;
    }

    if (flags & 1) {
        dlgTemplate.header.style |= WS_THICKFRAME;
    }

    if (flags & 8) {
        dlgTemplate.header.style |= WS_SYSMENU;
    }

    OSGUI_ASSERT(s_appInstance != 0);

    this->m_handle = CreateDialogIndirectParamA(s_appInstance, &dlgTemplate.header, static_cast<HWND>(parentWindow), sDlgProc, 0);
    sSetOsGuiPointer(this->m_handle, this);

    *s_dialogs.New() = this;

    this->ApplyModality(1);
}

// 0.5.3 0x00453B40
COsDialog::~COsDialog() {
    this->ApplyModality(0);

    // 0.5.3 deleted m_controls[i++] while DeleteControl swapped the last control into slot i, so every other control
    // was skipped and leaked; deleting from the end removes them all.
    while (this->m_controls.Count()) {
        this->DeleteControl(this->m_controls[this->m_controls.Count() - 1]);
    }

    if (this->m_contextMenu) {
        delete this->m_contextMenu;
    }

    if (this->m_tooltips) {
        DestroyWindow(static_cast<HWND>(this->m_tooltips));
    }

    sRemoveOsGuiPointer(this->m_handle);
    DestroyWindow(static_cast<HWND>(this->m_handle));

    int32_t id = -1;

    for (uint32_t i = 0; i < s_dialogs.Count(); i++) {
        if (s_dialogs[i] == this) {
            id = i;
            break;
        }
    }

    OSGUI_ASSERT(id != -1);

    if (id != -1) {
        uint32_t last = s_dialogs.Count() - 1;
        s_dialogs[id] = s_dialogs[last];
        s_dialogs.SetCount(last);
    }
}

// 0.5.3 0x00453D80: a modal dialog (flag 4) disables its parent window while it exists.
void COsDialog::ApplyModality(int32_t enable) {
    if (enable) {
        if (this->m_disabledWindows.Count()) {
            SErrDisplayAppFatal("mDisabledWindows.Count() == 0");
            return;
        }

        if (this->m_flags & 4) {
            OsGuiHandle parent = this->GetParentWindow();

            if (parent) {
                sDisableWindow(static_cast<HWND>(parent), reinterpret_cast<LPARAM>(&this->m_disabledWindows));
            }
        }
    } else {
        for (uint32_t i = 0; i < this->m_disabledWindows.Count(); i++) {
            EnableWindow(static_cast<HWND>(this->m_disabledWindows[i]), TRUE);
        }

        this->m_disabledWindows.Clear();
    }
}

// 0.5.3 0x00453FC0
void* COsDialog::GetTooltips() {
    if (!this->m_tooltips) {
        this->m_tooltips = sCreateTooltips(static_cast<HWND>(this->m_handle));
    }

    return this->m_tooltips;
}

// 0.5.3 0x00454060
void COsDialog::EnableTooltips(int32_t enable) {
    this->m_tooltipsEnabled = enable;

    if (this->m_tooltips) {
        SendMessageA(static_cast<HWND>(this->m_tooltips), TTM_ACTIVATE, enable, 0);
    }
}

// 0.5.3 0x00454090
void COsDialog::AddControl(COsControl* control) {
    *this->m_controls.New() = control;

    if (GetParent(static_cast<HWND>(control->m_handle)) != this->m_handle) {
        SetParent(static_cast<HWND>(control->m_handle), static_cast<HWND>(this->m_handle));
    }
}

// 0.5.3 0x00454130
int32_t COsDialog::FindControl(COsControl* control) {
    for (uint32_t i = 0; i < this->m_controls.Count(); i++) {
        if (this->m_controls[i] == control) {
            return i;
        }
    }

    return -1;
}

// 0.5.3 0x004541A0
void COsDialog::DeleteControl(COsControl* control) {
    this->DetachControl(control);
    control->OnDestroy();
    sRemoveOsGuiPointer(control->m_handle);
    DestroyWindow(static_cast<HWND>(control->m_handle));
    delete control;
}

// 0.5.3 0x004541E0
void COsDialog::DetachControl(COsControl* control) {
    int32_t index = this->FindControl(control);

    if (index != -1) {
        uint32_t last = this->m_controls.Count() - 1;
        this->m_controls[index] = this->m_controls[last];
        this->m_controls.SetCount(last);
    }
}

// 0.5.3 0x004544D0: pumps one pending message through this dialog.
void COsDialog::CheckEvents() {
    MSG message;

    if (PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE)) {
        GetMessageA(&message, nullptr, 0, 0);
        this->ProcessMessage(&message);
    }
}

// 0.5.3 0x00454510: with mouse tracking on, the callback gets enter (3) and leave (4) events.
void COsDialog::SetTrackMouse(int32_t track) {
    this->m_trackMouse = track;

    if (track) {
        this->m_mouseInside = this->IsMouseInside();
        this->m_needNewTrack = 1;
    }
}

// 0.5.3 0x00454540
int32_t COsDialog::IsMouseInside() {
    POINT cursor;
    RECT rect;

    return GetCursorPos(&cursor) && GetWindowRect(static_cast<HWND>(this->m_handle), &rect) && cursor.x > rect.left
        && cursor.x < rect.right && cursor.y > rect.top && cursor.y < rect.bottom;
}

// 0.5.3 0x004545A0
void COsDialog::SetCallback(OsGuiCallback callback, void* param) {
    this->m_callbackInfo.function = callback;
    this->m_callbackInfo.userParam = param;
}

// 0.5.3 0x004545C0: brings up the dialog's popup owner chain (GW_ENABLEDPOPUP), or the dialog itself.
void COsDialog::BringToFront() {
    HWND window = GetWindow(static_cast<HWND>(this->m_handle), GW_ENABLEDPOPUP);

    if (!window) {
        window = static_cast<HWND>(this->m_handle);
    }

    OsGuiBringWindowToFront(window);
}

// 0.5.3 0x00454600
void COsDialog::SetInputFocus() {
    SetFocus(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x00454610
void COsDialog::Show(int32_t show) {
    ShowWindow(static_cast<HWND>(this->m_handle), show ? SW_SHOW : SW_HIDE);
}

// 0.5.3 0x00454630
int32_t COsDialog::IsShowing() {
    return IsWindowVisible(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x00454640
int32_t COsDialog::IsEnabled() {
    return IsWindowEnabled(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x00454650
void COsDialog::SetRedraw(int32_t redraw) {
    SendMessageA(static_cast<HWND>(this->m_handle), WM_SETREDRAW, redraw, 0);
}

// 0.5.3 0x00454670
void COsDialog::Refresh(int32_t erase) {
    if (this->IsShowing()) {
        InvalidateRect(static_cast<HWND>(this->m_handle), nullptr, erase);
    }
}

// 0.5.3 0x004546A0
void COsDialog::SetPosition(int32_t x, int32_t y) {
    SetWindowPos(static_cast<HWND>(this->m_handle), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// 0.5.3 0x004546D0: screen position of the window, or of its client area.
void COsDialog::GetPosition(int32_t* x, int32_t* y, int32_t client) {
    WINDOWINFO info;
    info.cbSize = sizeof(info);
    GetWindowInfo(static_cast<HWND>(this->m_handle), &info);

    if (client) {
        *x = info.rcClient.left;
        *y = info.rcClient.top;
    } else {
        *x = info.rcWindow.left;
        *y = info.rcWindow.top;
    }
}

// 0.5.3 0x00454720
void COsDialog::SetSize(int32_t width, int32_t height) {
    SetWindowPos(static_cast<HWND>(this->m_handle), nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// 0.5.3 0x00454750
void COsDialog::GetSize(int32_t* width, int32_t* height, int32_t clientOnly) {
    CiRect rect = OsGuiGetWindowRect(this->m_handle, clientOnly);
    *width = rect.maxX - rect.minX;
    *height = rect.maxY - rect.minY;
}

// 0.5.3 0x00454790: -1 leaves that dimension free.
void COsDialog::SetMinSize(int32_t width, int32_t height) {
    this->m_minSize.x = width;
    this->m_minSize.y = height;
}

// 0.5.3 0x004547B0
int32_t COsDialog::GetMinSize(int32_t* width, int32_t* height) {
    *width = this->m_minSize.x;
    *height = this->m_minSize.y;
    return this->m_minSize.x > -1 || this->m_minSize.y > -1;
}

// 0.5.3 0x004547F0
void COsDialog::SetTitle(const char* title) {
    SetWindowTextA(static_cast<HWND>(this->m_handle), title);
}

// 0.5.3 0x00454810
int32_t COsDialog::OnAccept() {
    return 1;
}

// 0.5.3 0x00454860
int32_t COsDialog::OnMouseDown() {
    for (uint32_t i = 0; i < this->m_controls.Count(); i++) {
        this->m_controls[i]->OnMouseDown();
    }

    return 0;
}

// 0.5.3 0x004548C0
int32_t COsDialog::OnMouseUp() {
    for (uint32_t i = 0; i < this->m_controls.Count(); i++) {
        this->m_controls[i]->OnMouseUp();
    }

    return 0;
}

// 0.5.3 0x00454920
int32_t COsDialog::OnMouseLeave() {
    if (!this->m_trackMouse) {
        return 0;
    }

    int32_t wasInside = this->m_mouseInside;
    this->m_needNewTrack = 1;
    this->m_mouseInside = this->IsMouseInside();

    if (!this->m_mouseInside && wasInside) {
        return this->OnEvent(-1, 4, 0);
    }

    return 0;
}

// 0.5.3 0x00454960: x, y in client coordinates.
int32_t COsDialog::OnMouseMove(int32_t x, int32_t y) {
    for (uint32_t i = 0; i < this->m_controls.Count(); i++) {
        this->m_controls[i]->OnMouseMove(x, y);
    }

    if (!this->m_trackMouse) {
        return 0;
    }

    int32_t wasInside = this->m_mouseInside;
    this->m_mouseInside = this->IsMouseInside();

    if (this->m_needNewTrack) {
        TRACKMOUSEEVENT track;
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = static_cast<HWND>(this->m_handle);
        track.dwHoverTime = 0;

        BOOL success = _TrackMouseEvent(&track);
        OSGUI_ASSERT(success);

        this->m_needNewTrack = 0;
    }

    if (this->m_mouseInside && !wasInside) {
        return this->OnEvent(-1, 3, 0);
    }

    return 0;
}

// 0.5.3 0x00454B30: the dialog owns the menu, which must be a popup menu (COsMenu()).
void COsDialog::SetContextMenu(COsMenu* menu) {
    if (this->m_contextMenu) {
        delete this->m_contextMenu;
    }

    this->m_contextMenu = menu;
    OSGUI_ASSERT(this->m_contextMenu->m_id == 0xFF);
}

// 0.5.3 0x00454B90: the picked item reaches the callback as item id -3, notify code 0xFFFF0000 | position.
int32_t COsDialog::OnContextMenu(int32_t x, int32_t y) {
    if (!this->m_contextMenuEnabled || !this->m_contextMenu) {
        return 0;
    }

    int32_t command = TrackPopupMenu(static_cast<HMENU>(this->m_contextMenu->m_menuHandle), TPM_RETURNCMD | TPM_NONOTIFY,
                                     x, y, 0, static_cast<HWND>(this->m_handle), nullptr);

    if (!command) {
        return 0;
    }

    return this->OnEvent(-3, command | 0xFFFF0000, 0);
}

// 0.5.3 0x00454C10: clipboard actions (0 cut, 1 copy, 2 paste, 3 clear, 4 select all, 5 undo) go to the focus.
int32_t COsDialog::CanDoClipboardAction(int32_t action) {
    COsControl* control = this->FindControl(GetFocus());
    return control ? control->CanDoClipboardAction(action) : 0;
}

// 0.5.3 0x00454C40
int32_t COsDialog::DoClipboardAction(int32_t action) {
    COsControl* control = this->FindControl(GetFocus());
    return control ? control->DoClipboardAction(action) : 0;
}

// ===== COsControl =====

// 0.5.3 0x00454C70: a control inside a COsDialog.
COsControl::COsControl(COsDialog* dialog, int32_t type, int16_t id, uint32_t flags) {
    this->m_dialog = dialog;
    this->Initialize(dialog->m_handle, type, id, flags);
    this->m_dialog->AddControl(this);
}

// 0.5.3 0x00454CE0: a control in any window (it destroys its own window).
COsControl::COsControl(void* window, int32_t type, int16_t id, uint32_t flags) {
    this->m_dialog = nullptr;
    this->Initialize(window, type, id, flags);
}

// 0.5.3 0x00454D20. Common flags: 1 sizing border, 2 horizontal scroll bar, 4 vertical scroll bar, 8 hidden,
// 0x10 border, 0x20 transparent; 0x10000 and up depend on the type (see the switch).
void COsControl::Initialize(void* window, int32_t type, int16_t id, uint32_t flags) {
    OSGUI_ASSERT(type >= 0 && type < static_cast<int32_t>(OSGUI_NUM_CONTROL_TYPES));
    OSGUI_ASSERT(id == -1 || id >= 10); // ids below 10 are reserved (IDOK, IDCANCEL...)

    this->m_id = id;
    this->m_callback = nullptr;
    this->m_callbackParam = nullptr;
    this->m_contextMenu = nullptr;
    this->m_redrawLevel = 0;
    this->m_type = type;
    this->m_flags = flags;
    this->m_contextMenuEnabled = 1;

    uint32_t style = s_controlStyle[type] | WS_CHILD | WS_VISIBLE;
    uint32_t exStyle = s_controlExStyle[type];

    if (flags & 8) {
        style = (s_controlStyle[type] & ~(WS_CHILD | WS_VISIBLE)) | WS_CHILD;
    }

    if (flags & 1) {
        style |= WS_THICKFRAME;
    }

    if (flags & 2) {
        style |= WS_HSCROLL;
    }

    if (flags & 4) {
        style |= WS_VSCROLL;
    }

    if (flags & 0x10) {
        style |= WS_BORDER;
    }

    if (flags & 0x20) {
        exStyle |= WS_EX_TRANSPARENT;
    }

    switch (type) {
        case TYPE_STATIC_TEXT:
            if (flags & 0x10000) {
                style |= SS_SUNKEN;
            }

            break;

        case TYPE_EDIT_BOX:
            if (flags & 0x10000) { // multi-line
                style = (style & ~(ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL)) | ES_MULTILINE | ES_AUTOVSCROLL;
            }

            if (flags & 0x20000) {
                style |= ES_WANTRETURN;
            }

            break;

        case TYPE_LIST_BOX:
            if (flags & 0x10000) { // multi-select (COsListBox::SelectItem...)
                style |= LBS_EXTENDEDSEL | LBS_MULTIPLESEL;
            }

            break;

        case TYPE_TREE_VIEW:
            if (flags & 0x10000) {
                style &= ~TVS_DISABLEDRAGDROP;
            }

            if (flags & 0x20000) {
                style |= TVS_EDITLABELS;
            }

            // 0x40000 is the tree view multi-selection, handled by COsTreeView itself.
            if (flags & 0x80000) {
                style = (style & ~(TVS_HASBUTTONS | TVS_SINGLEEXPAND)) | TVS_SINGLEEXPAND;
            }

            break;

        case TYPE_STATIC_BOX:
            if (flags & 0x10000) {
                style = (style & ~0x7) | SS_GRAYRECT;
            }

            if (flags & 0x20000) {
                style = (style & ~0x17) | SS_ETCHEDHORZ;
            }

            if (flags & 0x40000) {
                style = (style & ~0xF) | SS_OWNERDRAW;
            }

            break;

        case TYPE_UP_DOWN:
            if (flags & 0x10000) {
                style |= UDS_HORZ;
            }

            if (flags & 0x20000) {
                style |= UDS_WRAP;
            }

            break;

        case TYPE_RADIO_BUTTON:
            if (flags & 0x10000) { // first button of a group
                style |= WS_GROUP | WS_TABSTOP;
            }

            break;

        case TYPE_SCROLL_BAR:
            if (flags & 0x10000) {
                style |= SBS_VERT;
            }

            if (flags & 0x20000) {
                style |= WS_TABSTOP;
            }

            break;

        default:
            break;
    }

    OSGUI_ASSERT(s_appInstance != 0);

    this->m_handle = CreateWindowExA(exStyle, s_controlClassName[this->m_type], "", style, 0, 0, 0, 0, static_cast<HWND>(window),
                                     reinterpret_cast<HMENU>(static_cast<INT_PTR>(this->m_id)), s_appInstance, nullptr);

    if (!this->m_handle) {
        DWORD error = GetLastError();

        if (error) {
            char message[128];
            SStrPrintf(message, sizeof(message), "COsControl::Initialize: CreateWindowEx failed, error %u (0x%08X)", error, error);
            SErrDisplayError(STORM_ERROR_ASSERTION, __FILE__, __LINE__, message, 0, 1, 0);
        }
    }

    this->SetFont(s_controlFont[this->m_type]);
    sSetOsGuiPointer(this->m_handle, this);
}

// 0.5.3 0x00455060
COsControl::~COsControl() {
    if (!this->m_dialog) {
        sRemoveOsGuiPointer(this->m_handle);
        DestroyWindow(static_cast<HWND>(this->m_handle));
    }

    if (this->m_contextMenu) {
        delete this->m_contextMenu;
    }
}

// 0.5.3 0x004550B0: nested; redraw is turned off by the first SetRedraw(0) and back on by the matching SetRedraw(1).
void COsControl::SetRedraw(int32_t redraw) {
    int32_t send;

    if (redraw) {
        send = this->m_redrawLevel == 1;

        if (this->m_redrawLevel > 0) {
            this->m_redrawLevel--;
        }
    } else {
        send = this->m_redrawLevel == 0;
        this->m_redrawLevel++;
    }

    if (send) {
        SendMessageA(static_cast<HWND>(this->m_handle), WM_SETREDRAW, redraw, 0);
    }
}

// 0.5.3 0x004550F0
void COsControl::Refresh(int32_t erase) {
    InvalidateRect(static_cast<HWND>(this->m_handle), nullptr, erase);
}

// 0.5.3 0x00455110
void COsControl::SetCallback(OsGuiCallback callback, void* param) {
    this->m_callback = callback;
    this->m_callbackParam = param;
}

// 0.5.3 0x00455130: the control's own callback; SendEvent falls back to the dialog's when there is none.
int32_t COsControl::OnEvent(int32_t itemId, int32_t notifyCode, int32_t code) {
    if (!this->m_callback) {
        return 0;
    }

    OsGuiCallbackParams params;
    params.type = itemId;
    params.subType = notifyCode;
    params.code = code;
    params.user = this->m_callbackParam;
    this->m_callback(params);

    return 1;
}

// 0.5.3 0x004551C0: 0 ANSI_VAR_FONT, 1 SYSTEM_FONT, 2 ANSI_FIXED_FONT.
void COsControl::SetFont(int32_t font) {
    HGDIOBJ object;

    switch (font) {
        case 0: object = GetStockObject(ANSI_VAR_FONT); break;
        case 1: object = GetStockObject(SYSTEM_FONT); break;
        case 2: object = GetStockObject(ANSI_FIXED_FONT); break;
        default: return;
    }

    if (object) {
        SendMessageA(static_cast<HWND>(this->m_handle), WM_SETFONT, reinterpret_cast<WPARAM>(object), 1);
    }
}

// 0.5.3 0x00455200: only while the control's dialog is the active window.
void COsControl::SetInputFocus() {
    if (this->m_dialog->m_handle == OsGuiGetWindow(OSGUI_WINDOW_TYPE_ACTIVE)) {
        SetFocus(static_cast<HWND>(this->m_handle));
    }
}

// 0.5.3 0x00455230
void COsControl::LoseInputFocus() {
    if (this->HasInputFocus()) {
        SetFocus(GetParent(static_cast<HWND>(this->m_handle)));
    }
}

// 0.5.3 0x00455250
int32_t COsControl::HasInputFocus() {
    return this->m_handle == GetFocus();
}

// 0.5.3 0x00455270
void COsControl::SetText(const char* text) {
    SetWindowTextA(static_cast<HWND>(this->m_handle), text);
    this->OnTextChange();
}

// 0.5.3 0x00455290
void COsControl::GetText(char* buf, int32_t bufSize) {
    GetWindowTextA(static_cast<HWND>(this->m_handle), buf, bufSize);
}

// 0.5.3 0x004552B0
int32_t COsControl::GetTextLength() {
    return GetWindowTextLengthA(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x004552C0: the size of a string drawn in the control's font.
void COsControl::GetTextSize(const char* text, int32_t* width, int32_t* height) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    HDC dc = GetDC(hwnd);
    HGDIOBJ font = reinterpret_cast<HGDIOBJ>(SendMessageA(hwnd, WM_GETFONT, 0, 0));
    HGDIOBJ oldFont = SelectObject(dc, font);

    SIZE size;
    GetTextExtentPoint32A(dc, text, static_cast<int32_t>(SStrLen(text)), &size);

    SelectObject(dc, oldFont);
    ReleaseDC(hwnd, dc);

    *width = size.cx;

    if (height) {
        *height = size.cy;
    }
}

// 0.5.3 0x00455340
void COsControl::GetTextSize(int32_t* width, int32_t* height) {
    char text[260];
    this->GetText(text, sizeof(text));
    this->GetTextSize(text, width, height);
}

// 0.5.3 0x00455380
void COsControl::Show(int32_t show) {
    ShowWindow(static_cast<HWND>(this->m_handle), show ? SW_SHOW : SW_HIDE);
}

// 0.5.3 0x004553A0
int32_t COsControl::IsShowing() {
    return IsWindowVisible(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x004553B0
void COsControl::Enable(int32_t enable) {
    EnableWindow(static_cast<HWND>(this->m_handle), enable);
}

// 0.5.3 0x004553D0
int32_t COsControl::IsEnabled() {
    return IsWindowEnabled(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x004553E0: x, y in the parent's client coordinates.
void COsControl::SetPosition(int32_t x, int32_t y) {
    SetWindowPos(static_cast<HWND>(this->m_handle), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// 0.5.3 0x00455410: screen position, or relative to the parent's client area.
void COsControl::GetPosition(int32_t* x, int32_t* y, int32_t parentRelative) {
    RECT rect;
    GetWindowRect(static_cast<HWND>(this->m_handle), &rect);

    int32_t left = rect.left;
    int32_t top = rect.top;

    if (parentRelative) {
        HWND parent = GetParent(static_cast<HWND>(this->m_handle));

        if (parent) {
            CiRect parentRect = OsGuiGetWindowRect(parent, 1);
            left -= parentRect.minX;
            top -= parentRect.minY;
        }
    }

    *x = left;
    *y = top;
}

// 0.5.3 0x004554A0
void COsControl::SetSize(int32_t width, int32_t height) {
    SetWindowPos(static_cast<HWND>(this->m_handle), nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    this->OnSizeChange();
}

// 0.5.3 0x004554D0
void COsControl::GetSize(int32_t* width, int32_t* height) {
    RECT rect;
    GetWindowRect(static_cast<HWND>(this->m_handle), &rect);
    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

// 0.5.3 0x00455510: registers the control with its dialog's tooltip window (or the global one outside a dialog).
void COsControl::SetTooltip(const char* text) {
    HWND tips;
    HWND owner;

    if (this->m_dialog) {
        tips = static_cast<HWND>(this->m_dialog->GetTooltips());
        owner = static_cast<HWND>(this->m_dialog->m_handle);
    } else {
        tips = sGetGlobalTips();
        owner = GetParent(static_cast<HWND>(this->m_handle));
    }

    if (!tips) {
        return;
    }

    TOOLINFOA info = {};
    info.cbSize = TTTOOLINFOA_V2_SIZE;
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    info.hwnd = owner;
    info.uId = reinterpret_cast<UINT_PTR>(this->m_handle);
    info.hinst = s_appInstance;
    info.lpszText = const_cast<char*>(text);

    if (!SendMessageA(tips, TTM_ADDTOOLA, 0, reinterpret_cast<LPARAM>(&info))) {
        OutputDebugStringA("Warning: COsControl::SetTooltip - TTM_ADDTOOL failed\n");
    }
}

// 0.5.3 0x004555C0: the control owns the menu, which must be a popup menu (COsMenu()).
void COsControl::SetContextMenu(COsMenu* menu) {
    if (this->m_contextMenu) {
        delete this->m_contextMenu;
    }

    this->m_contextMenu = menu;
    OSGUI_ASSERT(this->m_contextMenu->m_id == 0xFF);
}

// 0.5.3 0x00455620: event 17 lets the owner update the menu first; the picked item comes back as item id -3,
// notify code (control id << 16) | position.
int32_t COsControl::OnContextMenu(int32_t x, int32_t y) {
    if (!this->m_contextMenuEnabled || !this->m_contextMenu) {
        return 0;
    }

    this->SendEvent(17, 0);

    int32_t command = TrackPopupMenu(static_cast<HMENU>(this->m_contextMenu->m_menuHandle), TPM_RETURNCMD | TPM_NONOTIFY,
                                     x, y, 0, static_cast<HWND>(this->m_handle), nullptr);

    if (!command) {
        return 0;
    }

    return this->OnEvent(-3, command | (this->m_id << 16), 0);
}

// 0.5.3 0x00455690
int32_t COsControl::IsHandleFromControl(OsGuiHandle handle) {
    return handle == this->m_handle;
}

// 0.5.3 0x004556B0
int32_t COsControl::OnNotify(int32_t code, void* data) {
    int32_t event = sNCodeToItemCode(code, this->m_type);
    return event == -1 ? 0 : this->SendEvent(event, 0);
}

// 0.5.3 0x00455720: wparam of WM_COMMAND (the notification code is the high word).
int32_t COsControl::OnCommand(int32_t code) {
    int32_t event = sNCodeToItemCode(HIWORD(code), this->m_type);
    return event == -1 ? 0 : this->SendEvent(event, 0);
}

// 0.5.3 0x00455750: wparam of WM_HSCROLL/WM_VSCROLL (the scroll code is the low word).
int32_t COsControl::OnScroll(int32_t code) {
    int32_t event = sNCodeToItemCode(LOWORD(code), this->m_type);
    return event == -1 ? 0 : this->SendEvent(event, 0);
}

// 0.5.3 0x0045AE70..0x0045AF50: the default virtuals.
void COsControl::OnDestroy() {
}

int32_t COsControl::OnDraw(void* dc, uint32_t state, CiRect& rect) {
    return 0;
}

void* COsControl::OnSetColors(void* dc) {
    return nullptr;
}

int32_t COsControl::OnReturn() {
    return 0;
}

int32_t COsControl::OnEscape() {
    return 0;
}

int32_t COsControl::OnMouseDown() {
    return 0;
}

int32_t COsControl::OnMouseUp() {
    return 0;
}

void COsControl::OnMouseMove(int32_t x, int32_t y) {
}

void COsControl::OnSizeChange() {
}

void COsControl::OnTextChange() {
}

int32_t COsControl::OnMouseWheel(int32_t delta) {
    return 0;
}

int32_t COsControl::CanDoClipboardAction(int32_t action) {
    return 0;
}

int32_t COsControl::DoClipboardAction(int32_t action) {
    return 0;
}

void COsControl::SetValue(int32_t value) {
}

int32_t COsControl::GetValue() {
    return -1;
}

// ===== COsButton =====

// 0.5.3 0x00455780
COsButton::COsButton(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_BUTTON, id, flags) {
}

// 0.5.3 0x004557E0: the button Enter presses (DM_SETDEFID).
void COsButton::SetDefaultButton() {
    if (this->m_dialog) {
        SendMessageA(static_cast<HWND>(this->m_dialog->m_handle), DM_SETDEFID, this->m_id, 0);
    }
}

// 0.5.3 0x00455800: the button Escape presses (COsDialog::OnCancel).
void COsButton::SetCancelButton() {
    if (this->m_dialog) {
        this->m_dialog->m_cancelButton = this;
    }
}

// 0.5.3 0x00455810: shows the button pushed (BM_SETSTATE), with the control briefly disabled around the change.
void COsButton::SetHighlight(int32_t highlight) {
    int32_t enabled = this->IsEnabled();
    this->Enable(0);
    SendMessageA(static_cast<HWND>(this->m_handle), BM_SETSTATE, highlight, 0);
    this->Enable(enabled);
}

// ===== COsImageButton =====

// 0.5.3 0x00455850
COsImageButton::COsImageButton(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_IMAGE_BUTTON, id, flags) {
}

// 0.5.3 0x004558B0
COsImageButton::COsImageButton(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_IMAGE_BUTTON, id, flags) {
}

// 0.5.3 0x004558E0
COsImageButton::~COsImageButton() {
}

// 0.5.3 0x004558F0
void COsImageButton::OnDestroy() {
    HGDIOBJ bitmap = reinterpret_cast<HGDIOBJ>(SendMessageA(static_cast<HWND>(this->m_handle), BM_GETIMAGE, IMAGE_BITMAP, 0));

    if (bitmap) {
        DeleteObject(bitmap);
    }
}

// 0.5.3 0x00455910: data is width * height BGRA pixels, top row first.
void COsImageButton::SetImage(int32_t width, int32_t height, void* data) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    HDC dc = GetDC(hwnd);
    HBITMAP bitmap = sBitmapFromImageData(width, height, data, dc);
    HGDIOBJ old = reinterpret_cast<HGDIOBJ>(SendMessageA(hwnd, BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(bitmap)));

    if (old) {
        DeleteObject(old);
    }

    ReleaseDC(hwnd, dc);
}

// 0.5.3 0x004559D0
void COsImageButton::SetHighlight(int32_t highlight) {
    int32_t enabled = this->IsEnabled();
    this->Enable(0);
    SendMessageA(static_cast<HWND>(this->m_handle), BM_SETSTATE, highlight, 0);
    this->Enable(enabled);
}

// 0.5.3 0x00455A10
int32_t COsImageButton::IsPushed() {
    return SendMessageA(static_cast<HWND>(this->m_handle), BM_GETSTATE, 0, 0) == BST_PUSHED;
}

// ===== COsStaticText =====

// 0.5.3 0x00455A30
COsStaticText::COsStaticText(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_STATIC_TEXT, id, flags) {
    this->m_textColor = CImVector(0, 0, 0, 0);
    this->Initialize();
}

// 0.5.3 0x00455AA0
COsStaticText::COsStaticText(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_STATIC_TEXT, id, flags) {
    this->m_textColor = CImVector(0, 0, 0, 0);
    this->Initialize();
}

// 0.5.3 0x00455AE0: alpha 0 means the default text color.
void COsStaticText::Initialize() {
    this->m_textColor.a = 0;
}

// 0.5.3 0x00455AF0: 0 left, 1 center, 2 right.
void COsStaticText::SetJustification(int32_t justification) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    LONG_PTR style = GetWindowLongPtrA(hwnd, GWL_STYLE) & ~static_cast<LONG_PTR>(SS_CENTER | SS_RIGHT);

    if (justification == 1) {
        style |= SS_CENTER;
    } else if (justification == 2) {
        style |= SS_RIGHT;
    }

    SetWindowLongPtrA(hwnd, GWL_STYLE, style);
}

// 0.5.3 0x00455B40
void COsStaticText::SetTextColor(const CImVector& color) {
    CImVector newColor = color;
    newColor.a = 0xFF;

    if (sColorValue(this->m_textColor) != sColorValue(newColor)) {
        this->m_textColor = newColor;
        this->Refresh(1);
    }
}

// 0.5.3 0x00455B70
void* COsStaticText::OnSetColors(void* dc) {
    if (!this->m_textColor.a) {
        return nullptr;
    }

    HDC hdc = static_cast<HDC>(dc);
    ::SetTextColor(hdc, sColorRef(this->m_textColor));
    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
    return GetSysColorBrush(COLOR_BTNFACE);
}

// ===== COsStaticImage =====

// 0.5.3 0x00455BC0
COsStaticImage::COsStaticImage(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_STATIC_IMAGE, id, 0) {
}

// 0.5.3 0x00455C20
COsStaticImage::~COsStaticImage() {
}

// 0.5.3 0x00455C30
void COsStaticImage::OnDestroy() {
    HGDIOBJ bitmap = reinterpret_cast<HGDIOBJ>(SendMessageA(static_cast<HWND>(this->m_handle), STM_GETIMAGE, IMAGE_BITMAP, 0));

    if (bitmap) {
        DeleteObject(bitmap);
    }
}

// 0.5.3 0x00455C50
void COsStaticImage::SetImage(int32_t width, int32_t height, void* data) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    HDC dc = GetDC(hwnd);
    HBITMAP bitmap = sBitmapFromImageData(width, height, data, dc);
    HGDIOBJ old = reinterpret_cast<HGDIOBJ>(SendMessageA(hwnd, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(bitmap)));

    if (old) {
        DeleteObject(old);
    }

    ReleaseDC(hwnd, dc);
}

// 0.5.3 0x00455CB0: keeps the control size (an SS_BITMAP static resizes itself to its image).
void COsStaticImage::ClearImage() {
    int32_t width;
    int32_t height;
    this->GetSize(&width, &height);

    HGDIOBJ old = reinterpret_cast<HGDIOBJ>(SendMessageA(static_cast<HWND>(this->m_handle), STM_SETIMAGE, IMAGE_BITMAP, 0));

    if (old) {
        DeleteObject(old);
    }

    this->SetSize(width, height);
}

// ===== COsEditBox =====

// 0.5.3 0x00455D00
COsEditBox::COsEditBox(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_EDIT_BOX, id, flags) {
    this->Initialize();
}

// 0.5.3 0x00455D60
COsEditBox::COsEditBox(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_EDIT_BOX, id, flags) {
    this->Initialize();
}

// 0.5.3 0x00455D90
void COsEditBox::Initialize() {
    this->m_filtersEnabled = 0;
    this->m_filters = 0;
    this->m_selSize = 0;
    SetWindowLongPtrA(static_cast<HWND>(this->m_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(sEditBoxProc));
}

// 0.5.3 0x00455E20: event 19 when the selection grows or shrinks (clipboard menu items follow it).
void COsEditBox::UpdateSelection() {
    int32_t selSize = this->GetSelectionSize();

    if (selSize != this->m_selSize) {
        this->m_selSize = selSize;
        this->SendEvent(19, 0);
    }
}

// 0.5.3 0x00455E40
int32_t COsEditBox::OnReturn() {
    return this->SendEvent(12, 0);
}

// 0.5.3 0x00455E50
void COsEditBox::SetTextLimit(int32_t limit) {
    SendMessageA(static_cast<HWND>(this->m_handle), EM_LIMITTEXT, limit, 0);
}

// 0.5.3 0x00455E70
void COsEditBox::SelectAll() {
    SendMessageA(static_cast<HWND>(this->m_handle), EM_SETSEL, 0, -1);
}

// 0.5.3 0x00455E90
int32_t COsEditBox::GetSelectionSize() {
    DWORD selStart = 0;
    DWORD selEnd = 0;
    SendMessageA(static_cast<HWND>(this->m_handle), EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));
    return selEnd - selStart;
}

// 0.5.3 0x00455EC0
void COsEditBox::EnableFilters(int32_t enable) {
    this->m_filtersEnabled = enable;
}

// 0.5.3 0x00455ED0: OSGUI_FILTER_* bits of the characters to accept.
void COsEditBox::SetFilter(uint32_t filter, int32_t enable) {
    if (enable) {
        this->m_filters |= filter;
    } else {
        this->m_filters &= ~filter;
    }
}

// 0.5.3 0x00455FE0
int32_t COsEditBox::CanDoClipboardAction(int32_t action) {
    switch (action) {
        case 0:
        case 1:
        case 3:
            return this->GetSelectionSize() > 0;

        case 2: {
            if (!OpenClipboard(static_cast<HWND>(this->m_handle))) {
                return 0;
            }

            int32_t hasText = GetClipboardData(CF_TEXT) != nullptr;
            CloseClipboard();
            return hasText;
        }

        case 4:
            return 1;

        case 5:
            return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), EM_CANUNDO, 0, 0));

        default:
            return 0;
    }
}

// 0.5.3 0x00456070
int32_t COsEditBox::DoClipboardAction(int32_t action) {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    switch (action) {
        case 0: SendMessageA(hwnd, WM_CUT, 0, 0); return 1;
        case 1: SendMessageA(hwnd, WM_COPY, 0, 0); return 1;
        case 2: SendMessageA(hwnd, WM_PASTE, 0, 0); return 1;
        case 3: SendMessageA(hwnd, WM_CLEAR, 0, 0); return 1;
        case 4: this->SelectAll(); return 1;
        case 5: SendMessageA(hwnd, EM_UNDO, 0, 0); return 1;
        default: return 0;
    }
}

// ===== COsPopupMenu (drop-down list combo box) =====

// 0.5.3 0x00456140: the drop-down is at most half the screen high.
COsPopupMenu::COsPopupMenu(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_COMBO_BOX, id, 0) {
    CiRect bounds = OsGuiGetScreenBounds();
    this->m_maxHeight = (bounds.maxY - bounds.minY) / 2 - 20;
    this->m_baseHeight = 0;
}

// 0.5.3 0x004561C0
COsPopupMenu::~COsPopupMenu() {
}

// 0.5.3 0x004561D0: a combo box's window height includes its drop-down list, so the height is recomputed.
void COsPopupMenu::SetSize(int32_t width, int32_t height) {
    this->m_baseHeight = height;
    COsControl::SetSize(width, height);
    this->AdjustHeight();
}

// 0.5.3 0x00456200
void COsPopupMenu::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), CB_SETCURSEL, value, 0);
}

// 0.5.3 0x00456220
int32_t COsPopupMenu::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), CB_GETCURSEL, 0, 0));
}

// 0.5.3 0x00456240
void COsPopupMenu::ClearItems() {
    SendMessageA(static_cast<HWND>(this->m_handle), CB_RESETCONTENT, 0, 0);
}

// 0.5.3 0x00456260
int32_t COsPopupMenu::GetNumItems() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), CB_GETCOUNT, 0, 0));
}

// 0.5.3 0x00456280: the first item inserted becomes the selection.
void COsPopupMenu::InsertItem(const char* text, int32_t pos) {
    int32_t wasEmpty = this->GetNumItems() == 0;
    SendMessageA(static_cast<HWND>(this->m_handle), CB_INSERTSTRING, pos, reinterpret_cast<LPARAM>(text));
    this->AdjustHeight();

    if (wasEmpty) {
        this->SetValue(0);
    }
}

// 0.5.3 0x004562D0
void COsPopupMenu::SetItemHeight(int32_t height) {
    SendMessageA(static_cast<HWND>(this->m_handle), CB_SETITEMHEIGHT, 0, height);
    this->AdjustHeight();
}

// 0.5.3 0x00456300
int32_t COsPopupMenu::GetItemHeight() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), CB_GETITEMHEIGHT, 0, 0));
}

// 0.5.3 0x00456320: sizes the drop-down to show every item (plus the edit field), up to m_maxHeight.
void COsPopupMenu::AdjustHeight() {
    int32_t height = this->GetItemHeight() * (this->GetNumItems() + 2);

    if (height >= this->m_maxHeight) {
        height = this->m_maxHeight;
    }

    int32_t width;
    int32_t oldHeight;
    this->GetSize(&width, &oldHeight);
    COsControl::SetSize(width, height);
}

// 0.5.3 0x00456370
void COsPopupMenu::SetMaxHeight(int32_t height) {
    this->m_maxHeight = height;
    this->AdjustHeight();
}

// 0.5.3 0x00456390
void COsPopupMenu::DeleteItem(int32_t pos) {
    SendMessageA(static_cast<HWND>(this->m_handle), CB_DELETESTRING, pos, 0);
    this->AdjustHeight();
}

// 0.5.3 0x004563C0
void COsPopupMenu::SetItemText(int32_t pos, const char* text) {
    this->DeleteItem(pos);
    this->InsertItem(text, pos);
}

// ===== COsListBox =====

// 0.5.3 0x004563F0. Flag 0x10000: multiple selection (SelectItem/IsItemSelected/SelectAll instead of Set/GetValue).
COsListBox::COsListBox(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_LIST_BOX, id, flags) {
}

// 0.5.3 0x00456450
COsListBox::~COsListBox() {
}

// 0.5.3 0x00456460
void COsListBox::SetValue(int32_t value) {
    if (this->m_flags & 0x10000) {
        SErrDisplayAppFatal("!(mFlags & OSGUI_LISTBOX_MULTISELECT)");
    }

    SendMessageA(static_cast<HWND>(this->m_handle), LB_SETCURSEL, value, 0);
}

// 0.5.3 0x004564B0
int32_t COsListBox::GetValue() {
    if (this->m_flags & 0x10000) {
        SErrDisplayAppFatal("!(mFlags & OSGUI_LISTBOX_MULTISELECT)");
    }

    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_GETCURSEL, 0, 0));
}

// 0.5.3 0x00456500
void COsListBox::SelectItem(int32_t pos, int32_t select) {
    if (!(this->m_flags & 0x10000)) {
        SErrDisplayAppFatal("mFlags & OSGUI_LISTBOX_MULTISELECT");
    }

    SendMessageA(static_cast<HWND>(this->m_handle), LB_SETSEL, select, pos);
}

// 0.5.3 0x00456550
int32_t COsListBox::IsItemSelected(int32_t pos) {
    if (!(this->m_flags & 0x10000)) {
        SErrDisplayAppFatal("mFlags & OSGUI_LISTBOX_MULTISELECT");
    }

    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_GETSEL, pos, 0));
}

// 0.5.3 0x004565A0
void COsListBox::SelectAll(int32_t select) {
    if (!(this->m_flags & 0x10000)) {
        SErrDisplayAppFatal("mFlags & OSGUI_LISTBOX_MULTISELECT");
    }

    int32_t count = this->GetNumItems();

    for (int32_t i = 0; i < count; i++) {
        this->SelectItem(i, select);
    }
}

// 0.5.3 0x00456610
void COsListBox::ClearItems() {
    SendMessageA(static_cast<HWND>(this->m_handle), LB_RESETCONTENT, 0, 0);
}

// 0.5.3 0x00456630
int32_t COsListBox::GetNumItems() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_GETCOUNT, 0, 0));
}

// 0.5.3 0x00456650: pos -1 appends.
void COsListBox::InsertItem(const char* text, int32_t pos) {
    SendMessageA(static_cast<HWND>(this->m_handle), LB_INSERTSTRING, pos, reinterpret_cast<LPARAM>(text));
}

// 0.5.3 0x00456670: pos -1 deletes the last item.
void COsListBox::DeleteItem(int32_t pos) {
    if (pos == -1) {
        pos = this->GetNumItems() - 1;
    }

    SendMessageA(static_cast<HWND>(this->m_handle), LB_DELETESTRING, pos, 0);
}

// 0.5.3 0x004566A0
void COsListBox::SetItemText(int32_t pos, const char* text) {
    this->DeleteItem(pos);
    this->InsertItem(text, pos);
}

// 0.5.3 0x004566D0
int32_t COsListBox::GetItemTextLength(int32_t pos) {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_GETTEXTLEN, pos, 0));
}

// 0.5.3 0x004566F0
void COsListBox::GetItemText(int32_t pos, char* buf, int32_t bufSize) {
    if (bufSize <= this->GetItemTextLength(pos)) {
        SErrDisplayAppFatal("inBufSize > textLength");
    }

    SendMessageA(static_cast<HWND>(this->m_handle), LB_GETTEXT, pos, reinterpret_cast<LPARAM>(buf));
}

// 0.5.3 0x00456750
void COsListBox::SetItemHeight(int32_t height) {
    SendMessageA(static_cast<HWND>(this->m_handle), LB_SETITEMHEIGHT, 0, height);
}

// 0.5.3 0x00456770
int32_t COsListBox::GetItemHeight() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_GETITEMHEIGHT, 0, 0));
}

// 0.5.3 0x00456790: a right click first selects the item under the cursor (event 2 when that changes the selection).
int32_t COsListBox::OnContextMenu(int32_t x, int32_t y) {
    int32_t posX;
    int32_t posY;
    this->GetPosition(&posX, &posY, 0);

    LPARAM point = MAKELPARAM(static_cast<uint16_t>(x - posX), static_cast<uint16_t>(y - posY));
    int32_t item = static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LB_ITEMFROMPOINT, 0, point));

    if (item < 0 || item >= this->GetNumItems()) {
        return 0;
    }

    if (this->m_flags & 0x10000) {
        this->SelectAll(0);
        this->SelectItem(item, 1);
    } else {
        int32_t oldValue = this->GetValue();
        this->SetValue(item);

        if (oldValue == item) {
            return COsControl::OnContextMenu(x, y);
        }
    }

    this->SendEvent(2, 0);
    return COsControl::OnContextMenu(x, y);
}

// 0.5.3 0x00456850
int32_t COsListBox::OnReturn() {
    return this->SendEvent(9, 0);
}

// ===== COsCheckbox =====

// 0.5.3 0x00456860: sizes itself to its text (OnTextChange).
COsCheckbox::COsCheckbox(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_CHECKBOX, id, 0) {
    this->m_settingSize = 0;
    this->m_maxWidth = 0;
    this->OnTextChange();
}

// 0.5.3 0x004568D0
COsCheckbox::COsCheckbox(void* window, int16_t id) : COsControl(window, TYPE_CHECKBOX, id, 0) {
    this->m_settingSize = 0;
    this->m_maxWidth = 0;
    this->OnTextChange();
}

// 0.5.3 0x00456910
void COsCheckbox::SetMaxWidth(int32_t width) {
    this->m_maxWidth = width;
    this->OnTextChange();
}

// 0.5.3 0x00456930
void COsCheckbox::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), BM_SETCHECK, value != 0, 0);
}

// 0.5.3 0x00456960
int32_t COsCheckbox::GetValue() {
    return SendMessageA(static_cast<HWND>(this->m_handle), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

// 0.5.3 0x00456980: the indeterminate state, for a selection whose items disagree.
void COsCheckbox::ClearValue() {
    SendMessageA(static_cast<HWND>(this->m_handle), BM_SETCHECK, BST_INDETERMINATE, 0);
}

// 0.5.3 0x004569A0
int32_t COsCheckbox::HasValue() {
    return SendMessageA(static_cast<HWND>(this->m_handle), BM_GETCHECK, 0, 0) != BST_INDETERMINATE;
}

// 0.5.3 0x004569C0: BS_AUTO3STATE cycles through indeterminate; a click on it goes to unchecked instead.
int32_t COsCheckbox::OnEvent(int32_t itemId, int32_t notifyCode, int32_t code) {
    if (notifyCode == 2 && !this->HasValue()) {
        this->SetValue(0);
    }

    return COsControl::OnEvent(itemId, notifyCode, code);
}

// 0.5.3 0x00456A00: the box (20) plus the text width, at most m_maxWidth, 16 high.
void COsCheckbox::OnTextChange() {
    int32_t textWidth;
    this->GetTextSize(&textWidth, nullptr);

    int32_t width = textWidth + 20;

    if (this->m_maxWidth > 0 && width >= this->m_maxWidth) {
        width = this->m_maxWidth;
    }

    int32_t oldWidth;
    int32_t oldHeight;
    this->GetSize(&oldWidth, &oldHeight);

    if (oldWidth != width || oldHeight != 16) {
        this->m_settingSize = 1;
        this->SetSize(width, 16);
        this->m_settingSize = 0;
    }
}

// 0.5.3 0x00456A60
void COsCheckbox::OnSizeChange() {
    if (!this->m_settingSize) {
        char text[260];
        this->GetText(text, sizeof(text));

        char message[300];
        SStrPrintf(message, sizeof(message), "Unnecessary COsCheckbox::SetSize for '%s'\n", text);
        OutputDebugStringA(message);

        this->OnTextChange();
    }
}

// ===== COsProgressBar =====

// 0.5.3 0x00456AB0
COsProgressBar::COsProgressBar(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_PROGRESS_BAR, id, 0) {
}

// 0.5.3 0x00456B10
void COsProgressBar::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), PBM_SETPOS, value, 0);
}

// 0.5.3 0x00456B30
int32_t COsProgressBar::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), PBM_GETPOS, 0, 0));
}

// ===== COsSlider =====

// 0.5.3 0x00456B50
COsSlider::COsSlider(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_TRACK_BAR, id, 0) {
}

// 0.5.3 0x00456BB0
void COsSlider::SetMinValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), TBM_SETRANGEMIN, 1, value);
}

// 0.5.3 0x00456BD0
void COsSlider::SetMaxValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), TBM_SETRANGEMAX, 1, value);
}

// 0.5.3 0x00456BF0
void COsSlider::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), TBM_SETPOS, 1, value);
}

// 0.5.3 0x00456C10
int32_t COsSlider::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), TBM_GETPOS, 0, 0));
}

// ===== COsTreeView =====
//
// Each item's lParam indexes m_itemParams (user pointer and color), reused through OsGuiTreeItemParams::used.
// Item images live in m_images (16x16); image 0 is the blank default, freed slots go to m_unusedImageIDs.
// Flag 0x40000: multiple selection, done by hand with TVIS_SELECTED (the native control is single-selection).

// 0.5.3 0x00456C30
COsTreeView::COsTreeView(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_TREE_VIEW, id, flags) {
    this->InitializeTreeView();
}

// 0.5.3 0x00456CD0
COsTreeView::COsTreeView(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_TREE_VIEW, id, flags) {
    this->InitializeTreeView();
}

// 0.5.3 0x00456D40
void COsTreeView::InitializeTreeView() {
    this->m_dragImage = nullptr;
    this->m_dragging = 0;
    this->m_dragHandler = nullptr;
    this->m_dragHandlerParam = nullptr;
    this->m_dragInfo = {};
    this->m_dragInfo.treeView = this;
    this->m_canEditFunc = nullptr;
    this->m_canEditParam = nullptr;
    this->m_expandFunc = nullptr;
    this->m_expandParam = nullptr;
    this->m_textLimit = -1;
    this->m_filtersEnabled = 0;
    this->m_filters = 0;

    SetWindowLongPtrA(static_cast<HWND>(this->m_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(sTreeViewProc));

    this->m_images = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETIMAGELIST, TVSIL_NORMAL, reinterpret_cast<LPARAM>(this->m_images));

    uint8_t blank[16 * 16 * 4] = {};
    this->SetItemImage(nullptr, 16, 16, blank);
}

// 0.5.3 0x00456E50
COsTreeView::~COsTreeView() {
    this->DestroyDragImage();
    ImageList_Destroy(static_cast<HIMAGELIST>(this->m_images));
}

// 0.5.3 0x00456EB0
void COsTreeView::SetBackgroundColor(const CImVector& color) {
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETBKCOLOR, 0, sColorRef(color));
}

// 0.5.3 0x00456EE0
void COsTreeView::ClearItems() {
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(TVI_ROOT));
}

// 0.5.3 0x00456F00
void COsTreeView::DeleteItem(void* item) {
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(item));
}

// 0.5.3 0x00456F20: after null inserts first, after 0xFFFF inserts last.
void* COsTreeView::InsertItem(void* parent, void* after, const char* text) {
    TVINSERTSTRUCTA insert = {};
    insert.hParent = static_cast<HTREEITEM>(parent);
    insert.hInsertAfter = static_cast<HTREEITEM>(after);

    if (!after) {
        insert.hInsertAfter = TVI_FIRST;
    } else if (after == reinterpret_cast<void*>(0xFFFF)) {
        insert.hInsertAfter = TVI_LAST;
    }

    insert.item.mask = TVIF_TEXT;
    insert.item.pszText = const_cast<char*>(text);

    HTREEITEM item = reinterpret_cast<HTREEITEM>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&insert)));
    this->InitParams(item);
    return item;
}

// 0.5.3 0x00456F90
void COsTreeView::SetItemText(void* item, const char* text) {
    TVITEMA info = {};
    info.mask = TVIF_TEXT;
    info.hItem = static_cast<HTREEITEM>(item);
    info.pszText = const_cast<char*>(text);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x00456FD0
void COsTreeView::GetItemText(void* item, char* buf, int32_t bufSize) {
    TVITEMA info = {};
    info.mask = TVIF_TEXT;
    info.hItem = static_cast<HTREEITEM>(item);
    info.pszText = buf;
    info.cchTextMax = bufSize;
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x00457010
void COsTreeView::SetItemParam(void* item, void* param) {
    this->GetParams(item)->user = param;
}

// 0.5.3 0x00457030
void* COsTreeView::GetItemParam(void* item) {
    return this->GetParams(item)->user;
}

// 0.5.3 0x00457050: drawn through NM_CUSTOMDRAW (OnNotify) while the item is not selected.
void COsTreeView::SetItemColor(void* item, const CImVector& color) {
    OsGuiTreeItemParams* params = this->GetParams(item);

    CImVector newColor = color;
    newColor.a = 0xFF;

    if (sColorValue(params->color) != sColorValue(newColor)) {
        params->color = newColor;
        this->RefreshItem(item);
    }
}

// 0.5.3 0x00457090
void COsTreeView::ResetItemColor(void* item) {
    OsGuiTreeItemParams* params = this->GetParams(item);

    if (params->color.a) {
        params->color.a = 0;
        this->RefreshItem(item);
    }
}

// 0.5.3 0x004570C0
CImVector COsTreeView::GetItemColor(void* item) {
    return this->GetParams(item)->color;
}

// 0.5.3 0x004570E0
void* COsTreeView::GetItemParent(void* item) {
    return reinterpret_cast<void*>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETNEXTITEM, TVGN_PARENT, reinterpret_cast<LPARAM>(item)));
}

// 0.5.3 0x00457100
int32_t COsTreeView::GetItemNumChildren(void* item) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    int32_t count = 0;

    for (LRESULT child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(item)); child;
         child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_NEXT, child)) {
        count++;
    }

    return count;
}

// 0.5.3 0x00457150
void* COsTreeView::GetItemChild(void* item, int32_t index) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    LRESULT child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(item));

    for (int32_t i = 0; child && i != index; i++) {
        child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_NEXT, child);
    }

    return reinterpret_cast<void*>(child);
}

// 0.5.3 0x004571A0: depth first, the parent before its children.
void COsTreeView::EnumerateItems(void* parent, OsGuiTVEnumFunc func, void* param) {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    func(this, parent, param);

    for (LRESULT child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_CHILD, reinterpret_cast<LPARAM>(parent)); child;
         child = SendMessageA(hwnd, TVM_GETNEXTITEM, TVGN_NEXT, child)) {
        this->EnumerateItems(reinterpret_cast<void*>(child), func, param);
    }
}

// 0.5.3 0x00457200: only the first root item and its subtree, as in 0.5.3.
void COsTreeView::EnumerateAllItems(OsGuiTVEnumFunc func, void* param) {
    LRESULT root = SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETNEXTITEM, TVGN_ROOT, 0);

    if (root) {
        this->EnumerateItems(reinterpret_cast<void*>(root), func, param);
    }
}

// 0.5.3 0x00457240: 16x16 BGRA; item null sets the default image (slot 0).
void COsTreeView::SetItemImage(void* item, int32_t width, int32_t height, void* data) {
    if (width != 16) {
        SErrDisplayAppFatal("inWidth == 16");
    }

    if (height != 16) {
        SErrDisplayAppFatal("inHeight == 16");
    }

    HWND hwnd = static_cast<HWND>(this->m_handle);
    HIMAGELIST images = static_cast<HIMAGELIST>(this->m_images);
    HDC dc = GetDC(hwnd);
    HBITMAP bitmap = sBitmapFromImageData(16, 16, data, dc);
    HBITMAP mask = sMaskFromImageData(16, 16, data, dc);
    int32_t imageCount = ImageList_GetImageCount(images);

    if (!item) {
        if (imageCount == 0) {
            ImageList_Add(images, bitmap, mask);
        } else {
            ImageList_Replace(images, 0, bitmap, mask);
            this->Refresh(1);
        }
    } else {
        TVITEMA info = {};
        info.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        info.hItem = static_cast<HTREEITEM>(item);
        SendMessageA(hwnd, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&info));

        if (info.iImage > 0 && info.iImage < imageCount) {
            // The item has its own image already: replace it in place.
            ImageList_Replace(images, info.iImage, bitmap, mask);
            this->Refresh(1);
        } else {
            int32_t image;
            uint32_t unused = this->m_unusedImageIDs.Count();

            if (unused) {
                image = this->m_unusedImageIDs[unused - 1];
                this->m_unusedImageIDs.SetCount(unused - 1);
                ImageList_Replace(images, image, bitmap, mask);
            } else {
                image = ImageList_Add(images, bitmap, mask);
            }

            if (image != -1) {
                info.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                info.hItem = static_cast<HTREEITEM>(item);
                info.iImage = image;
                info.iSelectedImage = image;
                SendMessageA(hwnd, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&info));
            }
        }
    }

    DeleteObject(bitmap);
    DeleteObject(mask);
    ReleaseDC(hwnd, dc);
}

// 0.5.3 0x00457600
void COsTreeView::ExpandItem(void* item, int32_t expand) {
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_EXPAND, expand ? TVE_EXPAND : TVE_COLLAPSE, reinterpret_cast<LPARAM>(item));
}

// 0.5.3 0x00457630
int32_t COsTreeView::IsItemExpanded(void* item) {
    return (SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMSTATE, reinterpret_cast<WPARAM>(item), TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
}

// 0.5.3 0x00457660
void COsTreeView::OnSizeChange() {
}

// 0.5.3 0x00457670
void COsTreeView::OnExpandedItem(void* item) {
    if (this->m_expandFunc) {
        this->m_expandFunc(item, this->m_expandParam);
    }
}

// 0.5.3 0x00457690
void COsTreeView::EnsureItemVisible(void* item) {
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_ENSUREVISIBLE, 0, reinterpret_cast<LPARAM>(item));
}

// 0.5.3 0x004576B0
void* COsTreeView::GetSelectedItem() {
    if (!(this->m_flags & 0x40000)) {
        return reinterpret_cast<void*>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETNEXTITEM, TVGN_CARET, 0));
    }

    OsGuiTVSelectionInfo info;
    this->GetSelectionInfo(&info);
    return info.firstSelection;
}

// 0.5.3 0x004576F0
void COsTreeView::SelectItem(void* item, int32_t select) {
    if (this->m_flags & 0x40000) {
        TVITEMA info = {};
        info.mask = TVIF_STATE;
        info.hItem = static_cast<HTREEITEM>(item);
        info.state = select ? TVIS_SELECTED : 0;
        info.stateMask = TVIS_SELECTED;
        SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&info));
        this->SendEvent(2, 0);
    } else {
        SendMessageA(static_cast<HWND>(this->m_handle), TVM_SELECTITEM, TVGN_CARET, select ? reinterpret_cast<LPARAM>(item) : 0);
    }
}

// 0.5.3 0x00457770
int32_t COsTreeView::IsItemSelected(void* item) {
    if (this->m_flags & 0x40000) {
        return (SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMSTATE, reinterpret_cast<WPARAM>(item), TVIS_SELECTED) & TVIS_SELECTED) != 0;
    }

    return item == this->GetSelectedItem();
}

// 0.5.3 0x004577B0
void COsTreeView::GetSelectionInfo(OsGuiTVSelectionInfo* info) {
    OsGuiTVSIResults results = {};
    this->EnumerateAllItems(sTVGetSelectInfo, &results);
    *info = results.info;
}

// 0.5.3 0x00457880
void COsTreeView::SelectAll(int32_t select) {
    this->EnumerateAllItems(sTVSelect, &select);
}

// 0.5.3 0x004578C0
void COsTreeView::EditItem(void* item) {
    this->SetInputFocus();
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_EDITLABELA, 0, reinterpret_cast<LPARAM>(item));
}

// 0.5.3 0x004578F0: with multiple selection, the click is taken over (selection happens on OnMouseUp).
int32_t COsTreeView::OnMouseDown() {
    if (!(this->m_flags & 0x40000)) {
        return 0;
    }

    void* item = this->FindItemUnderCursor();
    this->SetInputFocus();
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SELECTITEM, TVGN_CARET, 0);
    return item != nullptr;
}

// 0.5.3 0x00457930: ends a drag, or applies the click: control toggles the item, a plain click selects only it
// (shift does nothing; 0.5.3 had no range selection).
int32_t COsTreeView::OnMouseUp() {
    if (this->m_dragging) {
        this->OnEndDrag();
        this->m_dragging = 0;
        return 0;
    }

    if (!(this->m_flags & 0x40000)) {
        return 0;
    }

    void* item = this->FindItemUnderCursor();
    int32_t shift = OsGuiIsModifierKeyDown(OSGUI_KEY_SHIFT);
    int32_t control = OsGuiIsModifierKeyDown(OSGUI_KEY_CONTROL);
    int32_t selected = this->IsItemSelected(item);

    if (shift) {
        return 0;
    }

    if (control) {
        this->SelectItem(item, !selected);
    } else {
        this->SelectAll(0);
        this->SelectItem(item, 1);
    }

    return 1;
}

// 0.5.3 0x004579E0: the item whose icon or label is under the cursor.
void* COsTreeView::FindItemUnderCursor() {
    int32_t cursorX = 0;
    int32_t cursorY = 0;
    OsGuiGetCursorPosition(&cursorX, &cursorY);

    int32_t controlX = 0;
    int32_t controlY = 0;
    this->GetPosition(&controlX, &controlY, 0);

    TVHITTESTINFO hitTest = {};
    hitTest.pt.x = cursorX - controlX;
    hitTest.pt.y = cursorY - controlY;
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hitTest));

    return (hitTest.flags & (TVHT_ONITEMICON | TVHT_ONITEMLABEL | TVHT_ONITEMSTATEICON)) ? hitTest.hItem : nullptr;
}

// 0.5.3 0x00457A60: Enter commits a label edit, otherwise sends event 9.
int32_t COsTreeView::OnReturn() {
    if (!this->GetEditControl()) {
        return this->SendEvent(9, 0);
    }

    SendMessageA(static_cast<HWND>(this->m_handle), TVM_ENDEDITLABELNOW, 0, 0);
    return 1;
}

// 0.5.3 0x00457AA0: Escape cancels a label edit.
int32_t COsTreeView::OnEscape() {
    if (!this->GetEditControl()) {
        return 0;
    }

    SendMessageA(static_cast<HWND>(this->m_handle), TVM_ENDEDITLABELNOW, 1, 0);
    return 1;
}

// 0.5.3 0x00457AD0
int32_t COsTreeView::OnNotify(int32_t code, void* data) {
    switch (static_cast<UINT>(code)) {
        case TVN_KEYDOWN: {
            switch (static_cast<NMTVKEYDOWN*>(data)->wVKey) {
                case VK_TAB: return this->SendEvent(11, 0);
                case VK_RETURN: return this->OnReturn();
                case VK_ESCAPE: return this->OnEscape();
                case VK_DELETE: return this->SendEvent(10, 0);
                default: break;
            }

            break;
        }

        case TVN_BEGINLABELEDITA:
        case TVN_ENDLABELEDITA: {
            NMTVDISPINFOA* info = static_cast<NMTVDISPINFOA*>(data);
            int32_t accepted = static_cast<UINT>(code) == TVN_ENDLABELEDITA ? this->OnEndEdit(info->item.hItem, info->item.pszText)
                                                         : this->OnBeginEdit(info->item.hItem);

            if (accepted) {
                break;
            }

            if (static_cast<UINT>(code) == TVN_BEGINLABELEDITA) {
                this->OnEscape();
                this->SetInputFocus();
                return 1;
            }

            return 0;
        }

        case TVN_DELETEITEMA:
            this->OnDeleteItem(static_cast<NMTREEVIEWA*>(data)->itemOld.hItem);
            break;

        case TVN_BEGINDRAGA: {
            NMTREEVIEWA* info = static_cast<NMTREEVIEWA*>(data);
            this->OnBeginDrag(info->itemNew.hItem, info->ptDrag.x, info->ptDrag.y);
            return 1;
        }

        case TVN_ITEMEXPANDEDA:
            this->OnExpandedItem(static_cast<NMTREEVIEWA*>(data)->itemNew.hItem);
            return 1;

        case TVN_SELCHANGINGA:
            // Multiple selection keeps the native single selection out of the way.
            if (this->m_flags & 0x40000) {
                return 1;
            }

            break;

        case NM_CUSTOMDRAW: {
            NMTVCUSTOMDRAW* draw = static_cast<NMTVCUSTOMDRAW*>(data);

            if (draw->nmcd.dwDrawStage == CDDS_PREPAINT) {
                if (this->m_dialog) {
                    SetWindowLongPtrA(static_cast<HWND>(this->m_dialog->m_handle), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
                }

                return CDRF_NOTIFYITEMDRAW;
            }

            if (draw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                void* item = reinterpret_cast<void*>(draw->nmcd.dwItemSpec);

                if (!this->IsItemSelected(item)) {
                    CImVector color = this->GetParams(item)->color;

                    if (color.a) {
                        draw->clrText = sColorRef(color);
                    }
                }
            }

            break;
        }

        default:
            break;
    }

    return COsControl::OnNotify(code, data);
}

// 0.5.3 0x00457D20: the label edit box belongs to the tree view too.
int32_t COsTreeView::IsHandleFromControl(void* handle) {
    void* editControl = this->GetEditControl();
    return (editControl && handle == editControl) || this->m_handle == handle;
}

// 0.5.3 0x00457D50
void COsTreeView::EnableDragDrop(int32_t enable) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    LONG_PTR style = GetWindowLongPtrA(hwnd, GWL_STYLE);

    if (enable) {
        style &= ~static_cast<LONG_PTR>(TVS_DISABLEDRAGDROP);
    } else {
        style |= TVS_DISABLEDRAGDROP;
    }

    SetWindowLongPtrA(hwnd, GWL_STYLE, style);
}

// 0.5.3 0x00457DA0: the handler gets each step (action 0 begin: return nonzero to allow the drag, 1 move, 2 drop).
void COsTreeView::SetDragDropHandler(OsGuiTVDragDropFunc func, void* param) {
    this->m_dragHandler = func;
    this->m_dragHandlerParam = param;
}

// 0.5.3 0x00457DC0
void COsTreeView::CreateDragImage(void* item) {
    this->DestroyDragImage();
    this->m_dragImage = reinterpret_cast<void*>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_CREATEDRAGIMAGE, 0, reinterpret_cast<LPARAM>(item)));
}

// 0.5.3 0x00457DF0
void COsTreeView::DestroyDragImage() {
    if (this->m_dragImage) {
        ImageList_Destroy(static_cast<HIMAGELIST>(this->m_dragImage));
        this->m_dragImage = nullptr;
    }
}

// 0.5.3 0x00457E10: the label rectangle, in the tree view's client coordinates.
CiRect COsTreeView::GetItemRect(void* item) {
    RECT rect = {};
    *reinterpret_cast<HTREEITEM*>(&rect) = static_cast<HTREEITEM>(item);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMRECT, 1, reinterpret_cast<LPARAM>(&rect));

    CiRect itemRect = {};
    sWinRectToCiRect(rect, itemRect);
    return itemRect;
}

// 0.5.3 0x00457E70
void COsTreeView::RefreshItem(void* item) {
    RECT rect = {};
    *reinterpret_cast<HTREEITEM*>(&rect) = static_cast<HTREEITEM>(item);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMRECT, 1, reinterpret_cast<LPARAM>(&rect));
    InvalidateRect(static_cast<HWND>(this->m_handle), &rect, FALSE);
}

// 0.5.3 0x00457EB0
void* COsTreeView::GetFirstVisibleItem() {
    return reinterpret_cast<void*>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0));
}

// 0.5.3 0x00457ED0: scrolls to the last visible item first, so that the item ends up at the top.
void COsTreeView::SetFirstVisibleItem(void* item) {
    this->SetRedraw(0);
    void* last = reinterpret_cast<void*>(SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETNEXTITEM, TVGN_LASTVISIBLE, 0));
    this->EnsureItemVisible(last);
    this->EnsureItemVisible(item);
    this->SetRedraw(1);
}

// 0.5.3 0x00457F20
int32_t COsTreeView::RunDragHandler() {
    if (!this->m_dragHandler) {
        return 0;
    }

    return this->m_dragHandler(this->m_dragInfo, this->m_dragHandlerParam);
}

// 0.5.3 0x00457F40: x, y in the tree view's client coordinates. The drag image is shown over the whole dialog,
// which captures the mouse (COsDialog::OnMouseMove/OnMouseUp forward the drag back here).
void COsTreeView::OnBeginDrag(void* item, int32_t x, int32_t y) {
    this->SetInputFocus();
    this->SelectItem(item, 1);

    this->m_dragInfo.action = 0;
    this->m_dragInfo.dragItem = item;
    this->m_dragInfo.targItem = item;
    this->m_dragInfo.targX = x;
    this->m_dragInfo.targY = y;

    if (!this->RunDragHandler()) {
        return;
    }

    this->CreateDragImage(item);

    int32_t controlX;
    int32_t controlY;
    this->GetPosition(&controlX, &controlY, 0);

    int32_t screenX = controlX + x;
    int32_t screenY = controlY + y;

    CiRect itemRect = this->GetItemRect(item);
    int32_t hotX = x - itemRect.minX + 16; // 16: the item image left of the label
    int32_t hotY = y - itemRect.minY;

    int32_t dialogX;
    int32_t dialogY;
    this->m_dialog->GetPosition(&dialogX, &dialogY, 0);

    HWND dialogWindow = static_cast<HWND>(this->m_dialog->m_handle);
    ImageList_BeginDrag(static_cast<HIMAGELIST>(this->m_dragImage), 0, hotX, hotY);
    ImageList_DragEnter(dialogWindow, screenX - dialogX, screenY - dialogY);
    SetCapture(dialogWindow);

    this->m_dragInfo.dragItem = item;
    this->m_dragging = 1;
}

// 0.5.3 0x00458040: x, y in the dialog's client coordinates (from COsDialog::OnMouseMove).
void COsTreeView::OnMouseMove(int32_t x, int32_t y) {
    if (!this->m_dragging) {
        return;
    }

    int32_t clientX;
    int32_t clientY;
    this->m_dialog->GetPosition(&clientX, &clientY, 1);

    int32_t windowX;
    int32_t windowY;
    this->m_dialog->GetPosition(&windowX, &windowY, 0);

    int32_t controlX;
    int32_t controlY;
    this->GetPosition(&controlX, &controlY, 0);

    int32_t screenX = clientX + x;
    int32_t screenY = clientY + y;
    ImageList_DragMove(screenX - windowX, screenY - windowY);

    TVHITTESTINFO hitTest = {};
    hitTest.pt.x = screenX - controlX;
    hitTest.pt.y = screenY - controlY;
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hitTest));

    this->m_dragInfo.targItem = hitTest.hItem;
    this->m_dragInfo.targX = 0;
    this->m_dragInfo.targY = 0;

    if (hitTest.hItem) {
        // Position inside the target item's label, for above/below/onto decisions.
        CiRect itemRect = this->GetItemRect(hitTest.hItem);
        this->m_dragInfo.targX = screenX - itemRect.minX - controlX;
        this->m_dragInfo.targY = screenY - itemRect.minY - controlY;
    }

    this->m_dragInfo.action = 1;
    this->RunDragHandler();
}

// 0.5.3 0x00458130: highlights the drop target (null clears it).
void COsTreeView::SetDropTarget(void* item) {
    ImageList_DragShowNolock(FALSE);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SELECTITEM, TVGN_DROPHILITE, reinterpret_cast<LPARAM>(item));
    ImageList_DragShowNolock(TRUE);
}

// 0.5.3 0x00458160: draws the insertion line before or after an item (null clears it).
void COsTreeView::SetInsertionMark(void* item, int32_t after) {
    ImageList_DragShowNolock(FALSE);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETINSERTMARK, after, reinterpret_cast<LPARAM>(item));
    ImageList_DragShowNolock(TRUE);
}

// 0.5.3 0x004581A0
void COsTreeView::OnEndDrag() {
    ImageList_EndDrag();
    ImageList_DragLeave(static_cast<HWND>(this->m_dialog->m_handle));
    this->DestroyDragImage();
    ReleaseCapture();
    this->SetDropTarget(nullptr);
    this->SetInsertionMark(nullptr, 0);
    this->SelectItem(this->m_dragInfo.dragItem, 1);

    this->m_dragInfo.action = 2;
    this->RunDragHandler();
}

// 0.5.3 0x00458200
int32_t COsTreeView::OnBeginEdit(void* item) {
    if (this->m_canEditFunc && !this->m_canEditFunc(item, this->m_canEditParam)) {
        return 0;
    }

    if (this->m_textLimit != -1) {
        HWND editControl = static_cast<HWND>(this->GetEditControl());

        if (editControl) {
            SendMessageA(editControl, EM_LIMITTEXT, this->m_textLimit, 0);
        }
    }

    return 1;
}

// 0.5.3 0x00458250: an empty (or cancelled) edit keeps the old text.
int32_t COsTreeView::OnEndEdit(void* item, const char* newText) {
    if (!newText || !*newText) {
        return 0;
    }

    this->SetItemText(item, newText);
    return 1;
}

// 0.5.3 0x00458280: the label edit length limit (-1 none).
void COsTreeView::SetTextLimit(int32_t limit) {
    this->m_textLimit = limit;
}

// 0.5.3 0x00458290
void COsTreeView::EnableFilters(int32_t enable) {
    this->m_filtersEnabled = enable;
}

// 0.5.3 0x004582A0: OSGUI_FILTER_* bits of the characters a label edit accepts.
void COsTreeView::SetFilter(uint32_t filter, int32_t enable) {
    if (enable) {
        this->m_filters |= filter;
    } else {
        this->m_filters &= ~filter;
    }
}

// 0.5.3 0x00458320
void COsTreeView::SetCanEditFunction(OsGuiTVCanEditFunc func, void* param) {
    this->m_canEditFunc = func;
    this->m_canEditParam = param;
}

// 0.5.3 0x00458340
void COsTreeView::SetExpandFunction(OsGuiTVExpandFunc func, void* param) {
    this->m_expandFunc = func;
    this->m_expandParam = param;
}

// 0.5.3 0x00458360: frees the item's image slot and params entry.
void COsTreeView::OnDeleteItem(void* item) {
    TVITEMA info = {};
    info.mask = TVIF_IMAGE;
    info.hItem = static_cast<HTREEITEM>(item);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&info));

    int32_t imageCount = ImageList_GetImageCount(static_cast<HIMAGELIST>(this->m_images));

    if (info.iImage > 0 && info.iImage < imageCount) {
        *this->m_unusedImageIDs.New() = info.iImage;
    }

    this->GetParams(item)->used = 0;
}

// 0.5.3 0x004584D0
int32_t COsTreeView::FindUnusedParams() {
    for (uint32_t i = 0; i < this->m_itemParams.Count(); i++) {
        if (!this->m_itemParams[i].used) {
            return i;
        }
    }

    return -1;
}

// 0.5.3 0x00458550
void COsTreeView::InitParams(void* item) {
    int32_t id = this->FindUnusedParams();

    if (id == -1) {
        OsGuiTreeItemParams* params = this->m_itemParams.New();
        params->color = CImVector(0, 0, 0, 0);
        id = this->m_itemParams.Count() - 1;
    }

    OsGuiTreeItemParams& params = this->m_itemParams[id];
    params.used = 1;
    params.color = CImVector(0, 0, 0, 0);
    params.user = nullptr;

    TVITEMA info = {};
    info.mask = TVIF_PARAM;
    info.hItem = static_cast<HTREEITEM>(item);
    info.lParam = id;
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x004586B0
OsGuiTreeItemParams* COsTreeView::GetParams(void* item) {
    TVITEMA info = {};
    info.mask = TVIF_PARAM;
    info.hItem = static_cast<HTREEITEM>(item);
    SendMessageA(static_cast<HWND>(this->m_handle), TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&info));

    int32_t id = static_cast<int32_t>(info.lParam);

    if (id < 0 || id >= static_cast<int32_t>(this->m_itemParams.Count())) {
        SErrDisplayAppFatal("paramID >= 0 && paramID < mItemParams.Count()");
    }

    return &this->m_itemParams[id];
}

// ===== COsTextButton =====

// 0.5.3 0x00458770: an owner-drawn hyperlink-style button (black, white when pushed, grey when disabled).
COsTextButton::COsTextButton(COsDialog* dialog, int16_t id) : COsControl(dialog, TYPE_TEXT_BUTTON, id, 0) {
    this->m_activeColor = CImVector(0x00, 0x00, 0x00, 0xFF);
    this->m_pushedColor = CImVector(0xFF, 0xFF, 0xFF, 0xFF);
    this->m_greyedColor = CImVector(0x80, 0x80, 0x80, 0xFF);
    this->m_underline = 1;
}

// 0.5.3 0x00458800
int32_t COsTextButton::OnDraw(void* dc, uint32_t state, CiRect& rect) {
    HDC hdc = static_cast<HDC>(dc);

    CImVector color = this->m_activeColor;

    if (state & 1) {
        color = this->m_greyedColor;
    } else if (state & 4) {
        color = this->m_pushedColor;
    }

    COLORREF colorRef = sColorRef(color);
    HPEN pen = CreatePen(PS_SOLID, 1, colorRef);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    COLORREF oldColor = ::SetTextColor(hdc, colorRef);

    RECT drawRect;
    sCiRectToWinRect(rect, drawRect);

    HBRUSH background = CreateSolidBrush(GetBkColor(hdc));
    FillRect(hdc, &drawRect, background);

    char text[260];
    this->GetText(text, sizeof(text));
    int32_t length = static_cast<int32_t>(SStrLen(text));
    DrawTextA(hdc, text, length, &drawRect, DT_NOCLIP);

    if (this->m_underline) {
        SIZE textSize;
        GetTextExtentPoint32A(hdc, text, length, &textSize);
        MoveToEx(hdc, drawRect.left, drawRect.top + textSize.cy, nullptr);
        LineTo(hdc, drawRect.left + textSize.cx, drawRect.top + textSize.cy);
    }

    SelectObject(hdc, oldPen);
    ::SetTextColor(hdc, oldColor);
    DeleteObject(pen);
    DeleteObject(background);

    return 1;
}

// ===== COsStaticBox =====

// 0.5.3 0x00458960
void COsStaticBox::ClearTransparentRects() {
    this->m_transRects.SetCount(0);
    this->Refresh(1);
}

// 0.5.3 0x00458970: a hole in the filled box (rect in the box's client coordinates), e.g. for a 3D view under it.
void COsStaticBox::AddTransparentRect(const CiRect& rect) {
    *this->m_transRects.New() = rect;
    this->Refresh(1);
}

// 0.5.3 0x00458A40: fills the box minus its transparent rectangles with the background color.
int32_t COsStaticBox::OnDraw(void* dc, uint32_t state, CiRect& rect) {
    HDC hdc = static_cast<HDC>(dc);

    RECT drawRect;
    sCiRectToWinRect(rect, drawRect);
    HRGN region = CreateRectRgnIndirect(&drawRect);

    for (uint32_t i = 0; i < this->m_transRects.Count(); i++) {
        RECT transRect;
        sCiRectToWinRect(this->m_transRects[i], transRect);
        HRGN hole = CreateRectRgnIndirect(&transRect);
        CombineRgn(region, region, hole, RGN_DIFF);
        DeleteObject(hole);
    }

    int32_t oldBkMode = SetBkMode(hdc, TRANSPARENT);
    HBRUSH brush = CreateSolidBrush(GetBkColor(hdc));
    FillRgn(hdc, region, brush);
    SetBkMode(hdc, oldBkMode);
    DeleteObject(brush);
    DeleteObject(region);

    return 1;
}

// ===== COsSpinButton =====

// 0.5.3 0x00458B40
COsSpinButton::COsSpinButton(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_UP_DOWN, id, flags) {
    this->Initialize();
}

// 0.5.3 0x00458BA0
COsSpinButton::COsSpinButton(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_UP_DOWN, id, flags) {
    this->Initialize();
}

// 0.5.3 0x00458BD0
void COsSpinButton::Initialize() {
    SetWindowLongPtrA(static_cast<HWND>(this->m_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(sSpinButtonProc));
}

// 0.5.3 0x00458C40: event 5 when the button is released (the value stops changing).
void COsSpinButton::OnSpinMouseUp() {
    this->SendEvent(5, 0);
}

// 0.5.3 0x00458C50
void COsSpinButton::SetValueRange(int32_t minValue, int32_t maxValue) {
    SendMessageA(static_cast<HWND>(this->m_handle), UDM_SETRANGE32, minValue, maxValue);
}

// 0.5.3 0x00458C70
void COsSpinButton::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), UDM_SETPOS32, 0, value);
}

// 0.5.3 0x00458C90
int32_t COsSpinButton::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), UDM_GETPOS32, 0, 0));
}

// ===== COsRadioButton =====

// 0.5.3 0x00458CB0. Flag 0x10000 starts a new group.
COsRadioButton::COsRadioButton(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_RADIO_BUTTON, id, flags) {
}

// 0.5.3 0x00458D10
void COsRadioButton::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), BM_SETCHECK, value != 0, 0);
}

// 0.5.3 0x00458D40
int32_t COsRadioButton::GetValue() {
    return SendMessageA(static_cast<HWND>(this->m_handle), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

// ===== COsTabControl =====

// 0.5.3 0x00458D60
void COsTabControl::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), TCM_SETCURSEL, value, 0);
}

// 0.5.3 0x00458D80
int32_t COsTabControl::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), TCM_GETCURSEL, 0, 0));
}

// 0.5.3 0x00458DA0: pos -1 appends.
void COsTabControl::InsertItem(const char* text, int32_t pos) {
    TCITEMA item = {};
    item.mask = TCIF_TEXT;
    item.pszText = const_cast<char*>(text);

    if (pos == -1) {
        pos = this->GetNumItems();
    }

    SendMessageA(static_cast<HWND>(this->m_handle), TCM_INSERTITEMA, pos, reinterpret_cast<LPARAM>(&item));
}

// ===== COsListView (report view) =====
//
// Each row's lParam holds its text color as a CImVector value (alpha 0: default), drawn through NM_CUSTOMDRAW.

// 0.5.3 0x00458E60. Flag 0x10000: multiple selection.
COsListView::COsListView(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_LIST_VIEW, id, flags) {
    HWND hwnd = static_cast<HWND>(this->m_handle);
    SendMessageA(hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    SendMessageA(hwnd, LVM_GETHEADER, 0, 0);
    sSetOsGuiPointer(this->m_handle, this);

    if (!(this->m_flags & 0x10000)) {
        // As in 0.5.3: clears style bit 2 (LVS_SMALLICON, not set by the table); LVS_SINGLESEL stays.
        SetWindowLongPtrA(hwnd, GWL_STYLE, GetWindowLongPtrA(hwnd, GWL_STYLE) & ~static_cast<LONG_PTR>(LVS_SMALLICON));
    }

    this->m_numCols = 0;
}

// 0.5.3 0x00458F10
COsListView::~COsListView() {
}

// 0.5.3 0x00458F20: pos -1 appends; new columns are 50 wide and untitled.
void COsListView::InsertColumn(int32_t pos) {
    if (pos == -1) {
        pos = this->GetNumColumns();
    }

    LVCOLUMNA column = {};
    column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    column.fmt = LVCFMT_LEFT;
    column.cx = 50;
    column.iSubItem = pos;
    column.pszText = const_cast<char*>("");
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_INSERTCOLUMNA, pos, reinterpret_cast<LPARAM>(&column));

    this->m_numCols++;
}

// 0.5.3 0x00458F80
void COsListView::DeleteColumn(int32_t pos) {
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_DELETECOLUMN, pos, 0);
    this->m_numCols--;
}

// 0.5.3 0x00458FB0
int32_t COsListView::GetNumColumns() {
    return this->m_numCols;
}

// 0.5.3 0x00458FC0: pos -1 appends.
void COsListView::InsertRow(int32_t pos) {
    if (pos == -1) {
        pos = this->GetNumRows();
    }

    LVITEMA item = {};
    item.mask = LVIF_PARAM;
    item.iItem = pos;
    item.iSubItem = 0;
    item.lParam = 0;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&item));
}

// 0.5.3 0x00459010
void COsListView::DeleteRow(int32_t pos) {
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_DELETEITEM, pos, 0);
}

// 0.5.3 0x00459030
void COsListView::ClearRows() {
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_DELETEALLITEMS, 0, 0);
}

// 0.5.3 0x00459050
int32_t COsListView::GetNumRows() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETITEMCOUNT, 0, 0));
}

// 0.5.3 0x00459070
void COsListView::SetRowColor(int32_t pos, const CImVector& color) {
    LVITEMA item = {};
    item.mask = LVIF_PARAM;
    item.iItem = pos;
    item.iSubItem = 0;
    item.lParam = sColorValue(color);
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&item));
}

// 0.5.3 0x004590B0
CImVector COsListView::GetRowColor(int32_t pos) {
    LVITEMA item = {};
    item.mask = LVIF_PARAM;
    item.iItem = pos;
    item.iSubItem = 0;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));
    return sColorFromValue(static_cast<uint32_t>(item.lParam));
}

// 0.5.3 0x004590F0
void COsListView::SetItemText(int32_t row, int32_t col, const char* text) {
    LVITEMA item = {};
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = col;
    item.pszText = const_cast<char*>(text);
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&item));
}

// 0.5.3 0x00459130
void COsListView::GetItemText(int32_t row, int32_t col, char* buf, int32_t bufSize) {
    LVITEMA item = {};
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = col;
    item.pszText = buf;
    item.cchTextMax = bufSize;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));
}

// 0.5.3 0x00459170
void COsListView::SetColumnWidth(int32_t col, int32_t width) {
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETCOLUMNWIDTH, col, MAKELPARAM(width, 0));
}

// 0.5.3 0x00459190
int32_t COsListView::GetColumnWidth(int32_t col) {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETCOLUMNWIDTH, col, 0));
}

// 0.5.3 0x004591B0
void COsListView::SetColumnTitle(int32_t col, const char* text) {
    LVCOLUMNA column = {};
    column.mask = LVCF_TEXT;
    column.pszText = const_cast<char*>(text);
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETCOLUMNA, col, reinterpret_cast<LPARAM>(&column));
}

// 0.5.3 0x004591E0
void COsListView::GetColumnTitle(int32_t col, char* buf, int32_t bufSize) {
    LVCOLUMNA column = {};
    column.mask = LVCF_TEXT;
    column.pszText = buf;
    column.cchTextMax = bufSize;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETCOLUMNA, col, reinterpret_cast<LPARAM>(&column));
}

// 0.5.3 0x00459220: 0 left, 1 center, 2 right (as COsStaticText::SetJustification).
void COsListView::SetColumnJustification(int32_t col, int32_t justification) {
    int32_t format;

    switch (justification) {
        case 0: format = LVCFMT_LEFT; break;
        case 1: format = LVCFMT_CENTER; break;
        case 2: format = LVCFMT_RIGHT; break;
        default: return;
    }

    LVCOLUMNA column = {};
    column.mask = LVCF_FMT;
    column.fmt = format;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETCOLUMNA, col, reinterpret_cast<LPARAM>(&column));
}

// 0.5.3 0x00459270
void COsListView::EnsureRowVisible(int32_t row) {
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_ENSUREVISIBLE, row, 0);
}

// 0.5.3 0x00459290: selects and focuses the row; an out of range value clears the selection.
void COsListView::SetValue(int32_t value) {
    if (value < 0 || value >= this->GetNumRows()) {
        value = -1;
    }

    LVITEMA item = {};
    item.mask = LVIF_STATE;
    item.iItem = value;
    item.iSubItem = 0;
    item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    item.state = value == -1 ? 0 : LVIS_SELECTED | LVIS_FOCUSED;
    SendMessageA(static_cast<HWND>(this->m_handle), LVM_SETITEMSTATE, value, reinterpret_cast<LPARAM>(&item));
}

// 0.5.3 0x004592F0: the first selected row, or -1.
int32_t COsListView::GetValue() {
    int32_t rows = this->GetNumRows();

    for (int32_t i = 0; i < rows; i++) {
        if (SendMessageA(static_cast<HWND>(this->m_handle), LVM_GETITEMSTATE, i, LVIS_SELECTED)) {
            return i;
        }
    }

    return -1;
}

// 0.5.3 0x00459340
void COsListView::OnSizeChange() {
}

// 0.5.3 0x00459350
void COsListView::OnSelectionChange() {
    this->SendEvent(2, 0);
}

// 0.5.3 0x00459360: event 15 with the column as code (e.g. to sort by it).
void COsListView::OnColumnClick(int32_t col) {
    this->SendEvent(15, col);
}

// 0.5.3 0x00459380
int32_t COsListView::OnNotify(int32_t code, void* data) {
    switch (static_cast<UINT>(code)) {
        case LVN_COLUMNCLICK:
            this->OnColumnClick(static_cast<NMLISTVIEW*>(data)->iSubItem);
            return 1;

        case HDN_ITEMCHANGEDA: // a column was resized
            return this->SendEvent(16, 0);

        case LVN_KEYDOWN: {
            WORD key = static_cast<NMLVKEYDOWN*>(data)->wVKey;

            if (key == VK_RETURN) {
                return this->OnReturn();
            }

            if (key == VK_DELETE) {
                return this->SendEvent(10, 0);
            }

            break;
        }

        case LVN_ITEMCHANGED: {
            NMLISTVIEW* info = static_cast<NMLISTVIEW*>(data);

            if ((info->uChanged & LVIF_STATE) && ((info->uNewState ^ info->uOldState) & LVIS_SELECTED)) {
                this->OnSelectionChange();
                return 1;
            }

            break;
        }

        case NM_CUSTOMDRAW: {
            NMLVCUSTOMDRAW* draw = static_cast<NMLVCUSTOMDRAW*>(data);

            if (draw->nmcd.dwDrawStage == CDDS_PREPAINT) {
                if (this->m_dialog) {
                    SetWindowLongPtrA(static_cast<HWND>(this->m_dialog->m_handle), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
                }

                return CDRF_NOTIFYITEMDRAW;
            }

            if (draw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                CImVector color = this->GetRowColor(static_cast<int32_t>(draw->nmcd.dwItemSpec));

                if (color.a) {
                    draw->clrText = sColorRef(color);
                }
            }

            break;
        }

        default:
            break;
    }

    return COsControl::OnNotify(code, data);
}

// 0.5.3 0x004594A0
int32_t COsListView::OnReturn() {
    return this->SendEvent(9, 0);
}

// ===== COsToolBar =====
//
// Button command ids are their insertion positions; a click sends event 0 with the id as code (OnCommand).

// 0.5.3 0x004594B0
COsToolBar::COsToolBar(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_TOOLBAR, id, flags) {
    this->InitializeToolBar();
}

// 0.5.3 0x00459510
COsToolBar::COsToolBar(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_TOOLBAR, id, flags) {
    this->InitializeToolBar();
}

// 0.5.3 0x00459540
COsToolBar::~COsToolBar() {
    ImageList_Destroy(static_cast<HIMAGELIST>(this->m_imageList));
}

// 0.5.3 0x00459560
void COsToolBar::InitializeToolBar() {
    SendMessageA(static_cast<HWND>(this->m_handle), TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);
    this->m_imageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
    SendMessageA(static_cast<HWND>(this->m_handle), TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(this->m_imageList));
}

// 0.5.3 0x004595A0
void COsToolBar::SetButtonSize(int32_t width, int32_t height) {
    SendMessageA(static_cast<HWND>(this->m_handle), TB_SETBUTTONSIZE, 0, MAKELPARAM(width, height));
}

// 0.5.3 0x004595D0
void COsToolBar::GetButtonSize(int32_t* width, int32_t* height) {
    LRESULT size = SendMessageA(static_cast<HWND>(this->m_handle), TB_GETBUTTONSIZE, 0, 0);
    *width = LOWORD(size);
    *height = HIWORD(size);
}

// 0.5.3 0x00459600
void COsToolBar::Clear() {
    for (int32_t i = this->GetNumButtons(); i >= 0; i--) {
        this->RemoveButton(i);
    }
}

// 0.5.3 0x00459620: pos -1 appends.
void COsToolBar::AddButton(int32_t pos) {
    if (pos == -1) {
        pos = this->GetNumButtons();
    }

    TBBUTTON button = {};
    button.iBitmap = I_IMAGENONE;
    button.idCommand = pos;
    button.fsState = TBSTATE_ENABLED;
    button.fsStyle = BTNS_BUTTON;
    button.dwData = 0;
    button.iString = 0;
    SendMessageA(static_cast<HWND>(this->m_handle), TB_INSERTBUTTONA, pos, reinterpret_cast<LPARAM>(&button));
}

// 0.5.3 0x00459670
void COsToolBar::AddSeparator(int32_t pos) {
    if (pos == -1) {
        pos = this->GetNumButtons();
    }

    TBBUTTON button = {};
    button.iBitmap = I_IMAGENONE;
    button.idCommand = pos;
    button.fsState = 0;
    button.fsStyle = BTNS_SEP;
    button.dwData = 0;
    button.iString = -1;
    SendMessageA(static_cast<HWND>(this->m_handle), TB_INSERTBUTTONA, pos, reinterpret_cast<LPARAM>(&button));
}

// 0.5.3 0x004596C0
void COsToolBar::RemoveButton(int32_t pos) {
    SendMessageA(static_cast<HWND>(this->m_handle), TB_DELETEBUTTON, pos, 0);
}

// 0.5.3 0x004596E0
int32_t COsToolBar::GetNumButtons() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), TB_BUTTONCOUNT, 0, 0));
}

// 0.5.3 0x00459700: BGRA pixels; the first image of a button is added to the list, later ones replace it.
void COsToolBar::SetButtonImage(int32_t pos, int32_t width, int32_t height, void* data) {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    TBBUTTONINFOA info = {};
    info.cbSize = sizeof(info);
    info.dwMask = TBIF_BYINDEX | TBIF_IMAGE;
    SendMessageA(hwnd, TB_GETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));

    HDC dc = GetDC(hwnd);
    HBITMAP bitmap = sBitmapFromImageData(width, height, data, dc);
    HBITMAP mask = sMaskFromImageData(width, height, data, dc);

    if (info.iImage == I_IMAGENONE) {
        int32_t image = ImageList_Add(static_cast<HIMAGELIST>(this->m_imageList), bitmap, mask);

        if (image != -1) {
            info.iImage = image;
            SendMessageA(hwnd, TB_SETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));
        }
    } else {
        ImageList_Replace(static_cast<HIMAGELIST>(this->m_imageList), info.iImage, bitmap, mask);
        this->Refresh(1);
    }

    DeleteObject(bitmap);
    DeleteObject(mask);
    ReleaseDC(hwnd, dc);
}

// 0.5.3 0x004597D0
void COsToolBar::SetButtonText(int32_t pos, const char* text) {
    TBBUTTONINFOA info = {};
    info.cbSize = sizeof(info);
    info.dwMask = TBIF_BYINDEX | TBIF_TEXT;
    info.pszText = const_cast<char*>(text);
    SendMessageA(static_cast<HWND>(this->m_handle), TB_SETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x00459810: by command id (no TBIF_BYINDEX), which equals the position for buttons added with AddButton.
void COsToolBar::GetButtonText(int32_t pos, char* buf, int32_t bufSize) {
    TBBUTTONINFOA info = {};
    info.cbSize = sizeof(info);
    info.dwMask = TBIF_TEXT;
    info.pszText = buf;
    info.cchText = bufSize;
    SendMessageA(static_cast<HWND>(this->m_handle), TB_GETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x00459850
void COsToolBar::EnableButton(int32_t pos, int32_t enable) {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    TBBUTTONINFOA info = {};
    info.cbSize = sizeof(info);
    info.dwMask = TBIF_BYINDEX | TBIF_STATE;
    SendMessageA(hwnd, TB_GETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));

    if (enable) {
        info.fsState |= TBSTATE_ENABLED;
    } else {
        info.fsState &= ~TBSTATE_ENABLED;
    }

    SendMessageA(hwnd, TB_SETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x004598B0
void COsToolBar::CheckButton(int32_t pos, int32_t check) {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    TBBUTTONINFOA info = {};
    info.cbSize = sizeof(info);
    info.dwMask = TBIF_BYINDEX | TBIF_STATE;
    SendMessageA(hwnd, TB_GETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));

    if (check) {
        info.fsState |= TBSTATE_CHECKED;
    } else {
        info.fsState &= ~TBSTATE_CHECKED;
    }

    SendMessageA(hwnd, TB_SETBUTTONINFOA, pos, reinterpret_cast<LPARAM>(&info));
}

// 0.5.3 0x00459910
int32_t COsToolBar::OnCommand(int32_t code) {
    return this->SendEvent(0, code);
}

// ===== COsScrollBar =====
//
// The range is [m_realMin, m_realMax] for the thumb's top; the Win32 range is extended by the page size so the
// thumb can reach m_realMax.

// 0.5.3 0x00459930. Flags: 0x10000 vertical, 0x20000 tab stop.
COsScrollBar::COsScrollBar(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_SCROLL_BAR, id, flags) {
    this->Initialize();
}

// 0.5.3 0x00459990
COsScrollBar::COsScrollBar(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_SCROLL_BAR, id, flags) {
    this->Initialize();
}

// 0.5.3 0x004599C0
void COsScrollBar::Initialize() {
    this->m_realMin = 0;
    this->m_realMax = 100;
    this->SetPageSize(1);
    this->UpdateRangeValues();
}

// 0.5.3 0x004599E0
void COsScrollBar::SetRange(int32_t minValue, int32_t maxValue) {
    this->m_realMin = minValue;
    this->m_realMax = maxValue;
    this->UpdateRangeValues();
}

// 0.5.3 0x00459A00
void COsScrollBar::SetPageSize(int32_t pageSize) {
    this->m_pageSize = pageSize;

    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_PAGE;
    info.nPage = pageSize;
    SetScrollInfo(static_cast<HWND>(this->m_handle), SB_CTL, &info, TRUE);

    this->UpdateRangeValues();
}

// 0.5.3 0x00459A40
void COsScrollBar::UpdateRangeValues() {
    int32_t minValue = this->m_realMin;
    int32_t maxValue = this->m_realMax + (this->m_pageSize - 1 <= 0 ? 0 : this->m_pageSize - 1);

    SendMessageA(static_cast<HWND>(this->m_handle), SBM_SETRANGE, minValue, maxValue);

    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE;
    info.nMin = minValue;
    info.nMax = maxValue;
    SetScrollInfo(static_cast<HWND>(this->m_handle), SB_CTL, &info, TRUE);
}

// 0.5.3 0x00459AA0
void COsScrollBar::SetValue(int32_t value) {
    SendMessageA(static_cast<HWND>(this->m_handle), SBM_SETPOS, value, 1);

    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_POS;
    info.nPos = value;
    SetScrollInfo(static_cast<HWND>(this->m_handle), SB_CTL, &info, TRUE);
}

// 0.5.3 0x00459AF0
int32_t COsScrollBar::GetValue() {
    return static_cast<int32_t>(SendMessageA(static_cast<HWND>(this->m_handle), SBM_GETPOS, 0, 0));
}

// 0.5.3 0x00459B10: event 2 when the position moved.
int32_t COsScrollBar::OnScroll(int32_t code) {
    int32_t action = sConvertScrollMsg(LOWORD(code));

    if (action == -1 || !sProcessScrollMessage(static_cast<HWND>(this->m_handle), SB_CTL, action, 1)) {
        return 0;
    }

    return this->SendEvent(2, 0);
}

// 0.5.3 0x00459C70: delta in wheel notches (positive scrolls up).
int32_t COsScrollBar::OnMouseWheel(int32_t delta) {
    if (!delta) {
        return 0;
    }

    if (sProcessScrollMessage(static_cast<HWND>(this->m_handle), SB_CTL, delta > 0 ? 3 : 2, std::abs(delta))) {
        return this->SendEvent(2, 0);
    }

    return 1;
}

// ===== COsDivider =====
//
// A draggable splitter bar: it moves itself between m_minPos and m_maxPos (parent client coordinates) and sends
// event 2 on release; the owner then lays out the panes. Flag 0x10000: horizontal bar (dragged vertically).

// 0.5.3 0x00459CD0
COsDivider::COsDivider(COsDialog* dialog, int16_t id, uint32_t flags) : COsControl(dialog, TYPE_DIVIDER, id, flags) {
    this->m_dragStartPos = CiRect(0, 0, 0, 0);
    this->Initialize();
}

// 0.5.3 0x00459D40
COsDivider::COsDivider(void* window, int16_t id, uint32_t flags) : COsControl(window, TYPE_DIVIDER, id, flags) {
    this->m_dragStartPos = CiRect(0, 0, 0, 0);
    this->Initialize();
}

// 0.5.3 0x00459D80
COsDivider::~COsDivider() {
}

// 0.5.3 0x00459D90
void COsDivider::Initialize() {
    this->m_dragging = 0;
    this->m_tracking = 0;
    this->m_maxPos = 100000;
    this->m_minPos = -100000;
    SetWindowLongPtrA(static_cast<HWND>(this->m_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(sDividerProc));
}

// 0.5.3 0x00459F00: the resize cursor while the mouse is over the bar.
void COsDivider::UpdateCursor() {
    if (this->m_tracking) {
        OsGuiSetCursor(((this->m_flags >> 16) & 1) | 2);
    } else {
        OsGuiSetCursor(0);
    }
}

// 0.5.3 0x00459F20
void COsDivider::OnDivMouseDown() {
    HWND hwnd = static_cast<HWND>(this->m_handle);

    SetCapture(hwnd);
    this->m_dragging = 1;
    OsGuiGetCursorPosition(&this->m_dragStartMouseX, &this->m_dragStartMouseY);

    CiRect parentRect = OsGuiGetWindowRect(GetParent(hwnd), 1);
    CiRect controlRect = OsGuiGetWindowRect(hwnd, 0);

    this->m_dragStartPos = controlRect;
    this->m_dragStartPos.minX -= parentRect.minX;
    this->m_dragStartPos.maxX -= parentRect.minX;
    this->m_dragStartPos.minY -= parentRect.minY;
    this->m_dragStartPos.maxY -= parentRect.minY;

    this->UpdateCursor();
}

// 0.5.3 0x00459FB0: the idle callback runs after each move so the tool can redraw during the drag.
void COsDivider::OnDivMouseMove(int32_t x, int32_t y) {
    if (!this->m_tracking) {
        TRACKMOUSEEVENT track;
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = static_cast<HWND>(this->m_handle);
        track.dwHoverTime = 0;
        this->m_tracking = _TrackMouseEvent(&track);
    }

    this->UpdateCursor();

    if (!this->m_dragging) {
        return;
    }

    int32_t mouseX = 0;
    int32_t mouseY = 0;
    OsGuiGetCursorPosition(&mouseX, &mouseY);

    CiRect newPos = this->m_dragStartPos;

    if (this->m_flags & 0x10000) {
        int32_t top = mouseY - this->m_dragStartMouseY + this->m_dragStartPos.minY;

        if (top <= this->m_minPos) {
            top = this->m_minPos;
        }

        if (top >= this->m_maxPos) {
            top = this->m_maxPos;
        }

        newPos.minY = top;
        newPos.maxY += top - this->m_dragStartPos.minY;
    } else {
        int32_t left = mouseX - this->m_dragStartMouseX + this->m_dragStartPos.minX;

        if (left <= this->m_minPos) {
            left = this->m_minPos;
        }

        if (left >= this->m_maxPos) {
            left = this->m_maxPos;
        }

        newPos.minX = left;
        newPos.maxX += left - this->m_dragStartPos.minX;
    }

    OsGuiSetWindowRect(this->m_handle, newPos);
    sDoCallback(1, 0, 0);
}

// 0.5.3 0x0045A0C0
void COsDivider::OnDivMouseUp() {
    if (this->m_dragging) {
        ReleaseCapture();
        this->m_dragging = 0;
        this->SendEvent(2, 0);
    }

    this->UpdateCursor();
}

// 0.5.3 0x0045A0F0
void COsDivider::OnDivMouseLeave() {
    this->m_tracking = 0;
    this->UpdateCursor();
}

// 0.5.3 0x0045A100
void COsDivider::SetPositionRange(int32_t minPos, int32_t maxPos) {
    this->m_minPos = minPos;
    this->m_maxPos = maxPos;
}

// ===== COsWindow =====
//
// Wraps a top-level window the tool created (with OsGuiWindowProc as its procedure) for resize events and the
// minimum tracking size.

// 0.5.3 0x0045A840
COsWindow::COsWindow(void* window) {
    this->m_handle = window;
    this->m_minSize.x = -1;
    this->m_minSize.y = -1;
    sSetOsGuiPointer(this->m_handle, this);
}

// 0.5.3 0x0045A8B0
COsWindow::~COsWindow() {
    sRemoveOsGuiPointer(this->m_handle);
}

// 0.5.3 0x0045A8C0
void COsWindow::SetMinSize(int32_t width, int32_t height) {
    this->m_minSize.x = width;
    this->m_minSize.y = height;
}

// 0.5.3 0x0045A8E0
void COsWindow::GetMinSize(int32_t* width, int32_t* height) {
    *width = this->m_minSize.x;
    *height = this->m_minSize.y;
}

// 0.5.3 0x0045A900: the class cursor (OsGuiSetCursor numbering).
void COsWindow::SetCursor(int32_t cursor) {
    HCURSOR winCursor = sWinCursor(cursor);

    if (winCursor) {
        SetClassLongPtrA(static_cast<HWND>(this->m_handle), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(winCursor));
    }
}

// 0.5.3 0x0045A930
void COsWindow::SetIcon(const char* name) {
    OsGuiSetWindowIcon(this->m_handle, name);
}

// 0.5.3 0x0045A950
void COsWindow::SetInputFocus() {
    SetFocus(static_cast<HWND>(this->m_handle));
}

// 0.5.3 0x0045AF60
void COsWindow::OnResize() {
}

// ===== COsSharedMemory =====
//
// A named file mapping shared between processes (e.g. a tool and the client it drives). The bool results are
// error flags, as in 0.5.3: true means failure.

// 0.5.3 0x0045B780
COsSharedMemory::COsSharedMemory() {
    this->m_data = nullptr;
    this->m_size = 0;
    this->m_opaqueData = nullptr;
}

// 0.5.3 0x0045B790
COsSharedMemory::~COsSharedMemory() {
    this->Destroy();
}

// 0.5.3 0x0045B7A0. Modes: 0 create (fails if it exists), 1 open for writing, 2 create or open, 3 open for reading.
bool COsSharedMemory::Initialize(const char* name, uint32_t size, int32_t mode) {
    DWORD access = FILE_MAP_WRITE;
    HANDLE mapping;

    switch (mode) {
        case 0:
        case 2:
            mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE | SEC_COMMIT, 0, size, name);
            break;

        case 3:
            access = FILE_MAP_READ;
            [[fallthrough]];

        case 1:
            mapping = OpenFileMappingA(access, FALSE, name);
            break;

        default:
            return true;
    }

    this->m_opaqueData = mapping;

    if (!mapping) {
        return true;
    }

    if ((GetLastError() != ERROR_ALREADY_EXISTS || mode != 0) && (this->m_data = MapViewOfFile(mapping, access, 0, 0, 0))) {
        this->m_size = size;
        return false;
    }

    CloseHandle(mapping);
    this->m_opaqueData = nullptr;
    return true;
}

// 0.5.3 0x0045B860: 0 read-only, 1 read-write.
bool COsSharedMemory::ChangeAccess(int32_t access) {
    if (!this->m_opaqueData || !this->m_data || !this->m_size) {
        return true;
    }

    DWORD protect;

    if (access == 0) {
        protect = PAGE_READONLY;
    } else if (access == 1) {
        protect = PAGE_READWRITE;
    } else {
        return true;
    }

    DWORD oldProtect;
    return !VirtualProtect(this->m_data, this->m_size, protect, &oldProtect);
}

// 0.5.3 0x0045B8C0
void COsSharedMemory::Destroy() {
    if (this->m_data) {
        UnmapViewOfFile(this->m_data);
        this->m_data = nullptr;
    }

    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = nullptr;
    }
}

#undef OSGUI_ASSERT
