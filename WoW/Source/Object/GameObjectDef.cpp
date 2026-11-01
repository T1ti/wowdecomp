#include "GameObjectDef.h"

// The final word differs from the old valueBaseType mapping; retain its raw value.
struct PropertyInfo {
    CGameObjectDef::PROPERTY_ID propId;
    const char* name;
    CGameObjectDef::VALUE_TYPE valueType;
    int32_t unknown0C;
};

// STATIC: 0x00A35A60
static const PropertyInfo s_propertyInfo[CGameObjectDef::PROPERTY_ID::NUM_PROP] = {
    { CGameObjectDef::PROPERTY_ID::PROP_TYPE, "type", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_TYPE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN, "startOpen", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STARTDESTROYED, "startDestroyed", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE, "autoClose", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LOCK, "open", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_LOCK, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_QUESTLISTID, "questList", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_QUESTGIVER, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CHESTLOOT, "chestLoot", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_LOOTTABLE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CHESTLOOTTIME, "chestRestockTime", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CONSUMABLE, "consumable", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CHARGES, "charges", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_SPELL, "spell", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CHAIRSLOTS, "chairslots", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CHAIRHEIGHT, "chairheight", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_TRIGGEREDEVENT, "triggeredEvent", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_SPELLFOCUSTYPE, "spellFocusType", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_TEXTID, "pageID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_PAGETEXT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_TEXTLANGUAGE, "language", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_TEXTMATERIAL, "pageMaterial", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_PAGEMATERIAL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_HIGHLIGHT, "highlight", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_FLOATINGTOOLTIP, "floatingTooltip", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_QUESTID, "questID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_QUEST, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_EVENTID, "eventID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM, "customAnim", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_COOLDOWN, "cooldown", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_RADIUS, "radius", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MINRESTOCK, "minRestock", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MAXRESTOCK, "maxRestock", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_MIN, "damageMin", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_MAX, "damageMax", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_SCHOOL, "damageSchool", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LINKED_TRAP, "linkedTrap", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_TRAP, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LEVEL, "level", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STARTDELAY, "startDelay", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CAMERAID, "camera", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_CAMERA, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CASTERS, "casters", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_TAXIPATHID, "taxiPathID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MOVESPEED, "moveSpeed", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_SERVERONLY, "serverOnly", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_GOSSIPID, "gossipID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_GOSSIP, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_ANIMSPELL, "animSpell", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_RITUALPERSISTENT, "ritualPersistent", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_ACTIONHOUSEID, "actionHouseID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_AUCTIONHOUSE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CREATUREID, "creatureID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_CREATURE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_ACCELRATE, "accelRate", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STARTEVENTID, "startEventID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STOPEVENTID, "stopEventID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_RITUALNOTARGETCHECK, "ritualNoTargetCheck", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_PARTYONLY, "partyOnly", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LARGE, "large", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CASTERTARGETSPELL, "casterTargetSpell", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CASTERTARGETSPELLTARGETS, "casterTargetSpellTargets", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MINLEVEL, "minLevel", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MAXLEVEL, "maxLevel", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_AREAID, "areaID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_AREA, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CASTERSGROUPED, "castersGrouped", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LOSOK, "losOK", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LEAVELOOT, "leaveLoot", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE, "noDamageImmune", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STEALTHAFFECTED, "stealthAffected", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_PICKUPSPELL, "pickupSpell", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_RETURNAURA, "returnAura", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_RETURNSPELL, "returnSpell", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_NOTINCOMBAT, "notInCombat", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_FLOATONWATER, "floatOnWater", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_GAMETYPE, "gameType", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STEALTHED, "stealthed", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_LOG_LOOT, "log loot", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE1, "worldState1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_WORLDSTATE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE2, "worldstate2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_WORLDSTATE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE3, "worldstate3", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_WORLDSTATE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WINEVENTID1, "winEventID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WINEVENTID2, "winEventID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CONTESTEDEVENTID1, "contestedEventID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CONTESTEDEVENTID2, "contestedEventID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_PROGRESSEVENTID1, "progressEventID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_PROGRESSEVENTID2, "progressEventID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_NEUTRALEVENTID1, "neutralEventID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_NEUTRALEVENTID2, "neutralEventID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_NEUTRALPERCENT, "neutralPercent", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MINSUPERIORITY, "minSuperiority", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MAXSUPERIORITY, "maxSuperiority", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MINTIME, "minTime", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MAXTIME, "maxTime", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_AURAID1, "auraID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_AURAID2, "auraID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_SPELL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1, "conditionID1", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_CONDITION, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID2, "conditionID2", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_CONDITION, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_MAPID, "mapID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_MAP, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DIFFICULTY, "difficulty", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID, "openTextID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_TEXT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID, "closeTextID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_TEXT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_ALLOWMOUNTED, "allowMounted", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_USE_GROUP_LOOT_RULES, "use group loot rules", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WHEN_TO_PAUSE_MS, "when to pause (ms)", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_ONLYCREATORUSE, "onlyCreatorUse", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_INTACT_NUM_HITS, "Intact: num hits", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGED_NUM_HITS, "Damaged: num hits", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_CREDIT_PROXY_CREATURE, "Credit Proxy Creature", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_CREATURE, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_98, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_TRANSPORTPHYSICS, "transportPhysics", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_TRANSPORTPHYSICS, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_100, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_101, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_102, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_104, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_105, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_106, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_107, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_108, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_INTACT_EVENT, "Intact Event", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGED_EVENT, "Damaged Event", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DESTROYED_EVENT, "Destroyed Event", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_REBUILDING_TIME_SECS, "Rebuilding: Time (secs)", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_113, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_REBUILDING_EVENT, "Rebuilding: Event", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_115, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_116, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_IGNORED_BY_PATHING, "Ignored By Pathing", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_STARTINGVALUE, "startingValue", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNIDIRECTIONAL, "unidirectional", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_EVENT, "Damage Event", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_HEIGHT_OFFSET_INCHES, "Height Offset (inches)", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_INT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_UNUSED_122, "", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_IGNORE_TOTEMS, "Ignore Totems", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_DESTRUCTIBLE_DATA, "Destructible Data", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_DESTRUCTIBLEMODELDATA, 1 },
    { CGameObjectDef::PROPERTY_ID::PROP_PAUSE1EVENTID, "pause1EventID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_PAUSE2EVENTID, "pause2EventID", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_EVENT, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATESETSSTATE, "WorldStateSetsState", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
    { CGameObjectDef::PROPERTY_ID::PROP_ALLOW_STOPPING, "allow stopping", CGameObjectDef::VALUE_TYPE::VALUE_TYPE_BOOL, 0 },
};

// STATIC: 0x00ADAD30
static const CGameObjectDef::PROPERTY_ID s_doorPropertiesList[CGameObjectDef::NUM_DOOR_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN,
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_IGNORED_BY_PATHING,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAD70
static const CGameObjectDef::PROPERTY_ID s_buttonPropertiesList[CGameObjectDef::NUM_BUTTON_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN,
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_LINKED_TRAP,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_LOSOK,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADADC0
static const CGameObjectDef::PROPERTY_ID s_questGiverPropertiesList[CGameObjectDef::NUM_QUESTGIVER_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_QUESTLISTID,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTMATERIAL,
    CGameObjectDef::PROPERTY_ID::PROP_GOSSIPID,
    CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_LOSOK,
    CGameObjectDef::PROPERTY_ID::PROP_ALLOWMOUNTED,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAE18
static const CGameObjectDef::PROPERTY_ID s_chestPropertiesList[CGameObjectDef::NUM_CHEST_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_CHESTLOOT,
    CGameObjectDef::PROPERTY_ID::PROP_CHESTLOOTTIME,
    CGameObjectDef::PROPERTY_ID::PROP_CONSUMABLE,
    CGameObjectDef::PROPERTY_ID::PROP_MINRESTOCK,
    CGameObjectDef::PROPERTY_ID::PROP_MAXRESTOCK,
    CGameObjectDef::PROPERTY_ID::PROP_TRIGGEREDEVENT,
    CGameObjectDef::PROPERTY_ID::PROP_LINKED_TRAP,
    CGameObjectDef::PROPERTY_ID::PROP_QUESTID,
    CGameObjectDef::PROPERTY_ID::PROP_LEVEL,
    CGameObjectDef::PROPERTY_ID::PROP_LOSOK,
    CGameObjectDef::PROPERTY_ID::PROP_LEAVELOOT,
    CGameObjectDef::PROPERTY_ID::PROP_NOTINCOMBAT,
    CGameObjectDef::PROPERTY_ID::PROP_LOG_LOOT,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_USE_GROUP_LOOT_RULES,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATINGTOOLTIP,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB06C
static const CGameObjectDef::PROPERTY_ID s_genericPropertiesList[CGameObjectDef::NUM_GENERIC_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_FLOATINGTOOLTIP,
    CGameObjectDef::PROPERTY_ID::PROP_HIGHLIGHT,
    CGameObjectDef::PROPERTY_ID::PROP_SERVERONLY,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATONWATER,
    CGameObjectDef::PROPERTY_ID::PROP_QUESTID,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAEE0
static const CGameObjectDef::PROPERTY_ID s_trapPropertiesList[CGameObjectDef::NUM_TRAP_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_LEVEL,
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_SPELL,
    CGameObjectDef::PROPERTY_ID::PROP_CHARGES,
    CGameObjectDef::PROPERTY_ID::PROP_COOLDOWN,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_STARTDELAY,
    CGameObjectDef::PROPERTY_ID::PROP_SERVERONLY,
    CGameObjectDef::PROPERTY_ID::PROP_STEALTHED,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_STEALTHAFFECTED,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_IGNORE_TOTEMS,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAEA8
static const CGameObjectDef::PROPERTY_ID s_chairPropertiesList[CGameObjectDef::NUM_CHAIR_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CHAIRSLOTS,
    CGameObjectDef::PROPERTY_ID::PROP_CHAIRHEIGHT,
    CGameObjectDef::PROPERTY_ID::PROP_ONLYCREATORUSE,
    CGameObjectDef::PROPERTY_ID::PROP_TRIGGEREDEVENT,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAFA0
static const CGameObjectDef::PROPERTY_ID s_spellFocusPropertiesList[CGameObjectDef::NUM_SPELLFOCUS_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_SPELLFOCUSTYPE,
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_LINKED_TRAP,
    CGameObjectDef::PROPERTY_ID::PROP_SERVERONLY,
    CGameObjectDef::PROPERTY_ID::PROP_QUESTID,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATINGTOOLTIP,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATONWATER,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADAFE8
static const CGameObjectDef::PROPERTY_ID s_textPropertiesList[CGameObjectDef::NUM_TEXT_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_TEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTLANGUAGE,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTMATERIAL,
    CGameObjectDef::PROPERTY_ID::PROP_ALLOWMOUNTED,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB010
static const CGameObjectDef::PROPERTY_ID s_gooberPropertiesList[CGameObjectDef::NUM_GOOBER_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_QUESTID,
    CGameObjectDef::PROPERTY_ID::PROP_EVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_CUSTOMANIM,
    CGameObjectDef::PROPERTY_ID::PROP_CONSUMABLE,
    CGameObjectDef::PROPERTY_ID::PROP_COOLDOWN,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTLANGUAGE,
    CGameObjectDef::PROPERTY_ID::PROP_TEXTMATERIAL,
    CGameObjectDef::PROPERTY_ID::PROP_SPELL,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_LINKED_TRAP,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_LOSOK,
    CGameObjectDef::PROPERTY_ID::PROP_ALLOWMOUNTED,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATINGTOOLTIP,
    CGameObjectDef::PROPERTY_ID::PROP_GOSSIPID,
    CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATESETSSTATE,
    CGameObjectDef::PROPERTY_ID::PROP_FLOATONWATER,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB170
static const CGameObjectDef::PROPERTY_ID s_transportPropertiesList[CGameObjectDef::NUM_TRANSPORT_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_WHEN_TO_PAUSE_MS,
    CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_PAUSE1EVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_PAUSE2EVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_MAPID,
};
// STATIC: 0x00ADAF60
static const CGameObjectDef::PROPERTY_ID s_areaDamagePropertiesList[CGameObjectDef::NUM_AREADAMAGE_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_MIN,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_MAX,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_SCHOOL,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CLOSETEXTID,
};
// STATIC: 0x00ADB100
static const CGameObjectDef::PROPERTY_ID s_cameraPropertiesList[CGameObjectDef::NUM_CAMERA_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_CAMERAID,
    CGameObjectDef::PROPERTY_ID::PROP_EVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB128
static const CGameObjectDef::PROPERTY_ID s_mapObjTransPropertiesList[CGameObjectDef::NUM_MOTRANSPORT_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_TAXIPATHID,
    CGameObjectDef::PROPERTY_ID::PROP_MOVESPEED,
    CGameObjectDef::PROPERTY_ID::PROP_ACCELRATE,
    CGameObjectDef::PROPERTY_ID::PROP_STARTEVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_STOPEVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_TRANSPORTPHYSICS,
    CGameObjectDef::PROPERTY_ID::PROP_MAPID,
    CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE1,
    CGameObjectDef::PROPERTY_ID::PROP_ALLOW_STOPPING,
};
// STATIC: 0x00ADB1A0
static const CGameObjectDef::PROPERTY_ID s_ritualPropertiesList[CGameObjectDef::NUM_RITUAL_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CASTERS,
    CGameObjectDef::PROPERTY_ID::PROP_SPELL,
    CGameObjectDef::PROPERTY_ID::PROP_ANIMSPELL,
    CGameObjectDef::PROPERTY_ID::PROP_RITUALPERSISTENT,
    CGameObjectDef::PROPERTY_ID::PROP_CASTERTARGETSPELL,
    CGameObjectDef::PROPERTY_ID::PROP_CASTERTARGETSPELLTARGETS,
    CGameObjectDef::PROPERTY_ID::PROP_CASTERSGROUPED,
    CGameObjectDef::PROPERTY_ID::PROP_RITUALNOTARGETCHECK,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB1E8
static const CGameObjectDef::PROPERTY_ID s_mailboxPropertiesList[CGameObjectDef::NUM_MAILBOX_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB1F0
static const CGameObjectDef::PROPERTY_ID s_guardPostPropertiesList[CGameObjectDef::NUM_GUARDPOST_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CREATUREID,
    CGameObjectDef::PROPERTY_ID::PROP_CHARGES,
};
// STATIC: 0x00ADB204
static const CGameObjectDef::PROPERTY_ID s_spellCasterPropertiesList[CGameObjectDef::NUM_SPELLCASTER_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_SPELL,
    CGameObjectDef::PROPERTY_ID::PROP_CHARGES,
    CGameObjectDef::PROPERTY_ID::PROP_PARTYONLY,
    CGameObjectDef::PROPERTY_ID::PROP_ALLOWMOUNTED,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB234
static const CGameObjectDef::PROPERTY_ID s_meetingStonePropertiesList[CGameObjectDef::NUM_MEETINGSTONE_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_MINLEVEL,
    CGameObjectDef::PROPERTY_ID::PROP_MAXLEVEL,
    CGameObjectDef::PROPERTY_ID::PROP_AREAID,
};
// STATIC: 0x00ADB24C
static const CGameObjectDef::PROPERTY_ID s_flagStandPropertiesList[CGameObjectDef::NUM_FLAGSTAND_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_PICKUPSPELL,
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_RETURNAURA,
    CGameObjectDef::PROPERTY_ID::PROP_RETURNSPELL,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
    CGameObjectDef::PROPERTY_ID::PROP_LOSOK,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB2BC
static const CGameObjectDef::PROPERTY_ID s_fishingHolePropertiesList[CGameObjectDef::NUM_FISHINGHOLE_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_CHESTLOOT,
    CGameObjectDef::PROPERTY_ID::PROP_MINRESTOCK,
    CGameObjectDef::PROPERTY_ID::PROP_MAXRESTOCK,
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
};
// STATIC: 0x00ADB294
static const CGameObjectDef::PROPERTY_ID s_flagDropPropertiesList[CGameObjectDef::NUM_FLAGDROP_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_LOCK,
    CGameObjectDef::PROPERTY_ID::PROP_EVENTID,
    CGameObjectDef::PROPERTY_ID::PROP_PICKUPSPELL,
    CGameObjectDef::PROPERTY_ID::PROP_NODAMAGEIMMUNE,
    CGameObjectDef::PROPERTY_ID::PROP_OPENTEXTID,
};
// STATIC: 0x00ADB2E4
static const CGameObjectDef::PROPERTY_ID s_minigamePropertiesList[CGameObjectDef::NUM_MINIGAME_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_GAMETYPE,
};
// STATIC: 0x00ADB308
static const CGameObjectDef::PROPERTY_ID s_capturePointPropertiesList[CGameObjectDef::NUM_CAPTUREPOINT_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_SPELL,
    CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE1,
    CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE2,
    CGameObjectDef::PROPERTY_ID::PROP_WINEVENTID1,
    CGameObjectDef::PROPERTY_ID::PROP_WINEVENTID2,
    CGameObjectDef::PROPERTY_ID::PROP_CONTESTEDEVENTID1,
    CGameObjectDef::PROPERTY_ID::PROP_CONTESTEDEVENTID2,
    CGameObjectDef::PROPERTY_ID::PROP_PROGRESSEVENTID1,
    CGameObjectDef::PROPERTY_ID::PROP_PROGRESSEVENTID2,
    CGameObjectDef::PROPERTY_ID::PROP_NEUTRALEVENTID1,
    CGameObjectDef::PROPERTY_ID::PROP_NEUTRALEVENTID2,
    CGameObjectDef::PROPERTY_ID::PROP_NEUTRALPERCENT,
    CGameObjectDef::PROPERTY_ID::PROP_WORLDSTATE3,
    CGameObjectDef::PROPERTY_ID::PROP_MINSUPERIORITY,
    CGameObjectDef::PROPERTY_ID::PROP_MAXSUPERIORITY,
    CGameObjectDef::PROPERTY_ID::PROP_MINTIME,
    CGameObjectDef::PROPERTY_ID::PROP_MAXTIME,
    CGameObjectDef::PROPERTY_ID::PROP_LARGE,
    CGameObjectDef::PROPERTY_ID::PROP_HIGHLIGHT,
    CGameObjectDef::PROPERTY_ID::PROP_STARTINGVALUE,
    CGameObjectDef::PROPERTY_ID::PROP_UNIDIRECTIONAL,
};
// STATIC: 0x00ADB2EC
static const CGameObjectDef::PROPERTY_ID s_auraGeneratorPropertiesList[CGameObjectDef::NUM_AURAGENERATOR_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN,
    CGameObjectDef::PROPERTY_ID::PROP_RADIUS,
    CGameObjectDef::PROPERTY_ID::PROP_AURAID1,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
    CGameObjectDef::PROPERTY_ID::PROP_AURAID2,
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID2,
    CGameObjectDef::PROPERTY_ID::PROP_SERVERONLY,
};
// STATIC: 0x00ADB3D4
static const CGameObjectDef::PROPERTY_ID s_dungeonDifficultyPropertiesList[CGameObjectDef::NUM_DUNGEONDIFFICULTY_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_MAPID,
    CGameObjectDef::PROPERTY_ID::PROP_DIFFICULTY,
};
// STATIC: 0x00ADAED0
static const CGameObjectDef::PROPERTY_ID s_barberChairPropertiesList[CGameObjectDef::NUM_BARBERCHAIR_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CHAIRHEIGHT,
    CGameObjectDef::PROPERTY_ID::PROP_HEIGHT_OFFSET_INCHES,
};
// STATIC: 0x00ADB3E8
static const CGameObjectDef::PROPERTY_ID s_destructibleBuildingPropertiesList[CGameObjectDef::NUM_DESTRUCTIBLEBUILDING_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_INTACT_NUM_HITS,
    CGameObjectDef::PROPERTY_ID::PROP_CREDIT_PROXY_CREATURE,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_INTACT_EVENT,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGED_NUM_HITS,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGED_EVENT,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_DESTROYED_EVENT,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_REBUILDING_TIME_SECS,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_DESTRUCTIBLE_DATA,
    CGameObjectDef::PROPERTY_ID::PROP_REBUILDING_EVENT,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
    CGameObjectDef::PROPERTY_ID::PROP_DAMAGE_EVENT,
    CGameObjectDef::PROPERTY_ID::PROP_UNKNOWN_103,
};
// STATIC: 0x00ADB3E4
static const CGameObjectDef::PROPERTY_ID s_guildBankPropertiesList[CGameObjectDef::NUM_GUILD_BANK_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_CONDITIONID1,
};
// STATIC: 0x00ADB4AC
static const CGameObjectDef::PROPERTY_ID s_trapdoorPropertiesList[CGameObjectDef::NUM_TRAPDOOR_PROPERTIES] = {
    CGameObjectDef::PROPERTY_ID::PROP_WHEN_TO_PAUSE_MS,
    CGameObjectDef::PROPERTY_ID::PROP_STARTOPEN,
    CGameObjectDef::PROPERTY_ID::PROP_AUTOCLOSE,
};

// STATIC: 0x00ADAD28
static const char* s_boolEnumValues[CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES] = { "false", "true" };

// STATIC: 0x00A36270
static const CGameObjectDef::EnumValue s_doorStartOpenValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3627C
static const CGameObjectDef::NumberValue s_doorLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A3628C
static const CGameObjectDef::NumberValue s_doorAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 3.0f };
// STATIC: 0x00A3629C
static const CGameObjectDef::EnumValue s_doorNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A362A8
static const CGameObjectDef::NumberValue s_doorOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362B8
static const CGameObjectDef::NumberValue s_doorCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362C8
static const CGameObjectDef::EnumValue s_doorIgnoredByPathingValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A362D4
static const CGameObjectDef::NumberValue s_doorConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362E4
static const CGameObjectDef::EnumValue s_buttonStartOpenValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A362F0
static const CGameObjectDef::NumberValue s_buttonLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36300
static const CGameObjectDef::NumberValue s_buttonAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 3.0f };
// STATIC: 0x00A36310
static const CGameObjectDef::NumberValue s_buttonLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36320
static const CGameObjectDef::EnumValue s_buttonNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3632C
static const CGameObjectDef::EnumValue s_buttonLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36338
static const CGameObjectDef::NumberValue s_buttonOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36348
static const CGameObjectDef::NumberValue s_buttonCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36358
static const CGameObjectDef::EnumValue s_buttonLosOKValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36364
static const CGameObjectDef::NumberValue s_buttonConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36374
static const CGameObjectDef::NumberValue s_questGiverLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36384
static const CGameObjectDef::NumberValue s_questGiverQuestListIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36394
static const CGameObjectDef::NumberValue s_questGiverTextMaterialValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A363A4
static const CGameObjectDef::NumberValue s_questGiverGossipIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A363B4
static const CGameObjectDef::NumberValue s_questGiverCustomAnimValue = { 0.0f, 4.0f, 1.0f, 0.0f };
// STATIC: 0x00A363C4
static const CGameObjectDef::EnumValue s_questGiverNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A363D0
static const CGameObjectDef::NumberValue s_questGiverOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A363E0
static const CGameObjectDef::EnumValue s_questGiverLosOKValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A363EC
static const CGameObjectDef::EnumValue s_questGiverAllowMountedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A363F8
static const CGameObjectDef::EnumValue s_questGiverLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36404
static const CGameObjectDef::NumberValue s_questGiverConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36414
static const CGameObjectDef::NumberValue s_chestLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36424
static const CGameObjectDef::NumberValue s_chestLootValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36434
static const CGameObjectDef::NumberValue s_chestLootTimeValue = { 0.0f, 1800000.0f, 1.0f, 30.0f };
// STATIC: 0x00A36444
static const CGameObjectDef::EnumValue s_chestConsumableValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36450
static const CGameObjectDef::NumberValue s_chestMinRestockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36460
static const CGameObjectDef::NumberValue s_chestMaxRestockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36470
static const CGameObjectDef::NumberValue s_chestLootEventValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36480
static const CGameObjectDef::NumberValue s_chestLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36490
static const CGameObjectDef::NumberValue s_chestQuestIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A364A0
static const CGameObjectDef::NumberValue s_chestLevelValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A364B0
static const CGameObjectDef::EnumValue s_chestLosOKValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A364BC
static const CGameObjectDef::EnumValue s_chestLeaveLootValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A364C8
static const CGameObjectDef::EnumValue s_chestNotInCombatValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A364D4
static const CGameObjectDef::EnumValue s_chestLogLootValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A364E0
static const CGameObjectDef::NumberValue s_chestOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A364F0
static const CGameObjectDef::EnumValue s_chestUseGroupLootRulesValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A364FC
static const CGameObjectDef::EnumValue s_chestFloatingTooltipValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36508
static const CGameObjectDef::NumberValue s_chestConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36518
static const CGameObjectDef::NumberValue s_chairHeightValue = { 0.0f, 2.0f, 1.0f, 1.0f };
// STATIC: 0x00A36528
static const CGameObjectDef::NumberValue s_chairSlotValue = { 1.0f, 5.0f, 1.0f, 1.0f };
// STATIC: 0x00A36538
static const CGameObjectDef::EnumValue s_chairOnlyCreatorUseValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36544
static const CGameObjectDef::NumberValue s_barberChairHeightOffsetInchesValue = { -100.0f, 100.0f, 1.0f, 0.0f };
// STATIC: 0x00A36554
static const CGameObjectDef::NumberValue s_chairTriggeredEventValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36564
static const CGameObjectDef::NumberValue s_chairConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36574
static const CGameObjectDef::NumberValue s_trapLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36584
static const CGameObjectDef::NumberValue s_trapLevelValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36594
static const CGameObjectDef::NumberValue s_trapRadiusValue = { 0.0f, 100.0f, 1.0f, 0.0f };
// STATIC: 0x00A365A4
static const CGameObjectDef::NumberValue s_trapSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A365B4
static const CGameObjectDef::NumberValue s_trapChargesValue = { 0.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A365C4
static const CGameObjectDef::NumberValue s_trapCooldownValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A365D4
static const CGameObjectDef::NumberValue s_trapAutoCloseValue = { -1.0f, 4294967296.0f, 1.0f, 0.0f };
// STATIC: 0x00A365E4
static const CGameObjectDef::NumberValue s_trapStartDelayValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A365F4
static const CGameObjectDef::EnumValue s_trapServerOnlyValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36600
static const CGameObjectDef::EnumValue s_trapStealthedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3660C
static const CGameObjectDef::EnumValue s_trapLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36618
static const CGameObjectDef::EnumValue s_trapStealthAffectedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36624
static const CGameObjectDef::NumberValue s_trapOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36634
static const CGameObjectDef::NumberValue s_trapCloseTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36644
static const CGameObjectDef::EnumValue s_trapIgnoreTotemsValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36650
static const CGameObjectDef::NumberValue s_trapConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36660
static const CGameObjectDef::NumberValue s_areaDamageLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36670
static const CGameObjectDef::NumberValue s_areaDamageRadiusValue = { 0.0f, 50.0f, 1.0f, 3.0f };
// STATIC: 0x00A36680
static const CGameObjectDef::NumberValue s_areaDamageMin = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36690
static const CGameObjectDef::NumberValue s_areaDamageMax = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A366A0
static const CGameObjectDef::NumberValue s_areaDamageSchool = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A366B0
static const CGameObjectDef::NumberValue s_areaDamageAutoCloseValue = { 0.0f, 4294967296.0f, 1.0f, 0.0f };
// STATIC: 0x00A366C0
static const CGameObjectDef::NumberValue s_areaDamageOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A366D0
static const CGameObjectDef::NumberValue s_areaDamageCloseTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A366E0
static const CGameObjectDef::NumberValue s_spellFocusTypeValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A366F0
static const CGameObjectDef::NumberValue s_spellFocusRadiusValue = { 0.0f, 50.0f, 1.0f, 10.0f };
// STATIC: 0x00A36700
static const CGameObjectDef::NumberValue s_spellFocusLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36710
static const CGameObjectDef::EnumValue s_spellFocusServerOnlyValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3671C
static const CGameObjectDef::NumberValue s_spellFocusQuestIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A3672C
static const CGameObjectDef::EnumValue s_spellFocusLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36738
static const CGameObjectDef::EnumValue s_spellFocusFloatingTooltipValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36744
static const CGameObjectDef::EnumValue s_spellFocusFloatOnWaterValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36750
static const CGameObjectDef::NumberValue s_spellFocusConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36760
static const CGameObjectDef::NumberValue s_textTypeValue = { 0.0f, 10000.0f, 1.0f, 0.0f };
// STATIC: 0x00A36770
static const CGameObjectDef::NumberValue s_languageTypeValue = { 0.0f, 10000.0f, 1.0f, 0.0f };
// STATIC: 0x00A36780
static const CGameObjectDef::NumberValue s_textMaterialValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36790
static const CGameObjectDef::EnumValue s_textAllowMountedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3679C
static const CGameObjectDef::NumberValue s_textConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A367AC
static const CGameObjectDef::NumberValue s_gooberLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A367BC
static const CGameObjectDef::NumberValue s_gooberQuestIDValue = { -1.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A367CC
static const CGameObjectDef::NumberValue s_gooberEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A367DC
static const CGameObjectDef::NumberValue s_gooberAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 3.0f };
// STATIC: 0x00A367EC
static const CGameObjectDef::NumberValue s_gooberCustomAnimValue = { 0.0f, 4.0f, 1.0f, 0.0f };
// STATIC: 0x00A367FC
static const CGameObjectDef::EnumValue s_gooberConsumableValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36808
static const CGameObjectDef::NumberValue s_gooberCooldownValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36818
static const CGameObjectDef::NumberValue s_gooberSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36828
static const CGameObjectDef::EnumValue s_gooberNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36834
static const CGameObjectDef::NumberValue s_gooberLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36844
static const CGameObjectDef::EnumValue s_gooberLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36850
static const CGameObjectDef::NumberValue s_gooberOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36860
static const CGameObjectDef::NumberValue s_gooberCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36870
static const CGameObjectDef::EnumValue s_gooberLosOKValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A3687C
static const CGameObjectDef::EnumValue s_gooberAllowMountedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36888
static const CGameObjectDef::EnumValue s_gooberFloatingTooltipValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36894
static const CGameObjectDef::NumberValue s_gooberGossipIDValue = { -1.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A368A4
static const CGameObjectDef::EnumValue s_gooberWorldStateSetsStateValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A368B0
static const CGameObjectDef::EnumValue s_gooberFloatOnWaterValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A368BC
static const CGameObjectDef::NumberValue s_gooberConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A368CC
static const CGameObjectDef::EnumValue s_genericFloatingTooltipValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A368D8
static const CGameObjectDef::EnumValue s_genericHighlightValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A368E4
static const CGameObjectDef::EnumValue s_genericServerOnlyValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A368F0
static const CGameObjectDef::EnumValue s_genericLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A368FC
static const CGameObjectDef::EnumValue s_genericFloatOnWaterValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36908
static const CGameObjectDef::NumberValue s_genericQuestIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36918
static const CGameObjectDef::NumberValue s_genericConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36928
static const CGameObjectDef::NumberValue s_cameraLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36938
static const CGameObjectDef::NumberValue s_cameraCameraIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36948
static const CGameObjectDef::NumberValue s_cameraEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36958
static const CGameObjectDef::NumberValue s_cameraOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36968
static const CGameObjectDef::NumberValue s_cameraConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36978
static const CGameObjectDef::NumberValue s_taxiPathIdValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36988
static const CGameObjectDef::NumberValue s_speedValue = { 1.0f, 60.0f, 1.0f, 1.0f };
// STATIC: 0x00A36998
static const CGameObjectDef::NumberValue s_mapObjTransAccelRateValue = { 1.0f, 20.0f, 1.0f, 1.0f };
// STATIC: 0x00A369A8
static const CGameObjectDef::NumberValue s_mapObjTransStartEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369B8
static const CGameObjectDef::NumberValue s_mapObjTransStopEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369C8
static const CGameObjectDef::NumberValue s_mapObjTransTransportPhysicsValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369D8
static const CGameObjectDef::NumberValue s_transportMapIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369E8
static const CGameObjectDef::NumberValue s_mapObjTransWorldState1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369F8
static const CGameObjectDef::EnumValue s_mapObjTransAllowStoppingValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36A04
static const CGameObjectDef::NumberValue s_transportWhenToPauseMsValue = { 0.0f, 2147483648.0f, 1.0f, 0.0f };
// STATIC: 0x00A36A14
static const CGameObjectDef::EnumValue s_transportStartOpenValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36A20
static const CGameObjectDef::NumberValue s_transportAutoCloseValue = { 0.0f, 4294967296.0f, 1.0f, 0.0f };
// STATIC: 0x00A36A30
static const CGameObjectDef::NumberValue s_transportPause1EventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36A40
static const CGameObjectDef::NumberValue s_ritualCastersValue = { 1.0f, 10.0f, 1.0f, 1.0f };
// STATIC: 0x00A36A50
static const CGameObjectDef::NumberValue s_ritualSpellValue = { 1.0f, 65536.0f, 1.0f, 1.0f };
// STATIC: 0x00A36A60
static const CGameObjectDef::NumberValue s_ritualAnimSpellValue = { 0.0f, 65536.0f, 1.0f, 0.0f };
// STATIC: 0x00A36A70
static const CGameObjectDef::EnumValue s_ritualRitualPersistentValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36A7C
static const CGameObjectDef::NumberValue s_ritualCasterTargetSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36A8C
static const CGameObjectDef::NumberValue s_ritualCasterTargetSpellTargetsValue = { 1.0f, 10.0f, 1.0f, 1.0f };
// STATIC: 0x00A36A9C
static const CGameObjectDef::EnumValue s_ritualCastersGroupedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A36AA8
static const CGameObjectDef::EnumValue s_ritualRitualNoTargetCheckValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A36AB4
static const CGameObjectDef::NumberValue s_ritualConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36AC4
static const CGameObjectDef::NumberValue s_mailboxConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36AD4
static const CGameObjectDef::NumberValue s_guardPostCreatureIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36AE4
static const CGameObjectDef::NumberValue s_guardPostChargesValue = { 0.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36B00
static const CGameObjectDef::NumberValue s_spellCasterSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36B10
static const CGameObjectDef::NumberValue s_spellCasterChargesValue = { -1.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36B20
static const CGameObjectDef::EnumValue s_spellCasterPartyOnlyValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36B2C
static const CGameObjectDef::EnumValue s_spellCasterAllowMountedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36B38
static const CGameObjectDef::EnumValue s_spellCasterLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36B44
static const CGameObjectDef::NumberValue s_spellCasterConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36B54
static const CGameObjectDef::NumberValue s_meetingStoneMinLevelValue = { 0.0f, 65535.0f, 0.0f, 1.0f };
// STATIC: 0x00A36B64
static const CGameObjectDef::NumberValue s_meetingStoneMaxLevelValue = { 1.0f, 65535.0f, 0.0f, 60.0f };
// STATIC: 0x00A36B74
static const CGameObjectDef::NumberValue s_meetingStoneAreaIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36B84
static const CGameObjectDef::NumberValue s_flagStandOpenValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36B94
static const CGameObjectDef::NumberValue s_flagStandPickupSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36BA4
static const CGameObjectDef::NumberValue s_flagStandRadiusValue = { 0.0f, 50.0f, 1.0f, 0.0f };
// STATIC: 0x00A36BB4
static const CGameObjectDef::NumberValue s_flagStandReturnAuraValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36BC4
static const CGameObjectDef::NumberValue s_flagStandReturnSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36BD4
static const CGameObjectDef::EnumValue s_flagStandNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36BE0
static const CGameObjectDef::NumberValue s_flagStandOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36BF0
static const CGameObjectDef::EnumValue s_flagStandLosOKValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A36BFC
static const CGameObjectDef::NumberValue s_flagStandConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C0C
static const CGameObjectDef::NumberValue s_flagDropOpenValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C1C
static const CGameObjectDef::NumberValue s_flagDropEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C2C
static const CGameObjectDef::NumberValue s_flagDropPickupSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36C3C
static const CGameObjectDef::EnumValue s_flagDropNoDamageImmuneValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36C48
static const CGameObjectDef::NumberValue s_flagDropOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C58
static const CGameObjectDef::NumberValue s_fishingHoleRadiusValue = { 0.0f, 50.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C68
static const CGameObjectDef::NumberValue s_fishingHoleChestLootValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C78
static const CGameObjectDef::NumberValue s_fishingHoleMinRestockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C88
static const CGameObjectDef::NumberValue s_fishingHoleMaxRestockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C98
static const CGameObjectDef::NumberValue s_fishingHoleOpenValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36CA8
static const CGameObjectDef::NumberValue s_minigameGameTypeValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36CB8
static const CGameObjectDef::NumberValue s_capturePointRadiusValue = { 0.0f, 100.0f, 1.0f, 10.0f };
// STATIC: 0x00A36CC8
static const CGameObjectDef::NumberValue s_capturePointSpellValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36CD8
static const CGameObjectDef::NumberValue s_capturePointWorldState1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36CE8
static const CGameObjectDef::NumberValue s_capturePointWorldstate2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36CF8
static const CGameObjectDef::NumberValue s_capturePointWinEventID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D08
static const CGameObjectDef::NumberValue s_capturePointWinEventID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D18
static const CGameObjectDef::NumberValue s_capturePointContestedEventID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D28
static const CGameObjectDef::NumberValue s_capturePointContestedEventID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D38
static const CGameObjectDef::NumberValue s_capturePointProgressEventID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D48
static const CGameObjectDef::NumberValue s_capturePointProgressEventID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D58
static const CGameObjectDef::NumberValue s_capturePointNeutralEventID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D68
static const CGameObjectDef::NumberValue s_capturePointNeutralEventID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D78
static const CGameObjectDef::NumberValue s_capturePointNeutralPercentValue = { 0.0f, 100.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D88
static const CGameObjectDef::NumberValue s_capturePointWorldstate3Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36D98
static const CGameObjectDef::NumberValue s_capturePointMinSuperiorityValue = { 1.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36DA8
static const CGameObjectDef::NumberValue s_capturePointMaxSuperiorityValue = { 1.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36DB8
static const CGameObjectDef::NumberValue s_capturePointMinTimeValue = { 1.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36DC8
static const CGameObjectDef::NumberValue s_capturePointMaxTimeValue = { 1.0f, 65535.0f, 1.0f, 1.0f };
// STATIC: 0x00A36DD8
static const CGameObjectDef::EnumValue s_capturePointLargeValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36DE4
static const CGameObjectDef::EnumValue s_capturePointHighlightValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A36DF0
static const CGameObjectDef::NumberValue s_capturePointStartingValueValue = { 0.0f, 100.0f, 1.0f, 50.0f };
// STATIC: 0x00A36E00
static const CGameObjectDef::EnumValue s_capturePointUnidirectionalValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36E0C
static const CGameObjectDef::EnumValue s_auraGeneratorStartOpenValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_TRUE };
// STATIC: 0x00A36E18
static const CGameObjectDef::NumberValue s_auraGeneratorRadiusValue = { 0.0f, 100.0f, 1.0f, 10.0f };
// STATIC: 0x00A36E28
static const CGameObjectDef::NumberValue s_auraGeneratorAuraID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E38
static const CGameObjectDef::NumberValue s_auraGeneratorConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E48
static const CGameObjectDef::NumberValue s_auraGeneratorAuraID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E58
static const CGameObjectDef::NumberValue s_auraGeneratorConditionID2Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E68
static const CGameObjectDef::EnumValue s_auraGeneratorServerOnlyValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36E74
static const CGameObjectDef::NumberValue s_dungeonDifficultyMapIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E84
static const CGameObjectDef::NumberValue s_dungeonDifficultyDifficultyValue = { 0.0f, 2.0f, 1.0f, 1.0f };
// STATIC: 0x00A36E94
static const CGameObjectDef::NumberValue s_destructibleBuildingIntactNumHitsValue = { 0.0f, 100000000.0f, 10.0f, 30.0f };
// STATIC: 0x00A36EA4
static const CGameObjectDef::EnumValue s_destructibleBuildingUnusedValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36EB0
static const CGameObjectDef::NumberValue s_destructibleBuildingRebuildingTimeSecsValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36EC0
static const CGameObjectDef::NumberValue s_destructibleBuildingIntactEventValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36ED0
static const CGameObjectDef::NumberValue s_destructibleBuildingDestructibleDataValue = { 1.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36EE0
static const CGameObjectDef::NumberValue s_destructibleBuildingCreditProxyCreatureValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36EF0
static const CGameObjectDef::NumberValue s_guildBankConditionID1Value = { 0.0f, 65535.0f, 0.1f, 0.0f };
// STATIC: 0x00A36F00
static const CGameObjectDef::NumberValue s_trapdoorWhenToPauseMsValue = { 0.0f, 65535.0f, 0.1f, 0.0f };
// STATIC: 0x00A36F10
static const CGameObjectDef::EnumValue s_trapdoorStartOpenValue = { CGameObjectDef::BOOL_INDEX::NUM_BOOL_VALUES, s_boolEnumValues, CGameObjectDef::BOOL_INDEX::BOOL_FALSE };
// STATIC: 0x00A36F1C
static const CGameObjectDef::NumberValue s_trapdoorAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 0.0f };

// STATIC: 0x00ADAD50
static const CGameObjectDef::ValueInfo* s_doorPropertyValues[CGameObjectDef::NUM_DOOR_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorCloseTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorIgnoredByPathingValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_doorConditionID1Value),
};

// STATIC: 0x00ADAD98
static const CGameObjectDef::ValueInfo* s_buttonPropertyValues[CGameObjectDef::NUM_BUTTON_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonLinkedTrapValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonCloseTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_buttonConditionID1Value),
};

// STATIC: 0x00ADADEC
static const CGameObjectDef::ValueInfo* s_questGiverPropertyValues[CGameObjectDef::NUM_QUESTGIVER_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverQuestListIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverTextMaterialValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverGossipIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverCustomAnimValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverConditionID1Value),
};

// STATIC: 0x00ADAE60
static const CGameObjectDef::ValueInfo* s_chestPropertyValues[CGameObjectDef::NUM_CHEST_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLootValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLootTimeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestConsumableValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestMinRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestMaxRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLootEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLinkedTrapValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestQuestIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLeaveLootValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestNotInCombatValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestLogLootValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestUseGroupLootRulesValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestFloatingTooltipValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chestConditionID1Value),
};

// STATIC: 0x00ADB0E4
static const CGameObjectDef::ValueInfo* s_genericPropertiesValues[CGameObjectDef::NUM_GENERIC_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericFloatingTooltipValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericHighlightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericServerOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericFloatOnWaterValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericQuestIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericConditionID1Value),
};

// STATIC: 0x00ADAF20
static const CGameObjectDef::ValueInfo* s_trapPropertyValues[CGameObjectDef::NUM_TRAP_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapChargesValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapCooldownValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapStartDelayValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapServerOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapStealthedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapStealthAffectedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapCloseTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapIgnoreTotemsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapConditionID1Value),
};

// STATIC: 0x00ADAEBC
static const CGameObjectDef::ValueInfo* s_chairPropertyValues[CGameObjectDef::NUM_CHAIR_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairSlotValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairHeightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairOnlyCreatorUseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairTriggeredEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairConditionID1Value),
};

// STATIC: 0x00ADAFC4
static const CGameObjectDef::ValueInfo* s_spellFocusPropertyValues[CGameObjectDef::NUM_SPELLFOCUS_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusLinkedTrapValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusServerOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusQuestIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusFloatingTooltipValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusFloatOnWaterValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellFocusConditionID1Value),
};

// STATIC: 0x00ADAFFC
static const CGameObjectDef::ValueInfo* s_textPropertyValues[CGameObjectDef::NUM_TEXT_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_languageTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textMaterialValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textConditionID1Value),
};

// STATIC: 0x00ADB088
static const CGameObjectDef::ValueInfo* s_gooberPropertyValues[CGameObjectDef::NUM_GOOBER_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberQuestIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberCustomAnimValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberConsumableValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberCooldownValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_languageTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textMaterialValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberLinkedTrapValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberCloseTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberFloatingTooltipValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberGossipIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberWorldStateSetsStateValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberFloatOnWaterValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_gooberConditionID1Value),
};

// STATIC: 0x00ADB188
static const CGameObjectDef::ValueInfo* s_transportPropertyValues[CGameObjectDef::NUM_TRANSPORT_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportWhenToPauseMsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportPause1EventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportPause1EventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportMapIDValue),
};

// STATIC: 0x00ADAF80
static const CGameObjectDef::ValueInfo* s_areaDamagePropertyValues[CGameObjectDef::NUM_AREADAMAGE_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageMin),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageMax),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageSchool),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_areaDamageCloseTextIDValue),
};

// STATIC: 0x00ADB114
static const CGameObjectDef::ValueInfo* s_cameraPropertyValues[CGameObjectDef::NUM_CAMERA_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraCameraIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraConditionID1Value),
};

// STATIC: 0x00ADB14C
static const CGameObjectDef::ValueInfo* s_mapObjTransPropertiesValues[CGameObjectDef::NUM_MOTRANSPORT_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_taxiPathIdValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_speedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransAccelRateValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransStartEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransStopEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransTransportPhysicsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportMapIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransWorldState1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mapObjTransAllowStoppingValue),
};

// STATIC: 0x00ADB1C4
static const CGameObjectDef::ValueInfo* s_ritualPropertyValues[CGameObjectDef::NUM_RITUAL_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualCastersValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualAnimSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualRitualPersistentValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualCasterTargetSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualCasterTargetSpellTargetsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualCastersGroupedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualRitualNoTargetCheckValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_ritualConditionID1Value),
};

// STATIC: 0x00ADB1EC
static const CGameObjectDef::ValueInfo* s_mailboxPropertyValues[CGameObjectDef::NUM_MAILBOX_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mailboxConditionID1Value),
};

// STATIC: 0x00ADB1F8
static const CGameObjectDef::ValueInfo* s_guardPostPropertyValues[CGameObjectDef::NUM_GUARDPOST_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guardPostCreatureIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guardPostChargesValue),
};

// STATIC: 0x00ADB21C
static const CGameObjectDef::ValueInfo* s_spellCasterPropertyValues[CGameObjectDef::NUM_SPELLCASTER_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterChargesValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterPartyOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterConditionID1Value),
};

// STATIC: 0x00ADB240
static const CGameObjectDef::ValueInfo* s_meetingStonePropertyValues[CGameObjectDef::NUM_MEETINGSTONE_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneMinLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneMaxLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneAreaIDValue),
};

// STATIC: 0x00ADB270
static const CGameObjectDef::ValueInfo* s_flagStandPropertyValues[CGameObjectDef::NUM_FLAGSTAND_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandPickupSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandReturnAuraValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandReturnSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagStandConditionID1Value),
};

