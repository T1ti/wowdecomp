#include "WoW/Source/Ui/UILCD/Windows/LCDBitmap.h"
#include <cmath>
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

typedef BOOL (WINAPI* ALPHABLENDPROC)(HDC hdcDest, int xDest, int yDest, int widthDest, int heightDest, HDC hdcSrc, int xSrc, int ySrc, int widthSrc, int heightSrc, BLENDFUNCTION blend);

// STATIC: 0x00DCE1A8
static bool s_alphaBlendLoaded;

// STATIC: 0x00DCE1AC
static ALPHABLENDPROC s_alphaBlend;

// VERIFY: 19290b2b manual
// OFFSET: 0x0095AD90
CLCDBitmap::CLCDBitmap() {
    this->m_fZoom = 1.0f;
    this->m_hBitmap = nullptr;
    this->m_dwROP = SRCCOPY;
    this->m_bAlpha = 1;

    if (!s_alphaBlend && !s_alphaBlendLoaded) {
        s_alphaBlendLoaded = true;
        s_alphaBlend = reinterpret_cast<ALPHABLENDPROC>(GetProcAddress(LoadLibraryA("Msimg32.dll"), "AlphaBlend"));
    }
}

// VERIFY: 03aee019 auto
// OFFSET: [0x0095ADF0, 0x0095AE40]
CLCDBitmap::~CLCDBitmap() {
}

// VERIFY: 173447ff auto
// OFFSET: 0x0095AE00
void CLCDBitmap::SetBitmap(HBITMAP bitmap) {
    this->m_hBitmap = bitmap;
}

// VERIFY: df4a60f4 auto
// OFFSET: 0x0095AE10
void CLCDBitmap::SetAlpha(int32_t alpha) {
    this->m_bAlpha = alpha;
}

// VERIFY: 7007d609 auto
// OFFSET: 0x0095AE20
void CLCDBitmap::SetBitmapSize(int32_t width, int32_t height) {
    this->m_bitmapSize.x = width;
    this->m_bitmapSize.y = height;
}

// VERIFY: c1ff4458 manual
// OFFSET: 0x0095AE70
void CLCDBitmap::OnDraw(CLCDGfxBase& gfx) {
    if (!this->m_hBitmap) {
        return;
    }

    HDC compatibleDC = CreateCompatibleDC(gfx.GetHDC());
    HGDIOBJ oldBitmap = SelectObject(compatibleDC, this->m_hBitmap);

    if (gfx.GetLCDScreen()->Format == LGLCD_BMP_FORMAT_160x43x1) {
        BitBlt(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_dwROP);
    } else if (std::fabs(1.0f - this->m_fZoom) >= 0.001f) {
        if (s_alphaBlend) {
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA };

            if (!this->m_bAlpha) {
                blend.AlphaFormat = 0;
            }

            s_alphaBlend(gfx.GetHDC(), 0, 0, static_cast<int>(this->m_sizeLogical.cx * this->m_fZoom), static_cast<int>(this->m_sizeLogical.cy * this->m_fZoom), compatibleDC, 0, 0, this->m_bitmapSize.x, this->m_bitmapSize.y, blend);
        }
    } else if (!this->m_bAlpha || !s_alphaBlend) {
        StretchBlt(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_bitmapSize.x, this->m_bitmapSize.y, this->m_dwROP);
    } else {
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA };
        s_alphaBlend(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_bitmapSize.x, this->m_bitmapSize.y, blend);
    }

    SelectObject(compatibleDC, oldBitmap);
    DeleteDC(compatibleDC);
}
