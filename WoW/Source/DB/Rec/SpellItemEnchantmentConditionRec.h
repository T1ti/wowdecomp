#ifndef WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H
#define WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTCONDITIONREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class SpellItemEnchantmentConditionRec {
    public:
    static const int32_t s_numColumns = 31;
    static const int32_t s_rowSize = 64;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_ltOperandType [5];
    /* 000C */ int32_t m_ltOperand [5];
    /* 0020 */ int8_t m_operator [5];
    /* 0025 */ int8_t m_rtOperandType [5];
    /* 002C */ int32_t m_rtOperand [5];
    /* 0040 */ int8_t m_logic [5];

    SpellItemEnchantmentConditionRec();
    ~SpellItemEnchantmentConditionRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);

    // The packed file stores five signed bytes; these constants name their
    // comparison meaning without widening the raw m_operator array.
    enum COMPARISON_OPERATOR : int32_t {
        OPERATOR_EQUAL = 0,
        OPERATOR_NOT_EQUAL = 1,
        OPERATOR_LESS_THAN = 2,
        OPERATOR_GREATER_THAN = 3,
        OPERATOR_LESS_OR_EQUAL = 4,
        OPERATOR_GREATER_OR_EQUAL = 5,
    };
};

#endif
