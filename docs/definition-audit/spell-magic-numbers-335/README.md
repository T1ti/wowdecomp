# Spell magic-number audit (3.3.5)

The [checklist](checklist.json) tracks all 85 contextual groups from the original `Spell_C.cpp` inventory at `53d7a05`. Original line numbers are historical anchors, not current line numbers. Matching consumers outside that file are included when a domain is named.

`done` means the identified context has been addressed; `partial` has remaining work; `pending` needs review; `retained` records a deliberate decision to preserve an original numeric expression. Ordinary booleans, element indices, mathematical identities and handler return values need not become constants merely because they appear in the literal inventory.

The 3.3.5 executable is authoritative. Names describe verified behavior and do not claim to reproduce original debug symbols. Unknown meanings stay `UNKNOWN`; `UNUSED` requires evidence of disuse in the stated scope. No modern DBC meanings are assumed to apply to Wrath.

Validation uses focused IDA reads, source value-equivalence comparisons, and MSVC Win32/x64 syntax checks. Numeric naming must preserve signedness, floating-point precision, evaluation order and record layout. A behavior correction needs a specific native instruction as evidence. This audit does not claim byte-identical compilation or runtime validation.

## Verified reconstruction correction

`a3cdfa0`: `CMissile::FireMissiles` previously checked the sign of the full `SpellVisualRec::m_flags`. Native instruction `0x0070260F` tests `byte [visual+0x34]` with `0x80`. The code now uses the named `0x80` mask. Other changes in the visual flag batch are value-preserving.
