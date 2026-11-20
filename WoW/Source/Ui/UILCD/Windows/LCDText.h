#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_LCDTEXT_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_LCDTEXT_H

#include <cstdint>
#include <windows.h>
#include "WoW/Source/Ui/UILCD/Windows/LCDBase.h"

class CLCDText : public CLCDBase {
    public:
    /* 0040 */ WCHAR m_szText[256];
    /* 0240 */ HFONT m_hFont;
    /* 0244 */ int32_t m_nTextLength;
    /* 0248 */ UINT m_nTextFormat;
    /* 024C */ int32_t m_bRecalcExtent;
    /* 0250 */ DRAWTEXTPARAMS m_dtp;
    /* 0264 */ UINT m_nTextAlignment;
    /* 0268 */ SIZE m_sizeVExtent;
    /* 0270 */ SIZE m_sizeHExtent;
    /* 0278 */ char m_szFontFaceName[32];

    CLCDText();
    virtual ~CLCDText();
    virtual int32_t Initialize();
    virtual void OnDraw(CLCDGfxBase& gfx);
    virtual void SetFont(LOGFONTA& font);
    virtual void SetFontFaceName(const char* faceName);
    virtual void SetFontPointSize(int32_t pointSize);
    virtual void SetFontWeight(int32_t weight);
    virtual void SetFontColor(COLORREF color);
    virtual HFONT GetFont();
    virtual void SetText(const WCHAR* text);
    virtual const WCHAR* GetText();
    virtual void SetWordWrap(BOOL enable);
    virtual SIZE& GetVExtent();
    virtual SIZE& GetHExtent();
    virtual void CalculateExtent(int32_t singleLine);
    virtual void SetLeftMargin(int32_t margin);
    virtual int32_t GetLeftMargin();
    virtual void SetRightMargin(int32_t margin);
    virtual int32_t GetRightMargin();
    virtual void SetAlignment(int32_t alignment);

    void DrawControlText(CLCDGfxBase& gfx);
};

#endif
