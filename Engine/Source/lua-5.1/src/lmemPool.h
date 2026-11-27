#ifndef lmemPool_h
#define lmemPool_h

#include <cstddef>
#include <cstdint>
#include "lua.h"
#include "Engine/Source/Storm/H/stpl.h"

class MemChunk {
    public:
    /* 0000 */ void* m_memblocks;
    /* 0004 */ void* m_free;
    /* 0008 */ uint32_t m_size;
    /* 000C */ uint32_t m_blockSize;
    /* 0010 */ uint32_t m_numBlocks;

    MemChunk(uint32_t blockSize, uint32_t size);
    ~MemChunk();
};

class MemPool {
    public:
    /* 0000 */ TSFixedArray<MemChunk*> m_memchunks;
    /* 000C */ uint32_t m_chunkSize;
    /* 0010 */ uint32_t m_blockSize;
    /* 0014 */ uint32_t m_numBlocks;

    MemPool();
    ~MemPool();
    void* MemAlloc();
    int32_t MemFree(void* ptr);
    bool Contains(void* ptr);
};

LUAI_FUNC void* luaM_initPool();
LUAI_FUNC void luaM_freePool(void* ud);
LUAI_FUNC void* luaM_reallocPool(void* ud, void* ptr, size_t osize, size_t nsize);

#endif
