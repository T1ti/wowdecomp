#include "WoW/Source/DB/Rec/TransportRotationRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

TransportRotationRec::TransportRotationRec() {
}

TransportRotationRec::~TransportRotationRec() {
}

// VERIFY: 42085d5b auto
// OFFSET: 0x008BA720
const char* TransportRotationRec::GetFilename() {
    return "DBFilesClient\\TransportRotation.dbc";
}

bool TransportRotationRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t gameObjectsIDRead = SFileReadFile(file, &this->m_gameObjectsID, sizeof(this->m_gameObjectsID), nullptr, nullptr, 0);
    int32_t timeIndexRead = SFileReadFile(file, &this->m_TimeIndex, sizeof(this->m_TimeIndex), nullptr, nullptr, 0);
    int32_t rotRead0 = SFileReadFile(file, &this->m_rot[0], sizeof(this->m_rot[0]), nullptr, nullptr, 0);
    int32_t rotRead1 = SFileReadFile(file, &this->m_rot[1], sizeof(this->m_rot[1]), nullptr, nullptr, 0);
    int32_t rotRead2 = SFileReadFile(file, &this->m_rot[2], sizeof(this->m_rot[2]), nullptr, nullptr, 0);
    int32_t rotRead3 = SFileReadFile(file, &this->m_rot[3], sizeof(this->m_rot[3]), nullptr, nullptr, 0);

    if (!idRead
        | !gameObjectsIDRead
        | !timeIndexRead
        | !rotRead0
        | !rotRead1
        | !rotRead2
        | !rotRead3) {
        return false;
    }

    return true;
}
