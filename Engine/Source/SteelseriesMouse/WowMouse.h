#ifndef ENGINE_SOURCE_STEELSERIESMOUSE_WOWMOUSE_H
#define ENGINE_SOURCE_STEELSERIESMOUSE_WOWMOUSE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class HidManager;

class SteelseriesMouse {
    public:
    virtual ~SteelseriesMouse();
    virtual BOOL IsConnected() = 0;
};

class WowMouse : public SteelseriesMouse {
    public:
    /* 0004 */ HidManager* m_hidManager;

    WowMouse();
    ~WowMouse() override;
    BOOL IsConnected() override;
};

#endif
