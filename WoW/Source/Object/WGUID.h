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
