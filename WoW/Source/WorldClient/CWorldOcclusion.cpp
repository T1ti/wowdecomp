#include "CWorldOcclusion.h"
#include <cfloat>
#include <malloc.h>
#include <cmath>
#include <cstring>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CPortalView.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

static const float CBSCALE = 64.0f;

// STATIC: 0x00AEEF60
TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> s_occluderFreeList;

// STATIC: 0x00AEEF6C
TSExplicitList<CWorldAntiOccluder, offsetof(CWorldAntiOccluder, m_link)> s_antiOccluderFreeList;

// STATIC: 0x00D2DCD8
TSGrowableArray<C4Plane> CWorldOcclusion::planeArray;

// STATIC: 0x00D2DCE8
TSGrowableArray<CClipVolume> CWorldOcclusion::clipVolumeArray;

// STATIC: 0x00ADF3F4
static int32_t aabbCornerSelectX[8] = { 0, 1, 1, 0, 0, 1, 1, 0 };

// STATIC: 0x00ADF414
static int32_t aabbCornerSelectY[8] = { 0, 0, 1, 1, 0, 0, 1, 1 };

// STATIC: 0x00ADF434
static int32_t aabbCornerSelectZ[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };

// STATIC: 0x00CD8FD8
static C3Vector s_clipBufferVerts[9];

// STATIC: 0x00D2DCD4
static int32_t s_occlusionVolumesInit;

// STATIC: 0x00AEFCE0
static C3Vector s_occlusionVolumeVerts0[4] = { C3Vector(-8814.79f, 474.86f, 0.0f), C3Vector(-8814.79f, 474.86f, 138.87f), C3Vector(-8986.46f, 679.74f, 138.87f), C3Vector(-8986.46f, 679.74f, 0.0f) };

// STATIC: 0x00AEF318
static C3Vector s_occlusionVolumeVerts1[8] = { C3Vector(5567.66f, 714.34f, 596.68f), C3Vector(5605.32f, 394.26f, 596.68f), C3Vector(5888.74f, 382.14f, 596.68f), C3Vector(6026.98f, 494.67f, 596.68f), C3Vector(6051.12f, 634.18f, 596.68f), C3Vector(5952.54f, 849.71f, 596.68f), C3Vector(5793.61f, 991.54f, 596.68f), C3Vector(5582.06f, 799.39f, 596.68f) };

// STATIC: 0x00AEF378
static C3Vector s_occlusionVolumeVerts2[6] = { C3Vector(5634.5f, 419.0f, 630.4f), C3Vector(5612.57f, 814.0f, 630.4f), C3Vector(5810.19f, 953.83f, 630.4f), C3Vector(5955.67f, 785.4f, 630.4f), C3Vector(6025.93f, 545.39f, 630.4f), C3Vector(5817.91f, 394.35f, 630.4f) };

// STATIC: 0x00AEF3C0
static C3Vector s_occlusionVolumeVerts3[4] = { C3Vector(5892.49f, 627.3f, 620.0f), C3Vector(5892.49f, 627.3f, 664.0f), C3Vector(5990.5f, 557.7f, 664.0f), C3Vector(5990.5f, 557.7f, 620.0f) };

// STATIC: 0x00AEF3F0
static C3Vector s_occlusionVolumeVerts4[4] = { C3Vector(5847.0f, 539.0f, 620.0f), C3Vector(5847.0f, 539.0f, 685.0f), C3Vector(5800.0f, 526.0f, 685.0f), C3Vector(5800.0f, 526.0f, 620.0f) };

// STATIC: 0x00AEF420
static C3Vector s_occlusionVolumeVerts5[4] = { C3Vector(5827.39f, 603.35f, 620.34f), C3Vector(5827.39f, 603.35f, 679.34f), C3Vector(5786.06f, 590.07f, 678.06f), C3Vector(5786.06f, 590.07f, 620.06f) };

// STATIC: 0x00AEF450
static C3Vector s_occlusionVolumeVerts6[4] = { C3Vector(5730.99f, 679.31f, 620.76f), C3Vector(5730.99f, 679.31f, 670.76f), C3Vector(5679.87f, 720.15f, 670.76f), C3Vector(5679.87f, 720.15f, 620.76f) };

// STATIC: 0x00AEF480
static C3Vector s_occlusionVolumeVerts7[4] = { C3Vector(5774.5f, 655.43f, 620.15f), C3Vector(5774.5f, 655.43f, 661.15f), C3Vector(5762.92f, 611.93f, 661.15f), C3Vector(5762.92f, 611.93f, 620.15f) };

// STATIC: 0x00AEF4B0
static C3Vector s_occlusionVolumeVerts8[4] = { C3Vector(5784.6499f, 664.97f, 620.02f), C3Vector(5784.6499f, 664.97f, 658.02f), C3Vector(5823.0f, 686.91f, 658.02f), C3Vector(5823.0f, 686.91f, 620.02f) };

// STATIC: 0x00AEF4E0
static C3Vector s_occlusionVolumeVerts9[4] = { C3Vector(5841.56f, 633.28f, 620.66f), C3Vector(5841.56f, 633.28f, 669.66f), C3Vector(5845.92f, 669.59f, 664.72f), C3Vector(5845.92f, 669.59f, 620.72f) };

// STATIC: 0x00AEF510
static C3Vector s_occlusionVolumeVerts10[4] = { C3Vector(5904.17f, 618.49f, 620.41f), C3Vector(5904.17f, 618.49f, 672.41f), C3Vector(5883.92f, 591.09f, 672.36f), C3Vector(5883.92f, 591.09f, 620.36f) };

// STATIC: 0x00AEF540
static C3Vector s_occlusionVolumeVerts11[4] = { C3Vector(5895.04f, 627.82f, 620.553f), C3Vector(5895.04f, 627.82f, 649.553f), C3Vector(5834.78f, 553.96f, 649.553f), C3Vector(5834.78f, 553.96f, 620.553f) };

// STATIC: 0x00AEF570
static C3Vector s_occlusionVolumeVerts12[4] = { C3Vector(5899.41f, 668.11f, 620.54f), C3Vector(5899.41f, 668.11f, 676.54f), C3Vector(5941.06f, 714.99f, 661.17f), C3Vector(5941.06f, 714.99f, 620.17f) };

// STATIC: 0x00AEF5A0
static C3Vector s_occlusionVolumeVerts13[4] = { C3Vector(5864.97f, 751.17f, 620.59f), C3Vector(5864.97f, 751.17f, 669.59f), C3Vector(5835.75f, 720.71f, 669.59f), C3Vector(5835.75f, 720.71f, 620.59f) };

