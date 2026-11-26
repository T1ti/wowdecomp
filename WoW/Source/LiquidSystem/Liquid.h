#ifndef WOW_SOURCE_LIQUIDSYSTEM_LIQUID_H
#define WOW_SOURCE_LIQUIDSYSTEM_LIQUID_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/LiquidSystem/Types.h"

template <class T>
class IDatabase;
class CGxShader;
class CGxTex;
class CM2Lighting;
class CTexture;
class LiquidMaterialRec;
class LiquidTypeRec;

namespace Liquid {
    class CInstance;
    class CMaterialSettings;
    class IWaveManager;
    class IEnvironment;
    class IGeomFactory;

    struct VSLight {
        /* 0000 */ C4Vector m_position;
        /* 0010 */ C4Vector m_color;
        /* 0020 */ C4Vector m_attenuation;
    };

    struct VSConstants {
        /* 0000 */ C44Matrix m_projection;
        /* 0040 */ C4Vector m_fog;
        /* 0050 */ C44Matrix m_modelView;
        /* 0090 */ C44Matrix m_texTransforms[6];
        /* 0210 */ C4Vector m_sunDir;
        /* 0220 */ C4Vector m_sunAmbient;
        /* 0230 */ C4Vector m_sunDiffuse;
        /* 0240 */ C4Vector m_sunSpecular;
        /* 0250 */ VSLight m_lights[3];
    };

    struct PSConstants {
        /* 0000 */ C44Matrix m_modelViewProj;
        /* 0040 */ C4Vector m_fogColor;
        /* 0050 */ C4Vector m_cameraPos;
    };

    class IEnvironment {
        public:
        virtual ~IEnvironment();
        virtual void Release() = 0;
        virtual void SetInteriorFog(int32_t interiorFog) = 0;
        virtual void SelectLights(CM2Lighting* lighting) = 0;
    };

    class IWaveManager {
        public:
        virtual ~IWaveManager();
        virtual void AddRef() = 0;
        virtual void Release() = 0;
        virtual void* GetData() = 0;
        virtual int32_t GetCount() = 0;
    };

    class IGeomFactory {
        public:
        virtual ~IGeomFactory();
        virtual void Release() = 0;
        virtual void GetBuffers(EGxVertexBufferFormat format, CGxBuf** vertexBuf, CGxBuf** indexBuf, CGxBatch* batch) = 0;
        virtual void Purge() = 0;
        virtual bool SetTransform(C44Matrix* transform) = 0;
    };

