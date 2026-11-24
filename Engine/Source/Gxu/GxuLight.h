#ifndef ENGINE_SOURCE_GXU_GXULIGHT_H
#define ENGINE_SOURCE_GXU_GXULIGHT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/cpriorityq.h"

struct CGxuLight;
struct CLightList;

class CGxuLightLink {
    public:
    /* 0000 */ CGxuLight* m_light;
    /* 0004 */ CLightList* m_list;
    /* 0008 */ float m_intensity;
    /* 000C */ TSLink<CGxuLightLink> m_lightLink;
    /* 0014 */ TSLink<CGxuLightLink> m_listLink;

    ~CGxuLightLink();
};

struct CGxuLight : public TSLinkedNode<CGxuLight> {
    /* 0008 */ CGxLight m_light;
    /* 0048 */ uint32_t m_index;
    /* 004C */ uint32_t m_frame;
    /* 0050 */ int32_t m_lockCount;
    /* 0054 */ TSExplicitList<CGxuLightLink, offsetof(CGxuLightLink, m_lightLink)> m_links;

    CGxuLight();
    ~CGxuLight();
    float CalcIntensity(const C3Vector& point, float linearAttenuation, float quadraticAttenuation);
    void ClearLinks();
    CGxuLightLink* NewLink();
};

struct CLightList : public TSHashObject<CLightList, HASHKEY_DWORD> {
    /* 0018 */ TSExplicitList<CGxuLightLink, offsetof(CGxuLightLink, m_listLink)> m_links;

    ~CLightList();
};

class CGxuLightQueue : public CPriorityQ<CGxuLight*> {
    public:
    CGxuLightQueue(const uint32_t (&params)[2]);
};

extern void (*GxuLightInitialize)();
extern void (*GxuLightShutdown)();
extern CGxuLight* (*GxuLightCreate)();
extern void (*GxuLightDestroy)(CGxuLight* light);
extern CGxLight* (*GxuLightLock)(CGxuLight* light);
extern void (*GxuLightUnlock)(CGxuLight* light);
extern void (*GxuLightApply)(float x, float y, int32_t unused, const C3Vector& viewPos, uint32_t maxLights);
extern int32_t (*GxuLightGetEnabled)(CGxuLight* light);
extern void (*GxuLightSetEnabled)(CGxuLight* light, bool enabled);
extern void (*GxuLightSetMaxLights)(uint32_t maxLights);
extern float (*GxuLightGetGridSize)();
extern void (*GxuLightSetGridSize)(float gridSize);
extern void (*GxuLightInvalidate)();

#endif
