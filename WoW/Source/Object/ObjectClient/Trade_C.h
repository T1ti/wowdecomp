#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_TRADE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_TRADE_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CDataStore;

bool Trade_C_AddItem(WOWGUID item, WOWGUID bag, uint8_t slot, uint8_t tradeSlot);
void Trade_C_BeginTrade();
void Trade_C_CancelTrade();
void Trade_C_AcceptTrade();
void Trade_C_RemoveItem(uint8_t tradeSlot);
void Trade_C_Destroy();
void Trade_C_PlayerBusy();
void Trade_C_PlayerIgnored();
void Trade_C_UnacceptTrade();

void Trade_C_Initialize();
int32_t Trade_C_GetEnchantSpell(int32_t player);
WOWGUID Trade_C_GetTradeTarget();
int32_t Trade_C_IsTradeRequested();
int32_t Trade_C_UseCursorItem();


#endif
