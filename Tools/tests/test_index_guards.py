"""Run extracted production tracking, item serialization, and table-boundary regressions.

Run in an x64 or x86 MSVC developer shell. --baseline additionally proves that
the selected Win32 source differs only in the two original unsigned comparisons.
"""
import argparse
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
TOKEN = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_]\w*|\d+|[^\s]')


def tokens(text):
    return [m[0] for m in TOKEN.finditer(text) if not m[0].startswith(('//', '/*'))]


def read(path):
    return (ROOT / path).read_text(encoding='utf-8')


def function(path, name):
    text = read(path)
    match = re.search(r'^[^\n;{}]*\b' + re.escape(name) + r'\([^;{}]*\)(?: const)? \{', text, re.M)
    assert match, name
    # Top-level definitions close in column zero. Counting braces before
    # preprocessing would count both alternative for-loop declarations in Pack.
    end = re.search(r'^}', text[match.end():], re.M)
    assert end, name
    return text[match.start():match.end() + end.end()]


def win32_source(text):
    # Resolve only the architecture branches added by this fix; retain other
    # preprocessor directives verbatim so baseline token equality is strict.
    output, enabled, stack = [], True, []
    for line in text.splitlines():
        if line == '#if defined(WOWDECOMP_ARCH_64)':
            stack.append(enabled)
            enabled = False
        elif stack and line == '#else':
            enabled = stack[-1]
        elif stack and line == '#endif':
            enabled = stack.pop()
        elif enabled:
            output.append(line)
    assert not stack
    return '\n'.join(output)


def preserve_win32(baseline):
    changed = subprocess.check_output(['git', 'diff', '--name-only', baseline, '--', 'WoW'], cwd=ROOT, text=True).splitlines()
    assert changed, 'No production changes to check'
    for path in changed:
        before = subprocess.check_output(['git', 'show', baseline + ':' + path], cwd=ROOT, text=True)
        if path.endswith('/MinimapFrame_Scripts.cpp'):
            for comparison in ('>=', '<'):
                old = f'index {comparison} static_cast<int32_t>(s_trackingSpells.Count())'
                new = f'static_cast<uint32_t>(index) {comparison} s_trackingSpells.Count()'
                assert before.count(old) == 1
                before = before.replace(old, new)
        assert tokens(win32_source(read(path))) == tokens(before), path
    print(f'PASS: Win32 source preserved in {len(changed)} files except the two unsigned minimap comparisons')


ITEM = 'WoW/Source/Object/ItemStats.cpp'
TRACKING = 'WoW/Source/Ui/MinimapFrame_Scripts.cpp'
TOOLTIP = 'WoW/Source/UIUtil/Tooltip.cpp'

