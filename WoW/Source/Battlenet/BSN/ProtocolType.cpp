#include "WoW/Source/Battlenet/BSN/Protocol.h"

// VERIFY: b7cad848 manual
// OFFSET: 0x0088A050
void BSN::Protocol::Type::Int::Get(int32_t intType, const uint8_t*& cursor, uint64_t& value) {
    uint32_t size = BSN::Protocol::Type::Int::ByteSize(intType);

    if ((intType & BSN::Protocol::Type::Int::TYPE_UNSIGNED) || *cursor < 0x80) {
        value = 0;
    } else {
        value = static_cast<uint64_t>(-1);
    }

    for (; size; size--) {
        value = (value << 8) | *cursor++;
    }
}

// VERIFY: b0a9090d auto
// OFFSET: 0x0088A0C0
void BSN::Protocol::Type::Int::Get(int32_t intType, const uint8_t*& cursor, uint32_t& value) {
    uint32_t size = BSN::Protocol::Type::Int::ByteSize(intType);

    for (value = 0; size; cursor++) {
        value <<= 8;
        size--;
        value |= *cursor;
    }
}

// VERIFY: ec7bb8f9 manual
// OFFSET: 0x0088A100
void BSN::Protocol::Type::Int::GetSize(const uint8_t*& cursor, uint32_t& value) {
    uint8_t intType = *cursor++;
    BSN::Protocol::Type::Int::Get(intType, cursor, value);
}

// VERIFY: 88b02841 manual
// OFFSET: 0x0088A130
bool BSN::Protocol::Type::Bounds::Contains(uint64_t value) const {
    if (this->m_intType & BSN::Protocol::Type::Int::TYPE_UNSIGNED) {
        return value >= this->m_min && value <= this->m_max;
    }

    return static_cast<int64_t>(value) >= static_cast<int64_t>(this->m_min) && static_cast<int64_t>(value) <= static_cast<int64_t>(this->m_max);
}

// VERIFY: fa5dc7e8 manual
// OFFSET: 0x0088A180
void BSN::Protocol::Type::Bounds::Get(bool fixed, const uint8_t*& cursor, Bounds& bounds) {
    uint8_t intType = *cursor++;
    bounds.m_intType = intType;

    if (fixed) {
        bounds.m_variable = false;
        bounds.m_bits = 0;
        BSN::Protocol::Type::Int::Get(intType, cursor, bounds.m_min);
        bounds.m_max = bounds.m_min;
    } else {
        bounds.m_variable = *cursor >> 7;
        bounds.m_bits = *cursor++ & 0x7F;
        BSN::Protocol::Type::Int::Get(bounds.m_intType, cursor, bounds.m_min);
        BSN::Protocol::Type::Int::Get(bounds.m_intType, cursor, bounds.m_max);
    }
}

// VERIFY: 3ee45e08 auto
// OFFSET: 0x0088A200
void BSN::Protocol::Type::GetTypeId(const uint8_t*& cursor, uint32_t& typeId) {
    typeId = 0;

    for (uint32_t i = 4; i; i--) {
        typeId <<= 8;
        typeId |= *cursor++;
    }
}
