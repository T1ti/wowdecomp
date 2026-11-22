#include "Engine/Source/Frame/CSimpleEmbeddedTexture.h"
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleTexture.h"

// INLINED
CSimpleEmbeddedTexture::CSimpleEmbeddedTexture() {
    void* data = CSimpleTexture::Allocate(sizeof(CSimpleTexture));
    this->m_texture = data ? new (data) CSimpleTexture(nullptr, 2, 1) : nullptr;
}

// INLINED
CSimpleEmbeddedTexture::~CSimpleEmbeddedTexture() {
    delete this->m_texture;
}