// STATIC: 0x00AEF5D0
static C3Vector s_occlusionVolumeVerts14[5] = { C3Vector(5636.4502f, 398.96f, 630.0f), C3Vector(5947.9502f, 827.4f, 630.0f), C3Vector(5924.75f, 796.8f, 574.11f), C3Vector(5806.25f, 636.31f, 397.4f), C3Vector(5636.4502f, 398.96f, 574.11f) };

// STATIC: 0x00AEF60C
static C3Vector s_occlusionVolumeVerts15[5] = { C3Vector(5590.91f, 783.86f, 630.0f), C3Vector(6006.9f, 537.08f, 630.0f), C3Vector(5971.75f, 552.69f, 558.75f), C3Vector(5806.25f, 636.31f, 397.4f), C3Vector(5631.61f, 751.34f, 527.85f) };

// STATIC: 0x00AEF648
static C3Vector s_occlusionVolumeVerts16[4] = { C3Vector(5749.8501f, 865.61f, 630.0f), C3Vector(5749.8501f, 865.61f, 704.0f), C3Vector(5864.17f, 851.47f, 704.0f), C3Vector(5864.17f, 851.47f, 630.0f) };

// STATIC: 0x00AEF678
static C3Vector s_occlusionVolumeVerts17[4] = { C3Vector(5800.51f, 816.42f, 630.0f), C3Vector(5800.51f, 816.42f, 704.0f), C3Vector(5814.84f, 906.58f, 704.0f), C3Vector(5814.84f, 906.58f, 630.0f) };

// STATIC: 0x00AEF6A8
static C3Vector s_occlusionVolumeVerts18[4] = { C3Vector(5868.96f, 572.64f, 623.44f), C3Vector(5868.96f, 572.64f, 665.44f), C3Vector(5847.01f, 539.19f, 665.44f), C3Vector(5847.01f, 539.19f, 623.44f) };

// STATIC: 0x00AEF6D8
static C3Vector s_occlusionVolumeVerts19[4] = { C3Vector(5867.43f, 720.05f, 624.9f), C3Vector(5867.43f, 720.05f, 654.9f), C3Vector(5893.14f, 700.98f, 654.69f), C3Vector(5893.14f, 700.98f, 624.69f) };

// STATIC: 0x00AEF708
static C3Vector s_occlusionVolumeVerts20[7] = { C3Vector(2478.83f, -5620.13f, 366.5f), C3Vector(2491.95f, -5599.4f, 366.5f), C3Vector(2487.5f, -5574.9502f, 366.5f), C3Vector(2466.24f, -5561.56f, 366.5f), C3Vector(2441.82f, -5566.63f, 366.5f), C3Vector(2433.41f, -5612.21f, 366.5f), C3Vector(2454.25f, -5625.66f, 366.5f) };

// STATIC: 0x00AEF75C
static C3Vector s_occlusionVolumeVerts21[4] = { C3Vector(2510.17f, -5702.89f, 364.0f), C3Vector(2491.45f, -5658.32f, 520.0f), C3Vector(2429.59f, -5523.72f, 520.0f), C3Vector(2413.68f, -5483.44f, 364.0f) };

// STATIC: 0x00AEF790
static C3Vector s_occlusionVolumeVerts22[7] = { C3Vector(2370.95f, -5680.14f, 382.0f), C3Vector(2351.14f, -5635.16f, 382.0f), C3Vector(2377.8f, -5472.44f, 382.0f), C3Vector(2512.1499f, -5448.47f, 382.0f), C3Vector(2586.21f, -5551.42f, 382.0f), C3Vector(2537.38f, -5704.73f, 382.0f), C3Vector(2425.22f, -5744.56f, 382.0f) };

// STATIC: 0x00AEF7E4
static C3Vector s_occlusionVolumeVerts23[5] = { C3Vector(2364.06f, -5692.39f, 382.24f), C3Vector(2347.53f, -5672.2f, 382.24f), C3Vector(2333.75f, -5647.47f, 382.24f), C3Vector(2353.04f, -5636.88f, 382.24f), C3Vector(2382.53f, -5679.76f, 382.24f) };

// STATIC: 0x00AEF820
static C3Vector s_occlusionVolumeVerts24[4] = { C3Vector(1204.96f, -4963.32f, 0.0f), C3Vector(1198.36f, -4933.69f, 346.08f), C3Vector(1158.87f, -4823.17f, 344.76f), C3Vector(1149.31f, -4791.83f, 0.0f) };

// STATIC: 0x00AEF850
static C3Vector s_occlusionVolumeVerts25[4] = { C3Vector(1219.4f, -5004.42f, 0.0f), C3Vector(1219.4f, -5004.42f, 173.61f), C3Vector(1128.43f, -4743.66f, 173.13f), C3Vector(1128.43f, -4743.66f, 0.0f) };

// STATIC: 0x00AEF880
static C3Vector s_occlusionVolumeVerts26[7] = { C3Vector(1111.31f, -4819.9f, 172.49f), C3Vector(1090.0f, -4809.76f, 172.49f), C3Vector(1073.63f, -4794.27f, 172.49f), C3Vector(1069.92f, -4767.41f, 172.49f), C3Vector(1098.59f, -4757.36f, 172.49f), C3Vector(1163.56f, -4754.04f, 172.49f), C3Vector(1167.65f, -4794.54f, 172.49f) };

// STATIC: 0x00AEF8D4
static C3Vector s_occlusionVolumeVerts27[5] = { C3Vector(1142.32f, -5006.38f, 172.12f), C3Vector(1153.18f, -4968.25f, 172.12f), C3Vector(1254.25f, -4932.14f, 172.12f), C3Vector(1286.37f, -4988.77f, 172.12f), C3Vector(1156.18f, -5028.96f, 172.12f) };

// STATIC: 0x00AEF910
static C3Vector s_occlusionVolumeVerts28[4] = { C3Vector(2181.39f, 5312.53f, -50.0f), C3Vector(2181.39f, 5312.53f, 44.97f), C3Vector(2116.82f, 5332.81f, 44.97f), C3Vector(2116.82f, 5332.81f, -50.0f) };

// STATIC: 0x00AEF940
static C3Vector s_occlusionVolumeVerts29[4] = { C3Vector(2116.82f, 5332.81f, -50.0f), C3Vector(2116.82f, 5332.81f, 44.97f), C3Vector(2100.22f, 5284.27f, 44.97f), C3Vector(2100.22f, 5284.27f, -50.0f) };

// STATIC: 0x00AEF970
static C3Vector s_occlusionVolumeVerts30[4] = { C3Vector(2324.46f, 5188.88f, -50.0f), C3Vector(2324.46f, 5188.88f, 38.29f), C3Vector(2273.21f, 5209.71f, 38.29f), C3Vector(2273.21f, 5209.71f, -50.0f) };

// STATIC: 0x00AEF9A0
static C3Vector s_occlusionVolumeVerts31[4] = { C3Vector(2275.63f, 5190.17f, -50.0f), C3Vector(2275.63f, 5190.17f, 39.58f), C3Vector(2286.96f, 5210.31f, 39.58f), C3Vector(2286.96f, 5210.31f, -50.0f) };

