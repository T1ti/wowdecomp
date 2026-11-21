#include "Engine/Source/Gxu/GxuDebugDraw.h"

#ifdef WOWDECOMP_GXU_DEBUG_DRAW

#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Storm/H/storm.h"

// Tool features: GxuBufferCache (4.1 Engine/Source/Gxu/GxuBufferCache.cpp, 13850 Mac 0x00AE5810..0x00AE6970).
// Shared vertex/index buffers addressed by small handles; the debug shape factories keep one handle each. Every entry
// owns one CGxPool sized for exactly one CGxBuf. Buffers survive until ReleaseCache, except that a device reset
// invalidates them: Exists notices (the buffer lost m_hasData or m_valid), frees the pair, and the owner recreates it
// through CreateBuffer with the same handle.

// 4.1 asserts with SErrDisplayError(STORM_ERROR_ASSERTION, file, line, expression, recoverable 1, exit code 1).
#define GXU_ASSERT(x) ((x) ? (void)0 : (void)SErrDisplayError(STORM_ERROR_ASSERTION, __FILE__, __LINE__, #x, 1, 1, 0))

static void DestroyEntry(GxuBufferCache::BufferEntry& entry) {
    // Static destruction may run after the device is gone; the pool and buffer went with it.
    if (!g_theGxDevicePtr) {
        entry.m_buffer = nullptr;
        entry.m_pool = nullptr;
        return;
    }

    if (entry.m_buffer) {
        g_theGxDevicePtr->BufDestroy(entry.m_buffer);
    }

    if (entry.m_pool) {
        g_theGxDevicePtr->PoolDestroy(entry.m_pool);
    }

    entry.m_buffer = nullptr;
    entry.m_pool = nullptr;
}

static void CreateEntryBuffers(GxuBufferCache::BufferEntry& entry) {
    // 4.1 passes a null pool name; 3.3.5 pools are named for the device's pool listings, so name it.
    entry.m_pool = g_theGxDevicePtr->PoolCreate(entry.m_target, GxPoolUsage_Static, static_cast<int32_t>(entry.m_itemSize * entry.m_itemCnt), static_cast<EGxPoolHintBits>(0), "GxuBufferCache");
    entry.m_buffer = g_theGxDevicePtr->BufCreate(entry.m_pool, entry.m_itemSize, entry.m_itemCnt, 0);
}

// TOOL: 4.1 0x00AE5810
GxuBufferCache::~GxuBufferCache() {
    this->ReleaseCache();
}

// TOOL: 4.1 0x00AE6010
// With a valid handle the entry must describe the same buffer; a lost pool/buffer pair is recreated in place. With
// INVALID_HANDLE a new entry is appended. Returns the entry's handle either way.
uint32_t GxuBufferCache::CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCnt) {
    if (handle != INVALID_HANDLE) {
        GXU_ASSERT(handle < this->m_allocatedBuffers.Count());

        BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];
        GXU_ASSERT(entry.m_target == target && entry.m_itemSize == itemSize && entry.m_itemCnt == itemCnt);

        if (entry.m_buffer && entry.m_pool) {
            return handle;
        }

        GXU_ASSERT(!entry.m_buffer && !entry.m_pool);
        GXU_ASSERT(entry.m_refs == 0);
        CreateEntryBuffers(entry);

        return handle;
    }

    uint32_t index = this->m_allocatedBuffers.Count();
    this->m_allocatedBuffers.SetCount(index + 1);

    BufferEntry& entry = this->m_allocatedBuffers.m_data[index];
    entry.m_target = target;
    entry.m_itemSize = itemSize;
    entry.m_itemCnt = itemCnt;
    entry.m_refs = 0;
    CreateEntryBuffers(entry);

    return index;
}

// TOOL: 4.1 0x00AE59E0
// True when the handle names a live buffer that holds data. A buffer the device invalidated (reset) is freed here so
// the next CreateBuffer rebuilds it.
bool GxuBufferCache::Exists(uint32_t handle) {
    if (handle == INVALID_HANDLE || handle >= this->m_allocatedBuffers.Count()) {
        return false;
    }

    BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];

    if (!entry.m_buffer && !entry.m_pool) {
        return false;
    }

    GXU_ASSERT(entry.m_buffer && entry.m_pool);

    if (entry.m_buffer->m_hasData && entry.m_buffer->m_valid) {
        return true;
    }

    GXU_ASSERT(entry.m_refs == 0);
    DestroyEntry(entry);

    return false;
}

// TOOL: 4.1 0x00AE5E20
CGxBuf* GxuBufferCache::AcquireBuffer(uint32_t handle) {
    GXU_ASSERT(handle < this->m_allocatedBuffers.Count());

    BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];
    GXU_ASSERT(entry.m_buffer && entry.m_pool);
    entry.m_refs++;

    return entry.m_buffer;
}

// TOOL: 4.1 0x00AE5BF0
void GxuBufferCache::ReleaseBuffer(uint32_t handle) {
    GXU_ASSERT(handle < this->m_allocatedBuffers.Count());

    BufferEntry& entry = this->m_allocatedBuffers.m_data[handle];
    GXU_ASSERT(entry.m_refs > 0);
    entry.m_refs--;
}

// TOOL: 4.1 0x00AE5860
// Frees every entry's buffer and pool. 4.1 keeps the entries (and so the handles the factories hold); this port drops
// them too, so the factories' handles are reset by GxuShutdownDebugShapeDrawer at the same time.
void GxuBufferCache::ReleaseCache() {
    for (uint32_t i = 0; i < this->m_allocatedBuffers.Count(); i++) {
        BufferEntry& entry = this->m_allocatedBuffers.m_data[i];
        GXU_ASSERT(entry.m_refs == 0);
        DestroyEntry(entry);
    }

    this->m_allocatedBuffers.SetCount(0);
}

#endif
