#ifndef WOW_SOURCE_UI_STABLEINFO_H
#define WOW_SOURCE_UI_STABLEINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

// SMSG_STABLE_RESULT result byte, named after how Packet_SMSG_STABLE_RESULT
// handles each value (inferred names).
enum STABLE_RESULT : uint8_t {
    STABLE_RESULT_NOT_ENOUGH_MONEY    = 1,  // ERR_NOT_ENOUGH_MONEY
    STABLE_RESULT_UNKNOWN_8           = 8,  // requests the stabled pet list
    STABLE_RESULT_UNKNOWN_9           = 9,  // requests the stabled pet list
    STABLE_RESULT_SLOT_BOUGHT         = 10, // adds a slot, then requests the list
    STABLE_RESULT_UNKNOWN_11          = 11, // signals PET_STABLE_UPDATE
    STABLE_RESULT_CANT_CONTROL_EXOTIC = 12, // PET_TAME_CANT_CONTROL_EXOTIC
};

// MSG_LIST_STABLED_PETS pet state bits; a pet without STABLED is the active pet
// and is moved to m_pets[0] (inferred name).
enum STABLE_PET_STATE : uint8_t {
    STABLE_PET_STATE_STABLED = 0x2,
};

struct StablePetInfo {
    /* 0000 */ uint32_t m_petNumber;
    /* 0004 */ uint32_t m_level;
    /* 0008 */ int32_t m_entry;
    /* 000C */ char m_name[80];
    /* 005C */ STABLE_PET_STATE m_state;
    /* 005D */ uint8_t unused_005D[3];
};

class CGStableInfo {
    public:
    static void ShutdownGame();
    static StablePetInfo m_pets[6];
    static WOWGUID m_stableMaster;
    static uint32_t m_numPets;
    static int32_t m_numSlots;
    static int32_t m_selectedPet;

    static void CloseStables(bool force);
    static void EnterWorld();
    static void LeaveWorld();
    static void InitializeGame();
    static StablePetInfo* GetPet(uint32_t index);
    static void SelectPet(uint32_t index);
    static int32_t GetSelectedPetIndex();
};

void StableInfoRegisterScriptFunctions();
void StableInfoUnregisterScriptFunctions();

class CDataStore;

int32_t Packet_MSG_LIST_STABLED_PETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_STABLE_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
