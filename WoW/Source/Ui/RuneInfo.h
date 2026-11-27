#ifndef WOW_SOURCE_UI_RUNEINFO_H
#define WOW_SOURCE_UI_RUNEINFO_H

#include <cstdint>

// Active rune count on initialization; the backing arrays have eight entries.
constexpr int32_t NUM_INITIAL_RUNES = 6;
constexpr int32_t RUNE_BASE_COOLDOWN_MS = 10000;
constexpr float RUNE_BASE_COOLDOWN_MS_FLOAT = 10000.0f;

class CDataStore;
class SpellRec;

class CGRuneInfo {
    public:
    // Rune types; FrameXML RUNETYPE_* are these plus one. SpellRuneCost lists blood, unholy, frost.
    enum RUNE_TYPE : int32_t {
        RUNE_TYPE_BLOOD = 0,
        RUNE_TYPE_UNHOLY = 1,
        RUNE_TYPE_FROST = 2,
        RUNE_TYPE_DEATH = 3,
        NUM_RUNE_TYPES = 4,
    };

    // Rune slot table size (inferred name); NUM_INITIAL_RUNES are in use.
    static const uint32_t MAX_RUNE_SLOTS = 8;

    static void LeaveWorld();
    static void ConvertRune(uint8_t rune, uint8_t type);
    static void ResyncRunes(CDataStore* msg);
    static void AddRunePower(CDataStore* msg);
    static void SetPredictedRunes(uint8_t available, uint8_t used, const uint8_t* cooldowns);
    static void InitializeGame();
    static void ShutdownGame();
    static bool HasRunesForSpell(int32_t spellId, int32_t* needed);
    static uint32_t GetRuneRechargeTime(int32_t rune);
    static bool ComputeRuneSpellCooldown(const SpellRec* spell, uint32_t* cooldown);
    static bool IsRuneIndexValid(int32_t index);
    static int32_t GetRuneTypeByIndex(int32_t index, bool base);
    static uint32_t GetRuneLastUsedTime(int32_t i);
    static void SetRuneLastUsedTime(int32_t i, uint32_t time);
    static void ComputeRuneRechargeTime(int32_t rune, float progress);
    static uint32_t GetPredictedRunes() { return m_predictedRunes; } // (inferred)

    private:
    static int32_t m_runes[MAX_RUNE_SLOTS];
    static int32_t m_baseRunes[MAX_RUNE_SLOTS];
    static uint32_t m_runeCooldowns[MAX_RUNE_SLOTS];
    static uint32_t m_runeLastUsed[MAX_RUNE_SLOTS];
    static uint32_t m_numRunes;
    static uint32_t m_predictedRunes;
    static int32_t CountPredictedRunesOfType(int32_t type);
    static void SetRuneRechargeTime(int32_t rune, uint32_t time); // (inferred)
};

#endif
