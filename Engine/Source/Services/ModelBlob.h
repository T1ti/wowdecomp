#ifndef ENGINE_SOURCE_SERVICES_MODELBLOB_H
#define ENGINE_SOURCE_SERVICES_MODELBLOB_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"

class CModelBlob;

struct HASHKEY_MODELBLOBFILE {
    /* 0000 */ const char* m_filename;

    HASHKEY_MODELBLOBFILE();
    bool operator==(const HASHKEY_MODELBLOBFILE& key) const;
};

class CModelBlob {
    private:
    struct SEntry {
        /* 0000 */ uint32_t m_filenameOffset;
        /* 0004 */ CAaBox m_boundingBox;
    };

    public:
    class CHashEntry : public TSHashObject<CHashEntry, HASHKEY_MODELBLOBFILE> {
        public:
        /* 0018 */ CModelBlob* m_blob;
        /* 001C */ SEntry* m_entry;

        CHashEntry();
        ~CHashEntry();
    };

    CModelBlob(const char* fileName);
    ~CModelBlob();
    int32_t Query(const char* modelName, C3Vector& min, C3Vector& max);

    bool IsLoaded() const { return this->m_fileData != nullptr; } // (inferred)

    private:
    /* 0000 */ char m_filename[256];
    /* 0100 */ uint32_t m_version;
    /* 0104 */ uint8_t* m_fileData;
    /* 0108 */ SEntry* m_entries;
    /* 010C */ uint32_t m_entryCount;
    /* 0110 */ const char* m_stringTable;

    void HashBlobEntry(SEntry* entry);
    void Load(const char* fileName);
};

void ModelBlobDestroy();
int32_t ModelBlobLoad(const char* fileName);
int32_t ModelBlobQuery(const char* modelName, C3Vector& min, C3Vector& max);

#endif
