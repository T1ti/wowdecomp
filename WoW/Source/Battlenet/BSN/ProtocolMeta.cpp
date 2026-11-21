#include "WoW/Source/Battlenet/BSN/Protocol.h"

// VERIFY: b89f54af auto
// OFFSET: 0x008889E0
uint32_t BSN::Protocol::Meta::PeekHeaderEnum(const Protocol* protocol, uint32_t typeId) {
    return *protocol->TypeInfo(typeId) & BSN::META_HEADER_TYPE_MASK;
}

// VERIFY: 254e0587 auto
// OFFSET: 0x00888A00
BSN::Protocol::Meta::Base::Base(const Protocol* protocol, uint32_t typeId) {
    this->m_protocol = protocol;
    this->m_typeId = typeId;
    this->m_typeInfo = protocol->TypeInfo(typeId);
    this->m_dumpInfo = protocol->DumpInfo(typeId);

    const void* const* codeInfo = protocol->m_codeInfo;
    uintptr_t index = reinterpret_cast<uintptr_t>(codeInfo[typeId + 1]);
    this->m_codeInfo = index ? &codeInfo[index] : nullptr;

    this->m_header = *this->m_typeInfo++;
}

// VERIFY: 37fb8121 auto
// OFFSET: 0x00888A60
BSN::Protocol::Meta::Array::Array(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get((this->m_header & BSN::META_HEADER_FIXED) != 0, this->m_typeInfo, this->m_bounds);
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_elementTypeId);
}

// VERIFY: e473c50c auto
// OFFSET: 0x00888AA0
BSN::Protocol::Meta::BitArray::BitArray(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get((this->m_header & BSN::META_HEADER_FIXED) != 0, this->m_typeInfo, this->m_bounds);
}

// VERIFY: 360f5363 auto
// OFFSET: 0x00888AA0
BSN::Protocol::Meta::Blob::Blob(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get((this->m_header & BSN::META_HEADER_FIXED) != 0, this->m_typeInfo, this->m_bounds);
}

// VERIFY: 2a93db84 manual
// OFFSET: 0x00888AE0
bool BSN::Protocol::Meta::Choice::GetTagField(uint64_t tag, uint32_t& field) const {
    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        uint32_t size = BSN::Protocol::Type::Int::ByteSize(this->m_bounds.m_intType);
        field = 0;

        if (this->m_count) {
            do {
                const uint8_t* cursor = &this->m_typeInfo[size * field];
                uint64_t value;
                BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, value);

                if (tag == value) {
                    return true;
                }
            } while (++field < this->m_count);
        }
    } else if (this->m_bounds.Contains(tag)) {
        field = static_cast<uint32_t>(tag - this->m_bounds.m_min);
        return true;
    }

    return false;
}

// VERIFY: 79b7916b manual
// OFFSET: 0x00888B80
bool BSN::Protocol::Meta::Choice::GetFieldTag(uint32_t field, uint64_t& tag) const {
    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        if (field < this->m_count) {
            const uint8_t* cursor = &this->m_typeInfo[field * BSN::Protocol::Type::Int::ByteSize(this->m_bounds.m_intType)];
            BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, tag);
            return true;
        }
    } else {
        tag = field + this->m_bounds.m_min;

        if (this->m_bounds.Contains(tag)) {
            return true;
        }
    }

    return false;
}

// VERIFY: 18d682f6 manual
// OFFSET: 0x00888C00
bool BSN::Protocol::Meta::Choice::GetFieldTypeId(uint32_t field, uint32_t& typeId) const {
    if (field >= this->m_count) {
        return false;
    }

    const uint8_t* cursor;

    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        cursor = &this->m_typeInfo[this->m_count * BSN::Protocol::Type::Int::ByteSize(this->m_bounds.m_intType) + 4 * field];
    } else {
        cursor = &this->m_typeInfo[4 * field];
    }

    BSN::Protocol::Type::GetTypeId(cursor, typeId);
    return true;
}

