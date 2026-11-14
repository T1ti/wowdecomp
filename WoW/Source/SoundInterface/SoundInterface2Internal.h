#ifndef WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2INTERNAL_H
#define WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2INTERNAL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/fmodex/src/fmod.hpp"
#include "Engine/Source/Storm/H/stpl.h"

enum SE2_FILE_SELECT_MODE : int32_t;

class SoundEntriesAdvancedRec;

class VSOUNDKITDEF {
    public:
    /* 0000 */ const char* m_name;
    /* 0004 */ char* m_files[10];
    /* 002C */ float m_minDistance;
    /* 0030 */ float m_distanceCutoff;
    /* 0034 */ float m_volume;
    /* 0038 */ int32_t m_id;
    /* 003C */ uint32_t m_flags;
    /* 0040 */ int32_t m_numFiles;
    /* 0044 */ int32_t m_lastIndex;
    /* 0048 */ int32_t m_frequencies[10];
    /* 0070 */ int32_t m_weights[10];
    /* 0098 */ FMOD::SoundGroup* m_loadSoundGroup;
    /* 009C */ FMOD::SoundGroup* m_soundGroup;
    /* 00A0 */ int32_t m_pendingIndex;
    /* 00A4 */ int32_t m_downloadPending;
    /* 00A8 */ int32_t m_advancedId;
    /* 00AC */ SoundEntriesAdvancedRec* m_advanced;

    VSOUNDKITDEF();
    ~VSOUNDKITDEF();
    void ResetWeights();
    const char* SelectFile(SE2_FILE_SELECT_MODE mode, int32_t* index, int32_t fileIndex, int32_t priority);
};

struct SOUNDKITLOOKUP : public TSHashObject<SOUNDKITLOOKUP, HASHKEY_STRI> {
    /* 0018 */ int32_t m_soundKitId;
};

struct ZONESOUNDOVERRIDELOOKUP : public TSHashObject<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI> {
    /* 0018 */ int32_t m_zoneIntroMusicId;
    /* 001C */ int32_t m_zoneMusicId;
    /* 0020 */ int32_t m_soundAmbienceId;
    /* 0024 */ int32_t m_soundProviderPreferencesId;
};

class WorldStateZoneSoundOverride {
    public:
    /* 0000 */ int32_t m_areaId;
    /* 0004 */ int32_t m_wmoAreaId;
    /* 0008 */ int32_t m_worldStateId;
    /* 000C */ int32_t m_worldStateValue;
    /* 0010 */ int32_t m_zoneIntroMusicId;
    /* 0014 */ int32_t m_zoneMusicId;
    /* 0018 */ int32_t m_soundAmbienceId;
    /* 001C */ int32_t m_soundProviderPreferencesId;
};

#endif
