#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/TextureBlob.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00AC3754
static TSExplicitList<CTextureBlob, offsetof(CTextureBlob, m_link)> s_textureBlobList;

// STATIC: 0x00B4A2D0
static TSHashTableReuse<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE> s_textureBlobTextureHash;

// INLINED
HASHKEY_TEXTUREBLOBFILE::HASHKEY_TEXTUREBLOBFILE() {
    this->m_filename = nullptr;
}

// INLINED
bool HASHKEY_TEXTUREBLOBFILE::operator==(const HASHKEY_TEXTUREBLOBFILE& key) const {
    return !SStrCmpI(this->m_filename, key.m_filename, 260);
}

// INLINED
CTextureBlobTexture::CTextureBlobTexture() {
    this->m_blob = nullptr;
    this->m_blobTexture = nullptr;
}

// INLINED
CTextureBlobTexture::~CTextureBlobTexture() {
    this->m_blob = nullptr;
    this->m_blobTexture = nullptr;
}

// INLINED
CTextureBlob::CTextureBlob() {
    this->m_filename[0] = '\0';
    this->m_fileData = nullptr;
    this->m_textures = nullptr;
    this->m_textureCount = 0;
    this->m_names = nullptr;
    this->m_levelData = nullptr;
}

// VERIFY: 9d0b7dab manual
// OFFSET: 0x004BFE70
const uint8_t* CTextureBlobTexture::GetLevel(uint32_t mipLevel) const {
    STORM_ASSERT(mipLevel < static_cast<uint8_t>(m_blobTexture->m_numLevels)); // 12213: "mipLevel < m_blobTexture->m_numLevels"

    const SBlobTexture* texture = this->m_blobTexture;
    int32_t shift = texture->m_format & SBlobTexture::ENCODING_MASK ? 0 : 1;
    uint32_t width = texture->m_width;
    uint32_t height = texture->m_height;
    const uint8_t* data = this->m_blob->m_levelData + texture->m_dataOffset + 8;

    for (; mipLevel; mipLevel--) {
        uint32_t size = height * width;

        if (shift) {
            size >>= shift;
        }

        if (size < 8) {
            size = 8;
        }

        data += size;
        height >>= 1;
        width >>= 1;
    }

    return data;
}

// VERIFY: 1d6f67aa manual
// OFFSET: 0x004BFF30
CTextureBlob::~CTextureBlob() {
    this->m_filename[0] = '\0';

    if (this->m_fileData) {
        SMemFree(this->m_fileData, __FILE__, __LINE__, 0);
    }

    this->m_fileData = nullptr;
    this->m_textures = nullptr;
    this->m_textureCount = 0;
    this->m_names = nullptr;
    this->m_levelData = nullptr;
}

// VERIFY: 04b4d023 auto
// OFFSET: 0x004C0210
CTextureBlob* CTextureBlob::FindBlob(const char* fileName) {
    for (CTextureBlob* blob = s_textureBlobList.Head(); !s_textureBlobList.IsEnd(blob); blob = s_textureBlobList.RawNext(blob)) {
        if (!SStrCmpI(fileName, blob->m_filename, STORM_MAX_STR)) {
            return blob;
        }
    }

    return nullptr;
}

// VERIFY: 28ffc9a7 auto
// OFFSET: 0x004C02F0
CTextureBlobTexture* CTextureBlob::GetTexture(const char* fileName) {
    if (!s_textureBlobList.Head()) {
        return nullptr;
    }

    HASHKEY_TEXTUREBLOBFILE key;
    key.m_filename = fileName;

    return s_textureBlobTextureHash.Ptr(SStrHashHT(fileName), key);
}

// VERIFY: c1e2e8db manual
// OFFSET: 0x004C0330
void CTextureBlob::RemoveTextures() {
    SBlobTexture* record = this->m_textures;

    for (uint32_t i = 0; i < this->m_textureCount; i++, record++) {
        if (record->m_numLevels >= 0) {
            const char* name = this->m_names + record->m_nameOffset;
            HASHKEY_TEXTUREBLOBFILE key;
            key.m_filename = name;
            CTextureBlobTexture* texture = s_textureBlobTextureHash.Ptr(SStrHashHT(name), key);
            STORM_ASSERT(texture);
            STORM_ASSERT(texture->m_blob == this);
            s_textureBlobTextureHash.Delete(texture);
        }

        record->m_numLevels &= ~SBlobTexture::LEVELS_DUPLICATE_NAME;
    }
}

