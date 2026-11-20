#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_LCDOUTPUT_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_LCDOUTPUT_H

#include <cstdint>
#include <windows.h>
#include "ThirdParty/lgLcd/lglcd.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDCollection.h"

class CLCDGfxBase;
class CLCDPage;

class CLCDOutput : public CLCDCollection {
    public:
    /* 004C */ CLCDPage* m_pActivePage;
    /* 0050 */ CLCDPage* m_pGlobalPage;
    /* 0054 */ int32_t m_hDevice;
    /* 0058 */ int32_t m_bSetAsForeground;
    /* 005C */ uint32_t m_dwButtonState;
    /* 0060 */ uint32_t m_nPriority;
    /* 0064 */ CLCDGfxBase* m_pGfx;
    /* 0068 */ lgLcdOpenByTypeContext m_OpenByTypeContext;

    CLCDOutput();
    virtual ~CLCDOutput();
    virtual void Shutdown();
    virtual void OnUpdate(uint32_t timestamp);
    virtual int32_t Draw();
    virtual void OnLCDButtonDown(int32_t button);
    virtual void OnLCDButtonUp(int32_t button);
    virtual void OnSoftButtonEvent(uint32_t buttons);
    virtual int32_t DoesBitmapNeedUpdate(lgLcdBitmapHeader* bitmap);
    virtual void OnPageShown(CLCDCollection* page);
    virtual void OnPageExpired(CLCDCollection* page);
    virtual void OnEnteringIdle();
    virtual void OnClosingDevice(int32_t device);
    virtual void OnOpenedDevice(int32_t device);
    void Close();
    void SetGfx(CLCDGfxBase* gfx);
    void SetGlobalPage(CLCDPage* page);
    BOOL IsOpened();
    HRESULT HandleErrorFromAPI(DWORD result);
    int32_t GetDeviceId();
    void ResetDeviceType();
    BOOL HasDeviceType();
    int32_t OpenByType(lgLcdOpenByTypeContext& context);
    int32_t ReOpenDeviceType();
    void ShowPage(CLCDPage* page, int32_t activate);
};

#endif
