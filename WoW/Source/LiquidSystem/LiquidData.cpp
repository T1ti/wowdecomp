#include "Liquid.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/MapChunk.h"

// STATIC: 0x00A59484
uint8_t Liquid::CAreaData::s_emptyTable[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// STATIC: 0x00A5948C
uint8_t Liquid::CAreaData::s_fullTable[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// STATIC: 0x00A59494
uint16_t Liquid::CAreaData::s_defaultTexCoord[2] = { 0, 0 };

// VERIFY: b3a3de07 auto
// OFFSET: 0x007D4F10
void Liquid::CAreaData::SetData(const void* data) {
    this->dataPtr = static_cast<const uint8_t*>(data);
}

// VERIFY: b03625ac auto
// OFFSET: 0x008A3050
Liquid::CAreaData::CAreaData(const void* data) {
    this->dataPtr = static_cast<const uint8_t*>(data);
}

// VERIFY: 71cd1ec5 manual
// OFFSET: 0x008A3060
bool Liquid::CAreaData::HasData() const {
    return this->dataPtr != nullptr;
}

// VERIFY: 6ee106da auto
// OFFSET: 0x008A3070
const SMLiquidChunkDesc* Liquid::CAreaData::GetChunkDesc(uint32_t cx, uint32_t cy) const {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(cx<16);
    STORM_ASSERT(cy<16);

    return reinterpret_cast<const SMLiquidChunkDesc*>(this->dataPtr) + (cx + 16 * cy);
}

// VERIFY: f4b46567 auto
// OFFSET: 0x008A3090
const SMLiquidInstance* Liquid::CAreaData::GetInstanceDesc(const SMLiquidChunkDesc* chunkDesc, uint32_t index) const {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(chunkDesc);

    return reinterpret_cast<const SMLiquidInstance*>(this->dataPtr + chunkDesc->m_offsetInstances) + index;
}

// VERIFY: 6e52450a auto
// OFFSET: 0x008A30B0
const uint8_t* Liquid::CAreaData::GetFishableTable(const SMLiquidChunkDesc* chunkDesc) const {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(chunkDesc);

    if (!chunkDesc->m_layerCount) {
        return Liquid::CAreaData::s_emptyTable;
    }

    if (chunkDesc->m_offsetAttributes) {
        return this->dataPtr + chunkDesc->m_offsetAttributes;
    }

    return Liquid::CAreaData::s_fullTable;
}

// VERIFY: 3dff8387 auto
// OFFSET: 0x008A30E0
const uint8_t* Liquid::CAreaData::GetDeepTable(const SMLiquidChunkDesc* chunkDesc) const {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(chunkDesc);

    if (!chunkDesc->m_layerCount) {
        return Liquid::CAreaData::s_emptyTable;
    }

    if (chunkDesc->m_offsetAttributes) {
        return this->dataPtr + chunkDesc->m_offsetAttributes + 8;
    }

    return Liquid::CAreaData::s_fullTable;
}

// VERIFY: 6b0d6dd8 manual
// OFFSET: 0x008A3110
const uint8_t* Liquid::CAreaData::GetExistsTable(const SMLiquidInstance* instDesc) const {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(instDesc);

    if (instDesc->m_offsetExistsBitmap) {
        return this->dataPtr + instDesc->m_offsetExistsBitmap;
    }

    return Liquid::CAreaData::s_fullTable;
}

// VERIFY: 0ecbc03f manual
// OFFSET: 0x008A3130
float Liquid::CInstanceVertexData::GetHeight(uint32_t index) {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(instDesc);

    if (this->instDesc->m_vertexFormat > LIQUID_VERTEX_HEIGHT_TEXCOORD) {
        return 0.0f;
    }

    STORM_ASSERT(instDesc->vtxOffset);
    return reinterpret_cast<const float*>(this->dataPtr + this->instDesc->vtxOffset)[index];
}

// VERIFY: 43850342 manual
// OFFSET: 0x008A3160
uint8_t Liquid::CInstanceVertexData::GetDepth(uint32_t index) {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(instDesc);

    const SMLiquidInstance* instDesc = this->instDesc;

    if (instDesc->m_vertexFormat == LIQUID_VERTEX_HEIGHT_DEPTH) {
        STORM_ASSERT(instDesc->vtxOffset);
        return (this->dataPtr + instDesc->vtxOffset + 4 * this->m_vertexCount)[index];
    }

    if (instDesc->m_vertexFormat != LIQUID_VERTEX_DEPTH_ONLY) {
        return 0;
    }

    if (instDesc->vtxOffset) {
        return (this->dataPtr + instDesc->vtxOffset)[index];
    }

    return 0xFF;
}

// VERIFY: 5fa5f4f8 manual
// OFFSET: 0x008A31B0
const uint16_t* Liquid::CInstanceVertexData::GetTexCoord(uint32_t index) {
    STORM_ASSERT(dataPtr);
    STORM_ASSERT(instDesc);

    if (this->instDesc->m_vertexFormat == LIQUID_VERTEX_HEIGHT_TEXCOORD) {
        STORM_ASSERT(instDesc->vtxOffset);
        return reinterpret_cast<const uint16_t*>(this->dataPtr + this->instDesc->vtxOffset + 4 * (index + this->m_vertexCount));
    }

    return Liquid::CAreaData::s_defaultTexCoord;
}

// VERIFY: 7b8fdcd4 auto
// OFFSET: 0x008A31E0
Liquid::CChunkVertexData::CChunkVertexData(const uint8_t* data, uint32_t format) {
    this->vtxData = data;
    this->m_format = format;
}

// VERIFY: 9dcee70a manual
// OFFSET: 0x008A3200
float Liquid::CChunkVertexData::GetHeight(uint32_t index) {
    STORM_ASSERT(vtxData);

    if (this->m_format > LIQUID_VERTEX_HEIGHT_TEXCOORD) {
        return 0.0f;
    }

    return reinterpret_cast<const SLVert*>(this->vtxData + 8 * index)->m_waterVert.m_height;
}

// VERIFY: 0121d633 manual
// OFFSET: 0x008A3220
uint8_t Liquid::CChunkVertexData::GetDepth(uint32_t index) {
    STORM_ASSERT(vtxData);

    uint32_t format = this->m_format;

    if (format == LIQUID_VERTEX_HEIGHT_DEPTH || format == LIQUID_VERTEX_DEPTH_ONLY) {
        return reinterpret_cast<const SLVert*>(this->vtxData + 8 * index)->m_waterVert.m_depth;
    }

    return 0;
}

// VERIFY: 681ed984 manual
// OFFSET: 0x008A3250
const uint16_t* Liquid::CChunkVertexData::GetTexCoord(uint32_t index) {
    STORM_ASSERT(vtxData);

    if (this->m_format == LIQUID_VERTEX_HEIGHT_TEXCOORD) {
        return &reinterpret_cast<const SLVert*>(this->vtxData + 8 * index)->m_magmaVert.m_s;
    }

    return Liquid::CAreaData::s_defaultTexCoord;
}

// VERIFY: 3d77147d auto
// OFFSET: 0x008A3280
Liquid::CInstanceVertexData::CInstanceVertexData(const Liquid::CAreaData* areaData, const SMLiquidInstance* instance) {
    this->dataPtr = areaData->dataPtr;
    this->instDesc = instance;
    this->m_vertexCount = (instance->m_width + 1) * (instance->m_height + 1);
}
