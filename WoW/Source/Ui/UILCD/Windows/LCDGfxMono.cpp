#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>

// VERIFY: e375fb59 auto
// OFFSET: 0x0095BEA0
CLCDGfxMono::CLCDGfxMono() {
    this->m_pLCDScreen = &this->m_lcdBitmap.hdr;
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
}

// VERIFY: 0fa5b226 manual
// OFFSET: 0x0095BF60
CLCDGfxMono::~CLCDGfxMono() {
}

// VERIFY: 5c18fa55 auto
// OFFSET: 0x0095BEC0
HRESULT CLCDGfxMono::Initialize() {
    this->Shutdown();

    this->m_size.x = LGLCD_BMP_WIDTH;
    this->m_size.y = LGLCD_BMP_HEIGHT;

    HRESULT hr = CLCDGfxBase::Initialize();

    if (FAILED(hr)) {
        return hr;
    }

    hr = this->CreateBitmap(8);

    return FAILED(hr) ? hr : S_OK;
}

// VERIFY: c758fcd4 auto
// OFFSET: 0x0095BF30
void CLCDGfxMono::ClearScreen() {
    memset(this->m_pBitmapBits, 0, this->m_size.x * this->m_size.y);
    CLCDGfxBase::ClearScreen();
}

// VERIFY: 92b29c0a auto
// OFFSET: 0x0095BF00
lgLcdBitmapHeader* CLCDGfxMono::GetLCDScreen() {
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
    memcpy(this->m_lcdBitmap.pixels, this->m_pBitmapBits, this->m_size.x * this->m_size.y);
    return this->m_pLCDScreen;
}

// VERIFY: a8d5361a auto
// OFFSET: 0x008C8DE0
uint32_t CLCDGfxMono::GetFamily() {
    return LGLCD_DEVICE_FAMILY_BW_160x43_GAMING;
}
