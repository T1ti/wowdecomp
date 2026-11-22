#ifndef WOW_SOURCE_WORLDCLIENT_CPORTALVIEW_H
#define WOW_SOURCE_WORLDCLIENT_CPORTALVIEW_H

#include <cfloat>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CRect.h"

class C3Vector;

class CPortalView {
    public:
    /* 0000 */ CRect m_rect;
    /* 0010 */ float m_maxViewDepth;
    /* 0014 */ C3Vector* m_verts;
    /* 0018 */ uint32_t m_numVertices;

    CPortalView();
    CPortalView(const CRect& rect, float maxViewDepth);
    void Merge(const CPortalView& other);
};

// INLINED
inline CPortalView::CPortalView() {
    this->m_rect.minX = FLT_MAX;
    this->m_verts = nullptr;
    this->m_rect.minY = FLT_MAX;
    this->m_numVertices = 0;
    this->m_rect.maxX = -FLT_MAX;
    this->m_rect.maxY = -FLT_MAX;
    this->m_maxViewDepth = -1.0f;
}

#endif
