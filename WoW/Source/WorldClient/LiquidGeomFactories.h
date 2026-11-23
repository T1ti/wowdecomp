#ifndef WOW_SOURCE_WORLDCLIENT_LIQUIDGEOMFACTORIES_H
#define WOW_SOURCE_WORLDCLIENT_LIQUIDGEOMFACTORIES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

class CChunkLiquid;
class CMapChunkLiquidBuf;
class CMapObj;
class CMapObjGroup;

namespace Liquid {
    class CChunkGeomFactory : public IGeomFactory {
        public:
        static CChunkGeomFactory* Create(const TSGrowableArray<CChunkLiquid*>& chunks);

        private:
        /* 0004 */ int32_t m_refs;
        /* 0008 */ BOOL m_dirty;
        /* 000C */ TSGrowableArray<CChunkLiquid*> m_chunks;
        /* 001C */ CMapChunkLiquidBuf* m_buf;
        /* 0020 */ CGxBatch m_batch;
        /* 0030 */ EGxVertexBufferFormat m_vtxFormat;
        /* 0034 */ C44Matrix m_transform;

        public:
        ~CChunkGeomFactory() override;
        void Release() override;
        void GetBuffers(EGxVertexBufferFormat vtxFormat, CGxBuf** vtxBuffer, CGxBuf** idxBuffer, CGxBatch* batch) override;
        void Purge() override;
        bool SetTransform(C44Matrix* transform) override;

        CChunkGeomFactory();
    };

    class CMeshGeomFactory : public IGeomFactory {
        public:
        static CMeshGeomFactory* Create(CMapObj* mapObj, CMapObjGroup* mapObjGroup);

        void SetDiffuseColor(const CImVector& color);
        void SetMagma(int32_t magma);
        void SetTexCoord1ValueX(float value);

        private:
        /* 0004 */ int32_t m_refs;
        /* 0008 */ CMapObj* m_mapObj;
        /* 000C */ CMapObjGroup* m_mapObjGroup;
        /* 0010 */ CGxBatch* m_batch;
        /* 0014 */ CImVector m_diffuseColor;
        /* 0018 */ int32_t unused_0018;
        /* 001C */ int32_t m_magma;
        /* 0020 */ float m_texCoord1ValueX;
        /* 0024 */ int32_t unused_0024;
        /* 0028 */ TSGrowableArray<uint8_t> m_sharedVerts[2];

        public:
        ~CMeshGeomFactory() override;
        void Release() override;
        void GetBuffers(EGxVertexBufferFormat vtxFormat, CGxBuf** vtxBuffer, CGxBuf** idxBuffer, CGxBatch* batch) override;
        void Purge() override;
        bool SetTransform(C44Matrix* transform) override;

        CMeshGeomFactory();
    };

    void ClearGeomFactories();
}

#endif
