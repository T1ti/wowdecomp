#include "WorldScene.h"
#include <new>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/WorldClient/CWorldOcclusion.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/LiquidWaveManager.h"
#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"

// STATIC: 0x00ADF350
float CWorldScene::s_fadeSizeDefault[5] = { 1.0f, 4.0f, 15.0f, 100.0f, 100000.0f };

// STATIC: 0x00ADF364
float CWorldScene::s_fadeDistDefault[5] = { 30.0f, 100.0f, 200.0f, 750.0f, 1250.0f };

// STATIC: 0x00ADF38C
float CWorldScene::s_fadeRangeDefault[5] = { 5.0f, 10.0f, 15.0f, 20.0f, 50.0f };

// STATIC: 0x00ADF3A0
float CWorldScene::s_fadeDist[5] = { 30.0f, 100.0f, 200.0f, 750.0f, 1250.0f };

// STATIC: 0x00ADF3C8
float CWorldScene::s_fadeDistMin[5] = { 25.0f, 90.0f, 185.0f, 730.0f, 1200.0f };

// STATIC: 0x00ADF3DC
float CWorldScene::s_fadeDistMinSqr[5] = { 625.0f, 8100.0f, 34225.0f, 532900.0f, 1440000.0f };

// STATIC: 0x00ADF6A8
TSExplicitList<CWFrustum, offsetof(CWFrustum, m_link)> CWorldScene::frustumFreeList;

// STATIC: 0x00B2EB68
GxuScene CWorldScene::s_unk_B2EB68;

// STATIC: 0x00CD8610
Liquid::CRenderer* CWorldScene::s_liquidRenderer;

// STATIC: 0x00CD8614
CTexture* CWorldScene::s_defaultBlendTexture;

// STATIC: 0x00CD8618
CTexture* CWorldScene::s_defaultTexture;

// STATIC: 0x00CD877C
float CWorldScene::s_fogFovPadding;

// STATIC: 0x00CD8794
uint32_t s_liquidTypeId;

// STATIC: 0x00CD87A8
CMapEntity* CWorldScene::camTargetEntity;

// STATIC: 0x00CD87B0
int32_t CWorldScene::s_cullPassStamp;

// STATIC: 0x00CD8F5C
C3Vector CWorldScene::s_activeWorldView;

// STATIC: 0x00CD8F68
C3Vector CWorldScene::camTarget;

// STATIC: 0x00D1C420
CMapObjDef* CWorldScene::s_curMapObjDef;

// VERIFY: bcfb85c1 auto
// OFFSET: 0x00798310
void CWorldScene::Destroy() {
    if (CWorldScene::s_defaultTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(CWorldScene::s_defaultTexture));
        CWorldScene::s_defaultTexture = nullptr;
    }

    if (CWorldScene::s_defaultBlendTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(CWorldScene::s_defaultBlendTexture));
        CWorldScene::s_defaultBlendTexture = nullptr;
    }

    if (CWorldScene::s_liquidRenderer) {
        Liquid::CRenderer* renderer = CWorldScene::s_liquidRenderer;
        renderer->~CRenderer();
        SMemFree(renderer, "delete", SLOG_FUNCTION, 0);
        CWorldScene::s_liquidRenderer = nullptr;
    }

    Liquid::ClearWaveManagers();
    Liquid::ClearGeomFactories();
    Liquid::ClearClientEnvironments();
    Liquid::Shutdown();
    CWorldScene::s_unk_B2EB68.Clear();
    CBarrier::Destroy();
    CWorldScene::frustumFreeList.Clear();
    s_occluderFreeList.Clear();
    s_antiOccluderFreeList.Clear();
}

// VERIFY: 09edb72e manual
// OFFSET: 0x007997D0
void CWorldScene::Initialize() {
    CBarrier::Initialize();

    CWorldScene::s_fogFovPadding = 0.0f;
    CWorldScene::s_cullPassStamp = 0;
    CWorldScene::camTargetEntity = nullptr;
    s_liquidTypeId = 0;

    Liquid::Initialize(1, 1.0f, 0, "", &g_liquidTypeDB, &g_liquidMaterialDB);

    CWorldScene::s_liquidRenderer = new (SMemAlloc(sizeof(Liquid::CRenderer), __FILE__, __LINE__, 0)) Liquid::CRenderer();

    CWorldOccluder::Initialize();

    CImVector color;
    color.b = 0x80;
    color.g = 0x80;
    color.r = 0x80;
    color.a = 0xFF;
    CWorldScene::s_defaultTexture = TextureCreateSolid(color);

    color.b = 0x00;
    color.g = 0x00;
    color.r = 0x00;
    color.a = 0xFF;
    CWorldScene::s_defaultBlendTexture = TextureCreateSolid(color);

    STORM_ASSERT(s_defaultTexture);
    STORM_ASSERT(s_defaultBlendTexture);
}