// STATIC: 0x00AEF9D0
static C3Vector s_occlusionVolumeVerts32[4] = { C3Vector(2878.06f, 6177.6f, 207.71f), C3Vector(2937.51f, 6113.32f, 207.71f), C3Vector(2905.17f, 6082.4f, 207.71f), C3Vector(2845.54f, 6145.42f, 207.71f) };

// STATIC: 0x00AEFA00
static C3Vector s_occlusionVolumeVerts33[4] = { C3Vector(2763.39f, 6089.57f, 207.81f), C3Vector(2828.63f, 6147.74f, 207.81f), C3Vector(2797.2f, 6179.5498f, 207.81f), C3Vector(2733.97f, 6120.93f, 207.81f) };

// STATIC: 0x00AEFA30
static C3Vector s_occlusionVolumeVerts34[4] = { C3Vector(2741.27f, 6261.16f, 207.81f), C3Vector(2799.12f, 6196.67f, 207.81f), C3Vector(2831.21f, 6228.19f, 207.81f), C3Vector(2773.36f, 6292.17f, 207.81f) };

// STATIC: 0x00AEFA60
static C3Vector s_occlusionVolumeVerts35[4] = { C3Vector(2902.29f, 6274.68f, 202.81f), C3Vector(2846.68f, 6224.76f, 202.81f), C3Vector(2880.4f, 6195.17f, 202.81f), C3Vector(2931.48f, 6241.73f, 202.81f) };

// STATIC: 0x00AEFA90
static C3Vector s_occlusionVolumeVerts36[4] = { C3Vector(2926.83f, 6120.47f, 79.05f), C3Vector(2781.04f, 6278.5498f, 79.08f), C3Vector(2772.78f, 6291.58f, 203.9f), C3Vector(2935.23f, 6114.39f, 192.48f) };

// STATIC: 0x00AEFAC0
static C3Vector s_occlusionVolumeVerts37[4] = { C3Vector(2853.66f, 6199.46f, 202.5f), C3Vector(2910.86f, 6255.33f, 200.34f), C3Vector(2885.69f, 6230.23f, 74.69f), C3Vector(2860.12f, 6206.81f, 82.69f) };

// STATIC: 0x00AEFAF0
static C3Vector s_occlusionVolumeVerts38[4] = { C3Vector(2826.04f, 6175.62f, 80.37f), C3Vector(2760.6499f, 6116.42f, 78.86f), C3Vector(2747.8f, 6104.57f, 203.55f), C3Vector(2826.79f, 6176.22f, 203.27f) };

// STATIC: 0x00AEFB20
static C3Vector s_occlusionVolumeVerts39[5] = { C3Vector(2924.92f, 6123.46f, 191.494f), C3Vector(2940.1f, 6198.47f, 191.494f), C3Vector(2914.91f, 6257.2f, 191.494f), C3Vector(2860.75f, 6282.54f, 191.494f), C3Vector(2781.79f, 6276.07f, 191.494f) };

// STATIC: 0x00AEFB5C
static C3Vector s_occlusionVolumeVerts40[4] = { C3Vector(3968.22f, -3566.96f, 256.79f), C3Vector(3968.22f, -3566.96f, 377.16f), C3Vector(3940.76f, -3451.53f, 354.33f), C3Vector(3940.76f, -3451.53f, 291.48f) };

// STATIC: 0x00AEFB8C
static C3Vector s_occlusionVolumeVerts41[4] = { C3Vector(4792.57f, -1936.71f, 160.0f), C3Vector(4792.57f, -1936.71f, 295.2f), C3Vector(4791.64f, -2034.22f, 295.2f), C3Vector(4791.64f, -2034.22f, 160.0f) };

// STATIC: 0x00AEFBBC
static C3Vector s_occlusionVolumeVerts42[4] = { C3Vector(4790.41f, -2159.31f, 295.2f), C3Vector(4790.41f, -2159.31f, 160.0f), C3Vector(4791.3501f, -2056.96f, 160.0f), C3Vector(4791.3501f, -2056.96f, 295.2f) };

// STATIC: 0x00AEFD88
static C3Vector s_occlusionVolumeVerts43[5] = { C3Vector(-208.17f, -661.93f, 120.12f), C3Vector(-232.25f, -655.88f, 120.12f), C3Vector(-262.43f, -661.54f, 120.12f), C3Vector(-261.61f, -693.07f, 120.12f), C3Vector(-220.75f, -701.13f, 120.12f) };

// STATIC: 0x00AEFDC4
static C3Vector s_occlusionVolumeVerts44[4] = { C3Vector(-336.99f, -570.87f, 46.54f), C3Vector(-327.17f, -684.76f, 46.54f), C3Vector(-510.79f, -626.06f, 46.54f), C3Vector(-467.16f, -575.2f, 46.54f) };

// STATIC: 0x00AEFDF4
static C3Vector s_occlusionVolumeVerts45[4] = { C3Vector(-336.72f, -777.35f, 0.0f), C3Vector(-336.72f, -777.35f, 126.6f), C3Vector(-335.71f, -707.36f, 126.6f), C3Vector(-335.71f, -707.36f, 0.0f) };

// STATIC: 0x00AEFE24
static C3Vector s_occlusionVolumeVerts46[4] = { C3Vector(-364.9f, -637.23f, 0.0f), C3Vector(-364.9f, -637.23f, 85.53f), C3Vector(-365.04f, -806.45f, 87.68f), C3Vector(-365.04f, -806.45f, 0.0f) };

// STATIC: 0x00AEFE54
static C3Vector s_occlusionVolumeVerts47[4] = { C3Vector(-227.26f, -760.09f, 0.0f), C3Vector(-227.26f, -760.09f, 101.34f), C3Vector(-444.86f, -740.53f, 91.18f), C3Vector(-444.86f, -740.53f, 0.0f) };

// STATIC: 0x00AEFE84
static C3Vector s_occlusionVolumeVerts48[5] = { C3Vector(-287.79f, -547.27f, 6.36f), C3Vector(-227.24f, -735.28f, 6.36f), C3Vector(-393.96f, -846.24f, 6.36f), C3Vector(-499.18f, -683.28f, 6.36f), C3Vector(-441.81f, -599.64f, 6.36f) };

// STATIC: 0x00AEFBEC
static C3Vector s_occlusionVolumeVerts49[4] = { C3Vector(1530.73f, -3570.53f, 158.3f), C3Vector(1418.82f, -3136.56f, 158.3f), C3Vector(1418.82f, -3136.56f, 0.0f), C3Vector(1530.73f, -3570.53f, 0.0f) };