PRELUDE = r'''
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <vector>
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Common/ComponentCore/Types.h"
#if defined(_M_X64)
#define WOWDECOMP_ARCH_64 1
#else
#define WOWDECOMP_ARCH_32 1
#endif
// The fixture preserves CDataStore's public read/error cursor contract.
struct CDataStore {
    std::vector<uint8_t> data;
    uint32_t m_read = 0, m_size = 0;
    CDataStore& Put(uint32_t value) {
        auto* p = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), p, p + 4); m_size = static_cast<uint32_t>(data.size()); return *this;
    }
    CDataStore& Get(uint32_t& value) {
        if (m_read <= m_size && m_size - m_read >= 4) { std::memcpy(&value, &data[m_read], 4); m_read += 4; }
        else m_read = m_size + 1;
        return *this;
    }
    void PutString(const char* s) {
        data.insert(data.end(), s, s + std::strlen(s) + 1); m_size = static_cast<uint32_t>(data.size());
    }
    void GetString(char* out, uint32_t capacity) {
        uint32_t used = 0;
        while (m_read < m_size) {
            char c = static_cast<char>(data[m_read++]);
            if (used + 1 < capacity) out[used++] = c;
            if (!c) { out[used] = 0; return; }
        }
        out[0] = 0; m_read = m_size + 1;
    }
};
char* SStrDupA(const char* s, const char*, int) {
    auto* p = static_cast<char*>(std::malloc(std::strlen(s) + 1)); std::strcpy(p, s); return p;
}
void SMemFree(void* p, const char*, int, int) { std::free(p); }
void initialize(ItemStats_C& item) {
    auto* start = reinterpret_cast<uint8_t*>(&item.m_class);
    std::memset(start, 0, reinterpret_cast<uint8_t*>(&item) + sizeof(item) - start);
    item.m_scalingStatDistribution = 0x12345678; item.m_holidayId = 0x76543210;
    for (int i = 0; i < 10; ++i) { item.m_statType[i] = i + 23; item.m_statValue[i] = i + 100; }
}
struct lua_State { double argument; bool number = true; const char* last = nullptr; };
double lua_tonumber(lua_State* L, int) { return L->argument; }
bool lua_isnumber(lua_State* L, int) { return L->number; }
void lua_pushstring(lua_State* L, const char* s) { L->last = s; }
void lua_pushnil(lua_State* L) { L->last = nullptr; }
void lua_pushnumber(lua_State*, double) {}
const char* FrameScript_GetText(const char* s, int, int) { return s; }
constexpr int GENDER_NOT_APPLICABLE = 0;
void SStrPrintf(char* out, size_t size, const char* format, ...) {
    va_list args; va_start(args, format); std::vsnprintf(out, size, format, args); va_end(args);
}
struct WOWGUID {};
struct SpellRec { int m_id = 123; const char* m_name = "spell"; int m_activeIconID = 1, m_spellIconID = 1; };
struct SpellIconRec { const char* m_textureFilename; };
struct SpellDB { bool GetRecord(int id, SpellRec*) { assert(id == 123); return true; } } g_spellDB;
struct IconDB { SpellIconRec* GetRecord(int) { return nullptr; } } g_spellIconDB;
struct TrackingTypeInfo { const char* m_name = "tracking"; const char* m_texture = "texture"; };
int trackingQueries = 0, castSpell = 0;
struct TrackingSpells { uint32_t count = 0; int* m_data = nullptr; uint32_t Count() { return count; } } s_trackingSpells;
struct CGMinimapFrame {
    static int s_trackingSpell;
    static const TrackingTypeInfo* s_trackingInfo;
    static const TrackingTypeInfo* GetTrackingTypeByIndex(int32_t index) {
        ++trackingQueries; static TrackingTypeInfo info; return index == 0 ? &info : nullptr;
    }
    static void LoadTrackingInfo(const TrackingTypeInfo* p) { s_trackingInfo = p; }
};
int CGMinimapFrame::s_trackingSpell = 0;
const TrackingTypeInfo* CGMinimapFrame::s_trackingInfo = nullptr;
struct Spell_C { static void CastSpell(int id, void*, WOWGUID, bool) { castSpell = id; } };
void* ClntObjMgrGetActivePlayerObj() { return nullptr; }
void Spell_C_CancelAura(SpellRec*, void*, int) {}
constexpr int SCRIPT_EVENT_MINIMAP_UPDATE_TRACKING = 0;
void Script_SendUniqueSignal(int) {}
template <class T> struct TSFixedArray {
    std::vector<T> values;
    uint32_t Count() { return static_cast<uint32_t>(values.size()); }
    void SetCount(uint32_t n) { assert(n < 1024); values.resize(n); }
    T& operator[](uint32_t i) { assert(i < values.size()); return values[i]; }
};
template <class T> using TSGrowableArray = TSFixedArray<T>;
struct ItemSubClassRec { int32_t m_classID, m_subClassID; uint32_t m_flags; };
constexpr uint32_t ITEM_SUBCLASS_FLAG_UNARMED = 4;
TSFixedArray<const ItemSubClassRec*> s_itemSubclasses[ITEM_CLASS::NUM_ITEM_CLASSES];
TSFixedArray<const ItemSubClassRec*> s_weaponSubclasses;
const ItemSubClassRec* s_unarmedWeapon = nullptr;
ItemSubClassRec weaponClass{ITEM_CLASS_WEAPON, 0, 0};
ItemSubClassRec* s_weaponItemClass = &weaponClass;
void LocateWeaponSubclass() {}
struct SubClassDB { int32_t m_numRecords; ItemSubClassRec* m_records; } g_itemSubClassDB;
using HRESULT = long;
constexpr HRESULT E_FAIL = static_cast<HRESULT>(0x80004005u), S_OK = 0;
constexpr uint32_t INFINITE = UINT32_MAX;
constexpr int LGLCD_QVGA_BMP_WIDTH = 320, LGLCD_QVGA_BMP_HEIGHT = 240;
struct CEzLcd;
int allocatedPages = 0;
struct CEzLcdPage {
    explicit CEzLcdPage(CEzLcd*) { ++allocatedPages; }
    ~CEzLcdPage() { --allocatedPages; }
    void Initialize() {} void SetExpiration(uint32_t) {} void SetSizeWH(int, int) {}
};
struct CLCDOutput { int shown = 0; void ShowPage(CEzLcdPage* p, int) { assert(p); ++shown; } };
struct LCDConnection {
    CLCDOutput mono, color;
    CLCDOutput* MonoOutput() { return &mono; }
    CLCDOutput* ColorOutput() { return &color; }
};
struct CEzLcd {
    LCDConnection m_connection;
    CLCDOutput* m_output = m_connection.MonoOutput();
    TSGrowableArray<CEzLcdPage*> m_monoPages, m_colorPages;
    CEzLcdPage* m_currentMonoPage = nullptr;
    CEzLcdPage* m_currentColorPage = nullptr;
    int m_currentMonoPageIndex = 0, m_currentColorPageIndex = 0;
    CEzLcd() { m_monoPages.SetCount(2); m_colorPages.SetCount(2); }
    ~CEzLcd() {
        for (auto* p : m_monoPages.values) delete p;
        for (auto* p : m_colorPages.values) delete p;
    }
    int32_t AddNewPage(int32_t page); int32_t ShowPage(int32_t page);
    HRESULT ModifyControlsOnPage(int32_t page);
    CEzLcdPage* GetGlobalPage() { static CEzLcdPage global(nullptr); return &global; }
    void SetCurrentPage(CEzLcdPage* p) { m_currentMonoPage = p; }
};
'''

