#ifndef ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H
#define ENGINE_SOURCE_SHADEREFFECT_SHADEREFFECT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/ShaderEffect/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"

class C44Matrix;
class CGxShader;
class CImVector;
class CM2Light;
class CM2Lighting;

struct LocalLights {
    /* 0000 */ C4Vector m_color[4];
    /* 0040 */ C4Vector m_position[4];
    /* 0080 */ float m_constantAttenuation[4];
    /* 0090 */ float m_linearAttenuation[4];
    /* 00A0 */ float m_quadraticAttenuation[4];

    LocalLights();
};

class CShaderEffect : public TSHashObject<CShaderEffect, HASHKEY_STRI> {
    public:
    static BOOL s_enableShaders;
    static float s_fogMul;
    static uint32_t s_shadowValue;
    static uint32_t s_usePcfFiltering;

    static void ComputeLocalLights(LocalLights* localLights, uint32_t lightCount, CM2Light** lights, const C3Vector* origin);
    static void InitShaderSystem(BOOL enableShaders, BOOL usePcfFiltering);
    static uint32_t SelectShadowShader();
    static void SetAlphaRef(float alphaRef);
    static void SetAlphaRefDefault();
    static void SetDefaultShaders(uint32_t lightCount);
    static void SetDiffuse(const C4Vector& color);
    static void SetEmissive(const C4Vector& color);
    static void SetFogEnabled(BOOL enabled);
    static void SetFogParams(float start, float end, float density, const CImVector& color);
    static void SetGlobalLighting(CM2Light* light);
    static void SetLightingEnabled(BOOL enabled);
    static void SetLocalLighting(CM2Lighting* lighting, BOOL enabled, const C3Vector* origin);
    static void SetShaders(uint32_t vsIndex, uint32_t psIndex);
    static void SetTexMtx(const C44Matrix& matrix, uint32_t stage);
    static void SetTexMtx_Identity(uint32_t stage);
    static void SetTexMtx_SphereMap(uint32_t stage);
    static void SetTexMtx_TexGen(EGxTexGen texGen, const C44Matrix& matrix, uint32_t stage);
    static void SetUsePcfFiltering(uint32_t usePcfFiltering);
    static void UpdateProjMatrix();
    static void UpdateWorldViewMatrix();

    CShaderEffect();
    ~CShaderEffect();
    void InitEffect(const char* vertexShader, const char* pixelShader);
    void InitFixedFuncPass(const EGxTexOp* colorOps, const EGxTexOp* alphaOps, uint32_t count);
    void SetCurrent();

    CGxShader* GetVertexShader(uint32_t vsIndex) const { // (inferred)
        return this->m_vertexShaders[vsIndex];
    }
    CGxShader* GetPixelShader(uint32_t psIndex) const { // (inferred)
        return this->m_pixelShaders[psIndex];
    }

    private:
    static constexpr uint32_t VTX_SHADER_PERM_CNT = 90;
    static constexpr uint32_t PIX_SHADER_PERM_CNT = 16;
    static CShaderEffect* s_curEffect;
    static C4Vector s_fogColorAlphaRef;
    static C4Vector s_fogParams;
    static int32_t s_lightEnabled;
    static uint32_t s_localLightCount;
    static LocalLights s_localLights;
    static C3Vector s_sunAmbient;
    static C3Vector s_sunDiffuse;
    static C3Vector s_sunDir;
    static C3Vector s_sunSpec;
    static BOOL s_useAlphaRef;

    /* 0018 */ uint32_t m_fixedFuncPassCount;
    /* 001C */ EGxTexOp m_colorOps[2];
    /* 0024 */ EGxTexOp m_alphaOps[2];
    /* 002C */ CGxShader* m_vertexShaders[VTX_SHADER_PERM_CNT];
    /* 0194 */ CGxShader* m_pixelShaders[PIX_SHADER_PERM_CNT];
};

#endif