// STATIC: 0x00AEFC1C
static C3Vector s_occlusionVolumeVerts50[4] = { C3Vector(1579.05f, -3461.45f, 147.2f), C3Vector(1535.65f, -3238.25f, 147.2f), C3Vector(1447.48f, -3239.19f, 147.2f), C3Vector(1467.61f, -3591.0f, 147.2f) };

// STATIC: 0x00AEFC50
static C3Vector s_occlusionVolumeVerts51[7] = { C3Vector(1713.96f, -3423.56f, 133.84f), C3Vector(1637.58f, -3417.66f, 133.84f), C3Vector(1608.12f, -3365.9f, 133.84f), C3Vector(1608.5f, -3307.29f, 133.84f), C3Vector(1635.45f, -3263.94f, 133.84f), C3Vector(1687.9f, -3252.33f, 133.84f), C3Vector(1747.5f, -3271.53f, 133.84f) };

// STATIC: 0x00AEFCA4
static C3Vector s_occlusionVolumeVerts52[5] = { C3Vector(1747.58f, -3461.09f, 139.0f), C3Vector(1658.71f, -3446.07f, 139.0f), C3Vector(1625.67f, -3548.94f, 139.0f), C3Vector(1676.45f, -3612.46f, 139.0f), C3Vector(1720.96f, -3582.86f, 139.0f) };

// STATIC: 0x00AEFD10
static C3Vector s_occlusionVolumeVerts53[5] = { C3Vector(301.84f, -475.68f, 104.58f), C3Vector(293.69f, -509.9f, 104.58f), C3Vector(307.11f, -528.01f, 104.58f), C3Vector(485.31f, -525.93f, 104.58f), C3Vector(496.53f, -493.97f, 104.58f) };

// STATIC: 0x00AEFD4C
static C3Vector s_occlusionVolumeVerts54[5] = { C3Vector(481.86f, -273.24f, 103.43f), C3Vector(499.19f, -264.26f, 103.43f), C3Vector(490.12f, -233.92f, 103.43f), C3Vector(304.62f, -229.97f, 103.43f), C3Vector(302.91f, -268.57f, 103.43f) };

// STATIC: 0x00AEFEC0
static C3Vector s_occlusionVolumeVerts55[4] = { C3Vector(2087.0f, 300.0f, 503.0f), C3Vector(2087.0f, -300.0f, 503.0f), C3Vector(2087.0f, -300.0f, -2000.0f), C3Vector(2087.0f, 300.0f, -2000.0f) };

// STATIC: 0x00AEFEF0
static C3Vector s_occlusionVolumeVerts56[4] = { C3Vector(2087.0f, -300.0f, 500.0f), C3Vector(2531.0f, -288.0f, 500.0f), C3Vector(2531.0f, -288.0f, -2000.0f), C3Vector(2087.0f, -300.0f, -2000.0f) };

// STATIC: 0x00AEFF20
static C3Vector s_occlusionVolumeVerts57[4] = { C3Vector(2087.0f, 300.0f, 500.0f), C3Vector(2538.0f, 212.0f, 500.0f), C3Vector(2538.0f, 212.0f, -2000.0f), C3Vector(2087.0f, 300.0f, -2000.0f) };

// STATIC: 0x00AEFF50
static C3Vector s_occlusionVolumeVerts58[4] = { C3Vector(9000.0f, -1300.0f, 10000.0f), C3Vector(9000.0f, -1300.0f, -10000.0f), C3Vector(-9000.0f, -1300.0f, -10000.0f), C3Vector(-9000.0f, -1300.0f, 10000.0f) };

// STATIC: 0x00AEFF80
static C3Vector s_occlusionVolumeVerts59[8] = { C3Vector(-274.3f, 2212.7f, 175.0f), C3Vector(-316.2f, 2120.5f, 175.0f), C3Vector(-435.9f, 2033.8f, 175.0f), C3Vector(-549.5f, 2104.6f, 175.0f), C3Vector(-613.6f, 2212.3f, 175.0f), C3Vector(-547.4f, 2320.6f, 175.0f), C3Vector(-438.1f, 2382.3f, 175.0f), C3Vector(-326.9f, 2320.5f, 175.0f) };

// STATIC: 0x00AEFFE0
static C3Vector s_occlusionVolumeVerts60[4] = { C3Vector(-470.6f, 2243.3f, 614.0f), C3Vector(-520.1f, 2293.4f, -163.9f), C3Vector(-336.1f, 2116.0f, -163.3f), C3Vector(-405.7f, 2179.5f, 614.0f) };

// STATIC: 0x00AF0010
static C3Vector s_occlusionVolumeVerts61[4] = { C3Vector(-475.6f, 2173.8f, 614.0f), C3Vector(-502.2f, 2148.7f, -158.0f), C3Vector(-344.4f, 2309.0f, -158.0f), C3Vector(-403.9f, 2243.7f, 614.0f) };

// STATIC: 0x00AF0040
static OcclusionVolume s_occlusionVolumes[62] = {
    { 0, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts0, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts1, 8 } },
    { 571, 2, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts2, 6 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts3, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts4, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts5, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts6, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts7, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts8, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts9, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts10, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts11, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts12, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts13, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts14, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts15, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts16, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts17, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts18, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts19, 4 } },
    { 609, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts20, 7 } },
    { 609, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts21, 4 } },
    { 609, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts22, 7 } },
    { 609, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts23, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts24, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts25, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts26, 7 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts27, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts28, 4 } },
    { 571, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts29, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts30, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts31, 4 } },
    { 571, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts32, 4 } },
    { 571, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts33, 4 } },
    { 571, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts34, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts35, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts36, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts37, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts38, 4 } },
    { 571, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts39, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts40, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts41, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts42, 4 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts43, 5 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts44, 4 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts45, 4 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts46, 4 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts47, 4 } },
    { 600, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts48, 5 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts49, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts50, 4 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts51, 7 } },
    { 571, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts52, 5 } },
    { 575, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts53, 5 } },
    { 575, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts54, 5 } },
    { 603, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts55, 4 } },
    { 603, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts56, 4 } },
    { 603, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts57, 4 } },
    { 631, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts58, 4 } },
    { 631, 0, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts59, 8 } },
    { 631, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts60, 4 } },
    { 631, 1, { CAaBox(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f)), s_occlusionVolumeVerts61, 4 } },
};

