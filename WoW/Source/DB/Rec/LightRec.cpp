#include "WoW/Source/DB/Rec/LightRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

LightRec::LightRec() {
}

LightRec::~LightRec() {
}

// VERIFY: 873bd1a8 auto
// OFFSET: 0x008BDFE0
const char* LightRec::GetFilename() {
    return "DBFilesClient\\Light.dbc";
}

// VERIFY: 65edd4a3 manual
// OFFSET: 0x008BDFF0
bool LightRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t continentIDRead = SFileReadFile(file, &this->m_continentID, sizeof(this->m_continentID), nullptr, nullptr, 0);
    int32_t gameCoordsRead0 = SFileReadFile(file, &this->m_gameCoords[0], sizeof(this->m_gameCoords[0]), nullptr, nullptr, 0);
    int32_t gameCoordsRead1 = SFileReadFile(file, &this->m_gameCoords[1], sizeof(this->m_gameCoords[1]), nullptr, nullptr, 0);
    int32_t gameCoordsRead2 = SFileReadFile(file, &this->m_gameCoords[2], sizeof(this->m_gameCoords[2]), nullptr, nullptr, 0);
    int32_t gameFalloffStartRead = SFileReadFile(file, &this->m_gameFalloff.m_l, sizeof(this->m_gameFalloff.m_l), nullptr, nullptr, 0);
    int32_t gameFalloffEndRead = SFileReadFile(file, &this->m_gameFalloff.m_h, sizeof(this->m_gameFalloff.m_h), nullptr, nullptr, 0);
    int32_t lightParamsIDRead = SFileReadFile(file, this->m_lightParamsID, sizeof(this->m_lightParamsID), nullptr, nullptr, 0);

    if (!idRead
        | !continentIDRead
        | !gameCoordsRead0
        | !gameCoordsRead1
        | !gameCoordsRead2
        | !gameFalloffStartRead
        | !gameFalloffEndRead
        | !lightParamsIDRead) {
        return false;
    }

    return true;
}
