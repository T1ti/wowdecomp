#ifndef WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_H
#define WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CM2Model;
class CGxVertexPC;

struct SWING {
    /* 0000 */ int32_t m_head;
    /* 0004 */ int32_t m_tail;
    /* 0008 */ C3Vector m_points [128];
    /* 0608 */ CImVector m_color;
    /* 060C */ uint32_t m_duration;
    /* 0610 */ uint32_t m_startTime;
    /* 0614 */ uint32_t m_lastTime;

    SWING();
    void FillVertices(CImVector color, uint8_t fade, CGxVertexPC* vertices);
    bool BuildVertexBuffer(uint32_t time, const C3Vector& top, const C3Vector& bottom);
};

class WTOBJECT {
    public:
    /* 0000 */ SWING* m_swing;
    /* 0004 */ CM2Model* m_model;

    ~WTOBJECT();
    void SetDrawTrail(const uint32_t* color, uint32_t duration);
};

WTOBJECT* WeaponTrailCreate(CM2Model* model);
void WeaponTrailClose(WTOBJECT* trail);
void WeaponTrailsShutdown();
void WeaponTrailSetDrawTrail(WTOBJECT* trail, const uint32_t* color, uint32_t duration);

void WorldTextInitialize();
void WorldTextShutdown();

#endif
