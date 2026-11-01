#include "GameObjectDef.h"

// STATIC: 0x00ADAD30
static const int32_t s_props0[8] = { 1, 4, 3, 57, 89, 90, 117, 85 };
// STATIC: 0x00ADAD70
static const int32_t s_props1[10] = { 1, 4, 3, 30, 57, 48, 89, 90, 55, 85 };
// STATIC: 0x00ADADC0
static const int32_t s_props2[11] = { 4, 5, 17, 38, 22, 57, 89, 55, 91, 48, 85 };
// STATIC: 0x00ADAE18
static const int32_t s_props3[18] = { 4, 6, 7, 8, 25, 26, 13, 30, 20, 31, 55, 56, 62, 66, 89, 92, 19, 85 };
// STATIC: 0x00ADB06C
static const int32_t s_props5[7] = { 19, 18, 37, 48, 63, 20, 85 };
// STATIC: 0x00ADAEE0
static const int32_t s_props6[16] = { 4, 31, 24, 10, 9, 23, 3, 32, 37, 65, 48, 58, 89, 90, 123, 85 };
// STATIC: 0x00ADAEA8
static const int32_t s_props7[5] = { 11, 12, 94, 13, 85 };
// STATIC: 0x00ADAFA0
static const int32_t s_props8[9] = { 14, 24, 30, 37, 20, 48, 19, 63, 85 };
// STATIC: 0x00ADAFE8
static const int32_t s_props9[5] = { 15, 16, 17, 91, 85 };
// STATIC: 0x00ADB010
static const int32_t s_props10[23] = { 4, 20, 21, 3, 22, 8, 23, 15, 16, 17, 10, 57, 30, 48, 89, 90, 55, 91, 19, 38, 127, 63, 85 };
// STATIC: 0x00ADB170
static const int32_t s_props11[6] = { 93, 1, 3, 125, 126, 87 };
// STATIC: 0x00ADAF60
static const int32_t s_props12[8] = { 4, 24, 27, 28, 29, 3, 89, 90 };
// STATIC: 0x00ADB100
static const int32_t s_props13[5] = { 4, 33, 21, 89, 85 };
// STATIC: 0x00ADB128
static const int32_t s_props15[9] = { 35, 36, 43, 44, 45, 99, 87, 67, 128 };
// STATIC: 0x00ADB1A0
static const int32_t s_props18[9] = { 34, 10, 39, 40, 49, 50, 54, 46, 85 };
// STATIC: 0x00ADB1E8
static const int32_t s_props19[1] = { 85 };
// STATIC: 0x00ADB1F0
static const int32_t s_props21[2] = { 42, 9 };
// STATIC: 0x00ADB204
static const int32_t s_props22[6] = { 10, 9, 47, 91, 48, 85 };
// STATIC: 0x00ADB234
static const int32_t s_props23[3] = { 51, 52, 53 };
// STATIC: 0x00ADB24C
static const int32_t s_props24[9] = { 4, 59, 24, 60, 61, 57, 89, 55, 85 };
// STATIC: 0x00ADB2BC
static const int32_t s_props25[5] = { 24, 6, 25, 26, 4 };
// STATIC: 0x00ADB294
static const int32_t s_props26[5] = { 4, 21, 59, 57, 89 };
// STATIC: 0x00ADB2E4
static const int32_t s_props27[1] = { 64 };
// STATIC: 0x00ADB308
static const int32_t s_props29[22] = { 24, 10, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 78, 69, 79, 80, 81, 82, 48, 18, 118, 119 };
// STATIC: 0x00ADB2EC
static const int32_t s_props30[7] = { 1, 24, 83, 85, 84, 86, 37 };
// STATIC: 0x00ADB3D4
static const int32_t s_props31[2] = { 87, 88 };
// STATIC: 0x00ADAED0
static const int32_t s_props32[2] = { 12, 121 };
// STATIC: 0x00ADB3E8
static const int32_t s_props33[24] = { 95, 97, 103, 109, 103, 96, 103, 103, 103, 110, 103, 103, 103, 103, 111, 103, 112, 103, 124, 114, 103, 103, 120, 103 };
// STATIC: 0x00ADB3E4
static const int32_t s_props34[1] = { 85 };
// STATIC: 0x00ADB4AC
static const int32_t s_props35[3] = { 93, 1, 3 };

