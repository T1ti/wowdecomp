#ifndef WOW_SOURCE_BATTLENET_BSN_PROTOCOL_H
#define WOW_SOURCE_BATTLENET_BSN_PROTOCOL_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BSN/Types.h"

namespace BSN {
class FourCC {
    public:
    /* 0000 */ uint32_t m_value;

    FourCC();
    FourCC(const char* string);
    void ToString(char* dest, uint32_t size) const;
};

namespace Protocol {
namespace Meta {
class Base;
}

class Protocol {
    public:
    Protocol(const uint8_t* bytecodeData, uint32_t bytecodeSize, const void* const* codeInfoData, uint32_t codeInfoCount);
    const uint8_t* TypeInfo(uint32_t t) const;
    const uint8_t* DumpInfo(uint32_t t) const;
    const char* DumpString(const uint8_t* dumpInfo, uint32_t index) const;
    bool HasDumpInfo() const { return m_dumpInfoSize != 0; } // (inferred)

    private:
    friend class Meta::Base; // Access inferred from its direct code table lookup.
    /* 0000 */ uint32_t bytecodeSize;
    /* 0004 */ const uint8_t* m_bytecode;
    /* 0008 */ uint32_t m_codeInfoCount;
    /* 000C */ const void* const* m_codeInfo;
    /* 0010 */ uint32_t m_version;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t typeIdCount;
    /* 001C */ uint32_t m_typeInfoSize;
    /* 0020 */ uint32_t m_dumpInfoSize;
    /* 0024 */ const uint8_t* m_typeOffsets;
    /* 0028 */ const uint8_t* m_typeInfo;
    /* 002C */ const uint8_t* m_dumpOffsets;
    /* 0030 */ const uint8_t* m_dumpInfo;
};

uint32_t GetUInt32(const uint8_t*& cursor);

namespace Type {
struct Bounds {
    // Non-fixed bounds byte, named after the fields it fills.
    static const uint8_t VARIABLE_FLAG = 0x80;
    static const uint8_t BITS_MASK = 0x7F;

    /* 0000 */ int32_t m_intType;
    /* 0004 */ uint32_t m_bits;
    /* 0008 */ uint64_t m_min;
    /* 0010 */ uint64_t m_max;
    /* 0018 */ bool m_variable;

    static void Get(bool fixed, const uint8_t*& cursor, Bounds& bounds);

    bool Contains(uint64_t value) const;
};

void GetTypeId(const uint8_t*& cursor, uint32_t& typeId);

namespace Int {
// Int type byte: bit 0 marks an unsigned value, the bits above it hold log2 of the
// encoded byte size. Names inferred from Int::Get and Bounds::Contains.
static const int32_t TYPE_UNSIGNED = 0x1;

inline uint32_t ByteSize(int32_t intType) {
    return 1 << (intType >> 1);
}

inline bool IsSigned(int32_t intType) {
    return (intType & TYPE_UNSIGNED) == 0;
}

void Get(int32_t intType, const uint8_t*& cursor, uint64_t& value);
void Get(int32_t e, const uint8_t*& cursor, uint32_t& value);
void GetSize(const uint8_t*& cursor, uint32_t& value);
}
}

namespace Meta {
uint32_t PeekHeaderEnum(const Protocol* protocol, uint32_t typeId);

class Base {
    public:
    /* 0000 */ const Protocol* m_protocol;
    /* 0004 */ uint32_t m_typeId;
    /* 0008 */ uint8_t m_header;
    /* 000C */ const uint8_t* m_typeInfo;
    /* 0010 */ const uint8_t* m_dumpInfo;
    /* 0014 */ const void* const* m_codeInfo;

    Base(const Protocol* protocol, uint32_t typeId);
};

class Bool : public Base {
    public:
    Bool(const Protocol* protocol, uint32_t typeId);
};

class FourCC : public Base {
    public:
    FourCC(const Protocol* protocol, uint32_t typeId);
};

class Null : public Base {
    public:
    Null(const Protocol* protocol, uint32_t typeId);
};

class Real32 : public Base {
    public:
    Real32(const Protocol* protocol, uint32_t typeId);
};

class Real64 : public Base {
    public:
    Real64(const Protocol* protocol, uint32_t typeId);
};

class Int : public Base {
    public:
    /* 0018 */ Type::Bounds bounds;

    Int(const Protocol* protocol, uint32_t typeId);
};

class AsciiString : public Base {
    public:
    /* 0018 */ Type::Bounds m_bounds;

    AsciiString(const Protocol* protocol, uint32_t typeId);
};

class BitArray : public Base {
    public:
    /* 0018 */ Type::Bounds m_bounds;

    BitArray(const Protocol* protocol, uint32_t typeId);
};

class Blob : public Base {
    public:
    /* 0018 */ Type::Bounds m_bounds;

    Blob(const Protocol* protocol, uint32_t typeId);
};

class Array : public Base {
    public:
    /* 0018 */ Type::Bounds m_bounds;
    /* 0038 */ uint32_t m_elementTypeId;

    Array(const Protocol* protocol, uint32_t typeId);
};

class Enum : public Base {
    public:
    /* 0018 */ Type::Bounds bounds;
    /* 0038 */ uint32_t m_count;

    Enum(const Protocol* protocol, uint32_t typeId);
    bool GetEnumField(uint64_t value, uint32_t& field) const;
};

class Choice : public Base {
    public:
    /* 0018 */ Type::Bounds bounds;
    /* 0038 */ uint32_t m_count;

    Choice(const Protocol* protocol, uint32_t typeId);
    bool GetTagField(uint64_t tag, uint32_t& field) const;
    bool GetFieldTag(uint32_t field, uint64_t& tag) const;
    bool GetFieldTypeId(uint32_t field, uint32_t& typeId) const;
};

class Struct : public Base {
    public:
    /* 0018 */ Type::Bounds m_bounds;
    /* 0038 */ uint32_t fieldCount;

    Struct(const Protocol* protocol, uint32_t typeId);
    bool GetMember(uint32_t index, uint32_t& offset, uint32_t& typeId) const;
};

class String : public Base {
    public:
    static bool IsValid(uint32_t length, const uint8_t* data, uint32_t& count);

    /* 0018 */ Type::Bounds bounds;
    /* 0038 */ Type::Bounds m_charBounds;

    String(const Protocol* protocol, uint32_t typeId);
};

class User : public Base {
    public:
    /* 0018 */ uint32_t m_userTypeId;

    User(const Protocol* protocol, uint32_t typeId);
};

class Optional : public Base {
    public:
    /* 0018 */ uint32_t m_elementTypeId;

    Optional(const Protocol* protocol, uint32_t typeId);
};
}
}

namespace Cpp {
void ReadInt(const void* data, uint32_t size, bool isSigned, uint64_t& value);
void WriteInt(uint64_t value, uint32_t size, void* data);
}

extern const Utf8Sequence s_utf8Sequences[9];
}

#endif
