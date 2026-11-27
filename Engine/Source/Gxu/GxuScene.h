#ifndef ENGINE_SOURCE_GXU_GXUSCENE_H
#define ENGINE_SOURCE_GXU_GXUSCENE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class GxuSceneObject {
    public:
    virtual void Unk00();
    virtual void Release();
    virtual bool IsAlive();
    virtual void Update(float elapsed);
    virtual void Render();
};

class GxuScene {
    public:
    void Render();
    /* 0004 */ TSGrowableArray<GxuSceneObject*> m_sceneObjects;
    /* 0014 */ uint32_t m_lastUpdateTime;

    virtual ~GxuScene();
    void Clear();
    void Update(uint32_t time);
};

#endif
