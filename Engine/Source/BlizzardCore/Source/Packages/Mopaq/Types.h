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

namespace System_SFile {

// One archive/open word is copied into BlockEntryTable;table users consume a subset.
//0x10 is retained without guessing its original mode name. File-entry flags differ.
enum ARCHIVE_FLAGS : uint32_t {
    ARCHIVE_FLAG_NO_HEADER_SCAN = 8,
    ARCHIVE_FLAG_UNKNOWN_10 = 0x10,
    ARCHIVE_FLAG_RETRY_HEADER_CANDIDATE = 0x20,
    ARCHIVE_FLAG_BLOCK_MD5 = 0x40,
    ARCHIVE_FLAG_BLOCK_CRC = 0x80,
    ARCHIVE_FLAG_BLOCK_FILETIME = 0x100,
    ARCHIVE_FLAG_PACKED_MD5_CHECK = 0x200,
    ARCHIVE_FLAG_CONVERT_LEGACY_TABLES = 0x400,
    ARCHIVE_FLAG_INITIALIZE_TABLES_ON_OPEN = 0x800,
};

}

#endif
