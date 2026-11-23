#ifndef WOW_SOURCE_OBJECTALLOC_IOBJECTALLOC_H
#define WOW_SOURCE_OBJECTALLOC_IOBJECTALLOC_H

#include <cstddef>
#include <cstdint>

class CObjectHeap {
    public:
    CObjectHeap();
    CObjectHeap(const CObjectHeap& source);
    ~CObjectHeap();
    bool Allocate(uint32_t objSize, uint32_t count, const char* name);
    void Delete(uint32_t index, uint32_t objSize, uint32_t count);
    int32_t New(uint32_t objSize, uint32_t count, uint32_t* index, const char* name, void** ptr, bool zero);
    void* Ptr(uint32_t index, uint32_t objSize, uint32_t count) const;

    private:
    friend class CObjectHeapList; // Access inferred from allocation and accounting.

    /* 0000 */ void* m_obj;
    /* 0004 */ uint32_t* m_indexStack;
    /* 0008 */ uint32_t m_allocated;

    void Free(); // (inferred)
};

#endif