// STATIC: 0x00ADAD28
static const char* s_boolEnumValues[2] = { "false", "true" };

// STATIC: 0x00A36270
static const CGameObjectDef::EnumValue s_doorStartOpenValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A3627C
static const CGameObjectDef::NumberValue s_doorLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A3628C
static const CGameObjectDef::NumberValue s_doorAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 3.0f };
// STATIC: 0x00A3629C
static const CGameObjectDef::EnumValue s_doorNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A362A8
static const CGameObjectDef::NumberValue s_doorOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362B8
static const CGameObjectDef::NumberValue s_doorCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362C8
static const CGameObjectDef::EnumValue s_doorIgnoredByPathingValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A362D4
static const CGameObjectDef::NumberValue s_doorConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A362E4
static const CGameObjectDef::EnumValue s_buttonStartOpenValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A362F0
static const CGameObjectDef::NumberValue s_buttonLockValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36300
static const CGameObjectDef::NumberValue s_buttonAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 3.0f };
// STATIC: 0x00A36310
static const CGameObjectDef::NumberValue s_buttonLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36320
static const CGameObjectDef::EnumValue s_buttonNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A3632C
static const CGameObjectDef::EnumValue s_buttonLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36338
static const CGameObjectDef::NumberValue s_buttonOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36348
static const CGameObjectDef::NumberValue s_buttonCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36358
static const CGameObjectDef::EnumValue s_buttonLosOKValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36364
static const CGameObjectDef::NumberValue s_buttonConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36374
static const CGameObjectDef::NumberValue s_questGiverLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36384
static const CGameObjectDef::NumberValue s_questGiverQuestListIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36394
static const CGameObjectDef::NumberValue s_questGiverTextMaterialValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A363A4
static const CGameObjectDef::NumberValue s_questgiverGossipIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A363B4
static const CGameObjectDef::NumberValue s_questgiverCustomAnimValue = { 0.0f, 4.0f, 1.0f, 0.0f };
// STATIC: 0x00A363C4
static const CGameObjectDef::EnumValue s_questgiverNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A363D0
static const CGameObjectDef::NumberValue s_questgiverOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A363E0
static const CGameObjectDef::EnumValue s_questgiverLosOKValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A363EC
static const CGameObjectDef::EnumValue s_questgiverAllowMountedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A363F8
static const CGameObjectDef::EnumValue s_questgiverLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36404
static const CGameObjectDef::NumberValue s_questgiverConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36414
static const CGameObjectDef::NumberValue s_chestLockValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36424
static const CGameObjectDef::NumberValue s_chestLootValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36434
static const CGameObjectDef::NumberValue s_chestLootTimeValue = { 0.0f, 1800000.0f, 1.0f, 30.0f };
// STATIC: 0x00A36444
static const CGameObjectDef::EnumValue s_chestConsumableValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_chestLosOKValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A364BC
static const CGameObjectDef::EnumValue s_chestLeaveLootValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A364C8
static const CGameObjectDef::EnumValue s_chestNotInCombatValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A364D4
static const CGameObjectDef::EnumValue s_chestLogLootValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A364E0
static const CGameObjectDef::NumberValue s_chestOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A364F0
static const CGameObjectDef::EnumValue s_chestUseGroupLootRulesValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A364FC
static const CGameObjectDef::EnumValue s_chestFloatingTooltipValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36508
static const CGameObjectDef::NumberValue s_chestConditionID1Value = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36518
static const CGameObjectDef::NumberValue s_chairHeightValue = { 0.0f, 2.0f, 1.0f, 1.0f };
// STATIC: 0x00A36528
static const CGameObjectDef::NumberValue s_chairSlotValue = { 1.0f, 5.0f, 1.0f, 1.0f };
// STATIC: 0x00A36538
static const CGameObjectDef::EnumValue s_chairOnlyCreatorUseValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_trapServerOnlyValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36600
static const CGameObjectDef::EnumValue s_trapStealthedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A3660C
static const CGameObjectDef::EnumValue s_trapLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36618
static const CGameObjectDef::EnumValue s_trapStealthAffectedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36624
static const CGameObjectDef::NumberValue s_trapOpenTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36634
static const CGameObjectDef::NumberValue s_trapCloseTextIDValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36644
static const CGameObjectDef::EnumValue s_trapIgnoreTotemsValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_spellFocusServerOnlyValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A3671C
static const CGameObjectDef::NumberValue s_spellFocusQuestIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A3672C
static const CGameObjectDef::EnumValue s_spellFocusLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36738
static const CGameObjectDef::EnumValue s_spellFocusFloatingTooltipValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36744
static const CGameObjectDef::EnumValue s_spellFocusFloatOnWaterValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36750
static const CGameObjectDef::NumberValue s_spellFocusConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36760
static const CGameObjectDef::NumberValue s_textTypeValue = { 0.0f, 10000.0f, 1.0f, 0.0f };
// STATIC: 0x00A36770
static const CGameObjectDef::NumberValue s_languageTypeValue = { 0.0f, 10000.0f, 1.0f, 0.0f };
// STATIC: 0x00A36780
static const CGameObjectDef::NumberValue s_textMaterialValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36790
static const CGameObjectDef::EnumValue s_textAllowMountedValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_gooberConsumableValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36808
static const CGameObjectDef::NumberValue s_gooberCooldownValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36818
static const CGameObjectDef::NumberValue s_gooberSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36828
static const CGameObjectDef::EnumValue s_gooberNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36834
static const CGameObjectDef::NumberValue s_gooberLinkedTrapValue = { 0.0f, 100000000.0f, 0.0f, 0.0f };
// STATIC: 0x00A36844
static const CGameObjectDef::EnumValue s_gooberLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36850
static const CGameObjectDef::NumberValue s_gooberOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36860
static const CGameObjectDef::NumberValue s_gooberCloseTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36870
static const CGameObjectDef::EnumValue s_gooberLosOKValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A3687C
static const CGameObjectDef::EnumValue s_gooberAllowMountedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36888
static const CGameObjectDef::EnumValue s_gooberFloatingTooltipValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36894
static const CGameObjectDef::NumberValue s_gooberGossipIDValue = { -1.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A368A4
static const CGameObjectDef::EnumValue s_gooberWorldStateSetsStateValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A368B0
static const CGameObjectDef::EnumValue s_gooberFloatOnWaterValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A368BC
static const CGameObjectDef::NumberValue s_gooberConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A368CC
static const CGameObjectDef::EnumValue s_genericFloatingTooltipValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A368D8
static const CGameObjectDef::EnumValue s_genericHighlightValue = { 2, s_boolEnumValues, 1 };
// STATIC: 0x00A368E4
static const CGameObjectDef::EnumValue s_genericServerOnlyValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A368F0
static const CGameObjectDef::EnumValue s_genericLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A368FC
static const CGameObjectDef::EnumValue s_genericFloatOnWaterValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::NumberValue s_moTransportAccelRateValue = { 1.0f, 20.0f, 1.0f, 1.0f };
// STATIC: 0x00A369A8
static const CGameObjectDef::NumberValue s_moTransportStartEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369B8
static const CGameObjectDef::NumberValue s_moTransportStopEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369C8
static const CGameObjectDef::NumberValue s_moTransportTransportPhysicsValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369D8
static const CGameObjectDef::NumberValue s_transportMapIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369E8
static const CGameObjectDef::NumberValue s_moTransportWorldState1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A369F8
static const CGameObjectDef::EnumValue s_moTransportAllowStoppingValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36A04
static const CGameObjectDef::NumberValue s_transportWhenToPauseMsValue = { 0.0f, 2147483648.0f, 1.0f, 0.0f };
// STATIC: 0x00A36A14
static const CGameObjectDef::EnumValue s_transportStartOpenValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_ritualRitualPersistentValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36A7C
static const CGameObjectDef::NumberValue s_ritualCasterTargetSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36A8C
static const CGameObjectDef::NumberValue s_ritualCasterTargetSpellTargetsValue = { 1.0f, 10.0f, 1.0f, 1.0f };
// STATIC: 0x00A36A9C
static const CGameObjectDef::EnumValue s_ritualCastersGroupedValue = { 2, s_boolEnumValues, 1 };
// STATIC: 0x00A36AA8
static const CGameObjectDef::EnumValue s_ritualRitualNoTargetCheckValue = { 2, s_boolEnumValues, 1 };
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
static const CGameObjectDef::EnumValue s_spellCasterPartyOnlyValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36B2C
static const CGameObjectDef::EnumValue s_spellCasterAllowMountedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36B38
static const CGameObjectDef::EnumValue s_spellCasterLargeValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_flagStandNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36BE0
static const CGameObjectDef::NumberValue s_flagStandOpenTextIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36BF0
static const CGameObjectDef::EnumValue s_flagStandLosOKValue = { 2, s_boolEnumValues, 1 };
// STATIC: 0x00A36BFC
static const CGameObjectDef::NumberValue s_flagStandConditionID1Value = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C0C
static const CGameObjectDef::NumberValue s_flagDropOpenValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C1C
static const CGameObjectDef::NumberValue s_flagDropEventIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36C2C
static const CGameObjectDef::NumberValue s_flagDropPickupSpellValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36C3C
static const CGameObjectDef::EnumValue s_flagDropNoDamageImmuneValue = { 2, s_boolEnumValues, 0 };
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
static const CGameObjectDef::EnumValue s_capturePointLargeValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36DE4
static const CGameObjectDef::EnumValue s_capturePointHighlightValue = { 2, s_boolEnumValues, 1 };
// STATIC: 0x00A36DF0
static const CGameObjectDef::NumberValue s_capturePointStartingValueValue = { 0.0f, 100.0f, 1.0f, 50.0f };
// STATIC: 0x00A36E00
static const CGameObjectDef::EnumValue s_capturePointUnidirectionalValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36E0C
static const CGameObjectDef::EnumValue s_auraGeneratorStartOpenValue = { 2, s_boolEnumValues, 1 };
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
static const CGameObjectDef::EnumValue s_auraGeneratorServerOnlyValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36E74
static const CGameObjectDef::NumberValue s_dungeonDifficultyMapIDValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36E84
static const CGameObjectDef::NumberValue s_dungeonDifficultyDifficultyValue = { 0.0f, 2.0f, 1.0f, 1.0f };
// STATIC: 0x00A36E94
static const CGameObjectDef::NumberValue s_destructibleBuildingIntactNumHitsValue = { 0.0f, 100000000.0f, 10.0f, 30.0f };
// STATIC: 0x00A36EA4
static const CGameObjectDef::EnumValue s_destructibleBuildingUnusedValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36EB0
static const CGameObjectDef::NumberValue s_destructibleBuildingRebuildingTimeSecsValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36EC0
static const CGameObjectDef::NumberValue s_destructibleBuildingIntactEventValue = { 0.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36ED0
static const CGameObjectDef::NumberValue s_destructibleBuildingDestructibleDataValue = { 1.0f, 65535.0f, 1.0f, 0.0f };
// STATIC: 0x00A36EE0
static const CGameObjectDef::NumberValue s_destructibleBuildingCreditProxyCreatureValue = { 0.0f, 65535.0f, 0.0f, 0.0f };
// STATIC: 0x00A36EF0
static const CGameObjectDef::NumberValue s_guildbankConditionID1Value = { 0.0f, 65535.0f, 0.1f, 0.0f };
// STATIC: 0x00A36F00
static const CGameObjectDef::NumberValue s_trapdoorWhenToPauseMsValue = { 0.0f, 65535.0f, 0.1f, 0.0f };
// STATIC: 0x00A36F10
static const CGameObjectDef::EnumValue s_trapdoorStartOpenValue = { 2, s_boolEnumValues, 0 };
// STATIC: 0x00A36F1C
static const CGameObjectDef::NumberValue s_trapdoorAutoCloseValue = { 0.0f, 4294967296.0f, 0.1f, 0.0f };

// STATIC: 0x00ADAD50
static const CGameObjectDef::ValueInfo* s_values0[8] = {
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
static const CGameObjectDef::ValueInfo* s_values1[10] = {
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
static const CGameObjectDef::ValueInfo* s_values2[11] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverQuestListIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questGiverTextMaterialValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverGossipIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverCustomAnimValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverLosOKValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_questgiverConditionID1Value),
};

// STATIC: 0x00ADAE60
static const CGameObjectDef::ValueInfo* s_values3[18] = {
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
static const CGameObjectDef::ValueInfo* s_values5[7] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericFloatingTooltipValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericHighlightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericServerOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericFloatOnWaterValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericQuestIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_genericConditionID1Value),
};

// STATIC: 0x00ADAF20
static const CGameObjectDef::ValueInfo* s_values6[16] = {
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
static const CGameObjectDef::ValueInfo* s_values7[5] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairSlotValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairHeightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairOnlyCreatorUseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairTriggeredEventValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairConditionID1Value),
};

