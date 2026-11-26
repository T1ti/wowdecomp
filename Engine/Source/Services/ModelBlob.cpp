#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ModelBlob.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

namespace {

struct ModelBlobChunk { // (inferred)
    enum TOKEN : uint32_t {
        MBVR = 0x4D425652,
        MBBB = 0x4D424242,
        MBFN = 0x4D42464E,
    };

    uint32_t token;
    uint32_t size;
};

}

// STATIC: 0x00B4A270
static CModelBlob* s_modelBlob;

// STATIC: 0x00B4A274
static TSHashTableReuse<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE> s_modelBlobHT;

// INLINED
HASHKEY_MODELBLOBFILE::HASHKEY_MODELBLOBFILE() {
    this->m_filename = nullptr;
}

// INLINED
bool HASHKEY_MODELBLOBFILE::operator==(const HASHKEY_MODELBLOBFILE& key) const {
    return !SStrCmpI(this->m_filename, key.m_filename, 260);
}

// INLINED
CModelBlob::CHashEntry::CHashEntry() {
    this->m_blob = nullptr;
    this->m_entry = nullptr;
}

// INLINED
CModelBlob::CHashEntry::~CHashEntry() {
    this->m_blob = nullptr;
    this->m_entry = nullptr;
}

// INLINED
CModelBlob::CModelBlob(const char* fileName) {
    this->m_fileData = nullptr;
    this->m_entries = nullptr;
    this->m_entryCount = 0;
    this->m_stringTable = nullptr;
    this->Load(fileName);
}

// VERIFY: e1be90a5 manual
// OFFSET: 0x004BAF60
CModelBlob::~CModelBlob() {
    if (this->m_fileData) {
        SMemFree(this->m_fileData, __FILE__, __LINE__, 0);
    }

    this->m_entries = nullptr;
    this->m_entryCount = 0;
    this->m_stringTable = nullptr;
    s_modelBlobHT.Clear();
}

// VERIFY: a7881e40 auto
// OFFSET: 0x004BB1C0
void ModelBlobDestroy() {
    if (s_modelBlob) {
        delete s_modelBlob;
        s_modelBlob = nullptr;
    }
}

// VERIFY: 74063afe manual
// OFFSET: 0x004BB370
int32_t CModelBlob::Query(const char* modelName, C3Vector& min, C3Vector& max) {
    uint32_t hashval = SStrHashHT(modelName);
    HASHKEY_MODELBLOBFILE key;
    key.m_filename = modelName;
    CHashEntry* entry = s_modelBlobHT.Ptr(hashval, key);

    if (!entry) {
        return 0;
    }

    min = entry->m_entry->m_boundingBox.b;
    max = entry->m_entry->m_boundingBox.t;

    return 1;
}

// VERIFY: 7fa98d10 auto
// OFFSET: 0x004BB3E0
int32_t ModelBlobQuery(const char* modelName, C3Vector& min, C3Vector& max) {
    if (!s_modelBlob) {
        return 0;
    }

    return s_modelBlob->Query(modelName, min, max);
}

// VERIFY: 8ef8f2aa manual
// OFFSET: 0x004BBA70
void CModelBlob::HashBlobEntry(SEntry* entry) {
    const char* name = this->m_stringTable + entry->m_filenameOffset;
    uint32_t hashval = SStrHashHT(name);
    HASHKEY_MODELBLOBFILE key;
    key.m_filename = name;

    if (s_modelBlobHT.Ptr(hashval, key)) {
        return;
    }

    CHashEntry* hashEntry = s_modelBlobHT.New(hashval, key, 0, 0);
    hashEntry->m_blob = this;
    hashEntry->m_entry = entry;
}

// VERIFY: 3c3bf082 manual
// OFFSET: 0x004BBB20
void CModelBlob::Load(const char* fileName) {
    SFileImpl* file = nullptr;
    SErrSetLastError(0);

    if (!SFile::OpenEx(nullptr, fileName, 0, &file)) {
        if (!SErrGetLastError()) {
            SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
        }

        return;
    }

    SStrCopy(this->m_filename, fileName, STORM_MAX_STR);
    uint32_t size = SFileGetFileSize(file, nullptr);
    this->m_fileData = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    STORM_ASSERT(m_fileData);
    SFileReadFile(file, this->m_fileData, size, nullptr, nullptr, 0);
    SFileCloseFile(file);

    const auto* chunkHdr = reinterpret_cast<const ModelBlobChunk*>(this->m_fileData);
    STORM_ASSERT(chunkHdr->token == ModelBlobChunk::MBVR); // 12213: "chunkHdr->token == 'MBVR'"

    auto* boundsData = this->m_fileData + sizeof(*chunkHdr) + chunkHdr->size;
    chunkHdr = reinterpret_cast<const ModelBlobChunk*>(boundsData);
    STORM_ASSERT(chunkHdr->token == ModelBlobChunk::MBBB); // 12213: "chunkHdr->token == 'MBBB'"
    uint32_t chunkSize = chunkHdr->size;
    this->m_entries = reinterpret_cast<SEntry*>(boundsData + sizeof(*chunkHdr));
    this->m_entryCount = chunkSize / sizeof(SEntry);

    const auto* namesData = reinterpret_cast<const uint8_t*>(this->m_entries) + chunkSize;
    chunkHdr = reinterpret_cast<const ModelBlobChunk*>(namesData);
    STORM_ASSERT(chunkHdr->token == ModelBlobChunk::MBFN); // 12213: "chunkHdr->token == 'MBFN'"
    this->m_stringTable = reinterpret_cast<const char*>(namesData + sizeof(*chunkHdr));

    SEntry* entry = this->m_entries;

    for (uint32_t i = 0; i < this->m_entryCount; i++, entry++) {
        this->HashBlobEntry(entry);
    }
}

// VERIFY: b703c86f manual
// OFFSET: 0x004BBC20
int32_t ModelBlobLoad(const char* fileName) {
    if (s_modelBlob) {
        delete s_modelBlob;
        s_modelBlob = nullptr;
    }

    s_modelBlob = new CModelBlob(fileName);

    if (s_modelBlob->IsLoaded()) {
        return 1;
    }

    delete s_modelBlob;
    s_modelBlob = nullptr;

    return 0;
}
