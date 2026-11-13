#include "PlayerName.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: dc49a19d manual
// OFFSET: 0x007E60E0
bool ObjectNameShowCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t mask = s_nameMask;

    if (SStrToInt(value)) {
        s_nameMask |= static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
    } else {
        s_nameMask &= ~static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
    }

    if (s_nameMask == mask) {
        return true;
    }

    for (PLAYERNAMEDESC* desc = s_playerNames.Head(); desc; desc = s_playerNames.Next(desc)) {
        desc->m_flags |= 1;
    }

    return true;
}

// VERIFY: b96c6600 auto
// OFFSET: 0x007E6150
void PlayerNameRegisterCVars() {
    CVar::Register("UnitNameOwn", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1), false);
    CVar::Register("UnitNameNPC", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2), false);
    CVar::Register("UnitNamePlayerGuild", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x4), false);
    CVar::Register("UnitNamePlayerPVPTitle", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x8), false);
    CVar::Register("UnitNameEnemyPlayerName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x10), false);
    CVar::Register("UnitNameEnemyPetName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x20), false);
    CVar::Register("UnitNameEnemyGuardianName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1000), false);
    CVar::Register("UnitNameEnemyTotemName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x40), false);
    CVar::Register("UnitNameFriendlyPlayerName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x80), false);
    CVar::Register("UnitNameFriendlyPetName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x100), false);
    CVar::Register("UnitNameFriendlyGuardianName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2000), false);
    CVar::Register("UnitNameFriendlyTotemName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x200), false);
    CVar::Register("UnitNameNonCombatCreatureName", nullptr, CVAR_ARCHIVE_SCOPE::CVAR_SCOPE_ACCOUNT, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x400), false);
}
