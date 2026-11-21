#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_TYPES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_TYPES_H

#include <cstdint>

namespace Blizzard {
namespace Mopaq {

// Compressed block type byte; named from the compressor and decompressor each
// value selects in System_SComp::Compress and Decompress.
enum COMPRESSION_TYPE : uint8_t {
    COMPRESSION_ZLIB = 0x02,
    COMPRESSION_PKWARE = 0x08,
    COMPRESSION_BZIP2 = 0x10,
    COMPRESSION_LZMA = 0x12,
    COMPRESSION_RLE = 0x20, // RLEZeroCompress
    COMPRESSION_RLE_ZLIB = 0x22,
    COMPRESSION_RLE_BZIP2 = 0x30,
};

struct CompressionAlgorithm {
    /* 0000 */ uint8_t m_type; // COMPRESSION_TYPE
    /* 0001 */ uint8_t m_level;
};

enum AuthResult {
    AUTH_UNKNOWN = 0,
    AUTH_FAILED = 1,
    AUTH_VERIFIED = 5,
};

}
}

#endif
