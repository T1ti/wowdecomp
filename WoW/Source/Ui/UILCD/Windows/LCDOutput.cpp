#include "WoW/Source/Ui/UILCD/Windows/LCDOutput.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDPage.h"

// VERIFY: 6d339fda auto
// OFFSET: 0x00959C80
CLCDOutput::CLCDOutput() {
    this->m_pActivePage = nullptr;
    this->m_pGlobalPage = nullptr;
    this->m_dwButtonState = 0;
    this->m_pGfx = nullptr;
    this->m_hDevice = LGLCD_INVALID_DEVICE;
    this->m_bSetAsForeground = 1;
    this->m_nPriority = LGLCD_PRIORITY_NORMAL;
    this->m_OpenByTypeContext.connection = 0;
    this->m_OpenByTypeContext.deviceType = 0;
    this->m_OpenByTypeContext.onSoftbuttonsChanged.softbuttonsChangedCallback = nullptr;
    this->m_OpenByTypeContext.onSoftbuttonsChanged.softbuttonsChangedContext = nullptr;
    this->m_OpenByTypeContext.device = 0;
}

// VERIFY: 414b5d59 auto
// OFFSET: 0x0095A290
CLCDOutput::~CLCDOutput() {
    this->Close();
}

// VERIFY: 89d33d8c auto
// OFFSET: 0x00959D20
void CLCDOutput::Close() {
    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->OnClosingDevice(this->m_hDevice);
        lgLcdClose(this->m_hDevice);
        this->m_hDevice = LGLCD_INVALID_DEVICE;
    }
}

// VERIFY: 325fbb1b auto
// OFFSET: 0x00959D20
void CLCDOutput::Shutdown() {
    this->Close();
}

// VERIFY: 7579de46 auto
// OFFSET: 0x00959D10
void CLCDOutput::SetGfx(CLCDGfxBase* gfx) {
    this->m_pGfx = gfx;
}

// VERIFY: 8459c6b4 auto
// OFFSET: 0x00959D50
void CLCDOutput::SetGlobalPage(CLCDPage* page) {
    this->m_pGlobalPage = page;
}

// VERIFY: 944c185d auto
// OFFSET: 0x00959D60
BOOL CLCDOutput::IsOpened() {
    return this->m_hDevice != LGLCD_INVALID_DEVICE;
}

// VERIFY: 0bc11f5d auto
// OFFSET: 0x00959D70
HRESULT CLCDOutput::HandleErrorFromAPI(DWORD result) {
    switch (result) {
        case ERROR_SUCCESS:
            return S_OK;

        case ERROR_DEVICE_NOT_CONNECTED:
            this->Close();

            break;

        case RPC_S_PROTOCOL_ERROR:
            break;

        default:
            this->Close();
            break;
    }

    return static_cast<HRESULT>(result | 0x80000000);
}

// VERIFY: 170398e5 auto
// OFFSET: 0x00959DE0
int32_t CLCDOutput::DoesBitmapNeedUpdate(lgLcdBitmapHeader* bitmap) {
    return 1;
}

// VERIFY: 97404d9b manual
// OFFSET: 0x00959DF0
void CLCDOutput::OnLCDButtonDown(int32_t button) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnLCDButtonDown(button);
    }
}

// VERIFY: d335ee0d manual
// OFFSET: 0x00959E10
void CLCDOutput::OnLCDButtonUp(int32_t button) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnLCDButtonUp(button);
    }
}

// VERIFY: 5c227f56 auto
// OFFSET: 0x00959E30
int32_t CLCDOutput::GetDeviceId() {
    return this->m_hDevice;
}

// VERIFY: f8ab72a2 auto
// OFFSET: 0x00959E40
void CLCDOutput::ResetDeviceType() {
    this->m_OpenByTypeContext.deviceType = 0;
}

// VERIFY: bf2ba328 auto
// OFFSET: 0x00959E50
BOOL CLCDOutput::HasDeviceType() {
    return this->m_OpenByTypeContext.deviceType != 0;
}

// VERIFY: 8757bd4b auto
// OFFSET: 0x00959E60
int32_t CLCDOutput::OpenByType(lgLcdOpenByTypeContext& context) {
    this->Close();

    if (lgLcdOpenByType(&context)) {
        return 0;
    }

    this->m_hDevice = context.device;
    this->m_dwButtonState = 0;

    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
    }

    this->m_OpenByTypeContext = context;
    this->OnOpenedDevice(this->m_hDevice);
    return 1;
}

// VERIFY: a92efac4 manual
// OFFSET: 0x00959F00
int32_t CLCDOutput::ReOpenDeviceType() {
    if (this->m_OpenByTypeContext.deviceType) {
        return this->OpenByType(this->m_OpenByTypeContext);
    }

    return 0;
}

// VERIFY: 34165a84 auto
// OFFSET: 0x00959F20
void CLCDOutput::ShowPage(CLCDPage* page, int32_t activate) {
    if (activate) {
        this->m_pActivePage = page;

        if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
            lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
        }

        this->OnPageShown(page);
    } else {
        page->SetExpiration(0);
        this->OnUpdate(GetTickCount());
    }
}

