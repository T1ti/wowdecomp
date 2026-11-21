#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// Tool features: Gxu debug texture and text drawers (4.1 Engine/Source/Gxu/GxuDebugDrawTexture.cpp and the text
// drawers whose shutdown functions survive in 4.1; see GxuDebugDraw.h). 4.1 addresses are the 13850 Mac build's.

static TSGrowableArray<GxuDebugTextObject> s_textObjects;
static TSGrowableArray<GxuDebugText3DObject> s_text3DObjects;
static TSGrowableArray<GxuDebugTextureObject> s_textureObjects;

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

void GxuDebugDrawTexture(CGxTex* texture, float x, float y, float width, float height) {
    GxuDebugTextureObject* object = s_textureObjects.New();
    object->m_texture = texture;
    object->m_x = x;
    object->m_y = y;
    object->m_width = width;
    object->m_height = height;
}

// TOOL: 4.1 0x00AEA650 GxuDebugTextureDrawer::Draw(const C3Vector&)
void GxuDebugTextureDrawer::Draw(const C3Vector& screenSize) {
    // TODO: 4.1 0x00AEA650 sets an orthographic projection (GxuXformCreateOrtho over the screen), pushes render
    // states (RsSet texture, blending, depth test off), and for each queued texture locks four corner vertices
    // (GxPrimLockVertexPtrs: position, color, texcoord) and draws two triangles (GxDrawLockedElements), then RsPop.
    // Its GetSamplePermutation(CGxTex*) helper (0x00AEA460) picks the shader for the texture format.
    s_textureObjects.SetCount(0);
}

// TOOL: 4.1 0x00AEA510
void GxuShutdownDebugTextureDrawer() {
    s_textureObjects.SetCount(0);
}

// TOOL: 4.1 0x00AE6A80. The 2D text drawer: TODO draw s_textObjects with a GxuFont string at normalized positions.
void GxuShutdownDebugTextDrawer() {
    s_textObjects.SetCount(0);
}

// TOOL: 4.1 0x00AE69A0. The 3D text drawer: TODO project s_text3DObjects to the screen and draw like the 2D text.
void GxuShutdownDebugText3DDrawer() {
    s_text3DObjects.SetCount(0);
}

#endif
