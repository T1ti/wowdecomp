#include "WoW/Source/Ui/UILCD/Windows/LCDPage.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

// VERIFY: 9323e88a auto
// OFFSET: 0x0095B4F0
CLCDPage::CLCDPage() : m_dwStartTime(0), m_dwEllapsedTime(0), m_dwExpirationTime(0) {
    DWORD now = GetTickCount();
    this->m_dwEllapsedTime = 0;
    this->m_bUseBitmapBackground = 0;
    this->m_bUseColorBackground = 0;
    this->m_dwStartTime = now;
    this->m_dwExpirationTime = INFINITE;
}

// VERIFY: 27bb27b2 auto
// OFFSET: [0x0095B430, 0x0095B540]
CLCDPage::~CLCDPage() {
}

// VERIFY: 985edee3 manual
// OFFSET: 0x0095B450
void CLCDPage::OnUpdate(uint32_t timestamp) {
    this->m_dwEllapsedTime = timestamp - this->m_dwStartTime;
    CLCDCollection::OnUpdate(timestamp);
}

// VERIFY: a8fa4a15 auto
// OFFSET: 0x0095B470
void CLCDPage::SetExpiration(DWORD milliseconds) {
    this->m_dwStartTime = GetTickCount();
    this->m_dwEllapsedTime = 0;
    this->m_dwExpirationTime = milliseconds;
}

// VERIFY: 0fbb7a63 auto
// OFFSET: 0x0095B4A0
BOOL CLCDPage::HasExpired() {
    if (this->m_dwStartTime && this->m_dwExpirationTime && this->m_dwEllapsedTime < this->m_dwExpirationTime) {
        return 0;
    }

    return 1;
}

// VERIFY: 4f88c1c3 auto
// OFFSET: 0x00632050
void CLCDPage::OnLCDButtonDown(int32_t button) {
}

// VERIFY: a2e85a41 auto
// OFFSET: 0x00632050
void CLCDPage::OnLCDButtonUp(int32_t button) {
}

// VERIFY: 6c6765d4 auto
// OFFSET: 0x0095B4C0
void CLCDPage::SetPageBackground(COLORREF color) {
    this->m_crPageBackground = color;
    this->m_bUseColorBackground = 1;
    this->m_bUseBitmapBackground = 0;
}

// VERIFY: 21f13202 manual
// OFFSET: 0x0095B580
void CLCDPage::OnDraw(CLCDGfxBase& gfx) {
    if (!this->IsVisible()) {
        return;
    }

    if (this->m_bUseBitmapBackground) {
        this->m_Background.OnDraw(gfx);
    } else if (this->m_bUseColorBackground) {
        HBRUSH brush = CreateSolidBrush(this->m_crPageBackground);
        HGDIOBJ oldBrush = SelectObject(gfx.GetHDC(), brush);
        Rectangle(gfx.GetHDC(), 0, 0, this->GetWidth(), this->GetHeight());
        SelectObject(gfx.GetHDC(), oldBrush);
        DeleteObject(brush);
    }

    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDBase* object = node->m_pObject;

        if (!object->IsVisible()) {
            continue;
        }

        HRGN region = CreateRectRgn(this->GetOrigin().x + object->GetOrigin().x, this->GetOrigin().y + object->GetOrigin().y, this->GetOrigin().x + object->GetOrigin().x + object->GetWidth(), this->GetOrigin().y + object->GetOrigin().y + object->GetHeight());
        SelectClipRgn(gfx.GetHDC(), region);
        DeleteObject(region);

        POINT previousOrigin = { 0, 0 };
        SetViewportOrgEx(gfx.GetHDC(), this->GetOrigin().x + object->GetOrigin().x, this->GetOrigin().y + object->GetOrigin().y, &previousOrigin);
        OffsetViewportOrgEx(gfx.GetHDC(), object->GetLogicalOrigin().x, object->GetLogicalOrigin().y, nullptr);
        object->OnDraw(gfx);
        SelectClipRgn(gfx.GetHDC(), nullptr);
        SetViewportOrgEx(gfx.GetHDC(), previousOrigin.x, previousOrigin.y, nullptr);
        OffsetViewportOrgEx(gfx.GetHDC(), 0, 0, nullptr);
    }
}
