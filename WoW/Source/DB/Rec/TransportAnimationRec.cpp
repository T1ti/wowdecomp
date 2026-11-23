#include "WoW/Source/DB/Rec/TransportAnimationRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

TransportAnimationRec::TransportAnimationRec() {
}

TransportAnimationRec::~TransportAnimationRec() {
}

// VERIFY: 855c5094 auto
// OFFSET: 0x008BA590
const char* TransportAnimationRec::GetFilename() {
    return "DBFilesClient\\TransportAnimation.dbc";
}

bool TransportAnimationRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t transportIDRead = SFileReadFile(file, &this->m_transportID, sizeof(this->m_transportID), nullptr, nullptr, 0);
    int32_t timeIndexRead = SFileReadFile(file, &this->m_TimeIndex, sizeof(this->m_TimeIndex), nullptr, nullptr, 0);
    int32_t posRead0 = SFileReadFile(file, &this->m_pos[0], sizeof(this->m_pos[0]), nullptr, nullptr, 0);
    int32_t posRead1 = SFileReadFile(file, &this->m_pos[1], sizeof(this->m_pos[1]), nullptr, nullptr, 0);
    int32_t posRead2 = SFileReadFile(file, &this->m_pos[2], sizeof(this->m_pos[2]), nullptr, nullptr, 0);
    int32_t sequenceIDRead = SFileReadFile(file, &this->m_sequenceID, sizeof(this->m_sequenceID), nullptr, nullptr, 0);

    if (!idRead
        | !transportIDRead
        | !timeIndexRead
        | !posRead0
        | !posRead1
        | !posRead2
        | !sequenceIDRead) {
        return false;
    }

    return true;
}