// STATIC: 0x00AEEF78
static C3Vector s_worldOccluderVerts0[22] = { C3Vector(2287.73f, 5169.48f, 44.17f), C3Vector(2297.8f, 5165.72f, 44.17f), C3Vector(2296.86f, 5166.19f, 33.63f), C3Vector(2328.38f, 5159.77f, 33.63f), C3Vector(2327.02f, 5159.73f, 45.0f), C3Vector(2337.21f, 5163.88f, 45.0f), C3Vector(2335.61f, 5162.59f, 30.9f), C3Vector(2364.9f, 5187.67f, 30.9f), C3Vector(2364.08f, 5186.96f, 39.15f), C3Vector(2372.46f, 5196.06f, 39.15f), C3Vector(2372.96f, 5192.9f, 27.81f), C3Vector(2381.72f, 5230.61f, 27.81f), C3Vector(2381.62f, 5229.2f, 40.9f), C3Vector(2381.38f, 5239.99f, 40.9f), C3Vector(2370.56f, 5272.84f, 47.0f), C3Vector(2365.48f, 5281.81f, 47.0f), C3Vector(2366.57f, 5281.31f, 33.1f), C3Vector(2342.17f, 5301.84f, 33.1f), C3Vector(2341.39f, 5302.19f, 50.63f), C3Vector(2325.29f, 5313.29f, 50.63f), C3Vector(2325.29f, 5313.29f, 50.63f), C3Vector(2314.61f, 5303.97f, 50.63f) };

// STATIC: 0x00AEF080
static C3Vector s_worldOccluderVerts1[16] = { C3Vector(2189.1f, 5348.63f, 44.74f), C3Vector(2200.34f, 5343.43f, 44.74f), C3Vector(2122.23f, 5368.76f, 32.36f), C3Vector(2076.05f, 5340.02f, 32.36f), C3Vector(2076.05f, 5340.02f, 32.36f), C3Vector(2064.68f, 5300.59f, 32.36f), C3Vector(2065.1f, 5302.51f, 64.32f), C3Vector(2063.47f, 5290.36f, 64.32f), C3Vector(2062.59f, 5291.6f, 51.33f), C3Vector(2077.09f, 5252.01f, 51.33f), C3Vector(2077.09f, 5252.01f, 51.33f), C3Vector(2109.76f, 5225.79f, 51.33f), C3Vector(2108.5f, 5228.31f, 39.76f), C3Vector(2190.55f, 5195.1f, 39.76f), C3Vector(2189.1f, 5348.63f, 44.74f), C3Vector(2200.34f, 5343.43f, 44.74f) };

// STATIC: 0x00AEF140
static C3Vector s_worldOccluderVerts2[6] = { C3Vector(701.41f, -5045.22f, 25.0f), C3Vector(702.15f, -5032.52f, 25.0f), C3Vector(702.32f, -5034.81f, 15.81f), C3Vector(704.03f, -4994.79f, 15.81f), C3Vector(703.46f, -4995.75f, 19.43f), C3Vector(701.07f, -4983.98f, 19.43f) };

// STATIC: 0x00AEF188
static C3Vector s_worldOccluderVerts3[6] = { C3Vector(693.52f, -4946.7f, 20.3f), C3Vector(690.32f, -4934.54f, 19.36f), C3Vector(691.16f, -4936.69f, 15.4f), C3Vector(675.85f, -4907.31f, 15.4f), C3Vector(676.57f, -4907.72f, 28.38f), C3Vector(670.09f, -4896.62f, 28.38f) };

// STATIC: 0x00AEF1D0
static C3Vector s_worldOccluderVerts4[22] = { C3Vector(3922.27f, -3780.83f, 223.88f), C3Vector(3931.13f, -3816.6499f, 225.3f), C3Vector(3930.43f, -3816.25f, 247.05f), C3Vector(3972.51f, -3865.33f, 246.88f), C3Vector(3985.02f, -3862.98f, 266.38f), C3Vector(4019.3f, -3871.8501f, 266.38f), C3Vector(4018.84f, -3870.28f, 283.14f), C3Vector(4057.42f, -3866.95f, 283.6f), C3Vector(4057.42f, -3866.95f, 283.6f), C3Vector(4090.54f, -3839.41f, 283.14f), C3Vector(4097.93f, -3813.61f, 312.86f), C3Vector(4108.39f, -3773.72f, 310.56f), C3Vector(4108.39f, -3773.72f, 310.56f), C3Vector(4085.14f, -3743.49f, 312.86f), C3Vector(4070.07f, -3716.09f, 279.63f), C3Vector(4032.64f, -3689.24f, 283.55f), C3Vector(4032.64f, -3689.24f, 283.55f), C3Vector(4005.67f, -3693.7f, 279.63f), C3Vector(4003.38f, -3697.04f, 266.74f), C3Vector(3963.34f, -3709.25f, 240.45f), C3Vector(3963.34f, -3709.25f, 240.45f), C3Vector(3936.94f, -3745.73f, 230.22f) };

// STATIC: 0x00D2DC20
static WorldOccluderData s_worldOccluders[5] = {
    WorldOccluderData(571, -100.0f, s_worldOccluderVerts0, 22),
    WorldOccluderData(571, -100.0f, s_worldOccluderVerts1, 16),
    WorldOccluderData(571, -50.0f, s_worldOccluderVerts2, 6),
    WorldOccluderData(571, -50.0f, s_worldOccluderVerts3, 6),
    WorldOccluderData(571, -100.0f, s_worldOccluderVerts4, 22)
};

// VERIFY: f52cc1f6 manual
// OFFSET: 0x005EEB70
void CWorldOccluder::Initialize() {
}

// VERIFY: 4db2afc6 manual
// OFFSET: 0x0078F6A0
void CWorldOcclusion::UpdateBufferChunk(const C3Vector* verts, const float* heights, const uint32_t* indices, int32_t count, const C3Vector* offset, int32_t holes) {
    if (!(CWorld::enables & Enables::Enable_Culling) || CWorldScene::camVec.z < -0.9f || CWorldScene::camVec.z > 0.9f) {
        return;
    }

    C3Vector* projected = s_clipBufferVerts;

    for (int32_t i = 0; i < count; i++) {
        projected->x = verts[*indices].x + offset->x;
        projected->y = verts[*indices].y + offset->y;
        projected->z = heights[*indices] + offset->z;
        *projected = *projected * CWorldScene::s_clipBufferMatrix;

        float inv = 1.0f / projected->z;
        projected++;
        indices++;
        projected[-1].x = projected[-1].x * inv;
        projected[-1].y = inv * projected[-1].y;
    }

    if (holes) {
        for (int32_t i = 0; i < count - 1; i++) {
            int32_t x0 = CMath::fint_(s_clipBufferVerts[i].x * CBSCALE - 0.5f) + 192;
            int32_t x1 = CMath::fint_(s_clipBufferVerts[i + 1].x * CBSCALE - 0.5f) + 192;

            if (x1 < x0) {
                int32_t swap = x0;
                x0 = x1;
                x1 = swap;
            }

            if (x0 < 0) {
                x0 = 0;
            }

            if (x1 >= 384) {
                x1 = 383;
            }

            for (; x0 <= x1; x0++) {
                if (!(CWorldScene::s_clipBufferMask[x0] & 1)) {
                    CWorldScene::s_clipBuffer[x0] = -1000001.0f;
                }
            }
        }

        return;
    }

    for (int32_t i = 0; i < count - 1; i++) {
        const C3Vector& p0 = s_clipBufferVerts[i];
        const C3Vector& p1 = s_clipBufferVerts[i + 1];

        if (p0.z < 0.027777778f || p1.z < 0.027777778f) {
            continue;
        }

        float height = p1.y <= p0.y ? p1.y : p0.y;
        int32_t x0 = CMath::fint_(p0.x * CBSCALE - 0.5f) + 192;
        int32_t x1 = CMath::fint_(p1.x * CBSCALE - 0.5f) + 192;

        if (x1 < x0) {
            int32_t swap = x0;
            x0 = x1;
            x1 = swap;
        }

        if (x0 < 0) {
            x0 = 0;
        }

        if (x1 >= 384) {
            x1 = 383;
        }

        for (; x0 <= x1; x0++) {
            if (height > CWorldScene::s_clipBuffer[x0]) {
                CWorldScene::s_clipBuffer[x0] = height;
            }
        }
    }
}

