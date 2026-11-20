#include <cstdint>
#include "Engine/Source/Gx/CGxMatrixStack.h"

// VERIFY: ff34dc12 manual
// OFFSET: 0x0057C340
void CGxMatrixStack::Identity() {
    if (this->m_flags[this->m_level] & F_Identity) {
        return;
    }

    C44Matrix& m = this->m_mtx[this->m_level];
    m.Identity();

    this->m_dirty = 1;
    this->m_flags[this->m_level] = F_Identity;
}

// VERIFY: b4e2821f manual
// OFFSET: 0x00683B90
CGxMatrixStack::CGxMatrixStack() {
    this->m_level = 0;
    this->m_dirty = 0;
    this->m_flags[0] = F_Identity;
    this->m_flags[1] = 0;
    this->m_flags[2] = 0;
    this->m_flags[3] = 0;
}
