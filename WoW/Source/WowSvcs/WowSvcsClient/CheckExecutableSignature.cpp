#include "CheckExecutableSignature.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/OsLibrary.h"
#include "Engine/Source/Os/OsVersionHash.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00A31508
static const uint8_t s_signaturePublicExponent[4] = { 0x01, 0x00, 0x01, 0x00 };

// STATIC: 0x00AD9728
static const uint8_t s_signatureModulus[256] = {
    0x4F, 0x6F, 0xD4, 0x65, 0x56, 0x85, 0x88, 0xD9, 0xAA, 0xE6, 0xF7, 0x05, 0x72, 0xBB, 0x2A, 0x20,
    0xE0, 0x24, 0x38, 0x2B, 0x77, 0xB3, 0x3D, 0xC2, 0xD3, 0x2D, 0x52, 0x5C, 0xB7, 0xA3, 0x0F, 0xE6,
    0x0F, 0x80, 0x93, 0x8D, 0x5C, 0xC4, 0x81, 0x5E, 0x9D, 0x58, 0x8E, 0xCA, 0xA6, 0x6F, 0x27, 0x36,
    0x52, 0x58, 0x0D, 0xCE, 0x65, 0xE2, 0xB5, 0x7A, 0xD2, 0x39, 0x74, 0x8A, 0xAC, 0xAA, 0xC1, 0x0E,
    0xE5, 0x67, 0x2B, 0x79, 0xC1, 0xFE, 0x08, 0x96, 0x54, 0x18, 0x2A, 0x34, 0x31, 0xA4, 0x06, 0x20,
    0x4B, 0x12, 0xD2, 0x47, 0x63, 0x53, 0x36, 0x38, 0x19, 0x88, 0x8E, 0x50, 0x2F, 0x2F, 0xD1, 0x68,
    0xC8, 0x5C, 0x9D, 0x3B, 0xDE, 0xFE, 0x56, 0x21, 0xD1, 0x59, 0x4D, 0x1C, 0x32, 0x1B, 0x70, 0x4B,
    0x40, 0xE2, 0x5E, 0x0C, 0x0E, 0x4B, 0x84, 0xD5, 0xD7, 0x52, 0x00, 0x50, 0x65, 0x81, 0xCE, 0x17,
    0x53, 0x25, 0x49, 0x9E, 0x40, 0xE9, 0x04, 0x94, 0x2C, 0xA7, 0x93, 0x5C, 0x5D, 0x54, 0xC8, 0xF9,
    0x6E, 0xD0, 0x69, 0xFC, 0xE5, 0x8F, 0xC0, 0xB6, 0x76, 0xF2, 0xEB, 0x34, 0x67, 0x9F, 0xF8, 0xD0,
    0x34, 0xB9, 0xEA, 0x9E, 0x80, 0xD0, 0x29, 0x8D, 0xB4, 0xB2, 0xEB, 0x3E, 0xF4, 0x4F, 0xD0, 0x84,
    0x43, 0x15, 0xF4, 0x19, 0xE3, 0xC6, 0x52, 0xA2, 0x18, 0x40, 0x08, 0x07, 0x21, 0xF4, 0x20, 0xEC,
    0xE6, 0x5A, 0x9C, 0x2B, 0x08, 0xF6, 0x17, 0x97, 0x72, 0xA6, 0xD7, 0xAB, 0x64, 0x0B, 0x04, 0x95,
    0x93, 0x0D, 0xCC, 0x8F, 0x02, 0x30, 0x91, 0xA2, 0xEE, 0x30, 0xE6, 0x5F, 0x2F, 0xD4, 0x13, 0xB5,
    0x5D, 0xCA, 0x5D, 0xCF, 0x7D, 0xC3, 0x6F, 0x6B, 0x9D, 0xBC, 0x6D, 0x86, 0x4C, 0x91, 0xC5, 0x51,
    0xE8, 0xFD, 0xA6, 0x33, 0x10, 0xD1, 0x12, 0x8F, 0x05, 0x80, 0xDA, 0x42, 0x57, 0x0C, 0xEB, 0xC0
};

