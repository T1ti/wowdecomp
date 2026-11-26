#ifndef WOW_SOURCE_GAME_GAMECLIENT_TABLES_H
#define WOW_SOURCE_GAME_GAMECLIENT_TABLES_H

#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/GameTablesRec.h"
#include "WoW/Source/DB/WowClientDB.h"

// Indices in tables.cpp's client game-table registry.
enum GAME_TABLE : int32_t {
    GAME_TABLE_BARBER_SHOP_COST_BASE = 0,
    GAME_TABLE_COMBAT_RATINGS = 1,
    GAME_TABLE_CHANCE_TO_MELEE_CRIT = 2,
    GAME_TABLE_CHANCE_TO_MELEE_CRIT_BASE = 3,
    GAME_TABLE_CHANCE_TO_SPELL_CRIT = 4,
    GAME_TABLE_CHANCE_TO_SPELL_CRIT_BASE = 5,
    GAME_TABLE_NPC_MANA_COST_SCALER = 6,
    GAME_TABLE_OCT_REGEN_HP = 7,
    GAME_TABLE_OCT_REGEN_MP = 8,
    GAME_TABLE_REGEN_HP_PER_SPIRIT = 9,
    GAME_TABLE_REGEN_MP_PER_SPIRIT = 10,
    GAME_TABLE_OCT_CLASS_COMBAT_RATING_SCALAR = 11,
    NUM_GAMETABLES = 12,
};

float GetGameTableValue(GAME_TABLE tableEnum, int32_t rowIndex, int32_t columnIndex);

class ClientGameTableBase {
    public:
    virtual float EvaluateTable(uint32_t rowIndex, uint32_t columnIndex) = 0;

    ClientGameTableBase(const char* name);
    void Initialize();

    private:
    /* 0004 */ const char* m_name;

    protected:
    /* 0008 */ const GameTablesRec* m_info;
};

template <class T>
class ClientGameTable : public ClientGameTableBase {
    public:
    float EvaluateTable(uint32_t rowIndex, uint32_t columnIndex) override;

    ClientGameTable(const char* name, WowClientDB<T>* db);

    private:
    /* 000C */ WowClientDB<T>* m_db;
};

// INLINED
template <class T>
ClientGameTable<T>::ClientGameTable(const char* name, WowClientDB<T>* db) : ClientGameTableBase(name) {
    this->m_db = db;
}

// VERIFY: 2e38a015 auto
// OFFSET: [0x007F69B0, 0x007F69E0]
template <class T>
float ClientGameTable<T>::EvaluateTable(uint32_t rowIndex, uint32_t columnIndex) {
    STORM_ASSERT(rowIndex < static_cast<uint32_t>(m_info->m_numRows)); // 12213: "rowIndex < (uint)m_info->m_numRows"
    STORM_ASSERT(columnIndex < static_cast<uint32_t>(m_info->m_numColumns)); // 12213: "columnIndex < (uint)m_info->m_numColumns"
    const T* rec = this->m_db->GetRecord(rowIndex + columnIndex * this->m_info->m_numRows);
    STORM_ASSERT(rec);
    return rec->m_data;
}

#endif
