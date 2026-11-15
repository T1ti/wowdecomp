#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>

// VERIFY: f9cf7e53 auto
// OFFSET: 0x0095BF90
CLCDGfxColor::CLCDGfxColor() {
    this->m_pLCDScreen = &this->m_lcdBitmap.hdr;
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
}

// VERIFY: 5b68ef2d manual
// OFFSET: 0x0095C030
CLCDGfxColor::~CLCDGfxColor() {
}

// VERIFY: 31d7ef43 auto
// OFFSET: 0x0095BFC0
HRESULT CLCDGfxColor::Initialize() {
    this->Shutdown();

    this->m_size.x = LGLCD_QVGA_BMP_WIDTH;
    this->m_size.y = LGLCD_QVGA_BMP_HEIGHT;

    HRESULT hr = CLCDGfxBase::Initialize();

    if (FAILED(hr)) {
        return hr;
    }

    hr = this->CreateBitmap(32);

    return FAILED(hr) ? hr : S_OK;
}

// VERIFY: ac930c58 manual
// OFFSET: 0x0095C000
lgLcdBitmapHeader* CLCDGfxColor::GetLCDScreen() {
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
    memcpy(this->m_lcdBitmap.pixels, this->m_pBitmapBits, 4 * this->m_size.x * this->m_size.y);
    return this->m_pLCDScreen;
}

// VERIFY: 2f296ced auto
// OFFSET: 0x0095BFB0
uint32_t CLCDGfxColor::GetFamily() {
    return LGLCD_DEVICE_FAMILY_QVGA_BASIC;
}