// VERIFY: a2e0b946 manual
// OFFSET: 0x004C03C0
static void UpdateBlobTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CTexture* texture = static_cast<CTexture*>(userArg);
    STORM_ASSERT(texture);
    CTextureBlobTexture* blobTexture = CTextureBlob::GetTexture(texture->m_filename);
    STORM_ASSERT(blobTexture);

    if (command != GxTex_Latch) {
        return;
    }

    texelStrideInBytes = GxCalcTexelStrideInBytes(texture->m_dataFormat, width);
    const uint8_t* bits = blobTexture->GetLevel(level);
    texels = bits;

    if (texture->m_gxTexTarget == GxTex_CubeMap) {
        uint32_t order = face;

        switch (face) {
            case 0:
                order = 0;
                break;

            case 1:
                order = 2;
                break;

            case 2:
                order = 4;
                break;

            case 3:
                order = 5;
                break;

            case 4:
                order = 3;
                break;

            case 5:
                order = 1;
                break;
        }

        texels = bits + order * texelStrideInBytes;
        texelStrideInBytes *= 6;
    }
}

// VERIFY: 328747cb manual
// OFFSET: 0x004C06C0
void CTextureBlob::UnloadAll() {
    for (CTextureBlob* blob = s_textureBlobList.Head(); !s_textureBlobList.IsEnd(blob); ) {
        CTextureBlob* next = s_textureBlobList.RawNext(blob);
        blob->RemoveTextures();
        s_textureBlobList.DeleteNode(blob);
        blob = next;
    }
    STORM_ASSERT(s_textureBlobList.Head() == nullptr); // 12213: "s_textureBlobList.Head() == __null"
    STORM_ASSERT(s_textureBlobTextureHash.Head() == nullptr); // 12213: "s_textureBlobTextureHash.Head() == __null"
}

// VERIFY: d589605f manual
// OFFSET: 0x004C0710
int32_t CTextureBlob::CreateTexture(CTexture* texture, CTextureBlobTexture* blobTexture) {
    texture->m_alphaBits = blobTexture->m_blobTexture->m_format >> 4;

    if (!texture->m_alphaBits) {
        texture->m_flags |= CTexture::FLAGS::IS_OPAQUE;
    }

    uint32_t width = blobTexture->m_blobTexture->m_width;
    uint32_t height = blobTexture->m_blobTexture->m_height;
    texture->m_bestMip = 0;

    PIXEL_FORMAT pixelFormat;
    EGxTexFormat gxTexFormat;
    uint32_t encoding = blobTexture->m_blobTexture->m_format & SBlobTexture::ENCODING_MASK;
    TextureGetTextureFormats(pixelFormat, gxTexFormat, encoding == SBlobTexture::ENCODING_DXT3 ? PIXEL_DXT3 : encoding == SBlobTexture::ENCODING_DXT5 ? PIXEL_DXT5 : PIXEL_DXT1, texture->m_alphaBits);

    EGxTexTarget target = GxTex_2d;

    if (width == 6 * height) {
        target = GxTex_CubeMap;
        width /= 6;
    }

    encoding = blobTexture->m_blobTexture->m_format & SBlobTexture::ENCODING_MASK;
    EGxTexFormat dataFormat = TexturePixelFormatToGxTexFormat(encoding == SBlobTexture::ENCODING_DXT3 ? PIXEL_DXT3 : encoding == SBlobTexture::ENCODING_DXT5 ? PIXEL_DXT5 : PIXEL_DXT1);
    texture->m_gxTexFormat = gxTexFormat;
    texture->m_dataFormat = dataFormat;
    texture->m_gxTexTarget = target;
    texture->m_gxWidth = width;
    texture->m_gxHeight = height;

    uint32_t filter = texture->m_gxTexFlags.m_filter;

    if (filter != GxTex_Nearest && filter != GxTex_Linear && blobTexture->m_blobTexture->m_numLevels == 1) {
        texture->m_gxTexFlags.m_filter = filter == GxTex_NearestMipNearest ? GxTex_Nearest : GxTex_Linear;
    }

    texture->m_gxTex = TextureAllocGxTex(target, width, height, 0, gxTexFormat, texture->m_gxTexFlags, texture, UpdateBlobTexture, dataFormat, texture->m_filename);

    if (!texture->m_gxTex) {
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" allocating %dx%d texture failed.\n", texture->m_filename, width, height);
        return 0;
    }

    GxTexUpdate(texture->m_gxTex, 0, 0, width, height, 1);

    return 1;
}

// VERIFY: 74fd3352 manual
// OFFSET: 0x004C0E60
void CTextureBlob::HashTexture(SBlobTexture* texture) {
    const char* name = this->m_names + texture->m_nameOffset;
    uint32_t hash = SStrHashHT(name);
    HASHKEY_TEXTUREBLOBFILE key;
    key.m_filename = name;

    if (s_textureBlobTextureHash.Ptr(hash, key)) {
        texture->m_numLevels |= SBlobTexture::LEVELS_DUPLICATE_NAME;
        return;
    }

    CTextureBlobTexture* blobTexture = s_textureBlobTextureHash.New(hash, key, 0, 0);
    blobTexture->m_blob = this;
    blobTexture->m_blobTexture = texture;
}

