#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

#define UNI_REPLACEMENT_CHAR 0xFFFD
#define UNI_MAX_BMP 0xFFFF
#define UNI_MAX_UTF16 0x10FFFF
#define UNI_SUR_HIGH_START 0xD800
#define UNI_SUR_LOW_START 0xDC00
#define UNI_SUR_MASK 0x3FF
#define UNI_HALF_SHIFT 10
#define UNI_HALF_BASE 0x10000

// STATIC: 0x00A3A240
static const uint32_t offsetsFromUTF8[6] = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080 };

// STATIC: 0x00A3A258
static const uint8_t trailingBytesForUTF8[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

// STATIC: 0x00A3A358
static const uint32_t firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

// VERIFY: 30a8cb8f manual
// OFFSET: 0x00775BD0
int32_t STORMAPI SUniConvertUTF16to8(char* dst, uint32_t dstmaxchars, const uint16_t* src, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars) {
    char* dstStart = dst;
    const uint16_t* srcStart = src;
    const uint16_t* srcEnd = srcmaxchars == STORM_MAX_STR ? reinterpret_cast<const uint16_t*>(-1) : src + srcmaxchars;
    char* dstEnd = dst + dstmaxchars;
    int32_t result;

    for (;;) {
        if (src >= srcEnd) {
            result = -1;
            break;
        }

        uint32_t ch = *src;
        uint32_t srcUnits = 1;

        if (ch - UNI_SUR_HIGH_START <= UNI_SUR_MASK) {
            if (src + 1 >= srcEnd) {
                result = -1;
                break;
            }

            uint32_t ch2 = src[1];

            if (ch2 - UNI_SUR_LOW_START <= UNI_SUR_MASK) {
                ch = ((ch - UNI_SUR_HIGH_START) << UNI_HALF_SHIFT) + (ch2 - UNI_SUR_LOW_START) + UNI_HALF_BASE;
                srcUnits = 2;
            }
        }

        uint32_t bytesToWrite;

        if (ch < 0x80) {
            bytesToWrite = 1;

            if (!ch) {
                if (dst < dstEnd) {
                    *dst++ = 0;
                    result = 0;
                } else {
                    result = bytesToWrite;
                }

                break;
            }
        } else if (ch < 0x800) {
            bytesToWrite = 2;
        } else if (ch < 0x10000) {
            bytesToWrite = 3;
        } else if (ch < 0x200000) {
            bytesToWrite = 4;
        } else if (ch < 0x4000000) {
            bytesToWrite = 5;
        } else if (ch <= 0x7FFFFFFF) {
            bytesToWrite = 6;
        } else {
            bytesToWrite = 2;
            ch = UNI_REPLACEMENT_CHAR;
        }

        if (dst + bytesToWrite > dstEnd) {
            result = bytesToWrite;
            break;
        }

        dst += bytesToWrite;

        switch (bytesToWrite) {
            case 6:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 5:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 4:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 3:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 2:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 1:
                *--dst = static_cast<char>(ch | firstByteMark[bytesToWrite]);
        }

        dst += bytesToWrite;
        src += srcUnits;
    }

    if (srcchars) {
        STORM_ASSERT((src) >= (srcStart));
        STORM_ASSERT((static_cast<ptrdiff_t>(src - srcStart)) <= 0x7FFFFFF); // 12213: "((ptrdiff_t)((src)-(srcStart))) <= 0x7FFFFFF"
        *srcchars = static_cast<uint32_t>(src - srcStart);
    }

    if (dstchars) {
        STORM_ASSERT((dst) >= (dstStart));
        STORM_ASSERT((static_cast<ptrdiff_t>(dst - dstStart)) <= 0x7FFFFFF); // 12213: "((ptrdiff_t)((dst)-(dstStart))) <= 0x7FFFFFF"
        *dstchars = static_cast<uint32_t>(dst - dstStart);
    }

    return result;
}

// VERIFY: 24f1c391 manual
// OFFSET: 0x00775D90
int32_t STORMAPI SUniConvertUTF8to16Len(const char* srcStart, uint32_t srcmaxchars, uint32_t* srcchars) {
    if (!srcmaxchars || !srcStart) {
        if (srcchars) {
            *srcchars = 0;
        }

        return -1;
    }

    const char* src = srcStart;
    const char* srcEnd = srcmaxchars == STORM_MAX_STR ? reinterpret_cast<const char*>(-1) : src + srcmaxchars;
    int32_t result = 0;

    for (;;) {
        if (src >= srcEnd) {
            result = -1;
            break;
        }

        uint32_t extraBytesToRead = trailingBytesForUTF8[static_cast<uint8_t>(*src)];

        if (src + extraBytesToRead >= srcEnd) {
            result = -1 - static_cast<int32_t>(extraBytesToRead);
            break;
        }

        uint32_t ch = 0;

        switch (extraBytesToRead) {
            case 5:
                ch += *src++;
                ch <<= 6;
            case 4:
                ch += *src++;
                ch <<= 6;
            case 3:
                ch += *src++;
                ch <<= 6;
            case 2:
                ch += *src++;
                ch <<= 6;
            case 1:
                ch += *src++;
                ch <<= 6;
            case 0:
                ch += *src++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (ch > UNI_MAX_BMP) {
            if (ch <= UNI_MAX_UTF16) {
                result += 2;
            } else {
                result++;
            }
        } else {
            result++;

            if (!ch) {
                break;
            }
        }
    }

    if (srcchars) {
        STORM_ASSERT((src) >= (srcStart));
        STORM_ASSERT((static_cast<ptrdiff_t>(src - srcStart)) <= 0x7FFFFFF); // 12213: "((ptrdiff_t)((src)-(srcStart))) <= 0x7FFFFFF"
        *srcchars = static_cast<uint32_t>(src - srcStart);
    }

    return result;
}

// VERIFY: 125cc92d manual
// OFFSET: 0x00775EB0
int32_t STORMAPI SUniConvertUTF8to16(uint16_t* dst, uint32_t dstmaxchars, const char* srcIn, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars) {
    if (!srcIn || !srcmaxchars) {
        return -1;
    }

    uint16_t* dstStart = dst;
    const auto* srcStart = reinterpret_cast<const uint8_t*>(srcIn);
    const uint8_t* src = srcStart;
    const uint8_t* srcEnd = srcmaxchars == STORM_MAX_STR ? reinterpret_cast<const uint8_t*>(-1) : src + srcmaxchars;
    uint16_t* dstEnd = dst + dstmaxchars;
    int32_t result;

    for (;;) {
        if (src >= srcEnd) {
            result = -1;
            break;
        }

        uint32_t extraBytesToRead = trailingBytesForUTF8[*src];

        if (src + extraBytesToRead >= srcEnd) {
            result = -1 - static_cast<int32_t>(extraBytesToRead);
            break;
        }

        const uint8_t* next = src;
        uint32_t ch = 0;

        switch (extraBytesToRead) {
            case 5:
                ch += *next++;
                ch <<= 6;
            case 4:
                ch += *next++;
                ch <<= 6;
            case 3:
                ch += *next++;
                ch <<= 6;
            case 2:
                ch += *next++;
                ch <<= 6;
            case 1:
                ch += *next++;
                ch <<= 6;
            case 0:
                ch += *next++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (dst >= dstEnd) {
            result = 1;
            break;
        }

        if (ch > UNI_MAX_BMP) {
            if (ch <= UNI_MAX_UTF16) {
                if (dst + 1 >= dstEnd) {
                    result = 1;
                    break;
                }

                ch -= UNI_HALF_BASE;
                *dst++ = static_cast<uint16_t>((ch >> UNI_HALF_SHIFT) + UNI_SUR_HIGH_START);
                *dst++ = static_cast<uint16_t>((ch & UNI_SUR_MASK) + UNI_SUR_LOW_START);
            } else {
                *dst++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
            *dst++ = static_cast<uint16_t>(ch);

            if (!ch) {
                result = 0;
                break;
            }
        }

        src = next;
    }

    if (srcchars) {
        STORM_ASSERT((src) >= (srcStart));
        STORM_ASSERT((static_cast<ptrdiff_t>(src - srcStart)) <= 0x7FFFFFF); // 12213: "((ptrdiff_t)((src)-(srcStart))) <= 0x7FFFFFF"
        *srcchars = static_cast<uint32_t>(src - srcStart);
    }

    if (dstchars) {
        STORM_ASSERT((dst) >= (dstStart));
        STORM_ASSERT((static_cast<ptrdiff_t>(dst - dstStart)) <= 0x7FFFFFF); // 12213: "((ptrdiff_t)((dst)-(dstStart))) <= 0x7FFFFFF"
        *dstchars = static_cast<uint32_t>(dst - dstStart);
    }

    return result;
}
