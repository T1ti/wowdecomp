#include <cstring>
#include "lmemPool.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00A47FBC
static const uint32_t s_blocksizes[] = { 16, 24, 32, 40, 64, 80, 128, 160, 256 };

// VERIFY: a5f551b2 manual
// OFFSET: 0x00855570
MemChunk::MemChunk(uint32_t blockSize, uint32_t size) {
    this->m_blockSize = blockSize;
    this->m_numBlocks = size / blockSize;
    this->m_size = blockSize * this->m_numBlocks;
    this->m_memblocks = SMemAlloc(this->m_size, __FILE__, __LINE__, 0);

    char* block = static_cast<char*>(this->m_memblocks);
    this->m_free = block;

    while (block < static_cast<char*>(this->m_memblocks) + this->m_size - this->m_blockSize) {
        *reinterpret_cast<void**>(block) = block + this->m_blockSize;
        block += this->m_blockSize;
    }

    *reinterpret_cast<void**>(block) = nullptr;
}

// INLINED
MemChunk::~MemChunk() {
    SMemFree(this->m_memblocks, __FILE__, __LINE__, 0);
}

// INLINED
MemPool::MemPool() {
}

// VERIFY: 403cd95b auto
// OFFSET: 0x00855670
int32_t MemPool::MemFree(void* ptr) {
    for (uint32_t i = 0; i < this->m_memchunks.Count(); i++) {
        MemChunk* chunk = this->m_memchunks[i];

        if (ptr >= chunk->m_memblocks && ptr < static_cast<char*>(chunk->m_memblocks) + chunk->m_size) {
            *static_cast<void**>(ptr) = chunk->m_free;
            chunk->m_numBlocks++;
            chunk->m_free = ptr;
            return 1;
        }
    }

    STORM_ASSERT(!"This should never happen!");
    return 0;
}

// INLINED
bool MemPool::Contains(void* ptr) {
    for (uint32_t i = 0; i < this->m_memchunks.Count(); i++) {
        MemChunk* chunk = this->m_memchunks[i];

        if (ptr >= chunk->m_memblocks && ptr < static_cast<char*>(chunk->m_memblocks) + chunk->m_size) {
            return true;
        }
    }

    return false;
}

// VERIFY: d5bd1438 manual
// OFFSET: 0x008556E0
MemPool::~MemPool() {
    for (uint32_t i = this->m_memchunks.Count(); i; i--) {
        delete this->m_memchunks[i - 1];
    }
}

// VERIFY: 2bca1af4 manual
// OFFSET: 0x00855740
void* luaM_initPool() {
    MemPool** pools = new (__FILE__, __LINE__) MemPool*[sizeof(s_blocksizes) / sizeof(s_blocksizes[0])];

    for (uint32_t i = 0; i < sizeof(s_blocksizes) / sizeof(s_blocksizes[0]); i++) {
        pools[i] = new (__FILE__, __LINE__) MemPool();
        STORM_ASSERT(pools[i]->m_memchunks.Count() == 0); // 12213: "m_memchunks.Count() == 0"
        pools[i]->m_blockSize = s_blocksizes[i];
        pools[i]->m_numBlocks = (s_blocksizes[i] * 1024) / s_blocksizes[i];
        pools[i]->m_chunkSize = s_blocksizes[i] * pools[i]->m_numBlocks;
    }

    return pools;
}

// VERIFY: 2d194715 manual
// OFFSET: 0x008557D0
void luaM_freePool(void* ud) {
    MemPool** pools = static_cast<MemPool**>(ud);

    if (!pools) {
        return;
    }

    for (uint32_t i = 0; i < sizeof(s_blocksizes) / sizeof(s_blocksizes[0]); i++) {
        delete pools[i];
    }

    SMemFree(pools, __FILE__, __LINE__, 0);
}

// VERIFY: 48e99a2f manual
// OFFSET: 0x00855820
void* MemPool::MemAlloc() {
    uint32_t count = this->m_memchunks.Count();

    for (uint32_t i = 0; i < count; i++) {
        MemChunk* chunk = this->m_memchunks[i];

        if (chunk->m_free) {
            void* block = chunk->m_free;
            chunk->m_numBlocks--;
            chunk->m_free = *static_cast<void**>(block);
            return block;
        }
    }

    this->m_memchunks.SetCount(count + 1);
    this->m_memchunks[count] = new (__FILE__, __LINE__) MemChunk(this->m_blockSize, this->m_chunkSize);

    MemChunk* chunk = this->m_memchunks[count];
    void* block = nullptr;

    if (chunk->m_free) {
        block = chunk->m_free;
        chunk->m_numBlocks--;
        chunk->m_free = *static_cast<void**>(block);
    }

    return block;
}

// VERIFY: f201d154 manual
// OFFSET: 0x008558E0
void* luaM_reallocPool(void* ud, void* ptr, size_t osize, size_t nsize) {
    MemPool** pools = static_cast<MemPool**>(ud);
    int32_t oldpool = -1;
    int32_t newpool = -1;

    if (ptr) {
        for (int32_t i = 0; i < static_cast<int32_t>(sizeof(s_blocksizes) / sizeof(s_blocksizes[0])); i++) {
            if (osize <= s_blocksizes[i]) {
                oldpool = i;
                break;
            }
        }

#if defined(WOWDECOMP_ASSERTIONS)
        if (oldpool >= 0) {
            STORM_ASSERT(pools[oldpool]->Contains(ptr)); // 12213: "mempool[i]->Contains(block)"
        }
#endif
    }

    if (!nsize) {
        if (ptr) {
            if (oldpool >= 0) {
                pools[oldpool]->MemFree(ptr);
                return nullptr;
            }

            SMemFree(ptr, __FILE__, __LINE__, 0);
        }

        return nullptr;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(s_blocksizes) / sizeof(s_blocksizes[0])); i++) {
        if (nsize <= s_blocksizes[i]) {
            newpool = i;
            break;
        }
    }

    if (oldpool == newpool) {
        if (oldpool < 0) {
            return SMemReAlloc(ptr, nsize, __FILE__, __LINE__, 0);
        }

        return ptr;
    }

    void* newptr = newpool < 0 ? SMemAlloc(nsize, __FILE__, __LINE__, 0) : pools[newpool]->MemAlloc();

    if (ptr) {
        memcpy(newptr, ptr, osize < nsize ? osize : nsize);

        if (oldpool >= 0) {
            pools[oldpool]->MemFree(ptr);
        } else {
            SMemFree(ptr, __FILE__, __LINE__, 0);
        }
    }

    return newptr;
}
