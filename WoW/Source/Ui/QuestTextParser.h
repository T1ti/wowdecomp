#ifndef WOW_SOURCE_UI_QUESTTEXTPARSER_H
#define WOW_SOURCE_UI_QUESTTEXTPARSER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class CGUnit_C;
class NameCache;
class SpellItemEnchantmentRec;
class SpellRec;

struct SpellVariable {
    /* 0000 */ char m_name[16];
    /* 0010 */ char m_text[32];
    /* 0030 */ float m_value;
    /* 0034 */ int32_t m_intValue;
};

struct MathFunction {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_argCount;
};

extern TSGrowableArray<SpellVariable> s_spellVariables;

void ParseCountDownTimerText(char* buffer, uint32_t size, int32_t time);
bool QuestParserParseToken(char* buffer, uint32_t size, const NameCache* record, const char* name, int32_t achievement, WOWGUID guid, const CGUnit_C* unit, const char** cursor);
void GenderConditional(char* buffer, uint32_t size, int32_t gender, const CGUnit_C* unit, const NameCache* name, const char** cursor);
int32_t SpellParserParseText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, BOOL useAuraDescription, int32_t scale, int32_t noModifiers);
int32_t SpellItemEnchantmentParseText(const SpellItemEnchantmentRec* enchant, char* buffer, uint32_t size, int32_t value);

#endif
