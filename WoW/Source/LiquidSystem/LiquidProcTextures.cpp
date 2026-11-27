#include "Liquid.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"

static const int32_t WATERTEX_HEIGHT = 64;
static const float kDeepDarken = 0.9f;

// Slots of s_procTexturesUpdated, named after the texture UpdateProceduralTextures uploads
// for each; slot 0 covers the s_unk_D43B50 array (name inferred).
enum PROC_TEXTURE : int32_t {
    PROC_TEXTURE_UNKNOWN_0 = 0,
    PROC_TEXTURE_WMO_WATER = 1,
    PROC_TEXTURE_RIVER_DEPTH = 2,
    PROC_TEXTURE_OCEAN_DEPTH = 3,
    NUM_PROC_TEXTURES = 4,
};

// STATIC: 0x00D43B48
static uint8_t s_procTexturesUpdated[NUM_PROC_TEXTURES];

// STATIC: 0x00D43B4C
static CTexture* s_unk_D43B4C;

// STATIC: 0x00D43B50
static CTexture* s_unk_D43B50[32];

// STATIC: 0x00D43BD0
static CTexture* wmoWaterTexid;

// STATIC: 0x00D43BD4
static CTexture* oceanDepthTexid;

// STATIC: 0x00D43BD8
static CTexture* riverDepthTexid;