// VERIFY: 5b9f55f1 manual
// OFFSET: 0x0078F900
void CWorldOcclusion::UpdateBuffer(const C3Vector* points, int32_t count, uint32_t flags) {
    if (!(CWorld::enables & Enables::Enable_Culling) || CWorldScene::camVec.z < -0.9f || CWorldScene::camVec.z > 0.9f) {
        return;
    }

    C3Vector* projected = s_clipBufferVerts;

    for (int32_t i = 0; i < count; i++) {
        *projected = *points * CWorldScene::s_clipBufferMatrix;

        float inv = 1.0f / projected->z;

        if (inv > 0.0f) {
            projected->x = projected->x * inv;
            projected->y = inv * projected->y;
        }

        points++;
        projected++;
    }

    for (int32_t i = 0; i < count - 1; i++) {
        const C3Vector& p0 = s_clipBufferVerts[i];
        const C3Vector& p1 = s_clipBufferVerts[i + 1];

        if (p0.z < 0.027777778f || p1.z < 0.027777778f) {
            continue;
        }

        float height = p1.y <= p0.y ? p1.y : p0.y;
        int32_t x0 = CMath::fint_(p0.x * CBSCALE - 0.5f) + 192;
        int32_t x1 = CMath::fint_(p1.x * CBSCALE - 0.5f) + 192;

        if (x1 < x0) {
            int32_t swap = x0;
            x0 = x1;
            x1 = swap;
        }

        if (x0 < 0) {
            x0 = 0;
        }

        if (x1 >= 384) {
            x1 = 383;
        }

        if (flags & UPDATE_FLAG_MARK_MASK) {
            if (x0 <= x1) {
                memset(&CWorldScene::s_clipBufferMask[x0], 1, x1 - x0 + 1);

                for (int32_t x = x0; x <= x1; x++) {
                    if (height > CWorldScene::s_clipBuffer[x]) {
                        CWorldScene::s_clipBuffer[x] = height;
                    }
                }
            }
        } else {
            for (int32_t x = x0; x <= x1; x++) {
                if (height > CWorldScene::s_clipBuffer[x]) {
                    CWorldScene::s_clipBuffer[x] = height;
                }
            }
        }
    }
}

// VERIFY: a0f63ed4 manual
// OFFSET: 0x0078FC40
uint32_t CWorldOcclusion::QueryBuffer(const C3Vector& center, float radius, uint8_t flags) {
    if (!(CWorld::enables & Enables::Enable_Culling) || fabs(radius) < 2.3841858e-07f || CWorldScene::camVec.z < -0.9f || CWorldScene::camVec.z > 0.9f) {
        return 0;
    }

    C3Vector view = center * CWorldScene::s_clipBufferMatrix;
    C3Vector extent = C3Vector(radius, radius, 0.0f) * CWorldScene::projMatrix;

    if (!(flags & QUERY_FLAG_TEST_NEAR) && view.z < 50.0f) {
        return 0;
    }

    float invZ = 1.0f / view.z;
    view.x *= invZ;
    extent.x *= invZ;
    extent.y *= invZ;
    view.y = extent.x + invZ * view.y;

    int32_t first = CMath::fint_((view.x - extent.y) * CBSCALE - 0.5f) + 192;
    int32_t last = CMath::fint_((extent.y + view.x) * CBSCALE - 0.5f) + 193;

    if (first >= 384 || last < 0) {
        return 0;
    }

    if (first < 0) {
        first = 0;
    }

    if (last >= 384) {
        last = 383;
    }

    for (int32_t i = first; i <= last; i++) {
        if (view.y > CWorldScene::s_clipBuffer[i]) {
            return 0;
        }
    }

    return 2;
}

// VERIFY: 3ac5bc19 auto
// OFFSET: 0x0078FDC0
uint32_t CWorldOcclusion::QueryBuffer(CAaBox* box, uint8_t flags) {
    if (!(CWorld::enables & Enables::Enable_Culling) || CWorldScene::camVec.z < -0.9f || CWorldScene::camVec.z > 0.9f) {
        return 0;
    }

    float minX = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;
    const C3Vector* corners[2] = { &box->b, &box->t };

    for (uint32_t i = 0; i < 8; i++) {
        C3Vector corner(corners[aabbCornerSelectX[i]]->x, corners[aabbCornerSelectY[i]]->y, corners[aabbCornerSelectZ[i]]->z);
        corner = corner * CWorldScene::s_clipBufferMatrix;

        if (!(flags & QUERY_FLAG_TEST_NEAR) && corner.z < 50.0f) {
            return 0;
        }

        float scale = 1.0f / corner.z;
        float x = corner.x * scale;
        float y = scale * corner.y;

        if (minX > x) {
            minX = x;
        }

        if (maxX < x) {
            maxX = x;
        }

        if (maxY < y) {
            maxY = y;
        }
    }

    int32_t first = CMath::fint_(minX * CBSCALE - 0.5f) + 192;
    int32_t last = CMath::fint_(maxX * CBSCALE - 0.5f) + 193;

    if (first >= 384 || last < 0) {
        return 0;
    }

    if (first < 0) {
        first = 0;
    }

    if (last >= 384) {
        last = 383;
    }

    for (int32_t i = first; i <= last; i++) {
        if (maxY > CWorldScene::s_clipBuffer[i]) {
            return 0;
        }
    }

    return 2;
}

