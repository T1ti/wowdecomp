#include "Liquid.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/ShaderEffect/Types.h"

static constexpr uint32_t MAX_DIR_WAVES = 3;
static constexpr uint32_t MAX_RAD_WAVES = 3;

struct VSWaveConstants {
    /* 0000 */ float m_directionalPhase[4];
    /* 0010 */ float m_circularPhase[4];
    /* 0020 */ C4Vector m_directionalWaves[MAX_DIR_WAVES];
    /* 0050 */ float unk_0050[4];
    /* 0060 */ float unk_0060[4];
    /* 0070 */ C4Vector m_circularWaves[MAX_RAD_WAVES];
    /* 00A0 */ float unk_00A0[4];
    /* 00B0 */ float unk_00B0[4];
};

struct PSProcWaterConstants {
    /* 0000 */ C4Vector m_sunDir;
    /* 0010 */ C4Vector m_sunAmbient;
    /* 0020 */ C4Vector m_sunDiffuse;
    /* 0030 */ C4Vector m_sunSpecular;
    /* 0040 */ float m_params[8];
};

// STATIC: 0x00D44C34
static int32_t s_materialProcWaterRefCnt;

// STATIC: 0x00D4505C
CDataAllocator Liquid::s_procWaterFFPAllocator(sizeof(Liquid::CMaterialProcWaterFFP), 8);

// STATIC: 0x00D45048
CDataAllocator Liquid::s_procWaterAllocator(sizeof(Liquid::CMaterialProcWater), 8);

// STATIC: 0x00D44C24
CGxShader* Liquid::CMaterialProcWater::s_vertexShaders[4];

// STATIC: 0x00D44C20
CGxShader* Liquid::CMaterialProcWater::s_pixelShader;

// STATIC: 0x00D44C48
static PSProcWaterConstants s_psProcWaterConstants;

// STATIC: 0x00D44F88
static VSWaveConstants s_vsWaveConstants;

// VERIFY: 241af1fe manual
// OFFSET: 0x008A3620
static void SetDirectionalWave(uint32_t idx, const C2Vector& direction, float unknown3, float unknown4, float unknown5, float speed) {
    STORM_ASSERT(idx < MAX_DIR_WAVES);

    s_vsWaveConstants.m_directionalPhase[idx] = static_cast<float>(static_cast<uint32_t>(OsGetAsyncTimeMs() * speed) & 0x1FFF) / 1024.0f * TWO_PI;

    float clampedUnknown3 = unknown3 < 0.001f ? 0.001f : unknown3;
    C4Vector& wave = s_vsWaveConstants.m_directionalWaves[idx];
    wave.x = direction.x;
    wave.y = direction.y;
    wave.z = 1.0f / clampedUnknown3;
    wave.w = unknown5;

    s_vsWaveConstants.unk_0050[idx] = 1.0f / (unknown4 < 0.001f ? 0.001f : unknown4);
}

// VERIFY: ffbfd269 manual
// OFFSET: 0x008A3710
// SetRadialWave (inferred)
static void SetRadialWave(uint32_t idx, const C2Vector& unknown2, const C2Vector& unknown3, float unknown4, float unknown5, float unknown6, float speed) {
    STORM_ASSERT(idx < MAX_RAD_WAVES);

    s_vsWaveConstants.m_circularPhase[idx] = static_cast<float>(static_cast<uint32_t>(OsGetAsyncTimeMs() * speed) & 0x1FFF) / 1024.0f * TWO_PI;

    float invUnknown4 = 1.0f / (unknown4 < 0.001f ? 0.001f : unknown4);
    s_vsWaveConstants.unk_0060[idx] = 1.0f / (unknown5 < 0.001f ? 0.001f : unknown5);

    C4Vector& wave = s_vsWaveConstants.m_circularWaves[idx];
    wave.x = unknown2.x;
    wave.y = unknown2.y;
    wave.z = unknown3.x;
    wave.w = unknown3.y;

    s_vsWaveConstants.unk_00A0[idx] = invUnknown4;
    s_vsWaveConstants.unk_00B0[idx] = unknown6;
}

// VERIFY: e47f29d4 manual
// OFFSET: 0x008A3810
void Liquid::SetProcWaterParams(float liquidFloat11, float liquidFloat12, float liquidFloat13, float liquidFloat14, float liquidFloat15, float liquidFloat16, float liquidFloat17) {
    float* params = s_psProcWaterConstants.m_params;
    params[0] = 1.0f / (liquidFloat11 < 0.001f ? 0.001f : liquidFloat11);
    params[1] = liquidFloat12;
    params[2] = liquidFloat13;
    params[3] = liquidFloat14;
    params[4] = liquidFloat17;
    params[5] = liquidFloat15;
    params[6] = liquidFloat16;
    params[7] = 0.0f;
}

