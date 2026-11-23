#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Gxu/GxuFontFace.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontUtil.h"
#include "Engine/Source/Gxu/IGxuFontGlyph.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
KERNINGHASHKEY& KERNINGHASHKEY::operator=(const KERNINGHASHKEY& key) {
    if (this != &key) {
        this->m_code = key.m_code;
    }

    return *this;
}

// INLINED
bool KERNINGHASHKEY::operator==(const KERNINGHASHKEY& key) {
    return this == &key || this->m_code == key.m_code;
}

// VERIFY: d974db0f auto
// OFFSET: 0x006C2260
const char* CGxFont::GetName() const {
    return FontFaceGetFontName(this->m_faceHandle);
}

// VERIFY: 637e266a auto
// OFFSET: 0x006C2270
uint32_t CGxFont::GetPixelSize() const {
    STORM_ASSERT(m_pixSize);
    return this->m_pixSize;
}

// VERIFY: 3c5dbf73 manual
// OFFSET: 0x006C2280
float CGxFont::FindNearestFixedSize(FT_Face face, float height) {
    if (face->num_fixed_sizes == 0) {
        return height;
    }

    int32_t smallest = 0x7FFFFFFF;

    for (int32_t i = 0; i < face->num_fixed_sizes; i++) {
        if (smallest >= face->available_sizes[i].height) {
            smallest = face->available_sizes[i].height;
        }
    }

    float fixedHeight = PixelToScreenHeight(smallest);

    return fixedHeight < height ? height : fixedHeight;
}

// VERIFY: d7c70ac7 manual
// OFFSET: 0x006C22F0
int32_t CGxFont::UpdateDimensions() {
    STORM_ASSERT(m_faceHandle);
    FT_Face theFace = FontFaceGetFace(this->m_faceHandle);
    STORM_ASSERT(theFace);

    float height = this->FindNearestFixedSize(theFace, this->m_fontHeight);
    double minHeight = 2.0 / g_heightPixels;
    height = height > minHeight ? height : minHeight;

    uint32_t pixelSize = static_cast<uint32_t>(ScreenToPixelHeight(0, height));

    if (pixelSize > 32) {
        pixelSize = 32;
    } else if (pixelSize == 0) {
        DebugLog("Error, font %s being created with height %g at screen res of %dx%d which is %d pixels!", this->m_fontName, height, g_widthPixels, g_heightPixels, pixelSize);
    }

    this->m_pixSize = pixelSize;

    int32_t units = theFace->ascender + abs(theFace->descender);

    if (units == 0) {
        return 0;
    }

    this->m_cellHeight = pixelSize;

    float baseline = static_cast<float>(pixelSize * theFace->ascender) / units;
    this->m_baseline = static_cast<uint32_t>(baseline + 0.5);

    if (this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_THICK_OUTLINE) {
        this->m_cellHeight = pixelSize + 4;
    } else if (this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_OUTLINE) {
        this->m_cellHeight = pixelSize + 2;
    }

    int32_t result = FT_Set_Pixel_Sizes(theFace, this->GetPixelSize(), 0) == FT_Err_Ok;

    this->m_pixelsPerUnit = static_cast<float>(theFace->size->metrics.x_ppem) / static_cast<float>(theFace->units_per_EM);

    STORM_ASSERT(m_pixelsPerUnit != 0.0f);
    return result;
}

// VERIFY: c831afcc auto
// OFFSET: 0x006C2480
bool CGxFont::GetGlyphData(GLYPHBITMAPDATA* data, uint32_t code) const {
    FT_Face face = FontFaceGetFace(this->m_faceHandle);
    STORM_ASSERT(face);
    [[maybe_unused]] const FT_Error error = FT_Set_Pixel_Sizes(face, this->GetPixelSize(), 0);
    STORM_ASSERT(!error);

    uint32_t padding = 0;

    if (this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_THICK_OUTLINE) {
        padding = 4;
    } else if (this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_OUTLINE) {
        padding = 2;
    }

    return IGxuFontGlyphRenderGlyph(face, this->GetPixelSize(), code, this->m_baseline, data, this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_MONOCHROME, padding);
}

// VERIFY: 3597e899 auto
// OFFSET: 0x006C24F0
float CGxFont::GetGlyphBearing(const CHARCODEDESC* desc, bool scaled, float height) const {
    if (!scaled) {
        float bearing = desc->bitmapData.m_glyphBearing;
        return ceil(bearing);
    }

    float pixelHeight = ScreenToPixelHeight(1, height);
    float scale = pixelHeight / this->GetPixelSize();

    return scale * desc->bitmapData.m_glyphBearing;
}

