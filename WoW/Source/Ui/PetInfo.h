#ifndef WOW_SOURCE_UI_PETINFO_H
#define WOW_SOURCE_UI_PETINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

class PetAction {
    public:
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

    static uint32_t GetID(uint32_t data) { return data & PackingMasks::ID_MASK; }
    static uint32_t GetKind(uint32_t data) { return (data >> 24) & PackingMasks::KIND_VALUE_MASK; }
    static uint32_t GetPackedKind(uint32_t data) { return data & PackingMasks::KIND_MASK; }
    static uint32_t GetIdentity(uint32_t data) { return data & PackingMasks::IDENTITY_MASK; }
    static uint32_t GetAttributes(uint32_t data) { return data & PackingMasks::ATTRIBUTES_MASK; }

    uint32_t GetID() const { return GetID(this->m_data); }
    uint32_t GetKind() const { return GetKind(this->m_data); }
    uint32_t GetPackedKind() const { return GetPackedKind(this->m_data); }
    uint32_t GetIdentity() const { return GetIdentity(this->m_data); }
    uint32_t GetAttributes() const { return GetAttributes(this->m_data); }
    uint32_t GetAutocastAllowedFlag() const { return this->m_data & AutocastFlags::AUTOCAST_ALLOWED; }
    uint32_t GetAutocastEnabledFlag() const { return this->m_data & AutocastFlags::AUTOCAST_ENABLED; }
    // Kind 6 is a mode; kind 7 is a command. Keep the original unsigned range test.
    bool IsModeOrCommand() const { return GetKind() - 6 <= 1; }

    void SetAutocast(bool enable);
};

class CGPetInfo {
    public:
    static uint8_t m_comboPoints;
    static int32_t m_possessSpell;
    static WOWGUID m_comboTarget;
    static int32_t m_attackingTarget;
    static PetAction m_actions[10];
    static uint32_t m_petMode;
    static uint32_t m_expirationTime;
    static WOWGUID m_pet;
    static uint32_t m_petBarState;

    static void InitializeGame();
    static void ShutdownGame();
    static void LeaveWorld();
    static void FindPossessedSpell();
    static WOWGUID GetPet(uint32_t index);
    static PetAction* FindSpellByID(int32_t spellId);
    static int32_t IsPetBarUsed();
    static void UpdateCooldowns();
    static WOWGUID GetTarget();
    static void HideGrid();
    static void ShowGrid();
    static int32_t PetActionSlotUsable(const PetAction* action);
    static void PetAbandon();
    static void PetRename(const char* name, const char* declinedNames);
    static void SetAutocast(uint32_t index, int32_t enable);
    static void PlaceAction(const PetAction& action, uint32_t index);
    static void PickupAction(uint32_t index, int32_t a2);
    static void PlaceCursorAction(PetAction action, uint32_t index);
    static uint8_t GetComboPoints(WOWGUID target);
    static bool PetActionsUsable(bool a1);
    static void PetClearTarget();
    static void PetDismiss();
    static void PetAttackTarget(const WOWGUID& target, int32_t a2);
    static void SetPetCount(uint32_t count);
    static int32_t IsAttackAction(uint32_t index);
    static uint32_t GetPetMode();
    static void SetPet(WOWGUID pet, uint32_t expiration, int32_t family);
    static void SetComboPoints(WOWGUID target, uint8_t points);
    static void SetIsAttackingTarget(int32_t attacking);
    static void UpdatePetTempBar();
    static void SendPetAction(const PetAction& action, const WOWGUID& target, int32_t a3);
    static PetAction* GetPetSpellAction(int32_t spellId);
    static void UpdatePetSpellAction(const PetAction* action);
    static void SetPossessedSpell(int32_t spellId);
    static int32_t SetAction(uint32_t index, const PetAction& action, int32_t a3);
    static void SetSpellAutocast(int32_t spellId, int32_t enable);
    static void SetPetMode(uint32_t mode);
    static void SetPetOrders(uint32_t orders);
    static const char* GetModeToken(uint32_t mode);
    static const char* GetOrdersToken(uint32_t order);
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
