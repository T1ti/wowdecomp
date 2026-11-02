# Later-reference compatibility review

The user supplied later official flag labels and the character-procedure table at <https://wowdev.wiki/DB/SpellVisualKit#Character_Procedures>. The automated page fetch returned HTTP 403; this review compares the supplied contents with the local 3.3.5 client and reconstructed consumers. Names are descriptions, not recovered debug symbols. No numeric values, signatures, record layouts or branch expressions were changed.

## Shapeshift flags

The supplied flag numbers are bit positions, not masks. The established meanings at positions 0, 1, 3, 5, 6, 7, 8, 10, 11 and 12 fit the native consumers. `CANNOT_CANCEL` was renamed `NOT_TOGGLEABLE`. The broader `CAN_INTERACT` name is retained because the native consumers include game objects as well as NPC interaction.

Positions 2 (`PersistOnDeath`), 4 (`DontUseWeapon`) and 9 (`ConsideredDead`) remain `UNKNOWN` with the later labels documented as unverified references. Positions 13/14 were not imported without Wrath evidence. Absence of a verified client use is not proof that a data bit is unused.

## Visual flags

The supplied labels fit the established masks and replace the earlier descriptive names at all matching consumers. In particular, the native 0x80 branch includes launch-distance and animation-delay timing in `CMissile::FireMissiles` (0x0070260F); its new label is `USE_CLIENT_ANIMATED_TIME_FOR_MISSILE`.

The reference skips bit position 1. This does not remove or renumber the bit: `UNKNOWN_1 = 0x2` remains. The independently verified raw-attachment mask `0x200` also remains.

## Item subclass flags

`UNARMED = 0x4` is established by the assignment to `s_unarmedWeapon` in native `InitWeaponSubclasses` (0x00634CC0). Existing `SKIP_AMMO_CHECK = 0x10` and `AUCTION_INVENTORY_FILTER = 0x200` names remain supported by their consumers.

The proposed meanings of masks 0x1, 0x2, 0x20 and 0x40 are not established by this pass and were not added as confirmed names. In particular, the checked `Spell_C_HaveEquippedSpellItems` path (0x008093D0) tests the ItemSubClass flag 0x10; its other early gate reads SpellRec target flags, a different field. The proposed reagent-subclass interpretation of 0x40 requires separate evidence.

The display flags are a separate word: 0x1 hides the subclass in tooltips; 0x2 hides it in auction selection; 0x4 enables the PaperDoll container-contents count. These now have shared names at every matching consumer. `SHOW_CONTENTS_COUNT` describes the actual `GetBag()->GetItemTypeCount` call rather than implying stack count.

## Override spell permissions

Masks 1/2/4 fit item use, attacks and other spells. Mask 2 is not melee-only: `CGActionBar::IsUsableAction` (0x005A9E20) gates both `IsAttackAction` and `IsRangedAttackAction`; `MeleeModeEnter` also reads it. `ALLOW_ATTACKS` therefore remains appropriate.

No extra-bar meaning was imported for mask 8. The checked `UpdateOverrideSpellBar` (0x005ABB40) selects the temporary action bar from the record's spells without testing mask 8. A later-version ExtraActionButton label is insufficient to establish its Wrath meaning.

## Character procedures

`NONE = -1` is defined separately from the valid chain procedure 0. IDs 0 and 12 share the native chain branch. ID 11 is now `FREEZE_ANIMATION`: its effect constructor at 0x006F80B0 sets the freeze-animation flag after updating model sequence time/flags. ID 17 is now `ITEM_VISUAL`: 0x006F8650 sets the item-visual flag and requests the item cache.

Existing color, eclipse, weapon-trail, blizzard, flash-color (13), alpha (14), procedural-fade (15), and mount-transition (16) descriptions remain. Reference labels Scale (2), Emissive (4), Stand/Walk Anim (7), and Fishing Line (10) are documented beside `UNKNOWN` entries, since their meaning is not established by the checked Wrath dispatcher.

The table's parameter descriptions must not be substituted into the native logic: the checked chain call uses `param2 != 0.0f` and `param3 != 0.0f`; it does not implement the supplied `>= 1f` rule or consume `param1` as the stated target count. The original expressions are preserved.

Validation: source value-equivalence checks and MSVC Win32/x64 syntax checks passed, including the affected DBC record readers. This pass makes no behavior correction.
