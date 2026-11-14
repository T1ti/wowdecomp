#ifndef WOW_SOURCE_OBJECT_WGUID_H
#define WOW_SOURCE_OBJECT_WGUID_H

#include <cstddef>
#include <cstdint>

class CDataStore;

// Native client entry extraction uses 28 bits after shifting by 24.
// Preserve that exact mask rather than substituting a different GUID format.
constexpr int32_t WOWGUID_ENTRY_SHIFT = 24;
constexpr int32_t WOWGUID_ENTRY_MASK = 0x0FFFFFFF;
constexpr int32_t WOWGUID_HIGH_WORD_SHIFT = 32;
constexpr uint32_t WOWGUID_WORD_ALL_ONES = 0xFFFFFFFF;

// Synthetic requester GUID for spell-backed item/tooltip/cache queries.
constexpr uint64_t WOWGUID_SPELL_QUERY_TAG = 0x1FE0000000000000ull;
// Script_GetGUIDFromToken distinguishes explicit "none" from a valid empty unit token.
constexpr uint64_t WOWGUID_NONE_TOKEN = 0xFFFFFFFFFFFFFFFFull;
constexpr uint64_t WOWGUID_EMPTY_UNIT_TOKEN = 0xFFFFFFFFFFFFFFFEull;

// Inlined GUID type tests on the high word (names follow the server HighGuid layout).
// A player GUID has a clear top nibble and a non-zero (high & PLAYER_VALUE_MASK) | low.
constexpr uint32_t WOWGUID_HIGH_NON_PLAYER_MASK = 0xF0000000;
constexpr uint32_t WOWGUID_HIGH_PLAYER_VALUE_MASK = 0xF07FFFFF;
constexpr uint32_t WOWGUID_HIGH_TYPE_MASK = 0xF0F00000;
constexpr uint32_t WOWGUID_HIGH_TYPE_GAMEOBJECT = 0xF0100000;
constexpr uint32_t WOWGUID_HIGH_TYPE_CREATURE = 0xF0300000;
constexpr uint32_t WOWGUID_HIGH_TYPE_PET = 0xF0400000;
constexpr uint32_t WOWGUID_HIGH_TYPE_VEHICLE = 0xF0500000;
// The same tests on the full 64-bit value.
constexpr uint64_t WOWGUID_NON_PLAYER_MASK = 0xF000000000000000ull;
constexpr uint64_t WOWGUID_PLAYER_VALUE_MASK = 0xF07FFFFFFFFFFFFFull;
constexpr uint64_t WOWGUID_TYPE_MASK = 0xF0F0000000000000ull;
constexpr uint64_t WOWGUID_TYPE_PET = 0xF040000000000000ull;
constexpr uint64_t WOWGUID_TYPE_VEHICLE = 0xF050000000000000ull;

struct WOWGUID {
    /* 0000 */ uint64_t m_guid;

    explicit operator bool() const {
        return this->m_guid != 0;
    }

    bool operator==(const WOWGUID& guid) const {
        return this->m_guid == guid.m_guid;
    }

    bool operator!=(const WOWGUID& guid) const {
        return this->m_guid != guid.m_guid;
    }
};

struct SmartGUID : public WOWGUID {
    CDataStore& Put(CDataStore& msg) const;
};

CDataStore& operator>>(CDataStore& msg, SmartGUID& guid);
CDataStore& operator<<(CDataStore& msg, const SmartGUID& guid);

#endif
