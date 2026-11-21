#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace Hardware {

// ProcessorFeatures::m_vendor; named from the CPUID vendor string matched.
// Cyrix and Centaur strings are compared but leave the vendor unset.
enum PROCESSOR_VENDOR : uint32_t {
    PROCESSOR_VENDOR_UNKNOWN = 0,
    PROCESSOR_VENDOR_INTEL = 1, // GenuineIntel
    PROCESSOR_VENDOR_AMD = 2, // AuthenticAMD
};

// ProcessorFeatures::m_features; named from the CPUID feature bit each one
// copies (same layout as OS_PROCESSOR_FEATURE_*).
enum PROCESSOR_FEATURE : uint32_t {
    PROCESSOR_FEATURE_TSC = 0x1, // CPUID 1 EDX bit 4
    PROCESSOR_FEATURE_MMX = 0x2, // CPUID 1 EDX bit 23
    PROCESSOR_FEATURE_SSE = 0x4, // CPUID 1 EDX bit 25
    PROCESSOR_FEATURE_3DNOW = 0x8, // CPUID 0x80000001 EDX bit 31
    PROCESSOR_FEATURE_SSE2 = 0x10, // CPUID 1 EDX bit 26, Intel and AMD only
    PROCESSOR_FEATURE_MULTICORE = 0x40, // more than one core reported
};

struct ProcessorFeatures {
    /* 0000 */ uint32_t m_vendor; // PROCESSOR_VENDOR
    /* 0004 */ uint32_t m_features; // PROCESSOR_FEATURE
    /* 0008 */ uint32_t m_coreCount;
    /* 000C */ uint32_t m_processorCount;
};

uint32_t GetProcessorFeatures();

}
}

namespace System_Hardware {

struct CPUIDInfo {
    /* 0000 */ uint32_t m_vendor[3];
    /* 000C */ uint32_t m_maxLeaf;
    /* 0010 */ uint32_t m_leaf1Ebx;
    /* 0014 */ uint32_t m_leaf1Edx;
    /* 0018 */ uint32_t m_leaf4Eax;
    /* 001C */ uint32_t m_maxExtLeaf;
    /* 0020 */ uint32_t m_ext1Ecx;
    /* 0024 */ uint32_t m_ext1Edx;
    /* 0028 */ uint32_t m_ext4Eax;
    /* 002C */ uint32_t m_ext8Ecx;
};

extern Blizzard::Hardware::ProcessorFeatures s_processorFeatures;
extern bool s_initialized;
extern Blizzard::Lock::Atomic s_initCount;

void InitProcessorFeatures();
bool ReadCPUID(CPUIDInfo* info);

}

#endif