// VERIFY: c855a200 auto
// OFFSET: 0x007CC810
void CWorldScene::AddWorldOccluders() {
    for (uint32_t i = 0; i < 5; i++) {
        WorldOccluderData& data = s_worldOccluders[i];

        if (data.m_mapId != s_mapId || CWorldScene::FrustumCull(&data.m_bounds)) {
            continue;
        }

        for (uint32_t j = 0; j < data.m_count; j += 2) {
            CWorldView::AddOccluder(data.m_verts[j], data.m_verts[j + 1]);
        }
    }
}

// VERIFY: be6bb79d auto
// OFFSET: 0x007CC880
void CWorldOccluder::ClipBufferUpdate() {
    CWorldOcclusion::UpdateBuffer(this->m_points, 2, CWorldOcclusion::UPDATE_FLAG_MARK_MASK);
}

// VERIFY: 7e61d360 manual
// OFFSET: 0x007CC890
WorldOccluderData::WorldOccluderData(int32_t mapId, float depth, C3Vector* verts, uint32_t count) {
    this->m_mapId = mapId;
    this->m_bounds.b = C3Vector(FLT_MAX, FLT_MAX, FLT_MAX);
    this->m_bounds.t = C3Vector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    this->m_verts = verts;
    this->m_count = count;

    for (uint32_t i = 0; i < this->m_count; i++) {
        const C3Vector& vert = verts[i];

        if (this->m_bounds.b.x > vert.x) {
            this->m_bounds.b.x = vert.x;
        }

        if (this->m_bounds.b.y > vert.y) {
            this->m_bounds.b.y = vert.y;
        }

        if (this->m_bounds.t.x < vert.x) {
            this->m_bounds.t.x = vert.x;
        }

        if (this->m_bounds.t.y < vert.y) {
            this->m_bounds.t.y = vert.y;
        }

        if (this->m_bounds.t.z < vert.z) {
            this->m_bounds.t.z = vert.z;
        }

        float bottom = vert.z + depth;

        if (bottom < this->m_bounds.b.z) {
            this->m_bounds.b.z = bottom;
        }
    }
}

// VERIFY: 8b33fdfb manual
// OFFSET: 0x007CC9A0
CWorldOccluder* CWorldOccluder::Alloc() {
    CWorldOccluder* occluder = s_occluderFreeList.Head();

    if (!occluder) {
        occluder = s_occluderFreeList.NewNode(TSLIST_LINK_BEFORE, 0, 0);
    }

    occluder->m_link.Unlink();

    return occluder;
}

// VERIFY: 21308e7d manual
// OFFSET: 0x007CCA90
void CWorldOccluder::Recycle() {
    this->m_link.Unlink();
    s_occluderFreeList.LinkToTail(this);
}

// VERIFY: 23d792fd manual
// OFFSET: 0x007CCD20
static void InitOcclusionVolumes() {
    if (s_occlusionVolumesInit) {
        return;
    }

    for (uint32_t i = 0; i < 62; i++) {
        OcclusionVolumeData& data = s_occlusionVolumes[i].m_data;
        const C3Vector* pos = data.m_positions;

        for (int32_t j = 0; j < static_cast<int32_t>(data.m_positionCount); j++, pos++) {
            if (data.m_bounds.b.x > pos->x) {
                data.m_bounds.b.x = pos->x;
            }

            if (data.m_bounds.b.y > pos->y) {
                data.m_bounds.b.y = pos->y;
            }

            if (data.m_bounds.b.z > pos->z) {
                data.m_bounds.b.z = pos->z;
            }

            if (data.m_bounds.t.x < pos->x) {
                data.m_bounds.t.x = pos->x;
            }

            if (data.m_bounds.t.y < pos->y) {
                data.m_bounds.t.y = pos->y;
            }

            if (data.m_bounds.t.z < pos->z) {
                data.m_bounds.t.z = pos->z;
            }
        }
    }

    s_occlusionVolumesInit = 1;
    CWorldOcclusion::planeArray.m_chunk = 32;
    CWorldOcclusion::clipVolumeArray.m_chunk = 32;
}

// VERIFY: 58f32a53 auto
// OFFSET: 0x007CCDF0
uint32_t CWorldOcclusion::GetClipVolumeCount() {
    return CWorldOcclusion::clipVolumeArray.Count();
}

