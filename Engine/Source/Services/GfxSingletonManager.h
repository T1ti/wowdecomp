#ifndef ENGINE_SOURCE_SERVICES_GFXSINGLETONMANAGER_H
#define ENGINE_SOURCE_SERVICES_GFXSINGLETONMANAGER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class C3Vector;
class CParticleEmitter;
class CParticleEmitter2;
class CRibbonEmitter;

class ParticleSystemManager {
    private:
    friend class CParticleEmitter2; // (inferred)
    static int32_t (*sm_projectCallback)(const C3Vector&, float&, void*);
    static void* sm_projectParam;

    public:
    static ParticleSystemManager* s_instance;
    static void SetProjectCallback(int32_t (*callback)(const C3Vector&, float&, void*), void* param) { // (inferred)
        sm_projectCallback = callback;
        sm_projectParam = param;
    }

    static void Destroy();
    static ParticleSystemManager* GetInstance();
    static float GetScaler();
    static void SetScaler(float scaler);

    /* 0000 */ TSGrowableArray<CParticleEmitter*> m_emitters;
    /* 0010 */ TSGrowableArray<CParticleEmitter2*> m_emitters2;
    /* 0020 */ TSGrowableArray<CParticleEmitter*> m_pendingEmitters;
    /* 0030 */ TSGrowableArray<CParticleEmitter2*> m_pendingEmitters2;

    ~ParticleSystemManager();
    CParticleEmitter2* DuplicateEmitter(const CParticleEmitter2* emitter, int32_t copyParticles);
    void Flush();
};

class RibbonManager {
    public:
    static RibbonManager* s_instance;

    static void Destroy();

    /* 0000 */ TSGrowableArray<CRibbonEmitter*> m_emitters;
    /* 0010 */ TSGrowableArray<CRibbonEmitter*> m_pendingEmitters;

    ~RibbonManager();
    void Flush();
};

#endif
