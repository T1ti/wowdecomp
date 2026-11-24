#include "WowEdit/Source/App/WowEditApp.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Base/Base.h"
#include "Engine/Source/BlizzardCore/OggDecompress.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/Os/OsIME.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/ClientTool.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/shadow.h"
#include "WoW/Source/WowSysMessageOutput.h"
#include "WowEdit/Source/App/WowEditSession.h"
#include "WowEdit/Source/App/WowEditSettings.h"

static CWowEditSession* s_session;

static int32_t UpdateFrameTime(const void* data, void* param) {
    const auto* update = static_cast<const EVENT_DATA_UPDATE*>(data);
    FrameTime::Update(update->m_elapsedSec, update->m_time);
    return 1;
}

int32_t CWowEditApp::InitializeContext(const void* data, void* param) {
    ClientToolInitializeRenderServices();
    EventRegister(EVENT_ID_UPDATE, UpdateFrameTime);
    SysMsgInitialize();
    WowSysMessageOutput::Initialize();
    ClientRegisterConsoleCommands();
    ClientDBInitialize();
    FrameScript_Initialize(0);
    SI2::Init(0);
    CWorldParam::Initialize();
    CWorld::Initialize();
    ShadowInit();
    GxuLightInitialize();
    GxuLightSetGridSize(16.665f);

    s_session = new CWowEditSession(SetViewportCursor);
    OpenShell(*s_session);
    return 1;
}

int32_t CWowEditApp::DestroyContext(const void* data, void* param) {
    CloseShell();
    delete s_session;
    s_session = nullptr;

    SI2::ShutDown(0);
    FrameScript_Destroy();
    WowSysMessageOutput::Shutdown();
    ClientDBShutdown();
    SysMsgShutdown();
    ClientToolDestroyRenderServices();
    return 1;
}

int32_t CWowEditApp::Run() {
    WowEditLoadSettings();

    StormInitialize();
    SErrCatchUnhandledExceptions();
    SErrSetLogTitleString("World of Warcraft Editor");

    int32_t option = 1;
    StormSetOption(10, &option, sizeof(option));
    option = 1;
    StormSetOption(11, &option, sizeof(option));

    OsSystemEnableCpuLog();
    ClientToolSetDataPath(WowEditGetSettings().m_basePath);
    WowEditEnsureDefaultConfig();
    ConsoleInitializeClientCommand();
    CVar::Initialize(WOWEDIT_CONFIG_FILE);
    CVar::ArchiveCodeRegisteredOnly();
    ClientToolOpenData();

    BaseInitializeGlobal();
    EventInitialize(1, 0);
    CVar* timingMethod = CVar::Register("timingMethod", "Desired timing method", CVAR_FLAGS::CVAR_LATCH, "0", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    OsTimeStartup(static_cast<OsTimeManager::TimingMethod>(timingMethod->m_intValue));

    g_startup_StringsDB.Load(__FILE__, __LINE__);
    ConsoleDeviceInitialize("World of Warcraft Editor", 1, 1);
    OsIMEInitialize();
    g_rndSeed.SetSeed(OsGetAsyncTimeMs());
    HEVENTCONTEXT context = EventCreateContextEx(1, InitializeContext, DestroyContext, 0, 0);

    if (context) {
        EventDoMessageLoop();
    }

    SMemSetDebugFlags(0, 8);
    OsIMEDestroy();
    OsTimeShutdown();
    EventDestroy();
    ConsoleDeviceDestroy();
    CVar::Destroy();
    ConsoleCommandDestroy();
    BaseDestroyGlobal();
    ClientToolCloseData();
    StormDestroy();

    if (SFile::IsTrial()) {
        ShutdownOggDecompression();
    }

    return context ? 0 : 1;
}

#endif
