#ifndef WOW_SOURCE_WORLDCLIENT_CMAPBASEOBJLINK_H
#define WOW_SOURCE_WORLDCLIENT_CMAPBASEOBJLINK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CMapBaseObj;
class CMapBaseObjLink;

class CMapBaseObjLink {
    public:
    /* 0000 */ uint32_t m_objectIndex;
    /* 0004 */ CMapBaseObj* owner;
    /* 0008 */ CMapBaseObj* ref;
    /* 000C */ TSLink<CMapBaseObjLink> m_refLink;
    /* 0014 */ TSLink<CMapBaseObjLink> m_ownerLink;
};

#endif
