#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CHECKEXECUTABLESIGNATURE_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_CHECKEXECUTABLESIGNATURE_H

#include <cstdint>

// Scan DLL result type, named after the result string ScanFileForHack reports for each.
enum SCANDLL_RESULT : int32_t {
    SCANDLL_RESULT_OK = 0,
    SCANDLL_RESULT_HACK = 1,
    SCANDLL_RESULT_TROJAN = 2,
    SCANDLL_RESULT_UNKNOWN_3 = 3, // also reported as "TROJAN"
};

typedef int32_t (__stdcall* SCANDLL_SCANFUNC)(int32_t* type, char* hackName, uint32_t hackNameSize);
typedef int32_t (__stdcall* SCANDLL_SCANFUNC2)(int32_t* type, int32_t* shouldBlockPlaying, char* hackName, uint32_t hackNameSize);

bool CheckExecutableSignature(const char* fileName);
bool GetDllVersionString(const char* fileName, char* buffer, uint32_t size);
bool ScanFileForHack(const char* fileName, uint8_t* hackFound, uint8_t* shouldBlockPlaying, const char** result, char* hackName, uint32_t hackNameSize);

#endif