    class IMaterial {
        public:
        virtual ~IMaterial();
        virtual void Release() = 0;
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings) = 0;
    };

    class CMaterialProcWater : public IMaterial {
        public:
        static CMaterialProcWater* Create();

        virtual ~CMaterialProcWater();
        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);

        private:
        static CGxShader* s_vertexShaders[4];
        static CGxShader* s_pixelShader;

        CMaterialProcWater();
    };

    class CMaterialProcWaterFFP : public IMaterial {
        public:
        static CMaterialProcWaterFFP* Create();

        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);
    };

    class CMaterialWater : public IMaterial {
        public:
        static CMaterialWater* Create();

        virtual ~CMaterialWater();
        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);

        private:
        static CGxShader* s_vertexShaders[4];
        static CGxShader* s_pixelShader;

        CMaterialWater();
    };

    class CMaterialWaterNoSpec : public IMaterial {
        public:
        static CMaterialWaterNoSpec* Create();

        virtual ~CMaterialWaterNoSpec();
        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);

        private:
        static CGxShader* s_vertexShaders[4];
        static CGxShader* s_pixelShader;

        CMaterialWaterNoSpec();
    };

    class CMaterialWaterFFP : public IMaterial {
        public:
        static CMaterialWaterFFP* Create();

        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);
    };

    class CMaterialMagma : public IMaterial {
        public:
        static int32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;

        static CMaterialMagma* Create();

        virtual ~CMaterialMagma();
        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);

        CMaterialMagma();
    };

    class CMaterialMagmaFFP : public IMaterial {
        public:
        static CMaterialMagmaFFP* Create();

        virtual void Release();
        virtual void Render(IEnvironment* environment, IGeomFactory* geomFactory, IWaveManager* waveManager, const C3Vector& cameraPos, const C44Matrix* transform, const CAaSphere* bounds, CMaterialSettings* settings);
    };

    class IVertexData {
        public:
        /* 0004 */ uint32_t unused_0004;

        virtual ~IVertexData();
        virtual float GetHeight(uint32_t index) = 0;
        virtual uint8_t GetDepth(uint32_t index) = 0;
        virtual const uint16_t* GetTexCoord(uint32_t index) = 0;
    };

    class CChunkVertexData : public IVertexData {
        public:
        /* 0008 */ const uint8_t* vtxData;
        /* 000C */ uint32_t m_format; // LIQUID_VERTEX_FORMAT values in the native unsigned DWORD carrier.

        virtual float GetHeight(uint32_t index);
        virtual uint8_t GetDepth(uint32_t index);
        virtual const uint16_t* GetTexCoord(uint32_t index);

        CChunkVertexData(const uint8_t* data, uint32_t format);
    };

    class CAreaData;

    class CInstanceVertexData : public IVertexData {
        public:
        /* 0008 */ const uint8_t* dataPtr;
        /* 000C */ const SMLiquidInstance* instDesc;
        /* 0010 */ uint32_t m_vertexCount;

        virtual float GetHeight(uint32_t index);
        virtual uint8_t GetDepth(uint32_t index);
        virtual const uint16_t* GetTexCoord(uint32_t index);

        CInstanceVertexData(const CAreaData* areaData, const SMLiquidInstance* instance);
    };

    class CAreaData {
        public:
        static uint8_t s_emptyTable[8];
        static uint8_t s_fullTable[8];
        static uint16_t s_defaultTexCoord[2];

        /* 0000 */ uint32_t unused_0000;
        /* 0004 */ const uint8_t* dataPtr;

        CAreaData(const void* data);
        void SetData(const void* data);
        bool HasData() const;
        const SMLiquidChunkDesc* GetChunkDesc(uint32_t cx, uint32_t cy) const;
        const SMLiquidInstance* GetInstanceDesc(const SMLiquidChunkDesc* chunkDesc, uint32_t index) const;
        const uint8_t* GetFishableTable(const SMLiquidChunkDesc* chunkDesc) const;
        const uint8_t* GetDeepTable(const SMLiquidChunkDesc* chunkDesc) const;
        const uint8_t* GetExistsTable(const SMLiquidInstance* instDesc) const;
    };

    class CMaterialSettings {
        public:
        enum : uint32_t {
            INT_CNT = 4,
            FLOAT_CNT = 18,
            TEXTURE_CNT = 6,
            MAX_TEXNAME_LEN = 128,
        };

        /* 0000 */ char m_textureNames[TEXTURE_CNT][MAX_TEXNAME_LEN];
        /* 0300 */ CImVector m_colors[2];
        /* 0308 */ int32_t m_ints[INT_CNT];
        /* 0318 */ float m_floats[FLOAT_CNT];
        /* 0360 */ int32_t m_transparent;
        /* 0364 */ int32_t m_texturesReady[TEXTURE_CNT];
        /* 037C */ TSGrowableArray<CTexture*> m_textures[TEXTURE_CNT];
        /* 03DC */ TSGrowableArray<CTexture*> m_pendingTextures[TEXTURE_CNT];

        CMaterialSettings();
        ~CMaterialSettings();
        void Destroy();
        float GetFloat(uint32_t floatIdx) const;
        int32_t GetInt(uint32_t intIdx) const;
        CGxTex* GetTexture(uint32_t texIdx, uint32_t period);
        void InitTexture();
    };

    class CInstance {
        public:
        static CInstance* Create();

        /* 0000 */ IMaterial* m_material;
        /* 0004 */ CMaterialSettings* m_settings;
        /* 0008 */ IGeomFactory* m_geomFactory;
        /* 000C */ IWaveManager* m_waveManager;
        /* 0010 */ IEnvironment* m_environment;
        /* 0014 */ C44Matrix m_transform;
        /* 0054 */ CAaSphere m_bounds;
        /* 0064 */ int32_t m_queued;
        /* 0068 */ int32_t m_refCount;

        CInstance();
        void AddRef();
        void Purge();
        void Release();
    };

    class CRenderer {
        public:
        typedef int32_t (*DrawListSorter)(const void* a, const void* b);

        static DrawListSorter s_drawListSorter[2];

        static int32_t CompareOpaque(const void* a, const void* b);
        static int32_t CompareTransparent(const void* a, const void* b);

        private:
        /* 0000 */ TSGrowableArray<CInstance*> m_drawList[2];

        public:
        ~CRenderer();
        void Draw(CInstance* instance);
        void Flush(const C3Vector& cameraPos, uint32_t pass);
        void Validate();
    };

    class CSettingsBank {
        static TSGrowableArray<CMaterialSettings*> m_settings;

        public:
        static inline void Initialize();
        static void Destroy();
        static CMaterialSettings* GetSettings(uint32_t liquidType);
        static void ResetTextures();
        static bool UpdateRecord(CMaterialSettings* settings, uint32_t liquidType);
    };

    class CMaterialBank {
        static TSGrowableArray<IMaterial*> m_materials;

        public:
        static inline void Initialize();
        static void Destroy();
        static IMaterial* GetMaterial(uint32_t liquidType);
    };

    extern CDataAllocator s_settingsAllocator;
    extern CDataAllocator s_instanceAllocator;
    extern CDataAllocator s_procWaterAllocator;
    extern CDataAllocator s_procWaterFFPAllocator;
    extern CDataAllocator s_waterAllocator;
    extern CDataAllocator s_waterNoSpecAllocator;
    extern CDataAllocator s_waterFFPAllocator;
    extern CDataAllocator s_magmaAllocator;
    extern CDataAllocator s_magmaFFPAllocator;
    extern float s_worldCoordScale;
    extern int32_t s_specularEnabled;
    extern IDatabase<LiquidMaterialRec>* s_liquidMaterialDB;
    extern IDatabase<LiquidTypeRec>* s_liquidTypeDB;
    extern char s_shaderPath[256];
    extern int32_t s_shadersEnabled;
    extern int32_t s_useCameraRelativeCoords;
    extern int32_t s_settingsBankRefCount;
    extern int32_t s_materialBankRefCount;
    extern VSConstants s_vsConstants;
    extern PSConstants s_psConstants;

    void ClearMaterials();
    void CreateProceduralTextures();
    void DestroyProceduralTextures();
    const char* GetShaderSuffix();
    C44Matrix* GetTexScrollMtx(const C2Vector& speed);
    int32_t Initialize(BOOL useCameraRelativeCoords, float worldCoordScale, int32_t shadersEnabled, const char* shaderPostfix, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB);
    void InvalidateProceduralTextures();
    void SetProcWaterLighting(const CM2Lighting& lighting);
    // Parameters are LiquidType.dbc m_float[11..17] (the first scaled to world coordinates).
    void SetProcWaterParams(float liquidFloat11, float liquidFloat12, float liquidFloat13, float liquidFloat14, float liquidFloat15, float liquidFloat16, float liquidFloat17);
    void SetShaderConstants();
    void SetShaderLighting(const CM2Lighting& lighting);
    void SetShaderTransforms(const C3Vector& cameraPos, const C44Matrix& transform);
    void Shutdown();
    void UpdateProceduralTextures();
    void SetShadersEnabled(BOOL enabled);
    void SetSpecularEnabled(BOOL enabled);
    int32_t UseCameraRelativeCoords();
    float WorldCoordScale();
}

// INLINED
inline Liquid::IVertexData::~IVertexData() {
}

#endif
