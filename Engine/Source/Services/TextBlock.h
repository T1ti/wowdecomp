#ifndef ENGINE_SOURCE_SERVICES_TEXTBLOCK_H
#define ENGINE_SOURCE_SERVICES_TEXTBLOCK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class C2Vector;
class CImVector;
struct CGxFont;
struct CGxString;

void TextBlockAddShadow(HTEXTBLOCK string, CImVector color, const C2Vector& offset);
HTEXTBLOCK TextBlockCreate(HTEXTFONT font, const char* text, const CImVector& color, const C3Vector& position, float fontHeight, float blockWidth, float blockHeight, uint32_t flags, float charSpacing, float lineSpacing, float scale);
HTEXTFONT TextBlockGenerateFont(const char* fontName, uint32_t fontFlags, float fontHeight);
uint32_t TextBlockGetFontFlags(HTEXTFONT font);
const char* TextBlockGetFontName(HTEXTFONT font);
CGxFont* TextBlockGetFontPtr(HTEXTFONT font);
uint32_t TextBlockGetMaxCharsWithinWidth(HTEXTFONT font, const char* text, float fontHeight, float maxWidth, uint32_t numBytes, float* extent, float shadowWidth, float scale, float unused, uint32_t flags);
uint32_t TextBlockGetMaxCharsWithinWidthAndHeight(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, float blockHeight, uint32_t numBytes, float shadowWidth, float scale, float spacing, uint32_t flags);
uint32_t TextBlockGetMaxCharsWithinWidthFromEnd(HTEXTFONT font, const char* text, float fontHeight, float maxWidth, uint32_t numBytes, float* extent, float shadowWidth, float scale, float unused, uint32_t flags);
C3Vector TextBlockGetStringPos(HTEXTBLOCK string);
CGxString* TextBlockGetStringPtr(HTEXTBLOCK string);
void TextBlockGetTextExtent(HTEXTFONT font, const char* text, uint32_t numBytes, float fontHeight, float* extent, float shadowWidth, float scale, float unused, uint32_t flags);
float TextBlockGetWrappedTextHeight(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, const C2Vector& shadowOffset, float scale, float spacing, uint32_t flags);
void TextBlockRemoveShadow(HTEXTBLOCK string);
int32_t TextBlockSetGradient(HTEXTBLOCK text, int32_t start, int32_t length);
void TextBlockSetStringPos(HTEXTBLOCK string, const C3Vector& position);
void TextBlockUpdateColor(HTEXTBLOCK string, const CImVector& color);
uint32_t TextBlockWrapText(HTEXTFONT font, const char* text, float fontHeight, float blockWidth, uint32_t* lineStarts, uint32_t maxLines, float shadowWidth, float scale, float unused, uint32_t flags);

#endif
