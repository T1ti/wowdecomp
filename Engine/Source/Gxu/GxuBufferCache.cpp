#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_TOOL_FEATURES

// Tool features: GxuBufferCache (4.1 Engine/Source/Gxu/GxuBufferCache.cpp, 13850 Mac 0x00AE5810..0x00AE6970).
// Shared vertex/index buffers addressed by small handles; the debug shape factories keep one handle each. 4.1 asserts
// "handle<m_allocatedBuffers.Count()", "entry.m_target==target && entry.m_itemSize==itemSize &&
// entry.m_itemCnt==itemCnt" and "entry.m_refs==0" in CreateBuffer, which fixes the entry layout in GxuDebugDraw.h.
// In 3.3.5 the pools come from CGxDevice::PoolCreate and the buffers from CGxDevice::BufCreate.

// TOOL: 4.1 0x00AE5810
GxuBufferCache::~GxuBufferCache() {
    this->ReleaseCache();
}

// TOOL: 4.1 0x00AE6010
uint32_t GxuBufferCache::CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCnt) {
    // TODO: 4.1 0x00AE6010: with a valid handle, check the entry matches (target, itemSize, itemCnt) and recreate its
    // pool/buffer if they were lost; otherwise take a free entry (or grow m_allocatedBuffers), create a CGxPool for
    // target and a CGxBuf of itemCnt * itemSize, and return the entry's index.
    return INVALID_HANDLE;
}

// TOOL: 4.1 0x00AE59E0
bool GxuBufferCache::Exists(uint32_t handle) {
    if (handle >= this->m_allocatedBuffers.Count()) {
        return false;
    }

    const BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];
    return entry.m_pool && entry.m_buffer;
}

// TOOL: 4.1 0x00AE5E20
void* GxuBufferCache::AcquireBuffer(uint32_t handle) {
    if (handle >= this->m_allocatedBuffers.Count()) {
        return nullptr;
    }

    BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];
    entry.m_refs++;
    return entry.m_buffer;
}

// TOOL: 4.1 0x00AE5BF0
void GxuBufferCache::ReleaseBuffer(uint32_t handle) {
    if (handle >= this->m_allocatedBuffers.Count()) {
        return;
    }

    BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];

    if (entry.m_refs) {
        entry.m_refs--;
    }
}

// TOOL: 4.1 0x00AE5860
void GxuBufferCache::ReleaseCache() {
    // TODO: 4.1 0x00AE5860 destroys each entry's buffer and pool (GxBufDestroy / PoolDestroy) before clearing.
    this->m_allocatedBuffers.SetCount(0);
}

#endif
