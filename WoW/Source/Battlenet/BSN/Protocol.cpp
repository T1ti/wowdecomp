#include "WoW/Source/Battlenet/BSN/Protocol.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: fcec72ef auto
// OFFSET: 0x00889E50
uint32_t BSN::Protocol::GetUInt32(const uint8_t*& cursor) {
    uint32_t value = 0;

    for (uint32_t i = 4; i; i--) {
        value = (value << 8) | *cursor++;
    }

    return value;
}

// VERIFY: af4a973b auto
// OFFSET: 0x00889E80
BSN::Protocol::Protocol::Protocol(const uint8_t* bytecodeData, uint32_t bytecodeSize, const void* const* codeInfoData, uint32_t codeInfoCount) {
    this->bytecodeSize = bytecodeSize;
    this->m_codeInfoCount = codeInfoCount;
    this->m_codeInfo = codeInfoData;
    this->m_bytecode = bytecodeData;

    // 12213: "bytecodeSize >= 5 * sizeof(u32)"
    STORM_ASSERT(bytecodeSize >= 5 * sizeof(uint32_t));
    const uint8_t* bytecode = bytecodeData;

    uint32_t version = GetUInt32(bytecode);
    this->m_version = version;
    STORM_ASSERT(version == 7);

    this->unk_0014 = GetUInt32(bytecode);

    uint32_t typeIdCount = GetUInt32(bytecode);
    this->typeIdCount = typeIdCount;
    STORM_ASSERT((size_t)codeInfoData[0] == typeIdCount);

    uint32_t typeInfoSize = GetUInt32(bytecode);
    this->m_typeInfoSize = typeInfoSize;

    uint32_t dumpInfoSize = GetUInt32(bytecode);
    this->m_dumpInfoSize = dumpInfoSize;

    this->m_typeOffsets = bytecode;
    const uint8_t* typeInfo = &bytecode[4 * typeIdCount];
    this->m_typeInfo = typeInfo;

    const uint8_t* dumpOffsets = &typeInfo[typeInfoSize];

    if (dumpOffsets == &bytecodeData[bytecodeSize]) {
        this->m_dumpInfoSize = 0;
        this->m_dumpOffsets = nullptr;
        this->m_dumpInfo = nullptr;
    } else {
        STORM_ASSERT(dumpInfoSize != 0);
        this->m_dumpInfo = &dumpOffsets[4 * typeIdCount];
        this->m_dumpOffsets = dumpOffsets;
#if defined(WOWDECOMP_ASSERTIONS)
        bytecode = this->m_dumpInfo + dumpInfoSize;
        STORM_ASSERT(bytecode == bytecodeData + bytecodeSize);
#endif
    }
}

// VERIFY: 40eb7cef auto
// OFFSET: 0x00889F80
const uint8_t* BSN::Protocol::Protocol::TypeInfo(uint32_t t) const {
    STORM_ASSERT(t < typeIdCount);

    const uint8_t* cursor = &this->m_typeOffsets[4 * t];
    uint32_t offset = GetUInt32(cursor);
    STORM_ASSERT(offset < bytecodeSize);

    return &this->m_bytecode[offset];
}

// VERIFY: de035afd auto
// OFFSET: 0x00889FC0
const uint8_t* BSN::Protocol::Protocol::DumpInfo(uint32_t t) const {
    STORM_ASSERT(t < typeIdCount);

    if (!this->m_dumpInfoSize) {
        return nullptr;
    }

    const uint8_t* cursor = &this->m_dumpOffsets[4 * t];
    uint32_t offset = GetUInt32(cursor);
    STORM_ASSERT(offset < bytecodeSize);

    if (!offset) {
        return nullptr;
    }

    return &this->m_bytecode[offset];
}

// VERIFY: 0f8cdc61 auto
// OFFSET: 0x0088A000
const char* BSN::Protocol::Protocol::DumpString(const uint8_t* dumpInfo, uint32_t index) const {
    if (!this->m_dumpInfoSize || !dumpInfo) {
        return "";
    }

    const uint8_t* cursor = &dumpInfo[4 * index];
    uint32_t offset = GetUInt32(cursor);
    STORM_ASSERT(offset < bytecodeSize);

    if (!offset) {
        return nullptr;
    }

    return reinterpret_cast<const char*>(&this->m_bytecode[offset]);
}
