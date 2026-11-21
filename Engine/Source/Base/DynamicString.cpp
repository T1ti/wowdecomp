#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: 62178965 auto
// OFFSET: 0x0095D680
CDynamicString::CDynamicString() {
    this->m_length = 0;
    this->m_capacity = 0;
    this->m_buffer = nullptr;
}

// VERIFY: a76c270c manual
// OFFSET: 0x0095D690
CDynamicString::~CDynamicString() {
    if (this->m_buffer) {
        SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    }
}

// VERIFY: ceda3bac manual
// OFFSET: 0x0095D6B0
void CDynamicString::Free() {
    this->m_length = 0;
    this->m_capacity = 0;

    if (this->m_buffer) {
        SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    }

    this->m_buffer = nullptr;
}

// VERIFY: da84b739 manual
// OFFSET: 0x0095D6E0
CDynamicString& CDynamicString::operator=(const char* str) {
    if (!str) {
        this->m_length = 0;
        this->m_capacity = 0;

        if (this->m_buffer) {
            SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
        }

        this->m_buffer = nullptr;
        return *this;
    }

    this->m_length = static_cast<int32_t>(SStrLen(str));
    this->m_capacity = (3 * this->m_length >> 1) + 16;
    this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, this->m_capacity, __FILE__, __LINE__, 0));
    SStrCopy(this->m_buffer, str, STORM_MAX_STR);

    return *this;
}

// VERIFY: 8f3cd479 manual
// OFFSET: 0x0095D760
void CDynamicString::SetCapacity(int32_t capacity) {
    if (capacity < 1) {
        this->Free();
        return;
    }

    if (this->m_buffer) {
        if (capacity <= this->m_length) {
            this->m_length = capacity - 1;
            this->m_buffer[capacity - 1] = '\0';
            this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, capacity, __FILE__, __LINE__, 0));
            this->m_capacity = capacity;
        } else {
            this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, capacity, __FILE__, __LINE__, 0));
            this->m_capacity = capacity;
        }
    } else {
        this->m_buffer = static_cast<char*>(SMemAlloc(capacity, __FILE__, __LINE__, 0));
        this->m_capacity = capacity;
        this->m_length = 0;
        this->m_buffer[0] = '\0';
    }
}

// VERIFY: 89e1717a manual
// OFFSET: 0x0095D820
void CDynamicString::Grow() {
    this->m_capacity = (3 * this->m_length >> 1) + 16;

    if (!this->m_buffer) {
        this->m_buffer = static_cast<char*>(SMemAlloc(this->m_capacity, __FILE__, __LINE__, 0));
        this->m_buffer[0] = '\0';
    } else {
        this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, this->m_capacity, __FILE__, __LINE__, 0));
    }
}

// VERIFY: 41863780 auto
// OFFSET: 0x0095D870
void CDynamicString::operator+=(const char* str) {
    if (str) {
        int32_t length = this->m_length;
        this->m_length += static_cast<int32_t>(SStrLen(str));

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        SStrCopy(this->m_buffer + length, str, STORM_MAX_STR);
    }
}

// VERIFY: 18192892 auto
// OFFSET: 0x0095D8B0
void CDynamicString::operator+=(char c) {
    if (c) {
        this->m_length++;

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        this->m_buffer[this->m_length - 1] = c;
        this->m_buffer[this->m_length] = '\0';
    }
}

// VERIFY: b38238e8 manual
// OFFSET: 0x0095D8F0
void CDynamicString::operator+=(int32_t value) {
    if (this->m_length + 15 >= this->m_capacity) {
        this->Grow();
    }

    SStrPrintf(this->m_buffer + this->m_length, 15, "%d", value);
    this->m_length = static_cast<int32_t>(SStrLen(this->m_buffer));
}

// VERIFY: 18e56cfd manual
// OFFSET: 0x0095D930
void CDynamicString::Append(const char* src, int32_t count) {
    if (src && count > 0) {
        this->m_length += count;

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        memcpy(this->m_buffer + this->m_length - count, src, count);
        this->m_buffer[this->m_length] = '\0';
    }
}
