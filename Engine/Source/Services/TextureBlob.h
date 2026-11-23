#ifndef ENGINE_SOURCE_SERVICES_TEXTUREBLOB_H
#define ENGINE_SOURCE_SERVICES_TEXTUREBLOB_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class CTexture;
class CTextureBlob;

struct SBlobTexture {
    // m_format low nibble; named from the PIXEL_DXT* format CreateTexture picks for each.
    enum ENCODING : uint8_t {
        ENCODING_DXT1 = 0,
        ENCODING_DXT3 = 1,
        ENCODING_DXT5 = 2,
        ENCODING_MASK = 0xF,
    };

    // m_numLevels high bit (so the field reads negative): HashTexture found the name already
    // hashed by another blob; RemoveTextures skips the hash delete and clears it (inferred).
    static constexpr uint8_t LEVELS_DUPLICATE_NAME = 0x80;

    /* 0000 */ uint32_t m_nameOffset;
    /* 0004 */ uint32_t m_dataOffset;
    /* 0008 */ uint8_t m_width;
    /* 0009 */ uint8_t m_height;
    /* 000A */ int8_t m_numLevels;
    /* 000B */ uint8_t m_format;
};

struct HASHKEY_TEXTUREBLOBFILE {
    /* 0000 */ const char* m_filename;

    HASHKEY_TEXTUREBLOBFILE();
    bool operator==(const HASHKEY_TEXTUREBLOBFILE& key) const;
};

class CTextureBlobTexture : public TSHashObject<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE> {
    public:
    CTextureBlobTexture();
    ~CTextureBlobTexture();
    const uint8_t* GetLevel(uint32_t mipLevel) const;

    private:
    friend class CTextureBlob;

    /* 0018 */ CTextureBlob* m_blob;
    /* 001C */ SBlobTexture* m_blobTexture;
};

class CTextureBlob {
    public:
    static int32_t CreateTexture(CTexture* texture, CTextureBlobTexture* blobTexture);
    static CTextureBlobTexture* GetTexture(const char* fileName);
    static int32_t Load(const char* fileName);
    static int32_t Unload(const char* fileName);
    static void UnloadAll();

    private:
    friend class CTextureBlobTexture;
    static void RebuildHashTable();
    static CTextureBlob* FindBlob(const char* fileName);

    /* 0000 */ char m_filename[260];
    /* 0104 */ uint8_t* m_fileData;
    /* 0108 */ SBlobTexture* m_textures;
    /* 010C */ uint32_t m_textureCount;
    /* 0110 */ const char* m_names;
    /* 0114 */ uint8_t* m_levelData;
    public:
    /* 0118 */ TSLink<CTextureBlob> m_link;

    CTextureBlob();
    ~CTextureBlob();
    private:
    void Create();
    void HashTexture(SBlobTexture* texture);
    void HashTextures();
    void RemoveTextures();
};

#endif
