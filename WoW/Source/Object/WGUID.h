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

// GUID type, tested by the client as the top nibble plus the type nibble of the high word.
// A player GUID has a clear top nibble and a non-zero value outside the 0x0F80... bits.
constexpr uint64_t WOWGUID_NON_PLAYER_MASK = 0xF000000000000000ull;
constexpr uint64_t WOWGUID_PLAYER_VALUE_MASK = 0xF07FFFFFFFFFFFFFull;
constexpr uint64_t WOWGUID_TYPE_MASK = 0xF0F0000000000000ull;
constexpr uint64_t WOWGUID_TYPE_GAMEOBJECT = 0xF010000000000000ull;
constexpr uint64_t WOWGUID_TYPE_CREATURE = 0xF030000000000000ull;
constexpr uint64_t WOWGUID_TYPE_PET = 0xF040000000000000ull;
constexpr uint64_t WOWGUID_TYPE_VEHICLE = 0xF050000000000000ull;

struct WOWGUID {
    /* 0000 */ uint64_t m_guid;

    explicit operator bool() const {
        return this->m_guid != 0;
    }

    // INLINED helpers; every client copy is expanded in place.
    uint64_t GetType() const {
        return this->m_guid & WOWGUID_TYPE_MASK;
    }

    int32_t GetEntry() const {
        return static_cast<int32_t>(this->m_guid >> WOWGUID_ENTRY_SHIFT) & WOWGUID_ENTRY_MASK;
    }

    bool IsPlayer() const {
        return !(this->m_guid & WOWGUID_NON_PLAYER_MASK) && (this->m_guid & WOWGUID_PLAYER_VALUE_MASK);
    }

    bool IsGameObject() const {
        return this->GetType() == WOWGUID_TYPE_GAMEOBJECT;
    }

    bool IsCreature() const {
        return this->GetType() == WOWGUID_TYPE_CREATURE;
    }

    bool IsPet() const {
        return this->GetType() == WOWGUID_TYPE_PET;
    }

    bool IsVehicle() const {
        return this->GetType() == WOWGUID_TYPE_VEHICLE;
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
