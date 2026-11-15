#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

DungeonMapRec::DungeonMapRec() {
}

DungeonMapRec::~DungeonMapRec() {
}

// VERIFY: 6e4c6357 auto
// OFFSET: 0x008AED10
const char* DungeonMapRec::GetFilename() {
    return "DBFilesClient\\DungeonMap.dbc";
}

bool DungeonMapRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t mapIDRead = SFileReadFile(file, &this->m_mapID, sizeof(this->m_mapID), nullptr, nullptr, 0);
    int32_t floorIndexRead = SFileReadFile(file, &this->m_floorIndex, sizeof(this->m_floorIndex), nullptr, nullptr, 0);
    int32_t minXRead = SFileReadFile(file, &this->m_boundsX.m_l, sizeof(this->m_boundsX.m_l), nullptr, nullptr, 0);
    int32_t maxXRead = SFileReadFile(file, &this->m_boundsX.m_h, sizeof(this->m_boundsX.m_h), nullptr, nullptr, 0);
    int32_t minYRead = SFileReadFile(file, &this->m_boundsY.m_l, sizeof(this->m_boundsY.m_l), nullptr, nullptr, 0);
    int32_t maxYRead = SFileReadFile(file, &this->m_boundsY.m_h, sizeof(this->m_boundsY.m_h), nullptr, nullptr, 0);
    int32_t parentWorldMapIDRead = SFileReadFile(file, &this->m_parentWorldMapID, sizeof(this->m_parentWorldMapID), nullptr, nullptr, 0);

    if (!idRead
        | !mapIDRead
        | !floorIndexRead
        | !minXRead
        | !maxXRead
        | !minYRead
        | !maxYRead
        | !parentWorldMapIDRead) {
        return false;
    }

    return true;
}