// VERIFY: e928fbf9 auto
// OFFSET: 0x008A3C90
void Liquid::SetProcWaterLighting(const CM2Lighting& lighting) {
    s_psProcWaterConstants.m_sunDir.x = -lighting.m_sunDir.x;
    s_psProcWaterConstants.m_sunDir.y = -lighting.m_sunDir.y;
    s_psProcWaterConstants.m_sunDir.z = -lighting.m_sunDir.z;
    s_psProcWaterConstants.m_sunDir.w = 0.0f;

    s_psProcWaterConstants.m_sunAmbient.x = lighting.m_sunAmbient.x;
    s_psProcWaterConstants.m_sunAmbient.y = lighting.m_sunAmbient.y;
    s_psProcWaterConstants.m_sunAmbient.z = lighting.m_sunAmbient.z;
    s_psProcWaterConstants.m_sunAmbient.w = 0.0f;

    s_psProcWaterConstants.m_sunDiffuse.x = lighting.m_sunDiffuse.x;
    s_psProcWaterConstants.m_sunDiffuse.y = lighting.m_sunDiffuse.y;
    s_psProcWaterConstants.m_sunDiffuse.z = lighting.m_sunDiffuse.z;
    s_psProcWaterConstants.m_sunDiffuse.w = 0.0f;

    s_psProcWaterConstants.m_sunSpecular.x = lighting.m_sunSpecular.x;
    s_psProcWaterConstants.m_sunSpecular.y = lighting.m_sunSpecular.y;
    s_psProcWaterConstants.m_sunSpecular.z = lighting.m_sunSpecular.z;
    s_psProcWaterConstants.m_sunSpecular.w = 50.0f;
}

// VERIFY: 4d2354fa auto
// OFFSET: 0x008A3DE0
void Liquid::CMaterialProcWater::Release() {
    this->~CMaterialProcWater();
    Liquid::s_procWaterAllocator.PutData(this, nullptr, 0);
}

// VERIFY: 624c4ea8 auto
// OFFSET: 0x008A3E00
Liquid::CMaterialProcWater::CMaterialProcWater() {
    if (s_materialProcWaterRefCnt++ == 0) {
        char name[256];
        SStrPrintf(name, sizeof(name), "vsLiquidProcWater%s", Liquid::GetShaderSuffix());
        g_theGxDevicePtr->ShaderCreate(Liquid::CMaterialProcWater::s_vertexShaders, GxSh_Vertex, "Shaders\\Vertex", name, 4);
        SStrPrintf(name, sizeof(name), "psLiquidProcWater%s", Liquid::GetShaderSuffix());
        g_theGxDevicePtr->ShaderCreate(&Liquid::CMaterialProcWater::s_pixelShader, GxSh_Pixel, "Shaders\\Pixel", name, 1);
    }
}

// VERIFY: 0b2449de manual
// OFFSET: [0x008A3EC0, 0x008A4740]
Liquid::CMaterialProcWater::~CMaterialProcWater() {
    STORM_ASSERT(s_materialProcWaterRefCnt > 0);

    if (s_materialProcWaterRefCnt-- == 1) {
        for (uint32_t i = 0; i < 4; i++) {
            if (Liquid::CMaterialProcWater::s_vertexShaders[i]) {
                g_theGxDevicePtr->ShaderDestroy(Liquid::CMaterialProcWater::s_vertexShaders[i]);
            }
        }

        if (Liquid::CMaterialProcWater::s_pixelShader) {
            g_theGxDevicePtr->ShaderDestroy(Liquid::CMaterialProcWater::s_pixelShader);
        }
    }
}

// VERIFY: c2386692 auto
// OFFSET: 0x008A3F30
void Liquid::CMaterialProcWaterFFP::Release() {
    this->~CMaterialProcWaterFFP();
    Liquid::s_procWaterFFPAllocator.PutData(this, nullptr, 0);
}

// VERIFY: 065234f4 auto
// OFFSET: 0x008A4710
Liquid::CMaterialProcWater* Liquid::CMaterialProcWater::Create() {
    void* mem = Liquid::s_procWaterAllocator.GetData(0, ".?AVCMaterialProcWater@Liquid@@", SLOG_OBJECT);
    return mem ? new (mem) Liquid::CMaterialProcWater() : nullptr;
}

