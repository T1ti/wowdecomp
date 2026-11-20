#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_LCDPAGE_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_LCDPAGE_H

#include <cstdint>
#include <windows.h>
#include "WoW/Source/Ui/UILCD/Windows/LCDBitmap.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDCollection.h"

class CLCDPage : public CLCDCollection {
    public:
    /* 004C */ DWORD m_dwStartTime;
    /* 0050 */ DWORD m_dwEllapsedTime;
    /* 0054 */ DWORD m_dwExpirationTime;
    /* 0058 */ int32_t m_bUseBitmapBackground;
    /* 005C */ CLCDBitmap m_Background;
    /* 00B4 */ int32_t m_bUseColorBackground;
    /* 00B8 */ COLORREF m_crPageBackground;

    CLCDPage();
    virtual ~CLCDPage();
    virtual void OnDraw(CLCDGfxBase& gfx);
    virtual void OnUpdate(uint32_t timestamp);
    virtual void SetExpiration(DWORD milliseconds);
    virtual BOOL HasExpired();
    virtual void OnLCDButtonDown(int32_t button);
    virtual void OnLCDButtonUp(int32_t button);

    void SetPageBackground(COLORREF color);
};

#endif