// STATIC: 0x00ADAFC4
static const CGameObjectDef::ValueInfo* s_values8[9] = {
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
static const CGameObjectDef::ValueInfo* s_values9[5] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_languageTypeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textMaterialValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_textConditionID1Value),
};

// STATIC: 0x00ADB088
static const CGameObjectDef::ValueInfo* s_values10[23] = {
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
static const CGameObjectDef::ValueInfo* s_values11[6] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportWhenToPauseMsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportAutoCloseValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportPause1EventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportPause1EventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportMapIDValue),
};

// STATIC: 0x00ADAF80
static const CGameObjectDef::ValueInfo* s_values12[8] = {
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
static const CGameObjectDef::ValueInfo* s_values13[5] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraLockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraCameraIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraOpenTextIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_cameraConditionID1Value),
};

// STATIC: 0x00ADB14C
static const CGameObjectDef::ValueInfo* s_values15[9] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_taxiPathIdValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_speedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportAccelRateValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportStartEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportStopEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportTransportPhysicsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_transportMapIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportWorldState1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_moTransportAllowStoppingValue),
};

// STATIC: 0x00ADB1C4
static const CGameObjectDef::ValueInfo* s_values18[9] = {
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
static const CGameObjectDef::ValueInfo* s_values19[1] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_mailboxConditionID1Value),
};

