#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_PAGE_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_PAGE_H

#include <cstdint>
#include <windows.h>
#include "WoW/Source/Ui/UILCD/Windows/LCDPage.h"

class CEzLcd;
class CLCDBitmap;
class CLCDText;

class CEzLcdPage : public CLCDPage {
    public:
    /* 00BC */ CEzLcd* m_container;
    /* 00C0 */ int32_t m_buttonIsPressed[11];
    /* 00EC */ int32_t m_buttonWasPressed[11];

    CEzLcdPage();
    CEzLcdPage(CEzLcd* container);
    virtual ~CEzLcdPage();
    CLCDText* AddText(LCD_OBJECT_TYPE type, int32_t size, int32_t alignment, int32_t maxLengthPixels, int32_t userData, int32_t numberOfLines);
    CLCDText* AddColorText(LCD_OBJECT_TYPE type, int32_t size, int32_t alignment, int32_t maxLengthPixels, int32_t userData, int32_t numberOfLines, int32_t fontWeight);
    CLCDBitmap* AddBitmap(int32_t width, int32_t height, int32_t bitmapWidth, int32_t bitmapHeight);
};

#endif