// VERIFY: 29412482 manual
// OFFSET: 0x006C2600
void CGxFont::RegisterEvictNotice(uint32_t pageNumber) {
    STORM_ASSERT(pageNumber < 8);

    for (CGxString* string = this->m_strings.Head(); string; string = this->m_strings.Next(string)) {
        string->TexturePageEvicted(pageNumber);
    }
}

// VERIFY: 22997850 auto
// OFFSET: 0x006C29A0
int32_t CGxFont::CheckStringGlyphs(const char* string) {
    while (string && *string) {
        int32_t advance;
        uint32_t code = sgetu8(reinterpret_cast<const uint8_t*>(string), &advance);
        string += advance;

        if (code != '\r' && code != '\n' && !this->m_activeCharacters.Ptr(code, HASHKEY_NONE())) {
            return 0;
        }
    }

    return 1;
}

// VERIFY: f8f5bfc8 auto
// OFFSET: 0x006C2A00
float CGxFont::GetGlyphAdvance(uint32_t code) {
    CHARCODEDESC* desc = this->m_activeCharacters.Ptr(code, HASHKEY_NONE());

    if (!desc) {
        return 0.0f;
    }

    float advance = desc->bitmapData.m_glyphAdvance;

    if (this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_THICK_OUTLINE) {
        advance += 1.0;
    }

    return advance;
}

// VERIFY: a8a8859d manual
// OFFSET: 0x006C3710
void CGxFont::ClearGlyphs() {
    for (uint32_t i = 0; i < 8; i++) {
        this->m_textureCache[i].Destroy();
    }

    this->m_activeCharacters.Clear();
    this->m_activeCharacterCache.Clear();
    this->m_kernInfo.Clear();
}

// VERIFY: 9be95581 manual
// OFFSET: 0x006C3960
void CGxFont::HandleScreenSizeChange() {
    STORM_ASSERT(m_faceHandle);

    this->ClearGlyphs();
    int32_t success = this->UpdateDimensions();
    STORM_ASSERT(success);

    for (CGxString* string = this->m_strings.Head(); string; string = this->m_strings.Next(string)) {
        string->HandleScreenSizeChange();
    }
}

// VERIFY: 4ab2e948 manual
// OFFSET: 0x006C39A0
CGxFont::CGxFont() : m_cellHeight(0), m_pixelsPerUnit(0.0f) {
}

// VERIFY: b72018df manual
// OFFSET: 0x006C3B20
CGxFont::~CGxFont() {
    if (this->m_faceHandle) {
        FontFaceCloseHandle(this->m_faceHandle);
    }

    this->m_faceHandle = nullptr;
    this->ClearGlyphs();
}

// VERIFY: 4d729130 auto
// OFFSET: 0x006C3CB0
int32_t CGxFont::Initialize(const char* name, uint32_t fontFlags, float fontHeight) {
    STORM_ASSERT(name && *name);

    SStrPrintf(this->m_fontName, sizeof(this->m_fontName), "%s", name);
    this->m_fontHeight = fontHeight;

    if (this->m_faceHandle) {
        FontFaceCloseHandle(this->m_faceHandle);
    }

    this->m_faceHandle = nullptr;
    this->ClearGlyphs();
    this->m_fontFlags = fontFlags;

    if (this->m_faceHandle) {
        FontFaceCloseHandle(this->m_faceHandle);
    }

    this->m_faceHandle = FontFaceGetHandle(name, g_FTLibrary);

    if (!this->m_faceHandle) {
        return 0;
    }

    return this->UpdateDimensions();
}

// VERIFY: 249ead28 manual
// OFFSET: 0x006C3D40
float CGxFont::ComputeStep(uint32_t code, uint32_t nextCode) {
    KERNINGHASHKEY key;
    key.m_code = (code << 16) | (nextCode & 0xFFFF);

    KERNNODE* node = this->m_kernInfo.Ptr(code, key);

    if (node && (node->m_flags & KERNNODE::KERN_PROPORTIONAL_SPACING)) {
        return node->m_proportionalSpacing;
    }

    FT_Face theFace = FontFaceGetFace(this->m_faceHandle);
    STORM_ASSERT(theFace);
    FT_UInt leftIndex = FT_Get_Char_Index(theFace, code);
    FT_UInt rightIndex = FT_Get_Char_Index(theFace, nextCode);

    FT_Vector kerning;
    kerning.x = 0;

    if (FT_HAS_KERNING(theFace)) {
        FT_Get_Kerning(theFace, leftIndex, rightIndex, ft_kerning_unscaled, &kerning);

        if (kerning.x >= 0) {
            kerning.x = 0;
        }
    }

    float step = this->GetGlyphAdvance(code);
    step = this->m_pixelsPerUnit * kerning.x + step;

    if (!node) {
        node = this->m_kernInfo.New(code, key, 0, 0);
    }

    node->m_flags |= KERNNODE::KERN_PROPORTIONAL_SPACING;
    node->m_proportionalSpacing = ceil(step);

    return node->m_proportionalSpacing;
}