// STATIC: 0x00ADB1F8
static const CGameObjectDef::ValueInfo* s_values21[2] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guardPostCreatureIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guardPostChargesValue),
};

// STATIC: 0x00ADB21C
static const CGameObjectDef::ValueInfo* s_values22[6] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterChargesValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterPartyOnlyValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterAllowMountedValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterLargeValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_spellCasterConditionID1Value),
};

// STATIC: 0x00ADB240
static const CGameObjectDef::ValueInfo* s_values23[3] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneMinLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneMaxLevelValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_meetingStoneAreaIDValue),
};

// STATIC: 0x00ADB270
static const CGameObjectDef::ValueInfo* s_values24[9] = {
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
static const CGameObjectDef::ValueInfo* s_values25[5] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleChestLootValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleMinRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleMaxRestockValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_fishingHoleOpenValue),
};

// STATIC: 0x00ADB2A8
static const CGameObjectDef::ValueInfo* s_values26[5] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropEventIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropPickupSpellValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropNoDamageImmuneValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_flagDropOpenTextIDValue),
};

// STATIC: 0x00ADB2E8
static const CGameObjectDef::ValueInfo* s_values27[1] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_minigameGameTypeValue),
};

// STATIC: 0x00ADB360
static const CGameObjectDef::ValueInfo* s_values29[22] = {
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
static const CGameObjectDef::ValueInfo* s_values30[7] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorRadiusValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorAuraID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorConditionID1Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorAuraID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorConditionID2Value),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_auraGeneratorServerOnlyValue),
};