MAIN = r'''
int main() {
    for (uint32_t count : {0u, 1u}) {
        int spell = 123; s_trackingSpells.count = count; s_trackingSpells.m_data = count ? &spell : nullptr;
        for (double input : {-1.0, 0.0}) {
            lua_State L{input}; trackingQueries = castSpell = 0;
            assert(Script_GetTrackingInfo(&L) == 0); assert(trackingQueries == 1);
            assert(Script_SetTracking(&L) == 0); assert(trackingQueries == 2 && castSpell == 0);
        }
        lua_State L{1.0}; trackingQueries = castSpell = 0;
        assert(Script_GetTrackingInfo(&L) == 4);
        Script_SetTracking(&L);
        assert(count ? castSpell == 123 : CGMinimapFrame::s_trackingInfo != nullptr);
        L.argument = count + 2.0; assert(Script_GetTrackingInfo(&L) == 0);
    }
    for (int count : {0, 1, 10}) {
        ItemStats_C item; initialize(item); item.m_statsCount = count;
        CDataStore wire; item.Pack(&wire);
        ItemStats_C parsed; initialize(parsed); parsed.Unpack(&wire);
        assert(parsed.m_statsCount == count && wire.m_read == wire.m_size);
        assert(parsed.m_scalingStatDistribution == 0x12345678 && parsed.m_holidayId == 0x76543210);
        for (int i = 0; i < 10; ++i) assert(parsed.m_statType[i] == (i < count ? uint32_t(i + 23) : UINT32_MAX));
    }
#if defined(WOWDECOMP_ARCH_64)
    ItemStats_C item; initialize(item); item.m_statsCount = 10;
    CDataStore original; item.Pack(&original);
    // Four empty names put the stat count at byte 100 in the wire format.
    constexpr size_t countOffset = 100;
    uint32_t count; std::memcpy(&count, &original.data[countOffset], 4); assert(count == 10);
    CDataStore extra = original; count = 11; std::memcpy(&extra.data[countOffset], &count, 4);
    extra.data.insert(extra.data.begin() + countOffset + 4 + 10 * 8, 8, 0);
    extra.m_size = static_cast<uint32_t>(extra.data.size());
    ItemStats_C parsed; initialize(parsed); parsed.Unpack(&extra);
    assert(parsed.m_statsCount == 10 && extra.m_read == extra.m_size);
    assert(parsed.m_scalingStatDistribution == 0x12345678 && parsed.m_holidayId == 0x76543210);
    item.m_statsCount = 11; CDataStore bounded; item.Pack(&bounded); assert(bounded.data == original.data);
    item.m_statsCount = -1; CDataStore negative; item.Pack(&negative);
    std::memcpy(&count, &negative.data[countOffset], 4); assert(count == 0);
    count = UINT32_MAX; std::memcpy(&negative.data[countOffset], &count, 4);
    ItemStats_C empty; initialize(empty); empty.Unpack(&negative);
    assert(empty.m_statsCount == 0 && negative.m_read == negative.m_size);
    assert(empty.m_scalingStatDistribution == 0x12345678 && empty.m_holidayId == 0x76543210);
    for (uint32_t type = 0; type < INVTYPE_COUNT; ++type) {
        item.m_inventoryType = type;
        assert(checked_slots(&item) == s_inventoryTypeSlots[type]);
        assert(inventory_token(&item) == s_inventoryTypeTokens[type]);
    }
    for (uint32_t type : {uint32_t(INVTYPE_COUNT), UINT32_MAX, 0x80000000u}) {
        item.m_inventoryType = type;
        assert(checked_slots(&item) == 0 && inventory_token(&item) == s_inventoryTypeTokens[0]);
    }
    ItemSubClassRec rows[] = {{2, 0, 0}, {2, 20, 4}, {2, -1, 0}, {17, 0, 0}, {0, INT32_MAX, 0}, {2, INT32_MAX, 0}};
    g_itemSubClassDB = {6, rows};
    SDBItemSubclassInitialize();
    assert(s_itemSubclasses[0].Count() == 0 && s_itemSubclasses[2].Count() == 21);
    assert(s_itemSubclasses[2][0] == &rows[0] && s_itemSubclasses[2][20] == &rows[1]);
    InitWeaponSubclasses();
    assert(s_weaponSubclasses.Count() == 21 && s_weaponSubclasses[20] == &rows[1] && s_unarmedWeapon == &rows[1]);
    CEzLcd lcd;
    assert(lcd.AddNewPage(-1) == -1 && lcd.AddNewPage(2) == -1 && allocatedPages == 0);
    assert(lcd.AddNewPage(0) == 2 && allocatedPages == 1);
    assert(lcd.ShowPage(-1) == E_FAIL && lcd.ShowPage(2) == E_FAIL && lcd.ShowPage(1) == E_FAIL);
    assert(lcd.ShowPage(0) == S_OK && lcd.m_connection.mono.shown == 1);
    assert(lcd.ModifyControlsOnPage(2) == E_FAIL && lcd.ModifyControlsOnPage(-1) == S_OK);
    lcd.m_output = lcd.m_connection.ColorOutput();
    assert(lcd.AddNewPage(0) == 2 && lcd.ShowPage(0) == S_OK && lcd.m_connection.color.shown == 1);
    std::puts("PASS: x64 production tracking, serialization, inventory, subclass, and LCD boundary cases");
#else
    std::puts("PASS: Win32 production tracking and valid item serialization");
#endif
}
'''