// VERIFY: 96f106cd manual
// OFFSET: 0x007CCE00
uint32_t CWorldOcclusion::QueryVolumes(CAaSphere* sphere) {
    for (uint32_t i = 0; i < CWorldOcclusion::clipVolumeArray.Count(); i++) {
        CClipVolume& volume = CWorldOcclusion::clipVolumeArray[i];
        C4Plane* planes = &CWorldOcclusion::planeArray[volume.m_planeStart];
        uint32_t j;

        for (j = 0; j < volume.m_planeCount; j++) {
            if (planes[j].n.z * sphere->c.z + planes[j].n.x * sphere->c.x + sphere->c.y * planes[j].n.y + planes[j].d + sphere->r > 0.0f) {
                break;
            }
        }

        if (j == volume.m_planeCount) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 9dd72105 manual
// OFFSET: 0x007CCFA0
uint32_t CWorldOcclusion::QueryVolumes(C3Vector* points, uint32_t count) {
    for (uint32_t i = 0; i < CWorldOcclusion::clipVolumeArray.Count(); i++) {
        CClipVolume& volume = CWorldOcclusion::clipVolumeArray[i];
        C4Plane* planes = &CWorldOcclusion::planeArray[volume.m_planeStart];
        uint32_t j;

        for (j = 0; j < volume.m_planeCount; j++) {
            uint32_t k;

            for (k = 0; k < count; k++) {
                if (points[k].z * planes[j].n.z + points[k].x * planes[j].n.x + planes[j].n.y * points[k].y + planes[j].d > 0.0f) {
                    break;
                }
            }

            if (k < count) {
                break;
            }
        }

        if (j == volume.m_planeCount) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 931f6673 manual
// OFFSET: 0x007CD4E0
void CWorldOcclusion::AddVolume(const C3Vector* eye, const C3Vector* verts, int32_t count, int32_t clipToView) {
    uint32_t planeStart = CWorldOcclusion::planeArray.Count();
    uint32_t planeCount = 0;

    if (clipToView && CWorldScene::frustumPortalView.m_maxViewDepth > 0.000001f) {
        float depth = CWorldScene::frustumPortalView.m_maxViewDepth;
        C4Plane viewPlane;
        viewPlane.n = CWorldScene::camVec;
        viewPlane.d = -((depth * CWorldScene::camVec.z + eye->z) * CWorldScene::camVec.z + CWorldScene::camVec.y * (CWorldScene::camVec.y * depth + eye->y) + CWorldScene::camVec.x * (CWorldScene::camVec.x * depth + eye->x));

        C3Vector* clipped = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * 2 * count));
        uint32_t clippedCount = 0;
        CWorldMath::ClipVertexListToPlane(verts, count, clipped, clippedCount, viewPlane, 0.0001f, 1);

        if (clippedCount <= 2) {
            return;
        }

        for (uint32_t i = 0; i < clippedCount; i++) {
            const C3Vector& a = clipped[i];
            const C3Vector& b = clipped[(i + 1) % clippedCount];
            C3Vector toEye(eye->x - a.x, eye->y - a.y, eye->z - a.z);
            C3Vector edge(b.x - a.x, b.y - a.y, b.z - a.z);

            C4Plane plane;
            plane.n.x = edge.y * toEye.z - edge.z * toEye.y;
            plane.n.y = edge.z * toEye.x - toEye.z * edge.x;
            plane.n.z = edge.x * toEye.y - toEye.x * edge.y;
            plane.d = 0.0f;

            float lengthSq = plane.n.y * plane.n.y + plane.n.z * plane.n.z + plane.n.x * plane.n.x;

            if (lengthSq > 0.0001f) {
                float inv = 1.0f / sqrtf(lengthSq);
                plane.n.x = plane.n.x * inv;
                plane.n.y = plane.n.y * inv;
                plane.n.z = inv * plane.n.z;
                plane.d = -(plane.n.y * a.y + plane.n.z * a.z + plane.n.x * a.x);
                CWorldOcclusion::planeArray.Add(1, &plane);
                planeCount++;
            }
        }
    } else {
        if (count <= 0) {
            return;
        }

        for (int32_t i = 0; i < count; i++) {
            C4Plane* plane = CWorldOcclusion::planeArray.New();
            plane->From3Pos(verts[i], verts[(i + 1) % count], *eye);
            planeCount++;
        }
    }

    if (!planeCount) {
        return;
    }

    C4Plane* cap = CWorldOcclusion::planeArray.New();
    cap->From3Pos(verts[0], verts[1], verts[2]);
    planeCount++;

    if (cap->n.z * eye->z + cap->n.y * eye->y + eye->x * cap->n.x + cap->d < 0.0f) {
        for (uint32_t i = 0; i < planeCount; i++) {
            C4Plane& plane = CWorldOcclusion::planeArray.m_data[planeStart + i];
            plane.d = -plane.d;
            plane.n.x = -plane.n.x;
            plane.n.y = -plane.n.y;
            plane.n.z = -plane.n.z;
        }
    }

    CClipVolume volume;
    volume.m_planeStart = planeStart;
    volume.m_planeCount = planeCount;
    CWorldOcclusion::clipVolumeArray.Add(1, &volume);
}

// VERIFY: 10cf9e8a auto
// OFFSET: 0x007CD850
void CWorldScene::CreateOcclusionVolumes(const C3Vector* eye, const C3Vector* corners, int32_t onlyFlagged) {
    CWorldOcclusion::planeArray.SetCount(0);
    CWorldOcclusion::clipVolumeArray.SetCount(0);

    if (!s_occlusionVolumesInit) {
        InitOcclusionVolumes();
    }

    CFrustum frustum;
    frustum.CalcPlanesFromCorners(corners);

    for (uint32_t i = 0; i < 62; i++) {
        OcclusionVolume& volume = s_occlusionVolumes[i];

        if (s_mapId != volume.m_mapID || (onlyFlagged && !(volume.m_flags & OcclusionVolume::FLAGS::UNKNOWN_2))) {
            continue;
        }

        if (frustum.Cull(volume.m_data.m_bounds)) {
            CWorldOcclusion::AddVolume(eye, volume.m_data.m_positions, volume.m_data.m_positionCount, !(volume.m_flags & OcclusionVolume::FLAGS::NO_VIEW_CLIP));
        }
    }
}

// VERIFY: 49cb931b auto
// OFFSET: 0x007CD910
void CWorldOcclusion::ClearVolumes() {
    CWorldOcclusion::planeArray.SetCount(0);
    CWorldOcclusion::clipVolumeArray.SetCount(0);
}

// VERIFY: 1ad39c5b manual
// OFFSET: 0x007CD930
void CWorldScene::RenderOcclusionVolumes() {
    if (!(CWorld::enables & Enables::Enable_PortalVis)) {
        return;
    }

    int32_t vertexCount = 0;
    int32_t indexCount = 0;

    for (uint32_t i = 0; i < 62; i++) {
        if (s_occlusionVolumes[i].m_mapID == s_mapId) {
            int32_t count = s_occlusionVolumes[i].m_data.m_positionCount;
            vertexCount += count;
            indexCount += 3 * (count - 2);
        }
    }

    if (!vertexCount || !indexCount) {
        return;
    }

    CImVector color;
    color.b = 0x20;
    color.g = 0x80;
    color.r = 0xFF;
    color.a = 0x8F;

    C44Matrix world;
    world.Translate(C3Vector(-CWorldScene::s_activeWorldView.x, -CWorldScene::s_activeWorldView.y, -CWorldScene::s_activeWorldView.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_PixelShader, 0);
    GxRsSet(GxRs_VertexShader, 0);
    GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<int32_t*>(&color));
    GxRsSet(GxRs_Culling, 0);

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPN), vertexCount);
    CGxVertexPN* vertex = reinterpret_cast<CGxVertexPN*>(g_theGxDevicePtr->BufLock(vertexBuf));

    for (uint32_t i = 0; i < 62; i++) {
        OcclusionVolumeData& data = s_occlusionVolumes[i].m_data;

        if (s_occlusionVolumes[i].m_mapID != s_mapId) {
            continue;
        }

        const C3Vector* positions = data.m_positions;
        C3Vector edgeA(positions[1].x - positions[0].x, positions[1].y - positions[0].y, positions[1].z - positions[0].z);
        C3Vector edgeB(positions[2].x - positions[0].x, positions[2].y - positions[0].y, positions[2].z - positions[0].z);
        C3Vector normal = C3Vector::Cross(edgeA, edgeB);

        for (int32_t j = 0; j < static_cast<int32_t>(data.m_positionCount); j++) {
            vertex->m_position = data.m_positions[j];
            vertex->m_normal = normal;
            vertex++;
        }
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->m_hasData = 1;
    GxPrimVertexPtr(vertexBuf, GxVBF_PN);

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    int32_t base = 0;

    for (uint32_t i = 0; i < 62; i++) {
        OcclusionVolume& volume = s_occlusionVolumes[i];

        if (volume.m_mapID != s_mapId) {
            continue;
        }

        for (int32_t j = 0; j < static_cast<int32_t>(volume.m_data.m_positionCount) - 2; j++) {
            indices[0] = static_cast<uint16_t>(base);
            indices[1] = static_cast<uint16_t>(base + j + 1);
            indices[2] = static_cast<uint16_t>(base + j + 2);
            indices += 3;
        }

        base += volume.m_data.m_positionCount;
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->m_hasData = 1;
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount);
    g_theGxDevicePtr->Draw(&batch, 1);

    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_World);
}
