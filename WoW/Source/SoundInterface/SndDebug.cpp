#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "SndDebugPingSound".
// 0.5.3 reference: PingSound (0x004A4CE0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t PingSound(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "SndDebugRoomType".
// 0.5.3 reference: RoomType (0x004A4DE0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t RoomType(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "env".
// 0.5.3 reference: EnvironmentHandler (0x0049F490).
// Registered by 0.5.3 ProviderPrefInitialize.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t EnvironmentHandler(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: console command "envlist".
// 0.5.3 reference: EnvironmentListHandler (0x0049F4B0).
// Registered by 0.5.3 ProviderPrefInitialize.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t EnvironmentListHandler(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: sound chunk dump.
// 0.5.3 reference: ShowCurrentChunkINDOORS (0x004AA5D0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ShowCurrentChunkINDOORS(const char* command, const char* arguments) {
    return 1;
}

// 0.5.3 reference: ShowCurrentChunkOUTDOORS (0x004A7ED0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t ShowCurrentChunkOUTDOORS(const char* command, const char* arguments) {
    return 1;
}

// 0.5.3 reference: SndDebugListChunksINDOORS (0x004AA1C0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t SndDebugListChunksINDOORS(const char* command, const char* arguments) {
    return 1;
}

// 0.5.3 reference: SndDebugListChunksOUTDOORS (0x004A85A0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t SndDebugListChunksOUTDOORS(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: sound debug tick (EVENT_ID 5).
// 0.5.3 reference: DebugTickHandler (0x004A4E10).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t DebugTickHandler(const void* data, void* param) {
    return 1;
}

// 0.5.3 reference: SndDebugDungeonTransition (0x004A4EC0).
// Used the SndDebugDumpChunks string and the ShowCurrentChunk* dumps.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SndDebugDungeonTransition(int32_t indoors, uint32_t id) {
}

// 0.5.3 reference: SndDebugRegisterContinent (0x004A8740).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SndDebugRegisterContinent(uint32_t continentId) {
}

// Debug feature: sound room type (EAX-style reverb preset).
// 0.5.3 reference: SndSetRoomType.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SndSetRoomType(SNDROOMTYPE type) {
}

// Debug feature: sound debug setup.
// 0.5.3 reference: SndDebugInitialize (0x004A4CA0).
// 0.5.3 also hooked DebugTickHandler to EVENT_ID 5; not hooked here yet.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SndDebugInitialize() {
    ConsoleCommandRegister("SndDebugPingSound", PingSound, CATEGORY_SOUND, nullptr);
    ConsoleCommandRegister("SndDebugRoomType", RoomType, CATEGORY_SOUND, nullptr);
    ConsoleCommandRegister("env", EnvironmentHandler, CATEGORY_SOUND, nullptr);
    ConsoleCommandRegister("envlist", EnvironmentListHandler, CATEGORY_SOUND, nullptr);
}

// Debug feature: sound debug teardown.
// 0.5.3 reference: SndDebugShutdown (0x004A4E80).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SndDebugShutdown() {
    ConsoleCommandUnregister("SndDebugPingSound");
    ConsoleCommandUnregister("SndDebugRoomType");
    ConsoleCommandUnregister("env");
    ConsoleCommandUnregister("envlist");
}

#endif
