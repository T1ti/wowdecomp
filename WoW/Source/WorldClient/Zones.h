#ifndef WOW_SOURCE_WORLDCLIENT_ZONES_H
#define WOW_SOURCE_WORLDCLIENT_ZONES_H

#include <cstdint>
#include "Engine/Source/Tempest/CAaRect.h"

class C2Vector;

struct ZonePath {
    /* 0000 */ C2Vector* m_points;
    /* 0004 */ int32_t m_pointCount;
    /* 0008 */ float m_offsetX;
    /* 000C */ float m_offsetY;
    /* 0010 */ const char* m_path;
    /* 0014 */ CAaRect m_bounds; // Transformed polygon XY bounds, expanded by 50 after loading.

    ZonePath(const char* path);
    ~ZonePath();
};

struct Zone {
    /* 0000 */ int32_t m_mapID;
    /* 0004 */ int32_t unused_0004;
    /* 0008 */ int32_t m_lightID;
    /* 000C */ ZonePath m_zonePath;
};

#endif
