#ifndef ENGINE_SOURCE_TEMPEST_CMEMBLOCK_H
#define ENGINE_SOURCE_TEMPEST_CMEMBLOCK_H

#include <cstddef>
#include <cstdint>

class CMemBlock {
    public:
    CMemBlock(uint32_t size, uint32_t offset, const char* name, int32_t line);
    virtual ~CMemBlock();
    bool Resize(uint32_t size, bool keep);

    protected:
    bool IsValid() const { return this->mem_ != nullptr; } // (inferred)

    private:
    /* 0004 */ uint8_t* mem_;
    /* 0008 */ uint32_t size_;

    protected:
    /* 000C */ uint8_t* mem;

    private:
    /* 0010 */ uint32_t size;

    static void Set32b_(uint32_t* dest, uint32_t value, uint32_t bytes);
    static void Set32b_(uint8_t* dest, uint8_t value, uint32_t bytes);
    static void SetM_(uint8_t* dest, uint8_t value, uint32_t bytes);

    void Constructor_(uint32_t size, uint32_t offset, const char* name, int32_t line);
    void Free(); // (inferred)
};

#endif
