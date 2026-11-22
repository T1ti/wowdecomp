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
BSN::Protocol::Protocol::Protocol(const uint8_t* bytecode, uint32_t bytecodeSize, const void* const* codeInfo, uint32_t codeInfoCount) {
    this->m_bytecodeSize = bytecodeSize;
    this->m_codeInfoCount = codeInfoCount;
    this->m_codeInfo = codeInfo;
    this->m_bytecode = bytecode;

    const uint8_t* cursor = bytecode;

    uint32_t version = 0;
    for (uint32_t i = 4; i; i--) {
        version = (version << 8) | *cursor++;
    }
    this->m_version = version;

    uint32_t value = 0;
    for (uint32_t i = 4; i; i--) {
        value = (value << 8) | *cursor++;
    }
    this->unk_0014 = value;

    uint32_t typeIdCount = 0;
    for (uint32_t i = 4; i; i--) {
        typeIdCount = (typeIdCount << 8) | *cursor++;
    }
    this->typeIdCount = typeIdCount;

    uint32_t typeInfoSize = 0;
    for (uint32_t i = 4; i; i--) {
        typeInfoSize = (typeInfoSize << 8) | *cursor++;
    }
    this->m_typeInfoSize = typeInfoSize;

    uint32_t dumpInfoSize = 0;
    for (uint32_t i = 4; i; i--) {
        dumpInfoSize = (dumpInfoSize << 8) | *cursor++;
    }
    this->m_dumpInfoSize = dumpInfoSize;

    this->m_typeOffsets = cursor;
    const uint8_t* typeInfo = &cursor[4 * typeIdCount];
    this->m_typeInfo = typeInfo;

    const uint8_t* dumpOffsets = &typeInfo[typeInfoSize];

    if (dumpOffsets == &bytecode[bytecodeSize]) {
        this->m_dumpInfoSize = 0;
        this->m_dumpOffsets = nullptr;
        this->m_dumpInfo = nullptr;
    } else {
        this->m_dumpInfo = &dumpOffsets[4 * typeIdCount];
        this->m_dumpOffsets = dumpOffsets;
    }
}

// VERIFY: 40eb7cef auto
// OFFSET: 0x00889F80
const uint8_t* BSN::Protocol::Protocol::TypeInfo(uint32_t t) const {
    STORM_ASSERT(t < typeIdCount);

    const uint8_t* cursor = &this->m_typeOffsets[4 * t];
    uint32_t offset = 0;

    for (uint32_t i = 4; i; i--) {
        offset = (offset << 8) | *cursor++;
    }

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
    uint32_t offset = BSN::Protocol::GetUInt32(cursor);

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
    uint32_t offset = BSN::Protocol::GetUInt32(cursor);

    if (!offset) {
        return nullptr;
    }

    return reinterpret_cast<const char*>(&this->m_bytecode[offset]);
}