// STATIC: 0x00ADB3DC
static const CGameObjectDef::ValueInfo* s_values31[2] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_dungeonDifficultyMapIDValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_dungeonDifficultyDifficultyValue),
};

// STATIC: 0x00ADAED8
static const CGameObjectDef::ValueInfo* s_values32[2] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_chairHeightValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_barberChairHeightOffsetInchesValue),
};

// STATIC: 0x00ADB448
static const CGameObjectDef::ValueInfo* s_values33[24] = {
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
static const CGameObjectDef::ValueInfo* s_values34[1] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_guildbankConditionID1Value),
};

// STATIC: 0x00ADB4B8
static const CGameObjectDef::ValueInfo* s_values35[3] = {
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorWhenToPauseMsValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorStartOpenValue),
    reinterpret_cast<const CGameObjectDef::ValueInfo*>(&s_trapdoorAutoCloseValue),
};

// STATIC: 0x00A36F30
const CGameObjectDef::TypeInfo CGameObjectDef::s_typeInfo[NUM_GAMEOBJECT_TYPE] = {
    { "door", 8, s_props0, s_values0 },
    { "button", 10, s_props1, s_values1 },
    { "questgiver", 11, s_props2, s_values2 },
    { "chest", 18, s_props3, s_values3 },
    { "binder", 0, nullptr, nullptr },
    { "generic", 7, s_props5, s_values5 },
    { "trap", 16, s_props6, s_values6 },
    { "chair", 5, s_props7, s_values7 },
    { "spellFocus", 9, s_props8, s_values8 },
    { "text", 5, s_props9, s_values9 },
    { "goober", 23, s_props10, s_values10 },
    { "transport", 6, s_props11, s_values11 },
    { "areaDamage", 8, s_props12, s_values12 },
    { "camera", 5, s_props13, s_values13 },
    { "mapobject", 0, nullptr, nullptr },
    { "moTransport", 9, s_props15, s_values15 },
    { "duelFlag", 0, nullptr, nullptr },
    { "fishingNode", 0, nullptr, nullptr },
    { "ritual", 9, s_props18, s_values18 },
    { "mailbox", 1, s_props19, s_values19 },
    { "DONOTUSE", 0, nullptr, nullptr },
    { "guardPost", 2, s_props21, s_values21 },
    { "spellCaster", 6, s_props22, s_values22 },
    { "meetingStone", 3, s_props23, s_values23 },
    { "flagStand", 9, s_props24, s_values24 },
    { "fishingHole", 5, s_props25, s_values25 },
    { "flagDrop", 5, s_props26, s_values26 },
    { "minigame", 1, s_props27, s_values27 },
    { "DONOTUSE", 0, nullptr, nullptr },
    { "capturePoint", 22, s_props29, s_values29 },
    { "auraGenerator", 7, s_props30, s_values30 },
    { "dungeonDifficulty", 2, s_props31, s_values31 },
    { "barberChair", 2, s_props32, s_values32 },
    { "destructibleBuilding", 24, s_props33, s_values33 },
    { "guild bank", 1, s_props34, s_values34 },
    { "trapdoor", 3, s_props35, s_values35 },
};

// VERIFY: 65bb1e58 auto
// OFFSET: 0x00746190
int32_t CGameObjectDef::GetPropNum(int32_t typeId, int32_t prop) {
    if (typeId >= NUM_GAMEOBJECT_TYPE || prop >= 129) {
        return -1;
    }

    int32_t count = CGameObjectDef::s_typeInfo[typeId].m_numProps;

    for (int32_t i = 0; i < count; i++) {
        if (CGameObjectDef::s_typeInfo[typeId].m_props[i] == prop) {
            return i;
        }
    }

    return -1;
}
