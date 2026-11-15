#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: bbc154bc auto
// OFFSET: 0x0095CA00
CLCDGfxBase::CLCDGfxBase() {
    this->m_pLCDScreen = nullptr;
    this->m_size.x = 0;
    this->m_size.y = 0;
    this->m_pBitmapInfo = nullptr;
    this->m_hDC = nullptr;
    this->m_hBitmap = nullptr;
    this->m_hPrevBitmap = nullptr;
    this->m_pBitmapBits = nullptr;
}

// VERIFY: 36052227 auto
// OFFSET: [0x0095CCE0, 0x0095CCF0]
CLCDGfxBase::~CLCDGfxBase() {
    this->Shutdown();
}

// VERIFY: 37497e68 auto
// OFFSET: 0x0095CA30
HRESULT CLCDGfxBase::Initialize() {
    this->m_hDC = CreateCompatibleDC(nullptr);

    if (!this->m_hDC) {
        this->Shutdown();
        return E_FAIL;
    }

    return S_OK;
}

// VERIFY: 3dc1c64c manual
// OFFSET: 0x0095CB60
void CLCDGfxBase::Shutdown() {
    if (this->m_hBitmap) {
        DeleteObject(this->m_hBitmap);
        this->m_hBitmap = nullptr;
        this->m_pBitmapBits = nullptr;
    }

    this->m_hPrevBitmap = nullptr;

    if (this->m_pBitmapInfo) {
        delete[] reinterpret_cast<uint8_t*>(this->m_pBitmapInfo);
        this->m_pBitmapInfo = nullptr;
    }

    if (this->m_hDC) {
        DeleteDC(this->m_hDC);
        this->m_hDC = nullptr;
    }

    this->m_size.y = 0;
    this->m_size.x = 0;
}

// VERIFY: ea555123 manual
// OFFSET: 0x0095CBB0
HRESULT CLCDGfxBase::CreateBitmap(uint16_t bitCount) {
    this->m_pBitmapInfo = reinterpret_cast<BITMAPINFO*>(new (__FILE__, __LINE__) uint8_t[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)]);

    if (!this->m_pBitmapInfo) {
        this->Shutdown();
        return E_OUTOFMEMORY;
    }

    memset(this->m_pBitmapInfo, 0, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));

    this->m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    this->m_pBitmapInfo->bmiHeader.biWidth = this->m_size.x;
    this->m_pBitmapInfo->bmiHeader.biHeight = -this->m_size.y;
    this->m_pBitmapInfo->bmiHeader.biPlanes = 1;
    this->m_pBitmapInfo->bmiHeader.biBitCount = bitCount;
    this->m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
    this->m_pBitmapInfo->bmiHeader.biSizeImage = this->m_size.x * this->m_size.y * this->m_pBitmapInfo->bmiHeader.biBitCount / 8;
    this->m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 3200;
    this->m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 3200;
    this->m_pBitmapInfo->bmiHeader.biClrUsed = 256;
    this->m_pBitmapInfo->bmiHeader.biClrImportant = 256;

    for (int32_t i = 0; i < 256; i++) {
        uint8_t color = i > 128 ? 0xFF : 0x00;
        this->m_pBitmapInfo->bmiColors[i].rgbRed = color;
        this->m_pBitmapInfo->bmiColors[i].rgbGreen = color;
        this->m_pBitmapInfo->bmiColors[i].rgbBlue = color;
        this->m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
    }

    this->m_hBitmap = CreateDIBSection(this->m_hDC, this->m_pBitmapInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&this->m_pBitmapBits), nullptr, 0);

    if (!this->m_hBitmap) {
        this->Shutdown();
        return E_FAIL;
    }

    return S_OK;
}

// VERIFY: 4488d159 auto
// OFFSET: 0x0095CA60
void CLCDGfxBase::ClearScreen() {
    RECT rect = { 0, 0, this->m_size.x, this->m_size.y };
    FillRect(this->m_hDC, &rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
}

// VERIFY: a0eaf199 auto
// OFFSET: 0x0095CAA0
void CLCDGfxBase::BeginDraw() {
    if (!this->m_hPrevBitmap) {
        this->m_hPrevBitmap = static_cast<HBITMAP>(SelectObject(this->m_hDC, this->m_hBitmap));
        SetTextColor(this->m_hDC, RGB(255, 255, 255));
        SetBkColor(this->m_hDC, RGB(0, 0, 0));
    }
}

// VERIFY: 733883b2 auto
// OFFSET: 0x0095CAE0
void CLCDGfxBase::EndDraw() {
    if (this->m_hPrevBitmap) {
        GdiFlush();
        SelectObject(this->m_hDC, this->m_hPrevBitmap);
        this->m_hPrevBitmap = nullptr;
    }
}

// VERIFY: 8ae02d29 auto
// OFFSET: 0x00431E70
HDC CLCDGfxBase::GetHDC() {
    return this->m_hDC;
}

// VERIFY: f9541fd8 manual
// OFFSET: 0x0095CB10
lgLcdBitmapHeader* CLCDGfxBase::GetLCDScreen() {
    if (this->m_pLCDScreen) {
        memcpy(this->m_pLCDScreen + 1, this->m_pBitmapBits, 4 * this->m_size.x * this->m_size.y);
    }

    return this->m_pLCDScreen;
}

// VERIFY: 4e283189 auto
// OFFSET: 0x0095CB40
BITMAPINFO* CLCDGfxBase::GetBitmapInfo() {
    return this->m_pBitmapInfo;
}

// VERIFY: 63b68908 auto
// OFFSET: 0x0095CB50
HBITMAP CLCDGfxBase::GetHBITMAP() {
    return this->m_hBitmap;
}

// VERIFY: d2a3386a auto
// OFFSET: 0x00885AE0
int32_t CLCDGfxBase::GetWidth() {
    return this->m_size.x;
}

// VERIFY: 6b833aa7 auto
// OFFSET: 0x008C32F0
int32_t CLCDGfxBase::GetHeight() {
    return this->m_size.y;
}
