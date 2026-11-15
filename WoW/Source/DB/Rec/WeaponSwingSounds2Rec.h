#ifndef WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H
#define WOW_SOURCE_DB_REC_WEAPONSWINGSOUNDS2REC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;
enum WEAPONSWING_SOUNDTYPES : int32_t;
enum WEAPONSWINGTYPES : int32_t;

class WeaponSwingSounds2Rec {
    public:
    static const int32_t s_numColumns = 4;
    static const int32_t s_rowSize = 16;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ WEAPONSWING_SOUNDTYPES m_swingType;
    /* 0008 */ WEAPONSWINGTYPES m_crit;
    /* 000C */ int32_t m_soundID;

    WeaponSwingSounds2Rec();
    ~WeaponSwingSounds2Rec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