// STATIC: 0x00ADB2D0
static const CGameObjectDef::ValueInfo* s_fishingHolePropertyValues[CGameObjectDef::NUM_FISHINGHOLE_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleChestLootValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleMinRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleMaxRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleOpenValue),
};

// STATIC: 0x00ADB2A8
static const CGameObjectDef::ValueInfo* s_flagDropPropertyValues[CGameObjectDef::NUM_FLAGDROP_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropPickupSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropOpenTextIDValue),
};

// STATIC: 0x00ADB2E8
static const CGameObjectDef::ValueInfo* s_minigamePropertyValues[CGameObjectDef::NUM_MINIGAME_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_minigameGameTypeValue),
};

// STATIC: 0x00ADB360
static const CGameObjectDef::ValueInfo* s_capturePointPropertyValues[CGameObjectDef::NUM_CAPTUREPOINT_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointWorldState1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointWorldstate2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointWinEventID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointWinEventID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointContestedEventID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointContestedEventID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointProgressEventID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointProgressEventID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointNeutralEventID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointNeutralEventID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointNeutralPercentValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointWorldstate3Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointMinSuperiorityValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointMaxSuperiorityValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointMinTimeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointMaxTimeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointHighlightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointStartingValueValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_capturePointUnidirectionalValue),
};

