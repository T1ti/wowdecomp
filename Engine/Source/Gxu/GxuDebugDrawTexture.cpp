#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_GXU_DEBUG_DRAW

#include <cstring>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/ShaderEffect/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"

// Tool features: Gxu debug texture and text drawers (4.1 Engine/Source/Gxu/GxuDebugDrawTexture.cpp, plus the text
// drawers of which 4.1 kept only the shutdown functions; see GxuDebugDraw.h). 4.1 addresses are the 13850 Mac
// build's.
//
// Screen space for both is 0..1 across the whole window with the origin at the top left, like the 4.1 texture
// drawer's GxuXformCreateOrtho(0, 1, 1, 0, -1, 1). GxuFont places strings with the origin at the bottom left, so the
// text drawer flips y.

static TSGrowableArray<GxuDebugTextObject> s_textObjects;
static TSGrowableArray<GxuDebugText3DObject> s_text3DObjects;
static TSGrowableArray<GxuDebugTextureObject> s_textureObjects;

// Text drawer state. Strings are rebuilt every frame (debug text changes from frame to frame), kept in s_strings only
// until the batch has rendered.
static CGxFont* s_font;
static CGxStringBatch* s_batch;
static TSGrowableArray<CGxString*> s_strings;
static bool s_fontFailed;

// Same height as the perf stock font (Scrn s_stockObjectHeights).
static const float DEBUG_FONT_HEIGHT = 0.01953125f;

void GxuDebugDrawText(const char* text, float x, float y, const CImVector& color) {
    GxuDebugTextObject* object = s_textObjects.New();
    SStrCopy(object->m_text, text, sizeof(object->m_text));
    object->m_x = x;
    object->m_y = y;
    object->m_color = color;
}

void GxuDebugDrawText3D(const char* text, const C3Vector& position, const CImVector& color) {
    GxuDebugText3DObject* object = s_text3DObjects.New();
    SStrCopy(object->m_text, text, sizeof(object->m_text));
    object->m_position = position;
    object->m_color = color;
}

void GxuDebugDrawTexture(CGxTex* texture, float x, float y, float width, float height, EGxBlend blend) {
    GxuDebugTextureObject* object = s_textureObjects.New();
    object->m_texture = texture;
    object->m_x = x;
    object->m_y = y;
    object->m_width = width;
    object->m_height = height;
    object->m_blend = blend;
}

// Saves the transforms and viewport the 2D drawers replace, and puts them back.
class GxuDebugScreenSpace {
    public:
    C44Matrix m_projection;
    C44Matrix m_view;
    float m_viewport[6];

    GxuDebugScreenSpace() {
        GxXformProjection(this->m_projection);
        GxXformView(this->m_view);
        GxXformViewport(this->m_viewport[0], this->m_viewport[1], this->m_viewport[2], this->m_viewport[3], this->m_viewport[4], this->m_viewport[5]);
        GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    }

    ~GxuDebugScreenSpace() {
        GxXformSetViewport(this->m_viewport[0], this->m_viewport[1], this->m_viewport[2], this->m_viewport[3], this->m_viewport[4], this->m_viewport[5]);
        GxXformSetView(this->m_view);
        GxXformSetProjection(this->m_projection);
    }
};

// TOOL: 4.1 0x00AEA650 GxuDebugTextureDrawer::Draw(const C3Vector&)
// 4.1 sets an identity view, the (0, 1, 1, 0) ortho projection and the object's blend mode, then draws a quad with a
// debug-texture shader pair (s_vsDebugTexture / s_psDebugTexture, picked per texture by GetSamplePermutation for
// multisampled render targets) that also scales the channels. 3.3.5 has no such shaders: the quad is drawn fixed
// function, texture modulated by white.
void GxuDebugTextureDrawer::Draw(const C3Vector& viewOrigin) {
    if (!s_textureObjects.m_count) {
        return;
    }

    {
        GxuDebugScreenSpace screen;

        C44Matrix identity;
        C44Matrix projection;
        GxuXformCreateOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, projection);
        GxXformSetView(identity);
        GxXformSetProjection(projection);
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, identity);

        CGxDevice* device = g_theGxDevicePtr;
        device->RsPush();
        device->RsSet(GxRs_Lighting, 0);
        device->RsSet(GxRs_Fog, 0);
        device->RsSet(GxRs_DepthTest, 0);
        device->RsSet(GxRs_DepthWrite, 0);
        device->RsSet(GxRs_Culling, 0);
        device->RsSet(GxRs_VertexShader, static_cast<void*>(nullptr));
        device->RsSet(GxRs_PixelShader, static_cast<void*>(nullptr));
        device->RsSet(GxRs_ColorOp0, GxTexOp_Mod);
        device->RsSet(GxRs_AlphaOp0, GxTexOp_Mod);

        static const C2Vector s_texCoords[4] = {
            C2Vector(0.0f, 0.0f),
            C2Vector(1.0f, 0.0f),
            C2Vector(0.0f, 1.0f),
            C2Vector(1.0f, 1.0f),
        };
        static const uint16_t s_indices[6] = { 0, 1, 2, 2, 1, 3 };
        CImVector white(0xFF, 0xFF, 0xFF, 0xFF);

        for (uint32_t i = 0; i < s_textureObjects.m_count; i++) {
            const GxuDebugTextureObject& object = s_textureObjects.m_data[i];

            if (!object.m_texture) {
                continue;
            }

            C3Vector positions[4] = {
                C3Vector(object.m_x, object.m_y, 0.0f),
                C3Vector(object.m_x + object.m_width, object.m_y, 0.0f),
                C3Vector(object.m_x, object.m_y + object.m_height, 0.0f),
                C3Vector(object.m_x + object.m_width, object.m_y + object.m_height, 0.0f),
            };

            device->RsSet(GxRs_Texture0, object.m_texture);
            device->RsSet(GxRs_BlendingMode, static_cast<int32_t>(object.m_blend));
            GxUpdateAlphaRef(GxRs_AlphaRef);

            GxPrimVertexPtr(4, positions, sizeof(C3Vector), nullptr, 0, &white, 0, s_texCoords, sizeof(C2Vector), nullptr, 0);
            GxPrimIndexPtr(6, s_indices);

            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = 6;
            batch.m_minIndex = 0;
            batch.m_maxIndex = 3;
            device->Draw(&batch, 1);
        }

        device->RsPop();
        GxXformPop(GxXform_World);
    }

    s_textureObjects.SetCount(0);
}

