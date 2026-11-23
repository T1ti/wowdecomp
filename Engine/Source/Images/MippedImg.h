#ifndef ENGINE_SOURCE_IMAGES_MIPPEDIMG_H
#define ENGINE_SOURCE_IMAGES_MIPPEDIMG_H

#include <cstdint>
#include "Engine/Source/BLPFile/Types.h"

uint32_t CalcLevelCount(uint32_t width, uint32_t height);
uint32_t CalcLevelSize(uint32_t level, uint32_t width, uint32_t height, uint32_t format);
void FullShrink(C4Pixel* dest, uint32_t destWidth, uint32_t destHeight, const C4Pixel* source, uint32_t sourceWidth, uint32_t sourceHeight);
MipBits* MippedImgAllocA(uint32_t format, uint32_t width, uint32_t height, const char* fileName, int32_t lineNumber);
uint32_t MippedImgCalcSize(uint32_t format, uint32_t width, uint32_t height);
void MippedImgSet(uint32_t format, uint32_t width, uint32_t height, MipBits* bits);

#endif
