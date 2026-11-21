#ifndef ENGINE_SOURCE_OS_PROCESSOR_H
#define ENGINE_SOURCE_OS_PROCESSOR_H

#include <cstdint>

// PPC retains its report-table slot; Windows detection emits 0, 1, 2 or 4.
enum : int32_t {
    OS_PROCESSOR_VENDOR_UNKNOWN = 0,
    OS_PROCESSOR_VENDOR_INTEL = 1,
    OS_PROCESSOR_VENDOR_AMD = 2,
    OS_PROCESSOR_VENDOR_PPC = 3,
    OS_PROCESSOR_VENDOR_CPUID_FAILED = 4,
};

// OsGetProcessorFeatures bits; named from the CPUID feature bit each one
// copies. VALID is always set once detection has run (the log masks it off).
enum : uint32_t {
    OS_PROCESSOR_FEATURE_TSC = 0x1, // CPUID 1 EDX bit 4
    OS_PROCESSOR_FEATURE_MMX = 0x2, // CPUID 1 EDX bit 23
    OS_PROCESSOR_FEATURE_SSE = 0x4, // CPUID 1 EDX bit 25
    OS_PROCESSOR_FEATURE_3DNOW = 0x8, // CPUID 0x80000001 EDX bit 31
    OS_PROCESSOR_FEATURE_SSE2 = 0x10, // CPUID 1 EDX bit 26
    OS_PROCESSOR_FEATURE_MULTICORE = 0x40, // more than one core reported
    OS_PROCESSOR_FEATURE_VALID = 0x80000000,
};

uint32_t OsGetProcessorFeatures();
uint32_t OsGetProcessorFeaturesEx(int32_t& vendor);
uint64_t OsGetProcessorTicksPerSecond();

#endif
