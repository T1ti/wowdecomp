#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gxu/GxuFontFace.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

struct FACEDATA : public CHandleObject, public TSHashObject<FACEDATA, HASHKEY_STRI> {
    /* 0020 */ void* m_data;
    /* 0024 */ FT_Face m_face;
    /* 0028 */ HOBJECT selfReference;

    FACEDATA();
    virtual ~FACEDATA();
};

// STATIC: 0x00C7D2FC
static TSHashTable<FACEDATA, HASHKEY_STRI> s_faceHash;

// VERIFY: e0237d01 auto
// OFFSET: 0x006C8080
FT_Face FontFaceGetFace(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    FACEDATA* dataPtr = reinterpret_cast<FACEDATA*>(handle);
    STORM_ASSERT(dataPtr->selfReference);
    return dataPtr->m_face;
}

// VERIFY: 5082eae0 auto
// OFFSET: 0x006C80A0
void FontFaceCloseHandle(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    FACEDATA* dataPtr = reinterpret_cast<FACEDATA*>(handle);

    HandleClose(reinterpret_cast<HOBJECT>(handle));

    int32_t refCount = dataPtr->m_refcount;
    STORM_ASSERT(refCount >= 1);
    STORM_ASSERT(dataPtr->selfReference);
    if (refCount <= 1) {
        HOBJECT selfReference = dataPtr->selfReference;
        dataPtr->selfReference = nullptr;
        HandleClose(selfReference);
    }
}

// VERIFY: c0728d10 auto
// OFFSET: 0x006C80E0
const char* FontFaceGetFontName(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    return reinterpret_cast<FACEDATA*>(handle)->m_key.m_str;
}

// VERIFY: 63774048 manual
// OFFSET: 0x006C8190
FACEDATA::FACEDATA() {
    this->m_data = nullptr;
    this->m_face = nullptr;
}

// VERIFY: 263b0e27 manual
// OFFSET: 0x006C81C0
FACEDATA::~FACEDATA() {
    STORM_ASSERT(!selfReference);

    if (this->m_face) {
        FT_Done_Face(this->m_face);
    }

    if (this->m_data) {
        SFile::Unload(this->m_data);
    }
}

// VERIFY: 45a8d799 auto
// OFFSET: 0x006C8AE0
HFACE FontFaceGetHandle(const char* fileName, FT_Library library) {
    if (!library || !fileName || !*fileName) {
        return nullptr;
    }

    FACEDATA* faceData = s_faceHash.Ptr(fileName);

    if (faceData) {
        STORM_ASSERT(faceData->selfReference);
        return reinterpret_cast<HFACE>(HandleDuplicate(faceData->selfReference));
    }

    HFACE handle = nullptr;
    void* data = nullptr;
    uint32_t size;

    if (SFile::ReadFile(nullptr, fileName, &data, &size, 0, 3, nullptr)) {
        if (!data) {
            return handle;
        }

        FT_Face ftFace;

        if (!FT_New_Memory_Face(library, static_cast<FT_Byte*>(data), size, 0, &ftFace) && ftFace && !FT_Select_Charmap(ftFace, ft_encoding_unicode)) {
            faceData = new ("HFACE", SLOG_OBJECT) FACEDATA;
            STORM_ASSERT(faceData);
            faceData->m_data = data;
            faceData->m_face = ftFace;
            s_faceHash.Insert(faceData, fileName);
            data = nullptr;
            ftFace = nullptr;
            faceData->selfReference = HandleCreate(faceData, "HFACE");
            handle = reinterpret_cast<HFACE>(HandleDuplicate(faceData->selfReference));
        }
    }

    if (data) {
        SFile::Unload(data);
    }

    return handle;
}