// VERIFY: 06a9f0ec auto
// OFFSET: 0x00959F80
int32_t CLCDOutput::Draw() {
    uint32_t priority = this->m_nPriority;

    if (!this->m_pActivePage || this->m_hDevice == LGLCD_INVALID_DEVICE || !priority) {
        return 1;
    }

    this->m_pGfx->BeginDraw();
    this->m_pGfx->ClearScreen();
    this->m_pActivePage->OnDraw(*this->m_pGfx);

    if (this->m_pGlobalPage) {
        this->m_pGlobalPage->OnDraw(*this->m_pGfx);
    }

    this->m_pGfx->EndDraw();
    lgLcdBitmapHeader* bitmap = this->m_pGfx->GetLCDScreen();

    if (this->DoesBitmapNeedUpdate(bitmap)) {
        this->HandleErrorFromAPI(lgLcdUpdateBitmap(this->m_hDevice, bitmap, priority));
    }

    return this->m_hDevice != LGLCD_INVALID_DEVICE;
}

// VERIFY: c42cf8f3 manual
// OFFSET: 0x0095A030
void CLCDOutput::OnSoftButtonEvent(uint32_t buttons) {
    if (this->m_pGfx->GetFamily() == LGLCD_DEVICE_FAMILY_QVGA_BASIC) {
        if ((this->m_dwButtonState & LGLCDBUTTON_LEFT) && !(buttons & LGLCDBUTTON_LEFT)) {
            this->OnLCDButtonUp(LGLCDBUTTON_LEFT);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_LEFT) && (buttons & LGLCDBUTTON_LEFT)) {
            this->OnLCDButtonDown(LGLCDBUTTON_LEFT);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_RIGHT) && !(buttons & LGLCDBUTTON_RIGHT)) {
            this->OnLCDButtonUp(LGLCDBUTTON_RIGHT);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_RIGHT) && (buttons & LGLCDBUTTON_RIGHT)) {
            this->OnLCDButtonDown(LGLCDBUTTON_RIGHT);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_OK) && !(buttons & LGLCDBUTTON_OK)) {
            this->OnLCDButtonUp(LGLCDBUTTON_OK);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_OK) && (buttons & LGLCDBUTTON_OK)) {
            this->OnLCDButtonDown(LGLCDBUTTON_OK);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_CANCEL) && !(buttons & LGLCDBUTTON_CANCEL)) {
            this->OnLCDButtonUp(LGLCDBUTTON_CANCEL);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_CANCEL) && (buttons & LGLCDBUTTON_CANCEL)) {
            this->OnLCDButtonDown(LGLCDBUTTON_CANCEL);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_UP) && !(buttons & LGLCDBUTTON_UP)) {
            this->OnLCDButtonUp(LGLCDBUTTON_UP);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_UP) && (buttons & LGLCDBUTTON_UP)) {
            this->OnLCDButtonDown(LGLCDBUTTON_UP);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_DOWN) && !(buttons & LGLCDBUTTON_DOWN)) {
            this->OnLCDButtonUp(LGLCDBUTTON_DOWN);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_DOWN) && (buttons & LGLCDBUTTON_DOWN)) {
            this->OnLCDButtonDown(LGLCDBUTTON_DOWN);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_MENU) && !(buttons & LGLCDBUTTON_MENU)) {
            this->OnLCDButtonUp(LGLCDBUTTON_MENU);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_MENU) && (buttons & LGLCDBUTTON_MENU)) {
            this->OnLCDButtonDown(LGLCDBUTTON_MENU);
        }
    } else {
        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON0) && !(buttons & LGLCDBUTTON_BUTTON0)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON0);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON0) && (buttons & LGLCDBUTTON_BUTTON0)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON0);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON1) && !(buttons & LGLCDBUTTON_BUTTON1)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON1);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON1) && (buttons & LGLCDBUTTON_BUTTON1)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON1);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON2) && !(buttons & LGLCDBUTTON_BUTTON2)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON2);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON2) && (buttons & LGLCDBUTTON_BUTTON2)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON2);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON3) && !(buttons & LGLCDBUTTON_BUTTON3)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON3);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON3) && (buttons & LGLCDBUTTON_BUTTON3)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON3);
        }
    }

    this->m_dwButtonState = buttons;
}

// VERIFY: d65e6738 auto
// OFFSET: 0x0095A2E0
void CLCDOutput::OnUpdate(uint32_t timestamp) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnUpdate(timestamp);
    }

    if (!this->m_pActivePage || !this->m_pActivePage->HasExpired()) {
        return;
    }

    this->m_pActivePage = nullptr;
    this->OnPageExpired(nullptr);

    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDPage* page = static_cast<CLCDPage*>(node->m_pObject);

        if (!page->HasExpired()) {
            this->m_pActivePage = page;

            if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
                lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
            }

            this->OnPageShown(page);
            break;
        }
    }

    if (this->m_pActivePage) {
        return;
    }

    this->OnEnteringIdle();

    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->m_pGfx->ClearScreen();
        lgLcdUpdateBitmap(this->m_hDevice, this->m_pGfx->GetLCDScreen(), 0);
    }
}

// VERIFY: 1030497a auto
// OFFSET: 0x00632050
void CLCDOutput::OnPageShown(CLCDCollection* page) {
}

// VERIFY: 68dde66a auto
// OFFSET: 0x00632050
void CLCDOutput::OnPageExpired(CLCDCollection* page) {
}

// VERIFY: 7b8ed439 auto
// OFFSET: 0x005EEB70
void CLCDOutput::OnEnteringIdle() {
}

// VERIFY: 5eda7916 auto
// OFFSET: 0x00632050
void CLCDOutput::OnClosingDevice(int32_t device) {
}

// VERIFY: 16a5474d auto
// OFFSET: 0x00632050
void CLCDOutput::OnOpenedDevice(int32_t device) {
}
