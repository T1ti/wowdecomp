#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

LiquidTypeRec::LiquidTypeRec() {
}

LiquidTypeRec::~LiquidTypeRec() {
}

// VERIFY: 43e47a4a auto
// OFFSET: 0x008B2710
const char* LiquidTypeRec::GetFilename() {
    return "DBFilesClient\\LiquidType.dbc";
}

// VERIFY: 19bc1530 auto
// OFFSET: 0x008B2720
bool LiquidTypeRec::Read(SFileImpl* file, const char* stringBuffer) {
    uint32_t nameOffset;
    uint32_t textureOffset[6];

    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t nameRead = SFileReadFile(file, &nameOffset, sizeof(nameOffset), nullptr, nullptr, 0);
    int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
    int32_t basicTypeRead = SFileReadFile(file, &this->m_basicType, sizeof(this->m_basicType), nullptr, nullptr, 0);
    int32_t soundIDRead = SFileReadFile(file, &this->m_soundID, sizeof(this->m_soundID), nullptr, nullptr, 0);
    int32_t spellIDRead = SFileReadFile(file, &this->m_spellID, sizeof(this->m_spellID), nullptr, nullptr, 0);
    int32_t maxDarkenDepthRead = SFileReadFile(file, &this->m_maxDarkenDepth, sizeof(this->m_maxDarkenDepth), nullptr, nullptr, 0);
    int32_t fogDarkenIntensityRead = SFileReadFile(file, &this->m_fogDarkenIntensity, sizeof(this->m_fogDarkenIntensity), nullptr, nullptr, 0);
    int32_t ambDarkenIntensityRead = SFileReadFile(file, &this->m_ambDarkenIntensity, sizeof(this->m_ambDarkenIntensity), nullptr, nullptr, 0);
    int32_t dirDarkenIntensityRead = SFileReadFile(file, &this->m_dirDarkenIntensity, sizeof(this->m_dirDarkenIntensity), nullptr, nullptr, 0);
    int32_t lightIDRead = SFileReadFile(file, &this->m_lightID, sizeof(this->m_lightID), nullptr, nullptr, 0);
    int32_t particleScaleRead = SFileReadFile(file, &this->m_particleScale, sizeof(this->m_particleScale), nullptr, nullptr, 0);
    int32_t particleMovementRead = SFileReadFile(file, &this->m_particleMovement, sizeof(this->m_particleMovement), nullptr, nullptr, 0);
    int32_t particleTexSlotsRead = SFileReadFile(file, &this->m_particleTexSlots, sizeof(this->m_particleTexSlots), nullptr, nullptr, 0);
    int32_t materialIDRead = SFileReadFile(file, &this->m_materialID, sizeof(this->m_materialID), nullptr, nullptr, 0);
    int32_t textureRead0 = SFileReadFile(file, &textureOffset[0], sizeof(textureOffset[0]), nullptr, nullptr, 0);
    int32_t textureRead1 = SFileReadFile(file, &textureOffset[1], sizeof(textureOffset[1]), nullptr, nullptr, 0);
    int32_t textureRead2 = SFileReadFile(file, &textureOffset[2], sizeof(textureOffset[2]), nullptr, nullptr, 0);
    int32_t textureRead3 = SFileReadFile(file, &textureOffset[3], sizeof(textureOffset[3]), nullptr, nullptr, 0);
    int32_t textureRead4 = SFileReadFile(file, &textureOffset[4], sizeof(textureOffset[4]), nullptr, nullptr, 0);
    int32_t textureRead5 = SFileReadFile(file, &textureOffset[5], sizeof(textureOffset[5]), nullptr, nullptr, 0);
    int32_t colorRead = SFileReadFile(file, this->m_color, sizeof(this->m_color), nullptr, nullptr, 0);
    int32_t floatRead = SFileReadFile(file, this->m_float, sizeof(this->m_float), nullptr, nullptr, 0);
    int32_t intRead = SFileReadFile(file, this->m_int, sizeof(this->m_int), nullptr, nullptr, 0);

    if (!idRead
        | !nameRead
        | !flagsRead
        | !basicTypeRead
        | !soundIDRead
        | !spellIDRead
        | !maxDarkenDepthRead
        | !fogDarkenIntensityRead
        | !ambDarkenIntensityRead
        | !dirDarkenIntensityRead
        | !lightIDRead
        | !particleScaleRead
        | !particleMovementRead
        | !particleTexSlotsRead
        | !materialIDRead
        | !textureRead0
        | !textureRead1
        | !textureRead2
        | !textureRead3
        | !textureRead4
        | !textureRead5
        | !colorRead
        | !floatRead
        | !intRead) {
        return false;
    }

    if (stringBuffer) {
        this->m_name = &stringBuffer[nameOffset];
        this->m_texture[0] = &stringBuffer[textureOffset[0]];
        this->m_texture[1] = &stringBuffer[textureOffset[1]];
        this->m_texture[2] = &stringBuffer[textureOffset[2]];
        this->m_texture[3] = &stringBuffer[textureOffset[3]];
        this->m_texture[4] = &stringBuffer[textureOffset[4]];
        this->m_texture[5] = &stringBuffer[textureOffset[5]];
    } else {
        this->m_name = "";
        this->m_texture[0] = "";
        this->m_texture[1] = "";
        this->m_texture[2] = "";
        this->m_texture[3] = "";
        this->m_texture[4] = "";
        this->m_texture[5] = "";
    }

    return true;
}
