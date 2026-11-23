#ifndef WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H
#define WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/CMoveSpline.h"

class CClientMoveUpdate {
    public:
    /* 0000 */ CMovementStatus m_status;
    /* 0060 */ float walkSpeed;
    /* 0064 */ float runSpeed;
    /* 0068 */ float runBackSpeed;
    /* 006C */ float swimSpeed;
    /* 0070 */ float swimBackSpeed;
    /* 0074 */ float flightSpeed;
    /* 0078 */ float flightBackSpeed;
    /* 007C */ float turnRate;
    /* 0080 */ float pitchRate;
    /* 0084 */ uint32_t unknown_0084;
    /* 0088 */ CMoveSpline m_moveSpline;

    static void Skip(CDataStore* msg);
};

CDataStore& operator>>(CDataStore& msg, CClientMoveUpdate& update);

#endif
