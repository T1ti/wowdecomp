#ifndef WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_H
#define WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Game/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;

// Longest player name a Lua call accepts before raising "Name too long". Name inferred.
constexpr uint32_t MAX_PLAYER_NAME_LENGTH = 48;

class NameCache {
    public:
    // SMSG_NAME_QUERY_RESPONSE result byte, named after what ReceiveName does with each value.
    enum QUERY_RESULT : uint8_t {
        QUERY_RESULT_OK = 0,
        QUERY_RESULT_UNKNOWN_1 = 1, // Denied like any other unhandled value.
        QUERY_RESULT_RETRY = 2,
        QUERY_RESULT_PLACEHOLDER = 3, // Cached temporarily as "?".
    };

    static const uint32_t s_cacheVersion = 4;

    struct DeclinedNames {
        /* 0000 */ char m_names[NUM_DECLENSIONS][64];
    };

    /* 0000 */ char m_name[48];
    /* 0030 */ NameCache::DeclinedNames* m_declinedNames;
    /* 0034 */ char m_realmName[256];
    /* 0134 */ uint32_t unused_0134;
    /* 0138 */ WOWGUID m_guid;
    /* 0140 */ uint32_t m_race;
    /* 0144 */ uint32_t m_sex;
    /* 0148 */ uint32_t m_class;

    NameCache();
    ~NameCache();
    NameCache& Copy(const NameCache& other);
    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
    char* FullName(char* buffer, uint32_t size);
};

#endif
