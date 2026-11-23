#ifndef WOW_SOURCE_UI_PETINFO_H
#define WOW_SOURCE_UI_PETINFO_H

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

// FrameXML NUM_PET_ACTION_SLOTS.
constexpr uint32_t NUM_PET_ACTION_SLOTS = 10;

// Inferred name: grammatical cases of a declined pet name sent by PetRename.
constexpr int32_t NUM_DECLINED_NAME_CASES = 5;

class PetAction {
    public:
    // Descriptive six-bit kind IDs. 8..17 are spell-like in dispatch but their
    // individual meanings are unproved; ten action slots do not establish names.
    enum Kind : int32_t {
        SPELL = 1,
        MODE = 6,
        ORDER = 7,
        UNKNOWN_8 = 8,
        UNKNOWN_9 = 9,
        UNKNOWN_10 = 10,
        UNKNOWN_11 = 11,
        UNKNOWN_12 = 12,
        UNKNOWN_13 = 13,
        UNKNOWN_14 = 14,
        UNKNOWN_15 = 15,
        UNKNOWN_16 = 16,
        UNKNOWN_17 = 17,
    };

    // Existing full-word tag tests/constructors use this encoded representation.
    // These tags exclude ID and autocast bits; reuse the separate packing masks.
    enum PackedKind : int32_t {
        PACKED_SPELL = 0x1000000,
        PACKED_MODE = 0x06000000,
        PACKED_ORDER = 0x07000000,
    };

    // Packed ID/kind boundaries and autocast bits; kind IDs remain a separate domain.
    enum PackingMasks : uint32_t {
        ID_MASK = 0xffffff,
        KIND_MASK = 0x3f000000,
        IDENTITY_MASK = 0x3fffffff,
        ATTRIBUTES_MASK = 0xff000000,
        KIND_VALUE_MASK = 0x3f,
    };

    enum AutocastFlags : uint32_t {
        AUTOCAST_ENABLED = 0x40000000,
        AUTOCAST_ALLOWED = 0x80000000,
    };

    /* 0000 */ uint32_t m_data;

    void SetAutocast(bool enable);
};

// Pet reaction mode and command, indexing the client tokens PASSIVE/DEFENSIVE/AGGRESSIVE
// and WAIT/FOLLOW/ATTACK/DISMISS (PET_MODE_*, PET_ACTION_* and PET_*_TEXTURE strings).
enum PET_MODE : uint32_t {
    PET_MODE_PASSIVE = 0,
    PET_MODE_DEFENSIVE = 1,
    PET_MODE_AGGRESSIVE = 2,
    NUM_PET_MODES = 3,
};

enum PET_ORDER : uint32_t {
    PET_ORDER_WAIT = 0,
    PET_ORDER_FOLLOW = 1,
    PET_ORDER_ATTACK = 2,
    PET_ORDER_DISMISS = 3,
    NUM_PET_ORDERS = 4,
};

class CGPetInfo {
    public:
    // m_petMode packs the PET_MODE in the low byte and the PET_ORDER from bit 8; bit 27 hides
    // the mode and blocks pet actions unless the player rides the pet (inferred names).
    enum : uint32_t {
        PET_MODE_MASK = 0xFF,
        PET_ORDERS_SHIFT = 8,
        PET_MODE_FLAG_UNKNOWN_27 = 0x8000000,
    };

    static int32_t m_possessSpell;
    static int32_t m_attackingTarget;
    static uint32_t m_petMode;

    static void InitializeGame();
    static void ShutdownGame();
    static void LeaveWorld();
    static void FindPossessedSpell();
    static WOWGUID GetPet(uint32_t index);
    static PetAction* FindSpellByID(int32_t spellId);
    static BOOL IsPetBarUsed();
    static void UpdateCooldowns();
    static WOWGUID GetTarget();
    static void HideGrid();
    static void ShowGrid();
    static int32_t PetActionSlotUsable(const PetAction* action);
    static void PetAbandon();
    static void PetRename(const char* name, const char* declinedNames);
    static void SetAutocast(uint32_t index, BOOL enable);
    static void PlaceAction(const PetAction& action, uint32_t index);
    static void PickupAction(uint32_t index, int32_t sendToServer);
    static void PlaceCursorAction(PetAction action, uint32_t index);
    static uint8_t GetComboPoints(WOWGUID target);
    static bool PetActionsUsable(bool ignoreControl);
    static void PetClearTarget();
    static void PetDismiss();
    static void PetAttackTarget(const WOWGUID& target, BOOL useGiven);
    static void SetPetCount(uint32_t count);
    static BOOL IsAttackAction(uint32_t index);
    static uint32_t GetPetMode();
    static void SetPet(WOWGUID pet, uint32_t expiration, int32_t family);
    static void SetComboPoints(WOWGUID target, uint8_t points);
    static void SetIsAttackingTarget(int32_t attacking);
    static void UpdatePetTempBar();
    static void SendPetAction(const PetAction& action, const WOWGUID& target, BOOL useGiven);
    static PetAction* GetPetSpellAction(int32_t spellId);
    static void UpdatePetSpellAction(const PetAction* action);
    static void SetPossessedSpell(int32_t spellId);
    static int32_t SetAction(uint32_t index, const PetAction& action, int32_t sendToServer);
    static void SetSpellAutocast(int32_t spellId, BOOL enable);
    static const char* GetModeToken(uint32_t id);
    static const char* GetOrdersToken(uint32_t id);
    static uint32_t GetExpirationTime() { return m_expirationTime; } // (inferred)

    private:
    friend PetAction* GetPetAction(uint32_t index); // Access inferred from the slot lookup.
    friend int32_t Packet_SMSG_PET_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg); // Access inferred from the action table reset.
    static PetAction m_actions[NUM_PET_ACTION_SLOTS];
    static uint8_t m_comboPoints;
    static WOWGUID m_comboTarget;
    static WOWGUID m_pet;
    static uint32_t m_petBarState;
    static uint32_t m_expirationTime;
    static void SetPetMode(uint32_t mode);
    static void SetPetOrders(uint32_t orders);
};

void PetInfoRegisterScriptFunctions();
void PetInfoUnregisterScriptFunctions();
int32_t Packet_SMSG_PET_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_7(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_MODE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_ACTION_FEEDBACK(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_BROKEN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_RENAMEABLE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_UPDATE_COMBO_POINTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_GUIDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

extern TSGrowableArray<WOWGUID> s_pets;

#endif