// VERIFY: 3eec8265 auto
// OFFSET: 0x008A4770
Liquid::CMaterialProcWaterFFP* Liquid::CMaterialProcWaterFFP::Create() {
    void* mem = Liquid::s_procWaterFFPAllocator.GetData(0, ".?AVCMaterialProcWaterFFP@Liquid@@", SLOG_OBJECT);
    return mem ? new (mem) Liquid::CMaterialProcWaterFFP() : nullptr;
}

// VERIFY: be2b8036 manual
// OFFSET: 0x008A48F0
void Liquid::CMaterialProcWater::Render(Liquid::IEnvironment* environment, Liquid::IGeomFactory* geomFactory, Liquid::IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, Liquid::CMaterialSettings* settings) {
    CGxTex* texture0 = settings->GetTexture(0, 1250);

    if (!texture0) {
        return;
    }

    CGxTex* texture1 = settings->GetTexture(1, 1250);

    if (!texture1) {
        return;
    }

    CGxTex* animTextures[2];

    for (uint32_t i = 0; i < 2; i++) {
        animTextures[i] = settings->GetTexture(i + 2, settings->GetInt(i + 1));

        if (!animTextures[i]) {
            return;
        }
    }

    CGxTex* texture4 = settings->GetTexture(4, 1250);

    if (!texture4) {
        return;
    }

    CGxTex* texture5 = settings->GetTexture(5, settings->GetInt(3));

    if (!texture5) {
        return;
    }

    float scales[4];
    float angles[4];

    for (uint32_t i = 0; i < 4; i++) {
        scales[i] = settings->GetFloat(i) * Liquid::WorldCoordScale();
        angles[i] = settings->GetFloat(i + 4) * RAD_TO_DEG;
    }

    float scale4 = settings->GetFloat(8) * Liquid::WorldCoordScale();
    float scale5 = settings->GetFloat(9) * Liquid::WorldCoordScale();
    float angle5 = settings->GetFloat(10) * RAD_TO_DEG;

    g_theGxDevicePtr->RsPush();

    CGxBuf* vertexBuf = nullptr;
    CGxBuf* indexBuf = nullptr;
    CGxBatch batch;
    batch.m_primType = GxPrims_Last;
    batch.m_start = 0;
    batch.m_count = 0;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 0;

    bool identity = geomFactory->SetTransform(const_cast<C44Matrix*>(transform));
    geomFactory->GetBuffers(GxVBF_PT2, &vertexBuf, &indexBuf, &batch);
    GxPrimVertexPtr(vertexBuf, GxVBF_PT2);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

    g_theGxDevicePtr->RsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_DepthWrite, 0);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    g_theGxDevicePtr->RsSet(GxRs_Fog, 1);

    GxXformProjNative(Liquid::s_vsConstants.m_projection);

    C44Matrix matrix;
    Liquid::SetShaderTransforms(cameraPos, identity ? matrix : *transform);

    CM2Lighting lighting(*bounds);
    environment->SelectLights(&lighting);
    Liquid::SetShaderLighting(lighting);
    Liquid::SetProcWaterLighting(lighting);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture1);
    g_theGxDevicePtr->RsSet(GxRs_Texture2, texture4);
    g_theGxDevicePtr->RsSet(GxRs_Texture3, texture5);

    for (uint32_t i = 0; i < 2; i++) {
        g_theGxDevicePtr->RsSet(static_cast<EGxRenderState>(GxRs_Texture4 + i), animTextures[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        matrix = C44Matrix::RotationAroundZ(angles[i]);
        matrix.Scale(scales[i]);
        Liquid::s_vsConstants.m_texTransforms[i] = matrix;
    }

    C44Matrix scaleMatrix;
    scaleMatrix.b1 = scale4;
    Liquid::s_vsConstants.m_texTransforms[4] = scaleMatrix;

    C44Matrix rotationMatrix = C44Matrix::RotationAroundZ(angle5);
    rotationMatrix.Scale(scale5);
    Liquid::s_vsConstants.m_texTransforms[5] = rotationMatrix;

    const float* waves = nullptr;
    const float* wavesEnd;

    if (waveManager) {
        waves = static_cast<const float*>(waveManager->GetData());
        wavesEnd = waves + waveManager->GetCount();
    } else {
        wavesEnd = waves;
    }

    for (uint32_t i = 0; i < MAX_DIR_WAVES; i++) {
        if (waves + 6 > wavesEnd) {
            C2Vector direction;
            SetDirectionalWave(i, direction, 0.0f, 0.0f, 0.0f, 0.0f);
        } else {
            C2Vector direction(waves[0], waves[1]);
            SetDirectionalWave(i, direction, waves[2], waves[3], waves[4], waves[5]);
        }

        waves += 6;
    }

    for (uint32_t i = 0; i < MAX_RAD_WAVES; i++) {
        if (waves + 8 > wavesEnd) {
            C2Vector unknown2;
            C2Vector unknown3;
            SetRadialWave(i, unknown2, unknown3, 0.0f, 0.0f, 0.0f, 0.0f);
        } else {
            C2Vector unknown3(waves[2], waves[3]);
            C2Vector unknown2(waves[0], waves[1]);
            SetRadialWave(i, unknown2, unknown3, waves[4], waves[5], waves[6], waves[7]);
        }

        waves += 8;
    }

    float liquidFloat11 = settings->GetFloat(11) * Liquid::WorldCoordScale();
    float liquidFloat12 = settings->GetFloat(12);
    float liquidFloat13 = settings->GetFloat(13);
    float liquidFloat14 = settings->GetFloat(14);
    float liquidFloat15 = settings->GetFloat(15);
    float liquidFloat16 = settings->GetFloat(16);
    float liquidFloat17 = settings->GetFloat(17);
    Liquid::SetProcWaterParams(liquidFloat11, liquidFloat12, liquidFloat13, liquidFloat14, liquidFloat15, liquidFloat16, liquidFloat17);

    Liquid::SetShaderConstants();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 46, reinterpret_cast<const float*>(&s_vsWaveConstants), 12);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 6, reinterpret_cast<const float*>(&s_psProcWaterConstants), 6);

    uint32_t lightCount = lighting.m_lightCount;

    if (lightCount >= 3) {
        lightCount = 3;
    }

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, Liquid::CMaterialProcWater::s_vertexShaders[lightCount]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, Liquid::CMaterialProcWater::s_pixelShader);
    g_theGxDevicePtr->Draw(&batch, 1);
    g_theGxDevicePtr->RsPop();
}

