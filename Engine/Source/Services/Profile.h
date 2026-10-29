#ifndef ENGINE_SOURCE_SERVICES_PROFILE_H
#define ENGINE_SOURCE_SERVICES_PROFILE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Storm/H/stpl.h"

namespace ProfileInternal {
    struct STRINGBLOCK : public TSLinkedNode<STRINGBLOCK> {
        static STRINGBLOCK* AllocBlock(uint32_t size);
        static char* AllocString(TSList<STRINGBLOCK, TSGetLink<STRINGBLOCK> >& list, const char* str, int32_t reuse);
        static void FreeString(TSList<STRINGBLOCK, TSGetLink<STRINGBLOCK> >& list, char* str);

        /* 0008 */ uint32_t m_refs;
        /* 000C */ uint32_t m_size;
        /* 0010 */ uint32_t m_used;

        char* Data();
    };

    struct KEYVALUE : public TSHashObject<KEYVALUE, HASHKEY_CONSTSTRI> {
        /* 0018 */ TSGrowableArray<char*> m_values;
    };

    struct SECTION : public TSHashObject<SECTION, HASHKEY_CONSTSTRI> {
        /* 0018 */ TSHashTable<KEYVALUE, HASHKEY_CONSTSTRI> m_keys;

        ~SECTION();
    };

    struct PROFILE : public CHandleObject {
        /* 0008 */ TSHashTable<SECTION, HASHKEY_CONSTSTRI> m_sections;
        /* 0030 */ TSList<STRINGBLOCK, TSGetLink<STRINGBLOCK> > m_strings;

        virtual ~PROFILE();
    };

    char* AddValue(PROFILE* profile, const char* section, const char* key, const char* value, int32_t replace, int32_t reuse);
    int32_t IReadBuffer(PROFILE* profile, const void* data, uint32_t size);
    int32_t IReadFile(PROFILE* profile, const char* path);
    int32_t PrfStrToInt(const char* str);
    void TokenizeStringValues(PROFILE* profile, const char* section, const char* key, char* values);
}

void ProfileClose(void* profile);
void* ProfileCreate();
int32_t ProfileGetValue(void* profile, const char* section, const char* key, int32_t* value, uint32_t index);
int32_t ProfileGetValue(void* profile, const char* section, const char* key, char* buffer, uint32_t bufferSize, uint32_t index);
int32_t ProfileReadFile(void* profile, const char* path);

#endif
