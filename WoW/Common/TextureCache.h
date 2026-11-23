#ifndef WOW_COMMON_TEXTURECACHE_H
#define WOW_COMMON_TEXTURECACHE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/ComponentCore/Types.h"

class CACHEENTRY;
typedef CACHEENTRY* HTEXTURECACHE;

void TextureCacheComputeLoadProgress(HTEXTURECACHE handle, int64_t& local, int64_t& total);
HTEXTURECACHE TextureCacheCopyTexture(HTEXTURECACHE handle);
HTEXTURECACHE TextureCacheCreateTexture(const char* fileName);
void TextureCacheDestroy();
void TextureCacheDestroyTexture(HTEXTURECACHE handle);
int32_t TextureCacheGetInfo(HTEXTURECACHE handle, TCTEXTUREINFO& info, BOOL wait);
void* TextureCacheGetMip(HTEXTURECACHE handle, uint32_t mipLevel);
BlpPalPixel* TextureCacheGetPal(HTEXTURECACHE handle);
int32_t TextureCacheHasMips(HTEXTURECACHE handle);
void TextureCacheResetLoadCount();

#endif