// VERIFY: 3685e63c manual
// OFFSET: 0x008A5170
void Liquid::CMaterialProcWaterFFP::Render(Liquid::IEnvironment* environment, Liquid::IGeomFactory* geomFactory, Liquid::IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, Liquid::CMaterialSettings* settings) {
    CGxTex* texture0 = settings->GetTexture(4, 1250);

    if (!texture0) {
        return;
    }

    CGxTex* texture1 = settings->GetTexture(5, settings->GetInt(2));

    if (!texture1) {
        return;
    }

    float scale0 = settings->GetFloat(8) * Liquid::WorldCoordScale();
    float scale1 = settings->GetFloat(9) * Liquid::WorldCoordScale();
    float angle1 = settings->GetFloat(10) * RAD_TO_DEG;

    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->XformPush(GxXform_World);
    g_theGxDevicePtr->RsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, g_theGxDevicePtr->Caps().m_maxClipPlanes == 0);

    C44Matrix world = *transform;

    if (Liquid::UseCameraRelativeCoords()) {
        world.d0 -= cameraPos.x;
        world.d1 -= cameraPos.y;
        world.d2 -= cameraPos.z;
    }

    GxXformSet(GxXform_World, world);

    CM2Lighting lighting(*bounds);
    environment->SelectLights(&lighting);
    lighting.SetupGxLights(&cameraPos);
    lighting.SetupGxFog();

    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture0);
    g_theGxDevicePtr->RsSet(GxRs_TextureCoord0, 1);

    C44Matrix texMatrix0;
    texMatrix0.b1 = scale0;
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex0, texMatrix0);

    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture1);
    g_theGxDevicePtr->RsSet(GxRs_TextureCoord1, 0);
    g_theGxDevicePtr->RsSet(GxRs_TextureShader1, 1);
    g_theGxDevicePtr->RsSet(GxRs_ColorOp1, GxTexOp_Add);
    g_theGxDevicePtr->RsSet(GxRs_AlphaOp1, GxTexOp_PassThru);

    C44Matrix texMatrix1 = C44Matrix::RotationAroundZ(angle1);
    texMatrix1.Scale(scale1);
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex1, texMatrix1);

    CGxBuf* vertexBuf = nullptr;
    CGxBuf* indexBuf = nullptr;
    CGxBatch batch;
    batch.m_primType = GxPrims_Last;
    batch.m_start = 0;
    batch.m_count = 0;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 0;
    geomFactory->GetBuffers(GxVBF_PNCT2, &vertexBuf, &indexBuf, &batch);
    GxPrimVertexPtr(vertexBuf, GxVBF_PNCT2);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->Draw(&batch, 1);

    GxXformPop(GxXform_Tex1);
    GxXformPop(GxXform_Tex0);
    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}