static void ExtWaterDepthTexCallback(EGxTexCommand command, uint32_t w, uint32_t h, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
static void WmoWaterTexCallback(EGxTexCommand command, uint32_t w, uint32_t h, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

// VERIFY: 901024cc manual
// OFFSET: 0x008A2980
void Liquid::DestroyProceduralTextures() {
    if (s_unk_D43B4C) {
        HandleClose(reinterpret_cast<HOBJECT>(s_unk_D43B4C));
        s_unk_D43B4C = nullptr;
    }

    if (riverDepthTexid) {
        HandleClose(reinterpret_cast<HOBJECT>(riverDepthTexid));
        riverDepthTexid = nullptr;
    }

    if (oceanDepthTexid) {
        HandleClose(reinterpret_cast<HOBJECT>(oceanDepthTexid));
        oceanDepthTexid = nullptr;
    }

    if (wmoWaterTexid) {
        HandleClose(reinterpret_cast<HOBJECT>(wmoWaterTexid));
        wmoWaterTexid = nullptr;
    }

    for (uint32_t i = 0; i < sizeof(s_unk_D43B50) / sizeof(s_unk_D43B50[0]); i++) {
        if (s_unk_D43B50[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_unk_D43B50[i]));
            s_unk_D43B50[i] = nullptr;
        }
    }
}

// VERIFY: bbba7190 auto
// OFFSET: 0x008A2A10
static void CacheProceduralTextures() {
    CGxTexFlags flags;

    TextureCacheNewTexture(riverDepthTexid);
    TextureCacheNewTexture(oceanDepthTexid);
    TextureCacheNewTexture(wmoWaterTexid);

    for (uint32_t i = 0; i < sizeof(s_unk_D43B50) / sizeof(s_unk_D43B50[0]); i++) {
        if (s_unk_D43B50[i]) {
            TextureCacheNewTexture(s_unk_D43B50[i]);
        }
    }

    if (s_unk_D43B4C) {
        TextureCacheNewTexture(s_unk_D43B4C);
    }

    Liquid::CSettingsBank::ResetTextures();
}

// VERIFY: 5d576297 auto
// OFFSET: 0x008A2E20
void Liquid::CreateProceduralTextures() {
    s_procTexturesUpdated[PROC_TEXTURE_OCEAN_DEPTH] = 0;
    s_procTexturesUpdated[PROC_TEXTURE_RIVER_DEPTH] = 0;
    s_procTexturesUpdated[PROC_TEXTURE_WMO_WATER] = 0;
    s_procTexturesUpdated[PROC_TEXTURE_UNKNOWN_0] = 0;

    CGxTexFlags flags;
    STORM_ASSERT(!riverDepthTexid);
    riverDepthTexid = TextureCreate(8, WATERTEX_HEIGHT, GxTex_Argb8888, GxTex_Argb8888, flags, reinterpret_cast<void*>(1), ExtWaterDepthTexCallback, "proceduralRiverDepthTex", 0);

    flags = CGxTexFlags();
    STORM_ASSERT(!oceanDepthTexid);
    oceanDepthTexid = TextureCreate(8, WATERTEX_HEIGHT, GxTex_Argb8888, GxTex_Argb8888, flags, nullptr, ExtWaterDepthTexCallback, "proceduralOceanDepthTex", 0);

    flags = CGxTexFlags();
    STORM_ASSERT(!wmoWaterTexid);
    wmoWaterTexid = TextureCreate(8, WATERTEX_HEIGHT, GxTex_Argb8888, GxTex_Argb8888, flags, nullptr, WmoWaterTexCallback, "proceduralWmoWaterTex", 0);

    CacheProceduralTextures();
}

// VERIFY: 7e3d28c3 manual
// OFFSET: 0x008A2F00
void Liquid::UpdateProceduralTextures() {
    if (!s_procTexturesUpdated[PROC_TEXTURE_OCEAN_DEPTH] && oceanDepthTexid) {
        CGxTex* gxTex = TextureGetGxTex(oceanDepthTexid, 0, nullptr);

        if (gxTex) {
            CiRect rect;
            rect.minY = 0;
            rect.minX = 0;
            rect.maxY = WATERTEX_HEIGHT;
            rect.maxX = 8;
            GxTexUpdate(gxTex, rect, 0);
            s_procTexturesUpdated[PROC_TEXTURE_OCEAN_DEPTH] = 1;
        }
    }

    if (!s_procTexturesUpdated[PROC_TEXTURE_RIVER_DEPTH] && riverDepthTexid) {
        CGxTex* gxTex = TextureGetGxTex(riverDepthTexid, 0, nullptr);

        if (gxTex) {
            CiRect rect;
            rect.minY = 0;
            rect.minX = 0;
            rect.maxY = WATERTEX_HEIGHT;
            rect.maxX = 8;
            GxTexUpdate(gxTex, rect, 0);
            s_procTexturesUpdated[PROC_TEXTURE_RIVER_DEPTH] = 1;
        }
    }

    if (!s_procTexturesUpdated[PROC_TEXTURE_WMO_WATER] && wmoWaterTexid) {
        CGxTex* gxTex = TextureGetGxTex(wmoWaterTexid, 0, nullptr);

        if (gxTex) {
            CiRect rect;
            rect.minY = 0;
            rect.minX = 0;
            rect.maxY = WATERTEX_HEIGHT;
            rect.maxX = 8;
            GxTexUpdate(gxTex, rect, 0);
            s_procTexturesUpdated[PROC_TEXTURE_WMO_WATER] = 1;
        }
    }

    if (!s_procTexturesUpdated[PROC_TEXTURE_UNKNOWN_0]) {
        for (uint32_t i = 0; i < sizeof(s_unk_D43B50) / sizeof(s_unk_D43B50[0]); i++) {
            if (s_unk_D43B50[i]) {
                GxTexUpdate(TextureGetGxTex(s_unk_D43B50[i], 0, nullptr), 0, 0, 0, 0, 1);
            }
        }

        s_procTexturesUpdated[PROC_TEXTURE_UNKNOWN_0] = 1;
    }

    if (s_unk_D43B4C) {
        CGxTex* gxTex = TextureGetGxTex(s_unk_D43B4C, 1, nullptr);

        if (gxTex) {
            GxTexStretch(gxTex, nullptr, nullptr, 0, 0);
        }
    }
}

// VERIFY: ff857bf4 auto
// OFFSET: 0x008A2AA0
void Liquid::InvalidateProceduralTextures() {
    s_procTexturesUpdated[PROC_TEXTURE_OCEAN_DEPTH] = 0;
    s_procTexturesUpdated[PROC_TEXTURE_RIVER_DEPTH] = 0;
    s_procTexturesUpdated[PROC_TEXTURE_WMO_WATER] = 0;
}

// VERIFY: 331f7c3c manual
// OFFSET: 0x008A2AC0
static void WmoWaterTexCallback(EGxTexCommand command, uint32_t w, uint32_t h, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    STORM_ASSERT(mipLevel == 0);
    STORM_ASSERT(h == 64);

    // STATIC: 0x00D43BE0
    static CImVector s_texels[WATERTEX_HEIGHT * 8];

    if (command != GxTex_Latch) {
        return;
    }

    DayNight::DNInfo* info = DayNight::GetInfo();
    CImVector band = info->m_bands.m_waterColors[3];
    uint8_t shallowAlpha = CMath::fint_(info->m_bands.m_waterShallowAlpha * 255.0f);
    uint8_t deepAlpha = CMath::fint_(info->m_bands.m_waterDeepAlpha * 255.0f);
    int32_t alphaStep = ((deepAlpha - shallowAlpha) << 8) >> 6;
    int32_t alpha = shallowAlpha << 8;

    CImVector color;
    color.b = band.b;
    color.g = band.g;
    color.r = band.r;

    CImVector white;
    white.b = 0xFF;
    white.g = 0xFF;
    white.r = 0xFF;

    for (uint32_t y = 0; y < WATERTEX_HEIGHT; y++) {
        for (uint32_t x = 0; x < 4; x++) {
            color.a = alpha >> 8;
            s_texels[y * 8 + x] = color;
        }

        for (uint32_t x = 4; x < 8; x++) {
            white.a = alpha >> 8;
            s_texels[y * 8 + x] = white;
        }

        alpha += alphaStep;
    }

    texelStrideInBytes = 8 * sizeof(CImVector);
    texels = s_texels;
}

// VERIFY: aa313e0a manual
// OFFSET: 0x008A2BF0
static void ExtWaterDepthTexCallback(EGxTexCommand command, uint32_t w, uint32_t h, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    const uint32_t texWidth = 8;
    STORM_ASSERT(mipLevel == 0);
    STORM_ASSERT(w==texWidth);
    STORM_ASSERT(h==WATERTEX_HEIGHT);

    // STATIC: 0x00D443E8
    static CImVector s_texels[WATERTEX_HEIGHT * 8];

    if (command != GxTex_Latch) {
        return;
    }

    DayNight::DNInfo* info = DayNight::GetInfo();

    uint8_t shallowAlpha[2];
    shallowAlpha[0] = CMath::fint_(info->m_bands.m_oceanShallowAlpha * 255.0f);
    shallowAlpha[1] = CMath::fint_(info->m_bands.m_waterShallowAlpha * 255.0f);

    uint8_t deepAlpha[2];
    deepAlpha[0] = CMath::fint_(info->m_bands.m_oceanDeepAlpha * 255.0f);
    deepAlpha[1] = CMath::fint_(info->m_bands.m_waterDeepAlpha * 255.0f);

    uintptr_t type = reinterpret_cast<uintptr_t>(userArg);
    CImVector shallowColor = info->m_bands.m_waterColors[type * 2];
    CImVector deepColor = info->m_bands.m_waterColors[type * 2 + 1];

    int32_t alphaStep = ((deepAlpha[type] - shallowAlpha[type]) << 8) >> 6;
    int32_t redStep = ((deepColor.r - shallowColor.r) << 8) >> 6;
    int32_t blueStep = ((deepColor.b - shallowColor.b) << 8) >> 6;
    int32_t greenStep = ((deepColor.g - shallowColor.g) << 8) >> 6;

    int32_t alpha = shallowAlpha[type] << 8;
    int32_t red = shallowColor.r << 8;
    int32_t green = shallowColor.g << 8;
    int32_t blue = shallowColor.b << 8;

    CImVector* row = s_texels;

    for (uint32_t y = 0; y < h; y++) {
        CImVector color;
        color.b = blue >> 8;
        color.g = green >> 8;
        color.r = red >> 8;
        color.a = alpha >> 8;

        if (y == h - 1 && type == 0) {
            C3Vector rgb(color);
            C3Vector hsv;
            RGBtoHSV(rgb, hsv);
            hsv.z *= kDeepDarken;
            HSVtoRGB(hsv, rgb);
            color = rgb;
        }

        for (uint32_t x = 0; x < w; x++) {
            row[x] = color;
        }

        green += greenStep;
        blue += blueStep;
        alpha += alphaStep;
        red += redStep;
        row += w;
    }

    texelStrideInBytes = w * sizeof(CImVector);
    texels = s_texels;
}
