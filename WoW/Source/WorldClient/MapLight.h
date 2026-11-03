#ifndef WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H
#define WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Model2/M2Light.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CMapLight : public CMapBaseObj {
    public:
    /* 0024 */ C3Vector m_position;
    /* 0030 */ CAaBox m_bbox;
    /* 0048 */ CAaSphere m_sphere;
    /* 0058 */ CM2Light m_light;
    // 0.5.3 names match the 3.3.5 creation and bounds paths; retain the CM2Light layout.
    /* 00C4 */ float m_attenStart;
    /* 00C8 */ float m_attenEnd;
    /* 00CC */ float m_attenDenom;
    /* 00D0 */ uint8_t m_dynamic;
    /* 00D1 */ uint8_t unk_00D1;
    /* 00D2 */ uint8_t unused_00D2;
    /* 00D3 */ uint8_t unused_00D3;

    CMapLight();
};

class CMapCacheLight {
    public:
    /* 0000 */ uint8_t unused_0000[132];
};

#endif