// VERIFY: b478940d manual
// OFFSET: 0x00888C70
bool BSN::Protocol::Meta::Enum::GetEnumField(uint64_t value, uint32_t& field) const {
    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        const uint8_t* cursor = this->m_typeInfo;
        field = 0;

        if (this->m_count) {
            do {
                uint64_t entry;
                BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, entry);

                if (value == entry) {
                    return true;
                }
            } while (++field < this->m_count);
        }
    } else if (this->m_bounds.Contains(value)) {
        field = static_cast<uint32_t>(value - this->m_bounds.m_min);
        return true;
    }

    return false;
}

// VERIFY: 1a17d977 auto
// OFFSET: 0x00888D00
BSN::Protocol::Meta::Int::Int(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);
}

// VERIFY: e404c9f1 auto
// OFFSET: 0x00888D00
BSN::Protocol::Meta::AsciiString::AsciiString(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);
}

// VERIFY: 1fc467b3 auto
// OFFSET: 0x00888D30
BSN::Protocol::Meta::User::User(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_userTypeId);
}

// VERIFY: d22bab36 auto
// OFFSET: 0x00888D30
BSN::Protocol::Meta::Optional::Optional(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_elementTypeId);
}

// VERIFY: c0b3cda7 auto
// OFFSET: 0x00888D60
BSN::Protocol::Meta::Bool::Bool(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// VERIFY: 0f1b830e auto
// OFFSET: 0x00888D60
BSN::Protocol::Meta::FourCC::FourCC(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// VERIFY: 4ef8154b auto
// OFFSET: 0x00888D60
BSN::Protocol::Meta::Null::Null(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// VERIFY: 5a46550a auto
// OFFSET: 0x00888D60
BSN::Protocol::Meta::Real32::Real32(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// VERIFY: 01c1ae32 auto
// OFFSET: 0x00888D60
BSN::Protocol::Meta::Real64::Real64(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// VERIFY: c8d2bcd7 manual
// OFFSET: 0x00888D80
BSN::Protocol::Meta::Enum::Enum(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// VERIFY: 8b106e47 manual
// OFFSET: 0x00888D80
BSN::Protocol::Meta::Choice::Choice(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// VERIFY: 39bb5ffb manual
// OFFSET: 0x00888D80
BSN::Protocol::Meta::Struct::Struct(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// VERIFY: e1ec8bc2 manual
// OFFSET: 0x00888DE0
bool BSN::Protocol::Meta::Struct::GetMember(uint32_t index, uint32_t& offset, uint32_t& typeId) const {
    if (index >= this->m_count) {
        return false;
    }

    offset = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this->m_codeInfo[index + 1]));

    const uint8_t* cursor;

    if (!(this->m_header & BSN::META_HEADER_FIXED)) {
        cursor = &this->m_typeInfo[4 * index + this->m_count * BSN::Protocol::Type::Int::ByteSize(this->m_bounds.m_intType)];
    } else {
        cursor = &this->m_typeInfo[4 * index];
    }

    BSN::Protocol::Type::GetTypeId(cursor, typeId);
    return true;
}

// VERIFY: 30249418 manual
// OFFSET: 0x00888E60
BSN::Protocol::Meta::String::String(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_charBounds);
    this->m_bounds = this->m_charBounds;
    this->m_bounds.m_bits += 2;
    this->m_bounds.m_max *= 4;
}

// VERIFY: d65c5c93 manual
// OFFSET: 0x00888EC0
bool BSN::Protocol::Meta::String::IsValid(uint32_t length, const uint8_t* data, uint32_t& count) {
    uint32_t position = 0;
    count = 0;

    if (!length) {
        return true;
    }

    do {
        const uint8_t* sequence = &data[position];
        uint32_t index = 0;

        while (*sequence < BSN::s_utf8Sequences[index].m_ranges[0][0] || *sequence > BSN::s_utf8Sequences[index].m_ranges[0][1]) {
            if (++index >= 9) {
                return false;
            }
        }

        uint32_t sequenceLength = BSN::s_utf8Sequences[index].m_length;

        if (length - position < sequenceLength) {
            return false;
        }

        for (uint32_t i = 1; i < sequenceLength; i++) {
            if (sequence[i] < BSN::s_utf8Sequences[index].m_ranges[i][0] || sequence[i] > BSN::s_utf8Sequences[index].m_ranges[i][1]) {
                return false;
            }
        }

        position += sequenceLength;
        count++;
    } while (position < length);

    return true;
}