// VERIFY: edd0ae78 manual
// OFFSET: 0x006BB240
bool ScanFileForHack(const char* fileName, uint8_t* hackFound, uint8_t* shouldBlockPlaying, const char** result, char* hackName, uint32_t hackNameSize) {
    if (!hackNameSize) {
        return false;
    }

    void* module = nullptr;
    bool success = false;
    HOSFILE file = OsCreateFile(fileName, OS_GENERIC_READ, 1, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file != OS_INVALID_FILE && CheckExecutableSignature(fileName)) {
        module = OsLoadLibrary(fileName);

        if (module) {
            int32_t type;
            int32_t block = 0;
            int32_t scanned = 0;
            SCANDLL_SCANFUNC2 scan2 = reinterpret_cast<SCANDLL_SCANFUNC2>(OsGetLibraryEntryPoint(module, reinterpret_cast<const char*>(3)));

            if (scan2) {
                scanned = scan2(&type, &block, hackName, hackNameSize);
            } else {
                SCANDLL_SCANFUNC scan = reinterpret_cast<SCANDLL_SCANFUNC>(OsGetLibraryEntryPoint(module, reinterpret_cast<const char*>(1)));

                if (scan) {
                    scanned = scan(&type, hackName, hackNameSize);
                }
            }

            if (scanned) {
                switch (type) {
                    case SCANDLL_RESULT_OK: {
                        *hackFound = 0;
                        *result = "OK";
                        hackName[0] = '\0';
                        block = 0;
                        *shouldBlockPlaying = block != 0;
                        success = true;
                        break;
                    }

                    case SCANDLL_RESULT_HACK: {
                        *hackFound = 1;
                        *result = "HACK";
                        *shouldBlockPlaying = block != 0;
                        success = true;
                        break;
                    }

                    case SCANDLL_RESULT_TROJAN:
                    case SCANDLL_RESULT_UNKNOWN_3: {
                        *hackFound = 1;
                        *result = "TROJAN";
                        *shouldBlockPlaying = block != 0;
                        success = true;
                        break;
                    }

                    default: {
                        block = 0;
                        success = false;
                        break;
                    }
                }
            }
        }
    }

    hackName[hackNameSize - 1] = '\0';

    if (module) {
        OsFreeLibrary(module);
    }

    if (file != OS_INVALID_FILE) {
        OsCloseFile(file);
    }

    return success;
}

// VERIFY: 4658ae23 manual
// OFFSET: 0x006BB390
bool GetDllVersionString(const char* fileName, char* buffer, uint32_t size) {
    if (size < 24) {
        return false;
    }

    if (!CheckExecutableSignature(fileName)) {
        return false;
    }

    OsFileVersionInfo info(fileName);

    if (!info.m_data.Count()) {
        return false;
    }

    SStrPrintf(buffer, size, "%u.%u.%u.%u", info.m_fileVersion[0], info.m_fileVersion[1], info.m_fileVersion[2], info.m_fileVersion[3]);
    return true;
}

// VERIFY: 46705ede manual
// OFFSET: 0x006BB430
bool CheckExecutableSignature(const char* fileName) {
    bool valid = false;
    HOSFILE file = OsCreateFile(fileName, OS_GENERIC_READ, 1, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return valid;
    }

    uint32_t size = static_cast<uint32_t>(OsGetFileSize(file));

    if (size) {
        uint8_t* data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
        uint32_t bytesRead = 0;
        OsReadFile(file, data, size, &bytesRead);

        if (data) {
            SSignatureData* token;
            SSignatureVerifyStream_Begin(&token, 256, 4);
            SSignatureVerifyStream_ProvideData(token, data, bytesRead);
            valid = SSignatureVerifyStream_Finish(token, s_signatureModulus, s_signaturePublicExponent) != 0;
            SMemFree(data, __FILE__, __LINE__, 0);
        }
    }

    OsCloseFile(file);
    return valid;
}
