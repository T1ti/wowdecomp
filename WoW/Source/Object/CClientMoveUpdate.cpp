#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Object/CClientMoveUpdate.h"
#include "WoW/Source/Object/Types.h"

// VERIFY: 02297c12 auto
// OFFSET: 0x004F5050
void CClientMoveUpdate::Skip(CDataStore* msg) {
    uint32_t moveFlags = CMovementStatus::Skip(msg);

    void* data;
    msg->GetDataInSitu(data, 36);

    if (moveFlags & MOVEMENTFLAG_SPLINE_ENABLED) {
        CMoveSpline::Skip(msg);
    }
}

// VERIFY: 96a84c7b auto
// OFFSET: 0x004F5090
CDataStore& operator>>(CDataStore& msg, CClientMoveUpdate& update) {
    (msg >> update.m_status).Get(update.walkSpeed).Get(update.runSpeed).Get(update.runBackSpeed).Get(update.swimSpeed).Get(update.swimBackSpeed).Get(update.flightSpeed).Get(update.flightBackSpeed).Get(update.turnRate).Get(update.pitchRate);

    if (update.m_status.m_moveFlags & MOVEMENTFLAG_SPLINE_ENABLED) {
        msg >> update.m_moveSpline;
    }

    return msg;
}