// VERIFY: 960b7c8a manual
// OFFSET: 0x006C3E80
float CGxFont::ComputeStepFixedWidth(uint32_t code, uint32_t nextCode) {
    KERNINGHASHKEY key;
    key.m_code = (code << 16) | (nextCode & 0xFFFF);

    KERNNODE* node = this->m_kernInfo.Ptr(code, key);

    if (node && (node->m_flags & KERNNODE::KERN_FIXED_WIDTH_SPACING)) {
        return node->m_fixedWidthSpacing;
    }

    if (!this->m_activeCharacters.Ptr(code, HASHKEY_NONE())) {
        if (!node) {
            node = this->m_kernInfo.New(code, key, 0, 0);
        }

        node->m_flags |= KERNNODE::KERN_FIXED_WIDTH_SPACING;
        node->m_fixedWidthSpacing = 0.0f;

        return 0.0f;
    }

    uint32_t advance = static_cast<uint32_t>(this->GetGlyphAdvance(code));

    if (advance >= this->m_cellHeight) {
        return this->m_cellHeight;
    }

    uint32_t padding = this->m_cellHeight - advance;

    if (padding & 1) {
        padding--;
    }

    uint32_t step = this->m_cellHeight - padding / 2;

    uint32_t nextAdvance = static_cast<uint32_t>(this->GetGlyphAdvance(nextCode));

    if (nextAdvance >= this->m_cellHeight) {
        return step;
    }

    uint32_t nextPadding = this->m_cellHeight - nextAdvance;

    if (nextPadding & 1) {
        nextPadding--;
    }

    return step + nextPadding / 2;
}

// VERIFY: 141d931e manual
// OFFSET: 0x006C3FC0
CHARCODEDESC* CGxFont::NewCodeDesc(uint32_t code) {
    CHARCODEDESC* desc = this->m_activeCharacters.Ptr(code, HASHKEY_NONE());

    if (desc) {
        this->m_activeCharacterCache.LinkToHead(desc);
        return desc;
    }

    GLYPHBITMAPDATA data;

    if (!this->GetGlyphData(&data, code)) {
        return nullptr;
    }

    TEXTURECACHE* cache = nullptr;
    uint32_t textureNumber;

    for (textureNumber = 0; textureNumber < 8; textureNumber++) {
        TEXTURECACHE* current = &this->m_textureCache[textureNumber];

        if (!current->m_texture || !TextureGetGxTex(current->m_texture, 1, nullptr)) {
            cache = current;
            cache->CreateTexture(this->m_fontFlags & CGxFont::FONT_FLAGS::FONT_LINEAR_FILTER);
            cache->Initialize(this, this->m_cellHeight);
            desc = cache->AllocateNewGlyph(&data);
            break;
        }

        desc = current->AllocateNewGlyph(&data);

        if (desc) {
            cache = current;
            break;
        }
    }

    if (desc) {
        desc->m_textureNumber = textureNumber;
    } else {
        CHARCODEDESC* oldest = this->m_activeCharacterCache.Tail();

        if (oldest) {
            textureNumber = oldest->m_textureNumber;
            uint32_t rowNumber = oldest->m_rowNumber;
            STORM_ASSERT(textureNumber < sizeof(m_textureCache) / sizeof(m_textureCache[0])); // 12213: "textureNumber < ARRSIZE(m_textureCache)"
            TEXTURECACHE* newTextureCache = &this->m_textureCache[textureNumber];
            STORM_ASSERT(rowNumber < newTextureCache->m_textureRows.Count());
            cache = newTextureCache;
            TEXTURECACHEROW* row = &cache->m_textureRows.m_data[rowNumber];

            row->EvictGlyph(oldest);
            this->RegisterEvictNotice(textureNumber);

            desc = row->CreateNewDesc(&data, rowNumber, this->m_cellHeight);

            if (desc) {
                desc->m_rowNumber = rowNumber;
                desc->m_textureNumber = textureNumber;
            }
        }
    }

    if (desc) {
        this->m_activeCharacters.Insert(desc, code, HASHKEY_NONE());
        this->m_activeCharacterCache.LinkToHead(desc);

        if (cache) {
            cache->m_anyDirtyGlyphs = 1;
        }
    }

    return desc;
}
