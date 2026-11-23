#ifndef WOW_SOURCE_OBJECTALLOC_OBJECTALLOC_H
#define WOW_SOURCE_OBJECTALLOC_OBJECTALLOC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/ObjectAlloc/IObjectAlloc.h"

class CObjectHeapList {
    private:
    friend uint32_t ObjectAllocAddHeap(uint32_t objectSize, uint32_t objectsPerBlock, const char* name, bool freeEmptyHeaps); // (inferred)
    void FreeEmptyHeaps();

    /* 0000 */ TSGrowableArray<CObjectHeap> m_heaps;
    /* 0010 */ uint32_t m_objSize;
    /* 0014 */ uint32_t m_objsPerBlock;
    /* 0018 */ uint32_t m_numFullHeaps;
    /* 001C */ uint32_t m_hasEmptyHeaps;
    /* 0020 */ uint32_t m_deleteCount;
    /* 0024 */ uint32_t m_fullestHeap;
    /* 0028 */ char m_heapName[80];
    /* 0078 */ uint64_t m_numAllocs;
    /* 0080 */ uint32_t m_uint80;
    /* 0084 */ uint8_t m_freeEmptyHeaps;

    public:
    CObjectHeapList();
    uint32_t BlocksAllocated() const;
    void Delete(uint32_t index);
    int32_t New(uint32_t* index, void** ptr, bool zero);
    uint32_t PercentUsed() const;
    uint32_t TotalHeaps() const;
    void* Ptr(uint32_t index) const; // (inferred)
    const char* GetName() const { return m_heapName; } // (inferred)
    uint32_t GetObjectSize() const { return m_objSize; } // (inferred)
    uint32_t GetObjectsPerBlock() const { return m_objsPerBlock; } // (inferred)
    uint64_t GetNumAllocs() const { return m_numAllocs; } // (inferred)
};

struct OBJALLOCGLOBALS {
    /* 0000 */ TSGrowableArray<CObjectHeapList> objects;
};

OBJALLOCGLOBALS* GetObjAllocGlobals();
void ObjectAllocDestroy();
void* ObjectPtr(uint32_t heapId, uint32_t memHandle);
void ReleaseObjAllocGlobals();

#endif