// VERIFY: cdf3ceec auto
// OFFSET: 0x004C0F20
void CTextureBlob::HashTextures() {
    SBlobTexture* texture = this->m_textures;

    for (uint32_t i = 0; i < this->m_textureCount; i++, texture++) {
        this->HashTexture(texture);
    }
}

namespace {

struct TextureBlobChunk { // (inferred)
    enum TOKEN : uint32_t {
        TXVR = 0x54585652,
        TXBT = 0x54584254,
        TXFN = 0x5458464E,
        TXMD = 0x54584D44,
    };

    uint32_t token;
    uint32_t size;
};

}

// INLINED
inline void CTextureBlob::Create() {
    auto* iffChunk = reinterpret_cast<TextureBlobChunk*>(this->m_fileData);
    STORM_ASSERT(iffChunk->token == TextureBlobChunk::TXVR); // 12213: "iffChunk->token == 'TXVR'"

    iffChunk = reinterpret_cast<TextureBlobChunk*>(reinterpret_cast<uint8_t*>(iffChunk + 1) + iffChunk->size);
    STORM_ASSERT(iffChunk->token == TextureBlobChunk::TXBT); // 12213: "iffChunk->token == 'TXBT'"
    this->m_textures = reinterpret_cast<SBlobTexture*>(iffChunk + 1);
    this->m_textureCount = iffChunk->size / sizeof(SBlobTexture);

    iffChunk = reinterpret_cast<TextureBlobChunk*>(reinterpret_cast<uint8_t*>(iffChunk + 1) + iffChunk->size);
    STORM_ASSERT(iffChunk->token == TextureBlobChunk::TXFN); // 12213: "iffChunk->token == 'TXFN'"
    this->m_names = reinterpret_cast<const char*>(iffChunk + 1);

    iffChunk = reinterpret_cast<TextureBlobChunk*>(reinterpret_cast<uint8_t*>(iffChunk + 1) + iffChunk->size);
    STORM_ASSERT(iffChunk->token == TextureBlobChunk::TXMD); // 12213: "iffChunk->token == 'TXMD'"
    this->m_levelData = reinterpret_cast<uint8_t*>(iffChunk);
    this->HashTextures();
}

// VERIFY: 6fbb3a53 manual
// OFFSET: 0x004C0F50
int32_t CTextureBlob::Load(const char* fileName) {
    if (CTextureBlob::FindBlob(fileName)) {
        return 1;
    }

    SFileImpl* file = TextureCreateOpenFile(fileName, 0);

    if (!file) {
        return 0;
    }

    CTextureBlob* textureBlob = s_textureBlobList.NewNode(TSLIST_LINK_BEFORE, 0, 0);
    STORM_ASSERT(textureBlob);
    SStrCopy(textureBlob->m_filename, fileName, STORM_MAX_STR);

    uint32_t size = SFileGetFileSize(file, nullptr);
    textureBlob->m_fileData = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    STORM_ASSERT(textureBlob->m_fileData);
    SFileReadFile(file, textureBlob->m_fileData, size, nullptr, nullptr, 0);
    SFileCloseFile(file);

    textureBlob->Create();

    return 1;
}

// VERIFY: 8ace8e6f manual
// OFFSET: 0x004C1030
void CTextureBlob::RebuildHashTable() {
    for (CTextureBlob* blob = s_textureBlobList.Head(); !s_textureBlobList.IsEnd(blob); blob = s_textureBlobList.RawNext(blob)) {
        blob->RemoveTextures();
    }

    STORM_ASSERT(s_textureBlobTextureHash.Head() == nullptr); // 12213: "s_textureBlobTextureHash.Head() == __null"

    for (CTextureBlob* blob = s_textureBlobList.Head(); !s_textureBlobList.IsEnd(blob); blob = s_textureBlobList.RawNext(blob)) {
        blob->HashTextures();
    }
}

// VERIFY: e70e77fa manual
// OFFSET: 0x004C10C0
int32_t CTextureBlob::Unload(const char* fileName) {
    for (CTextureBlob* blob = s_textureBlobList.Head(); !s_textureBlobList.IsEnd(blob); ) {
        CTextureBlob* next = s_textureBlobList.RawNext(blob);

        if (!SStrCmpI(fileName, blob->m_filename, STORM_MAX_STR)) {
            blob->RemoveTextures();
            s_textureBlobList.DeleteNode(blob);
            CTextureBlob::RebuildHashTable();
            return 1;
        }

        blob = next;
    }

    return 0;
}
