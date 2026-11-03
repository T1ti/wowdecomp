Invalid item and UI indices can escape into raw table lookups or Storm array
operators. An unsigned 32-bit `0xFFFFFFFF` index into four-byte elements produces
an offset of almost 16 GiB on x64; pointer elements produce almost 32 GiB.

The minimap APIs also contained a reconstruction error independent of target
architecture. The original 3.3.5 binary uses unsigned `JNB` at `0x0057F1EE`
(`GetTrackingInfo`) and `0x0057F3D1` (`SetTracking`) to exclude negative indices.
The source now uses unsigned comparisons on both targets. Zero and negative Lua
arguments take the existing fallback lookup instead of reading before the spell
array.

All remaining changes are inside `#if defined(WOWDECOMP_ARCH_64)` branches.
The original Win32 expressions and control flow are retained, including inherited
defects verified in the original binary. The x64 branches implement these policies:

| Input/path | x64 behavior for invalid values |
| --- | --- |
| Inventory type used as a slot mask | Return/use an empty mask; slot eligibility returns false |
| Inventory type used as a token | Use the existing empty, non-equipment token |
| Item quality color | Use the existing helper, which falls back to quality 1 |
| DBC class/subclass initialization | Skip invalid class IDs and negative subclass IDs; avoid signed count overflow |
| LCD page APIs | Reject negative/out-of-range indices before allocating or indexing; reject uncreated pages when showing them |
| Unknown LCD XML screen | Warn and skip the screen |
| Action-bar getters | Return no results (`GetActionInfo`) or nil (`GetActionText`) |
| Voice device enumeration | Return nil for negative indices or indices outside the local buffer |
| Guild bank permission mutation | Ignore invalid tab, permission, or selected-rank indices |
| Item-stat serialization | Store at most ten pairs, consume excess wire pairs to preserve following fields, normalize negative counts to zero, and bound packed counts |
| Profession stat-token lookup | Skip stat types outside the token table |

`WOWDECOMP_ARCH_64` is the existing CMake definition for eight-byte pointers.
The configured and validated 64-bit target is x64. These changes do not add ARM
support or change any serialized field types or object layouts.

Run the focused regression test from an x64 MSVC developer shell and again from
an x86 developer shell:

```powershell
python Tools/tests/test_index_guards.py
```

The test extracts production functions into a native harness with lightweight
dependency fixtures. It checks zero/negative minimap arguments, valid tracking,
valid item round trips, x64 count clamping and wire alignment, inventory
boundaries, invalid subclass records, and LCD page boundaries on both outputs.
It also compiles the original Win32 paths. It does not launch the game.

For this change's Win32 preservation proof, pass the commit immediately before
the fix as `--baseline <ref>`. The checker selects the Win32 branches and compares
source tokens, allowing only the two unsigned minimap comparison corrections.
Native source syntax checks and a Release client build complement the harness;
the harness does not replace gameplay testing.
