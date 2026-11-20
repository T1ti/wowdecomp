#ifndef WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2WATER_H
#define WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2WATER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

struct SI2WaterSound {
    /* 0000 */ float m_distanceSq;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ int32_t m_soundKitId;
    /* 0014 */ SE2SoundKitObject m_object;
    /* 0028 */ BOOL m_valid;
    /* 002C */ int32_t m_moved;
};

#endif