// STATIC: 0x00ADB3B8
static const CGameObjectDef::ValueInfo* s_auraGeneratorPropertyValues[CGameObjectDef::NUM_AURAGENERATOR_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorAuraID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorConditionID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorAuraID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorConditionID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorServerOnlyValue),
};

// STATIC: 0x00ADB3DC
static const CGameObjectDef::ValueInfo* s_dungeonDifficultyPropertyValues[CGameObjectDef::NUM_DUNGEONDIFFICULTY_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_dungeonDifficultyMapIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_dungeonDifficultyDifficultyValue),
};

// STATIC: 0x00ADAED8
static const CGameObjectDef::ValueInfo* s_barberChairPropertyValues[CGameObjectDef::NUM_BARBERCHAIR_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairHeightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_barberChairHeightOffsetInchesValue),
};

// STATIC: 0x00ADB448
static const CGameObjectDef::ValueInfo* s_destructibleBuildingPropertyValues[CGameObjectDef::NUM_DESTRUCTIBLEBUILDING_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactNumHitsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingCreditProxyCreatureValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactNumHitsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingRebuildingTimeSecsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingDestructibleDataValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingIntactEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_destructibleBuildingUnusedValue),
};

// STATIC: 0x00ADB4A8
static const CGameObjectDef::ValueInfo* s_guildBankPropertyValues[CGameObjectDef::NUM_GUILD_BANK_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guildBankConditionID1Value),
};

