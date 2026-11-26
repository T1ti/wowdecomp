#ifndef ENGINE_SOURCE_TEMPEST_CDYNTABLE_H
#define ENGINE_SOURCE_TEMPEST_CDYNTABLE_H

#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMemBlock.h"

template <class T>
class CDynTable : protected CMemBlock {
    protected:
    CDynTable(uint32_t capacity, uint32_t chunk, const char* name)
        : CMemBlock(sizeof(T) * capacity, 0, name, SLOG_OBJECT),
          m_chunk(chunk), m_capacity(capacity), iused(0) {
    }

    uint32_t Used() const {
        STORM_ASSERT(this->IsValid());
        return this->iused;
    }

    const T& operator[](uint32_t i) const {
        STORM_ASSERT(this->IsValid() && i < iused);
        return reinterpret_cast<const T*>(this->mem)[i];
    }

    T& operator[](uint32_t i) {
        STORM_ASSERT(this->IsValid() && i < iused);
        return reinterpret_cast<T*>(this->mem)[i];
    }

    bool Grow() {
        STORM_ASSERT(this->IsValid());

        if (this->iused + 1 > this->m_capacity) {
            uint32_t grow = this->m_chunk;

            if (!grow) {
                return false;
            }

            uint32_t needed = this->iused - this->m_capacity + 1;
            if (grow <= needed) {
                grow = needed;
            }

            if (!this->Resize(sizeof(T) * (grow + this->m_capacity), true)) {
                return false;
            }

            this->m_capacity += grow;
        }

        this->iused++;
        return true;
    }

    // OFFSET: 0x007A0C70 for the eight-byte record instantiations.
    void Erase(uint32_t index, uint32_t count) {
        STORM_ASSERT(this->IsValid());

        if (index >= this->iused) {
            return;
        }

        if (index + count > this->iused) {
            count = this->iused - index;
        }

        uint32_t tail = this->iused - index - count;
        if (tail) {
            memmove(&(*this)[index], &(*this)[index + count], sizeof(T) * tail);
        }

        this->iused -= count;
    }

    private:
    /* 0014 */ uint32_t m_chunk;
    /* 0018 */ uint32_t m_capacity;
    /* 001C */ uint32_t iused;
};

#endif