def native_test(output):
    bodies = [function(ITEM, name) for name in ('ItemStats_C::ItemStats_C', 'ItemStats_C::~ItemStats_C', 'ItemStats_C::Pack', 'ItemStats_C::Unpack')]
    bodies += [function(TRACKING, name) for name in ('Script_GetTrackingInfo', 'Script_SetTracking')]
    bodies += [function('WoW/Source/DB/DBClient/sdbItemSubclass.cpp', 'SDBItemSubclassInitialize'),
               function('WoW/Source/DB/DBClient/DBClient.cpp', 'InitWeaponSubclasses')]
    bodies += [function('WoW/Source/Ui/UILCD/Windows/EZ_LCD.cpp', name)
               for name in ('CEzLcd::AddNewPage', 'CEzLcd::ShowPage', 'CEzLcd::ModifyControlsOnPage')]
    slots = re.search(r'^const uint32_t s_inventoryTypeSlots[^;]+;', read('WoW/Source/Object/ObjectClient/Item_C.cpp'), re.M)[0]
    token_table = re.search(r'^const char\* s_inventoryTypeTokens[^;]+;', read(TOOLTIP), re.M)[0]
    slot_line = re.search(r'^    uint32_t slots = stats->m_inventoryType <[^\n]+', read(TOOLTIP), re.M)[0]
    token_line = re.search(r'^    lua_pushstring\(L, s_inventoryTypeTokens\[stats->m_inventoryType <[^\n]+', read('WoW/Source/Ui/GameUI_Scripts.cpp'), re.M)[0]
    helpers = '\n'.join([slots, token_table, '#if defined(WOWDECOMP_ARCH_64)',
        'uint32_t checked_slots(const ItemStats_C* stats) {', slot_line, 'return slots; }',
        'const char* inventory_token(const ItemStats_C* stats) { lua_State state{}; auto* L = &state;', token_line, 'return state.last; }', '#endif'])
    source = output / 'native.cpp'
    source.write_text(PRELUDE + '\n'.join(bodies) + helpers + MAIN, encoding='utf-8')
    exe = output / 'native.exe'
    command = ['cl.exe', '/nologo', '/std:c++17', '/EHsc', '/W1', '/I' + str(ROOT), str(source), '/Fe:' + str(exe), '/Fo:' + str(output / 'native.obj')]
    subprocess.run(command, cwd=output, check=True)
    subprocess.run([str(exe)], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', help='git ref before the index fixes')
    parser.add_argument('--output', type=Path, help='keep generated source and native executable here')
    args = parser.parse_args()
    if args.baseline:
        preserve_win32(args.baseline)
    if args.output:
        args.output.mkdir(parents=True, exist_ok=True)
        native_test(args.output.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='wow-index-guards-') as directory:
            native_test(Path(directory))


if __name__ == '__main__':
    main()