// STATIC: 0x00ADB4B8
static const CGameObjectDef::ValueInfo* s_trapdoorPropertyValues[CGameObjectDef::NUM_TRAPDOOR_PROPERTIES] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorWhenToPauseMsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorAutoCloseValue),
};

// STATIC: 0x00A36F30
const CGameObjectDef::ObjectInfo CGameObjectDef::s_objectInfo[NUM_GAMEOBJECT_TYPE] = {
    { "door", CGameObjectDef::NUM_DOOR_PROPERTIES, s_doorPropertiesList, s_doorPropertyValues },
    { "button", CGameObjectDef::NUM_BUTTON_PROPERTIES, s_buttonPropertiesList, s_buttonPropertyValues },
    { "questgiver", CGameObjectDef::NUM_QUESTGIVER_PROPERTIES, s_questGiverPropertiesList, s_questGiverPropertyValues },
    { "chest", CGameObjectDef::NUM_CHEST_PROPERTIES, s_chestPropertiesList, s_chestPropertyValues },
    { "binder", 0, nullptr, nullptr },
    { "generic", CGameObjectDef::NUM_GENERIC_PROPERTIES, s_genericPropertiesList, s_genericPropertiesValues },
    { "trap", CGameObjectDef::NUM_TRAP_PROPERTIES, s_trapPropertiesList, s_trapPropertyValues },
    { "chair", CGameObjectDef::NUM_CHAIR_PROPERTIES, s_chairPropertiesList, s_chairPropertyValues },
    { "spellFocus", CGameObjectDef::NUM_SPELLFOCUS_PROPERTIES, s_spellFocusPropertiesList, s_spellFocusPropertyValues },
    { "text", CGameObjectDef::NUM_TEXT_PROPERTIES, s_textPropertiesList, s_textPropertyValues },
    { "goober", CGameObjectDef::NUM_GOOBER_PROPERTIES, s_gooberPropertiesList, s_gooberPropertyValues },
    { "transport", CGameObjectDef::NUM_TRANSPORT_PROPERTIES, s_transportPropertiesList, s_transportPropertyValues },
    { "areaDamage", CGameObjectDef::NUM_AREADAMAGE_PROPERTIES, s_areaDamagePropertiesList, s_areaDamagePropertyValues },
    { "camera", CGameObjectDef::NUM_CAMERA_PROPERTIES, s_cameraPropertiesList, s_cameraPropertyValues },
    { "mapobject", 0, nullptr, nullptr },
    { "moTransport", CGameObjectDef::NUM_MOTRANSPORT_PROPERTIES, s_mapObjTransPropertiesList, s_mapObjTransPropertiesValues },
    { "duelFlag", 0, nullptr, nullptr },
    { "fishingNode", 0, nullptr, nullptr },
    { "ritual", CGameObjectDef::NUM_RITUAL_PROPERTIES, s_ritualPropertiesList, s_ritualPropertyValues },
    { "mailbox", CGameObjectDef::NUM_MAILBOX_PROPERTIES, s_mailboxPropertiesList, s_mailboxPropertyValues },
    { "DONOTUSE", 0, nullptr, nullptr },
    { "guardPost", CGameObjectDef::NUM_GUARDPOST_PROPERTIES, s_guardPostPropertiesList, s_guardPostPropertyValues },
    { "spellCaster", CGameObjectDef::NUM_SPELLCASTER_PROPERTIES, s_spellCasterPropertiesList, s_spellCasterPropertyValues },
    { "meetingStone", CGameObjectDef::NUM_MEETINGSTONE_PROPERTIES, s_meetingStonePropertiesList, s_meetingStonePropertyValues },
    { "flagStand", CGameObjectDef::NUM_FLAGSTAND_PROPERTIES, s_flagStandPropertiesList, s_flagStandPropertyValues },
    { "fishingHole", CGameObjectDef::NUM_FISHINGHOLE_PROPERTIES, s_fishingHolePropertiesList, s_fishingHolePropertyValues },
    { "flagDrop", CGameObjectDef::NUM_FLAGDROP_PROPERTIES, s_flagDropPropertiesList, s_flagDropPropertyValues },
    { "minigame", CGameObjectDef::NUM_MINIGAME_PROPERTIES, s_minigamePropertiesList, s_minigamePropertyValues },
    { "DONOTUSE", 0, nullptr, nullptr },
    { "capturePoint", CGameObjectDef::NUM_CAPTUREPOINT_PROPERTIES, s_capturePointPropertiesList, s_capturePointPropertyValues },
    { "auraGenerator", CGameObjectDef::NUM_AURAGENERATOR_PROPERTIES, s_auraGeneratorPropertiesList, s_auraGeneratorPropertyValues },
    { "dungeonDifficulty", CGameObjectDef::NUM_DUNGEONDIFFICULTY_PROPERTIES, s_dungeonDifficultyPropertiesList, s_dungeonDifficultyPropertyValues },
    { "barberChair", CGameObjectDef::NUM_BARBERCHAIR_PROPERTIES, s_barberChairPropertiesList, s_barberChairPropertyValues },
    { "destructibleBuilding", CGameObjectDef::NUM_DESTRUCTIBLEBUILDING_PROPERTIES, s_destructibleBuildingPropertiesList, s_destructibleBuildingPropertyValues },
    { "guild bank", CGameObjectDef::NUM_GUILD_BANK_PROPERTIES, s_guildBankPropertiesList, s_guildBankPropertyValues },
    { "trapdoor", CGameObjectDef::NUM_TRAPDOOR_PROPERTIES, s_trapdoorPropertiesList, s_trapdoorPropertyValues },
};

// VERIFY: 65bb1e58 auto
// OFFSET: 0x00746190
int32_t CGameObjectDef::GetPropNum(int32_t typeId, int32_t propId) {
    if (typeId >= NUM_GAMEOBJECT_TYPE || propId >= PROPERTY_ID::NUM_PROP) {
        return -1;
    }

    int32_t numProps = CGameObjectDef::s_objectInfo[typeId].numProperties;

    for (int32_t propNum = 0; propNum < numProps; propNum++) {
        if (CGameObjectDef::s_objectInfo[typeId].propertyInfo[propNum] == propId) {
            return propNum;
        }
    }

    return -1;
}