// TOOL: 4.1 0x00AEA510 (also releases the debug-texture shaders, which this port does not have)
void GxuShutdownDebugTextureDrawer() {
    s_textureObjects.SetCount(0);
}

// ---- Text (port; see the file comment)

static bool EnsureTextFont() {
    if (s_font && s_batch) {
        return true;
    }

    if (s_fontFailed) {
        return false;
    }

    // The stock font face, at the stock perf font size (Scrn IStockInitialize / ScrnSetStockFont).
    char fontPath[260];
    OsBuildFontFilePath("FRIZQT__.TTF", fontPath, sizeof(fontPath));

    if (!fontPath[0] || !GxuFontCreateFont(fontPath, NDCToDDCHeight(DEBUG_FONT_HEIGHT), s_font, 0)) {
        s_fontFailed = true;
        return false;
    }

    s_batch = GxuFontCreateBatch(false, false);

    if (!s_batch) {
        GxuFontDestroyFont(s_font);
        s_font = nullptr;
        s_fontFailed = true;
        return false;
    }

    return true;
}

// x and y are 0..1 screen coordinates in GxuFont's bottom-left space. String arguments as WowSysMessageOutput::Paint.
static void AddString(const char* text, float x, float y, const CImVector& color) {
    C3Vector position(x, y, 1.0f);
    CGxString* string = nullptr;
    GxuFontCreateString(s_font, text, 0.0f, position, 1.0f, DEBUG_FONT_HEIGHT, 0.0f, string, GxVJ_Middle, GxHJ_Left, 0x1F44, color, -0.45f, 1.0f);

    if (string) {
        *s_strings.New() = string;
        GxuFontAddToBatch(s_batch, string);
    }
}

// World text is projected with the world view and projection, so this runs before GxuDebugScreenSpace replaces them.
// Positions are camera relative (viewOrigin); points behind the camera are skipped.
static void AddText3DStrings(const C3Vector& viewOrigin) {
    C44Matrix viewProj;
    GxXformViewProj(viewProj);

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    for (uint32_t i = 0; i < s_text3DObjects.m_count; i++) {
        const GxuDebugText3DObject& object = s_text3DObjects.m_data[i];
        C3Vector p(object.m_position.x - viewOrigin.x, object.m_position.y - viewOrigin.y, object.m_position.z - viewOrigin.z);

        float clipX = p.x * viewProj.a0 + p.y * viewProj.b0 + p.z * viewProj.c0 + viewProj.d0;
        float clipY = p.x * viewProj.a1 + p.y * viewProj.b1 + p.z * viewProj.c1 + viewProj.d1;
        float clipW = p.x * viewProj.a3 + p.y * viewProj.b3 + p.z * viewProj.c3 + viewProj.d3;

        if (clipW <= 0.0f) {
            continue;
        }

        float x = minX + (clipX / clipW * 0.5f + 0.5f) * (maxX - minX);
        float y = minY + (clipY / clipW * 0.5f + 0.5f) * (maxY - minY);
        AddString(object.m_text, x, y, object.m_color);
    }
}

void GxuDebugTextDrawer::Draw(const C3Vector& viewOrigin) {
    if (!s_textObjects.m_count && !s_text3DObjects.m_count) {
        return;
    }

    if (EnsureTextFont()) {
        AddText3DStrings(viewOrigin);

        for (uint32_t i = 0; i < s_textObjects.m_count; i++) {
            const GxuDebugTextObject& object = s_textObjects.m_data[i];
            AddString(object.m_text, object.m_x, 1.0f - object.m_y, object.m_color);
        }

        {
            // The batch sets its own pixel projection over the viewport; make that the whole window.
            GxuDebugScreenSpace screen;
            GxuFontRenderBatch(s_batch);
        }

        GxuFontClearBatch(s_batch);

        for (uint32_t i = 0; i < s_strings.m_count; i++) {
            GxuFontDestroyString(s_strings.m_data[i]);
        }

        s_strings.SetCount(0);
    }

    s_textObjects.SetCount(0);
    s_text3DObjects.SetCount(0);
}

// TOOL: 4.1 0x00AE6A80. Also frees the port's font and batch.
void GxuShutdownDebugTextDrawer() {
    s_textObjects.SetCount(0);

    for (uint32_t i = 0; i < s_strings.m_count; i++) {
        GxuFontDestroyString(s_strings.m_data[i]);
    }

    s_strings.SetCount(0);

    if (s_batch) {
        GxuFontDestroyBatch(s_batch);
        s_batch = nullptr;
    }

    if (s_font) {
        GxuFontDestroyFont(s_font);
        s_font = nullptr;
    }

    s_fontFailed = false;
}

// TOOL: 4.1 0x00AE69A0
void GxuShutdownDebugText3DDrawer() {
    s_text3DObjects.SetCount(0);
}

#endif
