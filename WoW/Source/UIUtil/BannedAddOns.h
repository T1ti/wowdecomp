#ifndef WOW_SOURCE_UIUTIL_BANNEDADDONS_H
#define WOW_SOURCE_UIUTIL_BANNEDADDONS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class BannedAddOnCache {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ uint8_t m_nameMd5[16];
    /* 0014 */ uint8_t m_versionMd5[16];
    /* 0024 */ int32_t m_lastModified;
    /* 0028 */ int32_t m_flags; // BANNED_ADDON_FLAGS

    BannedAddOnCache();
    BannedAddOnCache& operator=(const BannedAddOnCache& source);
};

struct _BANNEDADDON : public TSHashObject<_BANNEDADDON, HASHKEY_NONE> {
    /* 0018 */ BannedAddOnCache m_cache;
    /* 0044 */ int32_t m_fromDB;
};

struct _BANNEDADDONLOADNODE : public TSLinkedNode<_BANNEDADDONLOADNODE> {
    /* 0008 */ BannedAddOnCache m_cache;
};

void DestroyBannedAddOns();
BannedAddOnCache* FindBannedAddOn(const char* name, const char* version);
int32_t GetLatestBannedAddOnTimestamp();
bool IsBannedAddOn(const char* name, const char* version);
int32_t LoadBannedAddOns();
BannedAddOnCache* RegisterBannedAddOn(int32_t id, const int32_t* lastModified, int32_t* updated);
int32_t SaveBannedAddOns();

#endif
