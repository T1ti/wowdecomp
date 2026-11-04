#ifndef ENGINE_SOURCE_FRAME_CBACKDROPGENERATOR_H
#define ENGINE_SOURCE_FRAME_CBACKDROPGENERATOR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/CImVector.h"

class CRect;
class CSimpleFrame;
class CStatus;
class XMLNode;
class CSimpleTexture;

class CBackdropGenerator {
    public:
    enum BACKDROPPIECES : uint32_t {
        LEFTSIDE = 0x01,
        RIGHTSIDE = 0x02,
        TOPSIDE = 0x04,
        BOTTOMSIDE = 0x08,
        TOPLEFTCORNER = 0x10,
        TOPRIGHTCORNER = 0x20,
        BOTTOMLEFTCORNER = 0x40,
        BOTTOMRIGHTCORNER = 0x80,
        BACKDROPONLY = 0,
        CORNERSONLY = TOPLEFTCORNER | TOPRIGHTCORNER | BOTTOMLEFTCORNER | BOTTOMRIGHTCORNER,
        SIDESONLY = LEFTSIDE | RIGHTSIDE | TOPSIDE | BOTTOMSIDE,
        THEWORKS = SIDESONLY | CORNERSONLY,
    };

    /* 0000 */ CSimpleTexture* m_backgroundTexture;
    /* 0004 */ CSimpleTexture* m_leftTexture;
    /* 0008 */ CSimpleTexture* m_rightTexture;
    /* 000C */ CSimpleTexture* m_topTexture;
    /* 0010 */ CSimpleTexture* m_bottomTexture;
    /* 0014 */ CSimpleTexture* m_topLeftTexture;
    /* 0018 */ CSimpleTexture* m_topRightTexture;
    /* 001C */ CSimpleTexture* m_bottomLeftTexture;
    /* 0020 */ CSimpleTexture* m_bottomRightTexture;
    /* 0024 */ RCString m_background; // Background texture file path.
    /* 0030 */ RCString m_border; // Border texture file path.
    /* 003C */ uint32_t m_pieces;
    /* 0040 */ uint32_t m_tileBackground;
    /* 0044 */ uint32_t unused_0044;
    /* 0048 */ float m_cornerSize; // Border thickness and square corner size.
    /* 004C */ float m_backgroundSize; // Background tile size; zero falls back to m_cornerSize.
    /* 0050 */ float m_topInset;
    /* 0054 */ float m_bottomInset;
    /* 0058 */ float m_leftInset;
    /* 005C */ float m_rightInset;
    /* 0060 */ CImVector m_color; // Background vertex tint.
    /* 0064 */ CImVector m_borderColor;
    /* 0068 */ EGxBlend m_alphaMode;

    CBackdropGenerator();
    void DestroyTextures();
    void Generate(const CRect* rect);
    void LoadXML(XMLNode* node, CStatus* status);
    void SetBorderVertexColor(const CImVector& color);
    void SetOutput(CSimpleFrame* frame);
    void SetVertexColor(const CImVector& color);
};

#endif
