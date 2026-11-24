# WowEdit64 / WowEdit32: the world editor application (WowEdit/Source/App/WowEditApp.h). Built only with
# WOWDECOMP_TOOL_FEATURES. The executable provides its own entry point and links the client sources for
# services and symbols still required by the shared libraries.
file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/*.cpp")

if (WOWDECOMP_ARCH_64)
    set(CLIENT_PLATFORM_SOURCES "${CMAKE_SOURCE_DIR}/WoW/Source/Win64/BotCheck.cpp")
    set(WOWEDITAPP_OUTPUT_NAME WowEdit64)
else ()
    set(WOWEDITAPP_OUTPUT_NAME WowEdit32)
endif ()

# The crash logger of WowClient (console window and a crash log next to the executable), when it is on.
set(WOWEDITAPP_TOOL_SOURCES "")
if (WOWCLIENT_CRASH_LOG)
    list(APPEND WOWEDITAPP_TOOL_SOURCES ${CMAKE_SOURCE_DIR}/Tools/WowClient/crash_log.cpp)
endif ()

add_executable(WowEditApp WIN32 ${CLIENT_SOURCES} ${CLIENT_PLATFORM_SOURCES} ${WOWEDITAPP_TOOL_SOURCES} WowEditAppMain.cpp)

set_target_properties(WowEditApp PROPERTIES
    OUTPUT_NAME ${WOWEDITAPP_OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Win
)

foreach (config ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${config} config)
    set_target_properties(WowEditApp PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_SOURCE_DIR}/bin/Win)
endforeach ()

target_include_directories(WowEditApp
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_options(WowEditApp PRIVATE "/INCREMENTAL:NO" "/ENTRY:start" "/MAP")

# Share the client libraries and link the editor implementation here.
get_target_property(WOWCLIENT_LIBRARIES WowClient LINK_LIBRARIES)
target_link_libraries(WowEditApp PRIVATE WowEdit ${WOWCLIENT_LIBRARIES})