// VERIFY: 45953486 auto
// OFFSET: 0x0078F570
void CWorldScene::ScaleFadeDist(float scale) {
    CWorldView::s_fadeSize[0] = CWorldScene::s_fadeSizeDefault[0];
    CWorldScene::s_fadeDist[0] = CWorldScene::s_fadeDistDefault[0];
    CWorldScene::s_fadeDistMin[0] = CWorldScene::s_fadeDist[0] - CWorldScene::s_fadeRangeDefault[0];
    CWorldView::s_fadeDistSqr[0] = CWorldScene::s_fadeDist[0] * CWorldScene::s_fadeDist[0];
    CWorldScene::s_fadeDistMinSqr[0] = CWorldScene::s_fadeDistMin[0] * CWorldScene::s_fadeDistMin[0];

    CWorldView::s_fadeSize[1] = CWorldScene::s_fadeSizeDefault[1];
    CWorldScene::s_fadeDist[1] = CWorldScene::s_fadeDistDefault[1] * scale;
    CWorldScene::s_fadeDistMin[1] = CWorldScene::s_fadeDist[1] - CWorldScene::s_fadeRangeDefault[1];
    CWorldView::s_fadeDistSqr[1] = CWorldScene::s_fadeDist[1] * CWorldScene::s_fadeDist[1];
    CWorldScene::s_fadeDistMinSqr[1] = CWorldScene::s_fadeDistMin[1] * CWorldScene::s_fadeDistMin[1];

    CWorldView::s_fadeSize[2] = CWorldScene::s_fadeSizeDefault[2];
    CWorldScene::s_fadeDist[2] = CWorldScene::s_fadeDistDefault[2] * scale;
    CWorldScene::s_fadeDistMin[2] = CWorldScene::s_fadeDist[2] - CWorldScene::s_fadeRangeDefault[2];
    CWorldView::s_fadeDistSqr[2] = CWorldScene::s_fadeDist[2] * CWorldScene::s_fadeDist[2];
    CWorldScene::s_fadeDistMinSqr[2] = CWorldScene::s_fadeDistMin[2] * CWorldScene::s_fadeDistMin[2];

    CWorldView::s_fadeSize[3] = CWorldScene::s_fadeSizeDefault[3];
    CWorldScene::s_fadeDist[3] = CWorldScene::s_fadeDistDefault[3] * scale;
    CWorldScene::s_fadeDistMin[3] = CWorldScene::s_fadeDist[3] - CWorldScene::s_fadeRangeDefault[3];
    CWorldView::s_fadeDistSqr[3] = CWorldScene::s_fadeDist[3] * CWorldScene::s_fadeDist[3];
    CWorldScene::s_fadeDistMinSqr[3] = CWorldScene::s_fadeDistMin[3] * CWorldScene::s_fadeDistMin[3];

    CWorldView::s_fadeSize[4] = CWorldScene::s_fadeSizeDefault[4];
    CWorldScene::s_fadeDist[4] = CWorldScene::s_fadeDistDefault[4];
    CWorldScene::s_fadeDistMin[4] = CWorldScene::s_fadeDist[4] - CWorldScene::s_fadeRangeDefault[4];
    CWorldView::s_fadeDistSqr[4] = CWorldScene::s_fadeDist[4] * CWorldScene::s_fadeDist[4];
    CWorldScene::s_fadeDistMinSqr[4] = CWorldScene::s_fadeDistMin[4] * CWorldScene::s_fadeDistMin[4];
}

// VERIFY: 6c8ec1cb auto
// OFFSET: 0x007B3B20
void CMapObjDef::RenderWithCallback(const TSGrowableArray<uint32_t>& inGroups) {
    STORM_ASSERT(inGroups.Count() != 0);

    CMapObj::SetGroupRenderCallback(reinterpret_cast<RENDER_CALLBACK>(CWorldScene::AddMapObjDefGroupToSortTable), this);
    CWorldScene::s_curMapObjDef = this;
    this->mapObj->RenderInterior(this->m_mat, this->m_invMat, CWorldScene::s_activeWorldView, CWorldScene::camTarget, inGroups);
}
